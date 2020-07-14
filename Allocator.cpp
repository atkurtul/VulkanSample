#include "Device.h"

#define GPU_PAGE_SIZE 268435456 // 2^28 256mb

struct page* local_pages = 0;
struct page* shared_pages = 0;
extern uint memory_idx[2];

typedef struct fnode
{
    struct fnode* next;
    struct chunk* key;
} fnode;

typedef struct chunk
{
    struct chunk* next;
    struct chunk* prev;
    struct page* pg;
    uint offset;
    uint size;
    uint freed;
    VkBuffer buffer;
} chunk;

typedef struct page
{
    struct page* next;
    chunk* head;
    fnode* freed;
    uint cap;
    uint size;
    VkDeviceMemory memory;
    char* map;
} page;

static page* detach_page(page** head, page* pg)
{
    while ((*head) != pg)
        head = &(*head)->next;
    page* tmp = *head;
    *head = (*head)->next;
    return tmp;
}

static page* delete_page(page* pg)
{
    chunk* next = pg->head;
    while (next)
    {
        chunk* tmp = next;
        next = next->next;
        free(tmp);
    }
    fnode* fn = pg->freed;
    while (fn)
    {
        fnode* tmp = fn;
        fn = fn->next;
        free(tmp);
    }
    FreeMemory(pg->memory);
    page* pnext = pg->next;
    free(pg);
    return pnext;
}

static fnode* detach_fnode(fnode** head, chunk* key)
{
    while ((*head)->key != key)
        head = &(*head)->next;
    fnode* tmp = *head;
    *head = (*head)->next;
    return tmp;
}

static void insert_fnode(fnode** head, fnode* insert)
{
    while (*head && (*head)->key->size <= insert->key->size)
        head = &(*head)->next;

    insert->next = *head;
    *head = insert;
}

static void replace_fnode(fnode** head, chunk* key)
{
    insert_fnode(head, detach_fnode(head, key));
}

static page** find_page(page** head, uint size)
{
    while (*head && (*head)->size + size > (*head)->cap)
        head = &(*head)->next;
    return head;
}

static fnode** find_fnode(fnode** head, uint size)
{
    while (*head && (*head)->key->size < size)
        head = &(*head)->next;
    return head;
}

static chunk* extract_fnode(fnode** node)
{
    fnode* tmp = *node;
    chunk* c = tmp->key;
    *node = tmp->next;
    free(tmp);
    c->freed = 0;
    return c;
}

static chunk* split_chunk(chunk* c, uint size)
{
    chunk* next = (chunk*)malloc(sizeof(chunk));
    next->offset = c->offset;
    next->size = size;
    next->prev = c;
    next->next = c->next;
    next->pg = c->pg;
    next->freed = 0;
    if (c->next)
        c->next->prev = next;
    c->offset += size;
    c->size -= size;
    c->next = next;
    return next;
}

static chunk* link_new_chunk(page* pg, uint size, uint offset)
{
    chunk* head = (chunk*)calloc(1, sizeof(chunk));
    head->size = size;
    head->offset = offset;
    head->pg = pg;
    head->next = pg->head;
    pg->head->prev = head;
    pg->head = head;
    pg->size += size;
    return head;

}

static page* link_new_page(page** tail, uint size, enum DeviceMemoryTypeIndex type)
{
    VkMemoryAllocateInfo info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    info.allocationSize = GPU_PAGE_SIZE;
    info.memoryTypeIndex = memory_idx[type];

    page* end = (page*)malloc(sizeof(page));
    end->head = (chunk*)calloc(1, sizeof(chunk));
    end->head->pg = end;
    end->head->size = size;
    end->freed = 0;
    end->cap = GPU_PAGE_SIZE;
    end->size = size;
    end->next = *tail;
    end->memory = AllocateMemory(&info);
    end->map = 0;
    if (type == SHARED_MEMORY)
        end->map = MapMemory(end->memory, 0, GPU_PAGE_SIZE);
    *tail = end;
    return end;
}

static chunk* detach_chunk(chunk* c)
{
    if (c->pg->head == c)
        c->pg->head = c->next;
    if (c->prev)
        c->prev->next = c->next;
    if (c->next)
        c->next->prev = c->prev;
    return c;
}

uint64 VkAllocLocal(uint size)
{
    //align to 1024 bytes
    size += (1024 - (size & 1023)) & 1023;
    page** ppg = find_page(&local_pages, size);
    if (*ppg)
    {
        page* pg = *ppg;
        //find fragmented free node
        fnode** pfc = find_fnode(&pg->freed, size);
        if (*pfc)
        {
            pg->size += size;
            //perfect match
            if ((*pfc)->key->size == size)
                return (uint64)extract_fnode(pfc);
            //split the chunk
            return (uint64)split_chunk((*pfc)->key, size);
        }
        // add a new node
        uint offset = pg->head->offset + pg->head->size;
        if (offset + size <= pg->cap)
            return (uint64)link_new_chunk(pg, size, offset);
    }

    return (uint64)link_new_page(&local_pages, size, LOCAL_MEMORY)->head;
}

void VkFreeLocal(uint64 handle)
{
    chunk* c = (chunk*)handle;

    if (c->freed)
        return;
    c->freed = 1;
    const uint size = c->size;
    page* pg = c->pg;
    pg->size -= size;
    chunk* prev = c->prev;
    chunk* next = c->next;
    //is the head node
    if (!prev)
    {
        //is the only node left
        if (!next)
        {
            delete_page(detach_page(&local_pages, pg));
            return;
        }
        pg->head = next;
        pg->head->prev = 0;
        if (next->freed)
        {
            //after merging with next node, the page is completely freed so delete it
            if (!next->next)
            {
                free(c);
                delete_page(detach_page(&local_pages, pg));
                return;
            }
            pg->head = next->next;
            pg->head->prev = 0;
            free(detach_fnode(&pg->freed, next));
            free(next);
        }

        free(c);
        return;
    }

    if (next && next->freed)
    {
        //merge with right
        next->size += size;
        next->prev = prev;
        prev->next = next;

        if (prev->freed)
        {
            //merge with prev also
            next->size += prev->size;
            next->prev = prev->prev;
            prev->prev->next = next;
            free(detach_fnode(&pg->freed, prev));
            free(prev);
        }

        //reinsert expanded node into correct position
        replace_fnode(&pg->freed, next);
        free(c);
        return;
    }
    if (prev->freed)
    {
        //merge with left if its free
        prev->offset -= size;
        prev->size += size;
        prev->next = next;
        if (next)
            next->prev = prev;

        replace_fnode(&pg->freed, prev);
        free(c);
        return;
    }
    //if no defragmentation available make a new free node
    fnode* node = (fnode*)malloc(sizeof(fnode));
    node->key = c;
    insert_fnode(&pg->freed, node);
}

uint64 VkAllocShared(uint size)
{
    //align to 256 bytes
    size += (256 - (size & 255)) & 255;
    page** ppg = find_page(&shared_pages, size);
    if (*ppg)
    {
        page* pg = *ppg;
        return (uint64)link_new_chunk(pg, size, pg->size);
    }
    return (uint64)link_new_page(ppg, size, SHARED_MEMORY)->head;
}

void VkFreeShared(uint64 handle)
{
    chunk* c = (chunk*)handle;
    if (c->freed)
        return;
    c->freed = 1;
    const uint size = c->size;
    page* pg = c->pg;
    pg->size -= size;
    chunk* prev = c->prev;
    while (prev)
    {
        prev->offset -= size;
        BindBufferMemory(prev->buffer, pg->memory, prev->offset);
        prev = prev->prev;
    }

    free(detach_chunk(c));

    if (!pg->head)
    {
        delete_page(detach_page(&shared_pages, pg));
    }
}

void FreeAllmemory()
{
    page* pg = local_pages;
    while (pg)
    {
        pg = delete_page(pg);
    }
    pg = shared_pages;
    while (pg)
    {
        pg = delete_page(pg);
    }
}

void BindMem(VkBuffer buffer, uint64 handle)
{
    chunk* c = (chunk*)handle;
    c->buffer = buffer;
    BindBufferMemory(buffer, c->pg->memory, c->offset);
}

void BindMem(VkImage image, uint64 handle)
{
    chunk* c = (chunk*)handle;
    BindImageMemory(image, c->pg->memory, c->offset);
}

void* MapMem(uint64 handle)
{
    chunk* c = (chunk*)handle;
    return c->pg->map + c->offset;
}

static void print_chunk(chunk* c)
{
    printf("prev : %#010x - this : %#010x - next : %#010x - - free %u - offset : %5u - size : %5u \n",
        c->prev, c, c->next, c->freed, c->offset, c->size);
}

static void print_fchain(page* pg)
{
    fnode* head = pg->freed;
    int c = 0;
    while (head)
    {
        ++c;
        printf("fthis : %#010x fnext : %#010x ------- ", head, head->next);
        print_chunk(head->key);
        head = head->next;
    }
    printf("Total free chunks %d\n", c);
}

static void print_chunks(page* pg)
{
    chunk* head = pg->head;
    int c = 0;
    while (head)
    {
        ++c;
        print_chunk(head);
        head = head->next;
    }
    printf("Total chunks %d -> ", c);
    printf("Allocated  memory %d KB -- Free space %d KB\n", pg->size / 1024, (pg->cap - pg->size) / 1024);
}

static void test_list(page* pg)
{
    chunk* head = pg->head;
    int wtfs = 0;
    while (head)
    {
        if (head->prev && head->offset + head->size != head->prev->offset)
        {
            wtfs++;
            printf("Wtf mann\n");
        }
        head = head->next;
    }
    if (!wtfs) printf("list is fine\n");
}

static void debug_page(page* pg)
{
    if (!pg)
    {
        printf("page is empty\n");
        return;
    }
    print_chunks(pg);
    print_fchain(pg);
    test_list(pg);
    printf("\n\n");
}

void debug_all_pages()
{
    page* pg = local_pages;

    while (pg)
    {
        debug_page(pg);
        pg = pg->next;
    }
}

void debug_all_shared_pages()
{
    page* pg = shared_pages;

    while (pg)
    {
        debug_page(pg);
        pg = pg->next;
    }
}
