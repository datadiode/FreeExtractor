/* zconf.h -- configuration of the zlib compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* @(#) $Id$ */

#ifndef _ZCONF_H
#define _ZCONF_H

/*
 * If you *really* need a unique prefix for all types and library functions,
 * compile with -DZ_PREFIX. The "standard" zlib should be compiled without it.
 */
#ifdef Z_PREFIX
#define deflateInit_	z_deflateInit_
#define deflate	z_deflate
#define deflateEnd	z_deflateEnd
#define inflateInit_ 	z_inflateInit_
#define inflate	z_inflate
#define inflateEnd	z_inflateEnd
#define deflateInit2_	z_deflateInit2_
#define deflateSetDictionary z_deflateSetDictionary
#define deflateCopy	z_deflateCopy
#define deflateReset	z_deflateReset
#define deflateParams	z_deflateParams
#define inflateInit2_	z_inflateInit2_
#define inflateSync	z_inflateSync
#define inflateSyncPoint z_inflateSyncPoint
#define inflateReset	z_inflateReset
#define compress	z_compress
#define compress2	z_compress2
#define uncompress	z_uncompress
#define adler32	z_adler32
#define crc32		z_crc32
#define get_crc_table z_get_crc_table

#define Byte		z_Byte
#define uInt		z_uInt
#define uLong		z_uLong
#define Bytef	        z_Bytef
#define charf		z_charf
#define intf		z_intf
#define uIntf		z_uIntf
#define uLongf	z_uLongf
#define voidpf	z_voidpf
#define voidp		z_voidp
#endif

#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#define WIN32
#endif
#if defined(__GNUC__) || defined(WIN32) || defined(__386__) || defined(i386)
#ifndef __32BIT__
#define __32BIT__
#endif

#endif

/*
 * Compile with -DMAXSEG_64K if the alloc function cannot allocate more
 * than 64k bytes at a time (needed on systems with 16-bit int).
 */
#if (defined(MSDOS) || defined(_WINDOWS) || defined(WIN32))  && !defined(STDC)
#define STDC
#endif
#if defined(__STDC__) || defined(__cplusplus) || defined(__OS2__)
#ifndef STDC
#define STDC
#endif
#endif

#ifndef STDC
#ifndef const /* cannot use !defined(STDC) && !defined(const) on Mac */
#define const
#endif
#endif

/* Maximum value for memLevel in deflateInit2 */
#ifndef MAX_MEM_LEVEL
#ifdef MAXSEG_64K
#define MAX_MEM_LEVEL 8
#else
#define MAX_MEM_LEVEL 9
#endif
#endif

#ifndef MAX_WBITS
#define MAX_WBITS   15 /* 32K LZ77 window */
#endif

#ifndef OF /* function prototypes */
#ifdef STDC
#define OF(args)  args
#else
#define OF(args)  ()
#endif
#endif

#ifndef ZEXPORT
#define ZEXPORT
#endif
#ifndef ZEXPORTVA
#define ZEXPORTVA
#endif
#ifndef ZEXTERN
#define ZEXTERN extern
#endif

#ifndef FAR
#define FAR
#endif

typedef unsigned char Byte;    /* 8 bits */
typedef unsigned int uInt;    /* 16 bits or more */
typedef unsigned long uLong;   /* 32 bits or more */

#ifdef SMALL_MEDIUM
#define Bytef Byte FAR
#else
typedef Byte FAR Bytef;
#endif

typedef char FAR charf;
typedef int FAR intf;
typedef uInt FAR uIntf;
typedef uLong FAR uLongf;

#ifdef STDC
typedef void FAR *voidpf;
typedef void *voidp;
#else
typedef Byte FAR *voidpf;
typedef Byte *voidp;
#endif


#ifndef z_off_t
#define  z_off_t long
#endif


#endif /* _ZCONF_H */
