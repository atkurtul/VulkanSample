/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2020 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#pragma once

#include <vector>
#include "configure.hpp"
#include "memory.hpp"
#include "object.hpp"

namespace mango
{

    // -----------------------------------------------------------------------
    // stream compression
    // -----------------------------------------------------------------------

    // WARNING! The memory allocation is caller's responsibility; use bound()
    // to get conservative estimate for destination size - the bound is
    // guaranteed to be sufficient.

    // The stream compression interface is abstraction for a stateful block compressor.
    // Each call to encode() will compress a block which can be decoded. The unique
    // feature is that the compressor and decompressor are stateful; the compressor
    // keeps track of what it had compressed before so that data can be compressed
    // efficiently in smaller chunks, which can be decoded individually. The caller
    // is responsible for transmitting the compressed block size - that is NOT part of
    // the protocol. This information is required so that the decompressor can allocate
    // enough memory for the decompressed data. The compressed blocks MUST be decompressed
    // at the same order they were compressed - this is a state machine with two end points:
    // the sender and receiver.

    // This API is useful for transmitting compressed realtime data stream over high latency,
    // low bandwidth connection.

    class StreamEncoder
    {
    public:
        StreamEncoder() {}
        virtual ~StreamEncoder() {}
        virtual size_t bound(size_t size) const = 0;
        virtual size_t encode(Memory dest, ConstMemory source) = 0;
    };

    class StreamDecoder
    {
    public:
        StreamDecoder() {}
        virtual ~StreamDecoder() {}
        virtual size_t decode(Memory dest, ConstMemory source) = 0;
    };

#ifdef MANGO_ENABLE_LICENSE_BSD

    namespace lz4
    {
        SharedObject<StreamEncoder> createStreamEncoder(int level = 4);
        SharedObject<StreamDecoder> createStreamDecoder();
    }

    namespace zstd
    {
        SharedObject<StreamEncoder> createStreamEncoder(int level = 4);
        SharedObject<StreamDecoder> createStreamDecoder();
    }

#endif

    // -----------------------------------------------------------------------
    // memory block compression
    // -----------------------------------------------------------------------

    // WARNING! The memory allocation is caller's responsibility; use bound()
    // to get conservative estimate for destination size - the bound is
    // guaranteed to be sufficient.

    // Compression levels are clamped to range [0, 10]
    // Level 6: default
    // Level 10: maximum compression
    // Other levels are implementation defined

    namespace nocompress
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

#ifdef MANGO_ENABLE_LICENSE_BSD

    namespace lz4
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace lzo
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace zstd
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

#endif

#ifdef MANGO_ENABLE_LICENSE_ZLIB

    namespace bzip2
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace lzfse
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

#endif

    namespace lzma
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace lzma2
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace ppmd8
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace deflate
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace zlib
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    namespace gzip
    {
        size_t bound(size_t size);
        size_t compress(Memory dest, ConstMemory source, int level = 6);
        size_t decompress(Memory dest, ConstMemory source);
    }

    // -----------------------------------------------------------------------
    // Compressor
    // -----------------------------------------------------------------------

    // Optional API to give compressors a name; the compression functions
    // can be used as-is but this allows to enumerate them or ask them by name.

    struct Compressor
    {
        enum Method
        {
            NONE = 0,
            BZIP2,
            LZ4,
            LZO,
            ZSTD,
            LZFSE,
            LZMA,
            LZMA2,
            PPMD8,
            DEFLATE,
            ZLIB,
            GZIP,
        } method;
        std::string name;

        size_t (*bound)(size_t size);
        size_t (*compress)(Memory dest, ConstMemory source, int level);
        size_t (*decompress)(Memory dest, ConstMemory source);
    };

    std::vector<Compressor> getCompressors();
    Compressor getCompressor(Compressor::Method method);
    Compressor getCompressor(const std::string& name);

} // namespace mango
