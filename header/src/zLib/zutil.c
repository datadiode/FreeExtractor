/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* @(#) $Id$ */

#include "zutil.h"

struct internal_state
{
   int dummy;
}
;   /* for buggy compilers */

voidpf zcalloc ( voidpf opaque, unsigned items, unsigned size )
{
   if ( opaque ) items += size - size;
   return ( voidpf ) VirtualAlloc( NULL, ( items * size ), MEM_COMMIT, PAGE_READWRITE );
}

void zcfree ( voidpf opaque, voidpf ptr )
{
   VirtualFree( ptr, 0, MEM_RELEASE );
   if ( opaque ) return ;
}
