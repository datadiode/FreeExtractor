/*
 
   stack.h
 
   Header for stack ADT.
 
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
#ifndef _STACK_H_
#define _STACK_H_

typedef struct stknode
{
   char pdata[ 255 ];
   struct stknode *pprev;
}
StkNode;

typedef struct stk
{
   StkNode *ptop;
   unsigned vcount;
}
Stk;

extern void stkInit( Stk *pstack );
extern int stkPush( Stk *pstack, char *pdata );
extern int stkPop( Stk *pstack );
extern unsigned stkCount( Stk *pstack );
extern char *stkPeek( Stk *pstack );

#endif // _STACK_H_
