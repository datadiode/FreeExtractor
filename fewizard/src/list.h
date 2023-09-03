/*
 
   list.h
 
   Header for Doubly-linked list.
 
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
#ifndef _LIST_H_
#define _LIST_H_

typedef struct Listnode
{
   char shortcut[ 255 ];
   char target[ 255 ];
   int UID;

   struct Listnode *pnext;
   struct Listnode *pprev;
}
ListNode;

typedef struct List
{
   ListNode *ptop;
   int vcount;
}
List;

extern void ListInit( List *plist );


extern void ListDeleteNode( List *plist );
extern void ListPush( List *plist, char *shortcut, char *target, int UID );
extern int ListCount( List *plist );

extern char *ListPeekTarget( List *plist );
extern char *ListPeekShortcut( List *plist );
extern int ListPeekUID( List *plist );

extern void ListSetUID( List *plist, int value );

extern void ListMoveNext( List *plist );
extern void ListMoveFirst( List *plist );
extern void ListMoveLast( List *plist );

#endif // _LIST_H_
