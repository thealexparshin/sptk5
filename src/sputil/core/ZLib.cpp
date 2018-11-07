/*
╔══════════════════════════════════════════════════════════════════════════════╗
║                       SIMPLY POWERFUL TOOLKIT (SPTK)                         ║
║                          ZLib.cpp - description                              ║
╟──────────────────────────────────────────────────────────────────────────────╢
║  begin                Thursday May 25 2000                                   ║
║  copyright            (C) 1999-2018 by Alexey Parshin. All rights reserved.  ║
║  email                alexeyp@gmail.com                                      ║
╚══════════════════════════════════════════════════════════════════════════════╝
┌──────────────────────────────────────────────────────────────────────────────┐
│   This library is free software; you can redistribute it and/or modify it    │
│   under the terms of the GNU Library General Public License as published by  │
│   the Free Software Foundation; either version 2 of the License, or (at your │
│   option) any later version.                                                 │
│                                                                              │
│   This library is distributed in the hope that it will be useful, but        │
│   WITHOUT ANY WARRANTY; without even the implied warranty of                 │
│   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library   │
│   General Public License for more details.                                   │
│                                                                              │
│   You should have received a copy of the GNU Library General Public License  │
│   along with this library; if not, write to the Free Software Foundation,    │
│   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.               │
│                                                                              │
│   Please report all bugs and problems to alexeyp@gmail.com.                  │
└──────────────────────────────────────────────────────────────────────────────┘
*/

#include <sptk5/Exception.h>
#include <sptk5/ZLib.h>
#include "zlib.h"

using namespace std;
using namespace sptk;

#define CHUNK 16384

void ZLib::compress(Buffer& dest, const Buffer& src)
{
    int ret, flush;
    unsigned have;
    z_stream strm = {};
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit2(&strm,
                       Z_DEFAULT_COMPRESSION,
                       Z_DEFLATED,
                       MAX_WBITS + 16,
                       ZLIB_VER_MAJOR,
                       Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
        throw Exception("deflateInit() error");

    bool eof = false;
    size_t readPosition = 0;
    /* compress until end of file */
    do {
        auto bytesToRead = uInt(src.bytes() - readPosition);
        if (bytesToRead > CHUNK)
            bytesToRead = CHUNK;
        else
            eof = true;
        memcpy(in, src.c_str() + readPosition, bytesToRead);
        readPosition += bytesToRead;
        strm.avail_in = bytesToRead;
        flush = eof ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            if (ret == Z_STREAM_ERROR)  /* state not clobbered */
                throw Exception("compressed data error");
            have = CHUNK - strm.avail_out;
            dest.append((char*) out, have);
        } while (strm.avail_out == 0);
        //assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    //assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
}

void ZLib::decompress(Buffer& dest, const Buffer& src)
{
    int ret;
    unsigned have;
    z_stream strm = {};
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, 16+MAX_WBITS);
    if (ret != Z_OK)
        throw Exception("inflateInit() error");

    uInt readPosition = 0;
    /* decompress until deflate stream ends or end of file */
    do {
        auto bytesToRead = uInt(src.bytes() - readPosition);
        if (bytesToRead > CHUNK)
            bytesToRead = CHUNK;
        memcpy(in, src.c_str() + readPosition, bytesToRead);
        readPosition += bytesToRead;
        strm.avail_in = bytesToRead;
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)  /* state not clobbered */
                throw Exception("compressed data error");
            switch (ret) {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    throw Exception("premature compressed data error");
                default:
                    break;
            }
            have = CHUNK - strm.avail_out;
            dest.append((char*) out, have);
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    //return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

#if USE_GTEST
#include <gtest/gtest.h>
#include <sptk5/Base64.h>

static const String originalTestString = "This is a test of compression using GZip algorithm";

// Note: ZLib under Windows produces slightly different result,
// due old Windows port version.
#ifdef _WIN32
	static const String originalTestStringBase64 = "H4sIAAAAAAAACwvJyCxWAKJEhZLU4hKF/DSF5PzcgqLU4uLM/DyF0uLMvHQF96jMAoXEnPT8osySjFwAes7C0zIAAAA=";
#else
	static const String originalTestStringBase64 = "H4sIAAAAAAAAAwvJyCxWAKJEhZLU4hKF/DSF5PzcgqLU4uLM/DyF0uLMvHQF96jMAoXEnPT8osySjFwAes7C0zIAAAA=";
#endif

TEST(SPTK_ZLib, compress)
{
    Buffer compressed;
    String compressedBase64;
    ZLib::compress(compressed, Buffer(originalTestString));
    Base64::encode(compressedBase64, compressed);
    
    EXPECT_STREQ(originalTestStringBase64.c_str(), compressedBase64.c_str());
}

TEST(SPTK_ZLib, decompress)
{
    Buffer compressed, decompressed;
    Base64::decode(compressed, originalTestStringBase64);
    ZLib::decompress(decompressed, compressed);

    EXPECT_STREQ(originalTestString.c_str(), decompressed.c_str());
}

#endif