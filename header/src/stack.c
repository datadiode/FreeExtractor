/*
 
   stack.c
 
   Implementation of stack ADT, to store extracted file information (for deletion).
 
   Author:     Andrew Fawcett (andrewfawcett@users.sourceforge.net)
 
 
 
 
   License
 
   (This license is borrowed from zLib.)
 
   This software is provided 'as-is', without any express or implied warranty.
   In no event will the author(s) be held liable for any damages arising from
   the use of this software. Permission is granted to anyone to use this
   software for any purpose, including commercial applications, and to alter
   it and redistribute it freely, subject to the following restrictions:
 
   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software in a
   product, an acknowledgment in the product documentation would be
   appreciated but is not required.
 
   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
 
   3. This notice may not be removed or altered from any source distribution.
 
   Code Copyright (C) 2000-2001 Andrew Fawcett (andrewfawcett@users.sourceforge.net)
 
*/
#include <windows.h>
#include "stack.h"

void stkInit( Stk *pstack )
{
   pstack->ptop = NULL;
   pstack->vcount = 0;
}

int stkPush( Stk *pstack, char *pdata )
{
   StkNode * padd;

   padd = ( StkNode * ) VirtualAlloc( NULL, sizeof( StkNode ), MEM_COMMIT, PAGE_READWRITE );

   if ( padd == NULL ) return ( 0 );
   lstrcpy( padd->pdata, pdata );

   if ( pstack->ptop == NULL )
   {
      padd->pprev = NULL;
      pstack->ptop = padd;
   }
   else
   {
      padd->pprev = pstack->ptop;
      pstack->ptop = padd;
   }
   pstack->vcount++;

   return 1;
}



char *stkPeek( Stk *pstack )
{
   return pstack->ptop->pdata;
}



int stkPop( Stk *pstack )
{
   StkNode * pdel;

   if ( pstack == NULL ) return 0 ;

   pdel = pstack->ptop;
   pstack->ptop = pstack->ptop->pprev;
   VirtualFree( pdel, 0, MEM_RELEASE );

   pstack->vcount--;

   return ( 1 );
}



unsigned stkCount( Stk *pstack )
{
   return ( pstack->vcount );
}
