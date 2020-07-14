#include "Window.h"
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <shobjidl.h> 

typedef unsigned uint;
typedef unsigned long long uint64;
typedef long long int64;

inline static std::vector<Window*> windows;


static vec2 cast_vec2(int64 lparam)
{
    vec2 v = { (float)(short)(lparam & 0xffff), (float)(short)(lparam >> 16) };
    return v;
}

static void set_input_down(Window* io, uint64 wparam)
{
    io->input[wparam].state++;
    io->input[wparam].time  = io->time;
    io->input[wparam].pos   = io->mpos;
}

static void set_input_up(Window* io, uint64 wparam)
{
    io->input[wparam].state = 0;
    io->input[wparam].time  = io->time;
    io->input[wparam].pos   = io->mpos;
}


static int64 prox(HWND hwnd, uint msg, uint64 wparam, int64 lparam)
{
    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
    Window* io = ((Window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA));
    switch (msg) {
    case WM_NCCREATE:
        SetCursor(LoadCursorA(0, IDC_ARROW));
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (int64)((CREATESTRUCTA*)lparam)->lpCreateParams);
        break;
    case WM_CHAR:
        if(wparam > 31)
        io->text += wparam;
        break;
    case WM_KEYDOWN:
        set_input_down(io, wparam);
        if (wparam == VK_ESCAPE) PostQuitMessage(1);
        break;
    case WM_KEYUP:
        set_input_up(io, wparam);
        break;
    case WM_MOVE:   io->pos = cast_vec2(lparam); break;
    case WM_SIZE:
        io->size = cast_vec2(lparam);
        io->invSize.x = 1.f / io->size.x;
        io->invSize.y = 1.f / io->size.y;
        break;
    case WM_MOUSEMOVE:  
        io->mpos = cast_vec2(lparam); 
        io->mNorm = io->mpos * 2 * io->invSize - 1; 
        break;
    case WM_LBUTTONDOWN: set_input_down(io, 256); break;
    case WM_RBUTTONDOWN: set_input_down(io, 257); break;
    case WM_MBUTTONDOWN: set_input_down(io, 258); break;
    case WM_LBUTTONUP:   set_input_up(io, 256); break;
    case WM_RBUTTONUP:   set_input_up(io, 257); break;
    case WM_MBUTTONUP:   set_input_up(io, 258); break;
    case WM_INPUT:
    {
        RAWINPUT buffer;
        uint size = sizeof(RAWINPUT);
        uint re = GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &buffer, &size, sizeof(RAWINPUTHEADER));
        io->mDelta.x += buffer.data.mouse.lLastX;
        io->mDelta.y += buffer.data.mouse.lLastY;
        break;
    }
    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

Window::Window(int x, int y, int fs)  : input{}
{
    windows.push_back(this);
    time = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&now);
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ::prox;
    wc.hInstance = GetModuleHandleA(0);
    wc.lpszClassName = "Window";
    RegisterClassExA(&wc);
    hwnd = CreateWindowExA(
        WS_EX_ACCEPTFILES, "Window", "Window",
        WS_VISIBLE | WS_SIZEBOX | (fs ? (WS_POPUP | WS_MAXIMIZE) : WS_SYSMENU),
        0, 0, x, y,
        0, 0, wc.hInstance, this);
    RAWINPUTDEVICE rid = { 1, 2 };
    RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
    ShowWindow((HWND)hwnd, SW_SHOW);
    UpdateWindow((HWND)hwnd);
}

void Window::Update()
{
    mDelta.x = mDelta.y = 0;
    LARGE_INTEGER nn;
    QueryPerformanceCounter(&nn);
    step = 0.000001 * (double)(nn.QuadPart - now);
    time += step;
    now = nn.QuadPart;
}

void Window::Resize(int x, int y)
{
    SetWindowPos((HWND)hwnd, 0, 0, 0, x, y, 0);
}

int PollWindows()
{
    for (Window* win : windows)
    {
        win->Update();
    }
    MSG msg;
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
        if (msg.message == WM_QUIT)
        {
            return 0;
        }
    }
    return 1;
}

string OpenFile()
{
    std::string re;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    std::wstring ws = pszFilePath;
                    re = std::string(ws.begin(), ws.end());
                    if (SUCCEEDED(hr))
                    {
                        //MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    return re;
}
