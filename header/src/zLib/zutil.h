/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef _Z_UTIL_H
#define _Z_UTIL_H

#include <windows.h>
#include "zlib.h"

#ifndef DEF_WBITS
#define DEF_WBITS MAX_WBITS
#endif

/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#define DEF_MEM_LEVEL 8
#else
#define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif

/* default memLevel */
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2

/* The three kinds of block type */
#define MIN_MATCH  3
#define MAX_MATCH  258

#define PRESET_DICT 0x20   // Preset dictionary flag in zlib header
#define OS_CODE  0x0b      // Windows

typedef uLong ( ZEXPORT *check_func ) OF( ( uLong check, const Bytef *buf, uInt len ) );
voidpf zcalloc OF( ( voidpf opaque, unsigned items, unsigned size ) );
void zcfree OF( ( voidpf opaque, voidpf ptr ) );

#define ZALLOC(strm, items, size)   (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)           (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p)  {if (p) ZFREE(s, p);}

#endif /* _Z_UTIL_H */
