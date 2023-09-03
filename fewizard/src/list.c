
#include <windows.h>
#include "list.h"



void ListInit( List *plist )
{
   plist->ptop = NULL;
   plist->vcount = 0;
}

void ListMoveNext( List *plist )
{
   if ( plist->ptop->pnext == NULL ) return ;
   plist->ptop = plist->ptop->pnext;
}

void ListMovePrev( List *plist )
{

   if ( plist->ptop->pprev == NULL ) return ;
   plist->ptop = plist->ptop->pprev;
}

int ListCount( List *plist )
{
   return( plist->vcount );
}

char *ListPeekShortcut( List *plist )
{
   if ( !plist->vcount ) return NULL;
   return plist->ptop->shortcut;
}

char *ListPeekTarget( List *plist )
{
   if ( !plist->vcount ) return NULL;
   return plist->ptop->target;
}

int ListPeekUID( List *plist )
{
   return plist->ptop->UID;
}

void ListSetUID( List *plist, int value )
{
   plist->ptop->UID = value;
}

void ListMoveFirst( List *plist )
{
   if ( plist->vcount == 0 ) return ;
   if ( plist->ptop->pprev == NULL ) return ;
   while ( plist->ptop->pprev != NULL )
   {
      ListMovePrev( plist );
   }
}

void ListMoveLast( List *plist )
{
   if ( plist->vcount == 0 ) return ;
   while ( plist->ptop->pnext != NULL )
   {
      ListMoveNext( plist );
   }
}

void ListPush( List *plist, char *shortcut, char *target, int UID )
{
   ListNode * padd;

   padd = ( ListNode * ) VirtualAlloc( NULL, sizeof( ListNode ), MEM_COMMIT, PAGE_READWRITE );

   if ( padd == NULL ) return ;

   ListMoveLast( plist );

   lstrcpy( padd->shortcut, shortcut );
   lstrcpy( padd->target, target );
   padd->UID = UID;

   if ( plist->ptop == NULL )
   {
      padd->pnext = NULL;
      padd->pprev = NULL;
      plist->ptop = padd;
   }
   else
   {
      if ( plist->ptop != NULL )
      {
         padd->pprev = plist->ptop;
      }
      else
      {
         padd->pprev = NULL;
      }

      plist->ptop->pnext = padd;

      padd->pnext = NULL;
      plist->ptop = padd;
   }
   plist->vcount++;

   return ;
}

void ListDeleteNode( List *plist )
{
   ListNode * pdel;

   if ( !plist ) return ;
   if ( plist->vcount == 1 )
   {
      ListInit( plist );
      return ;
   }

   pdel = plist->ptop;

   if ( pdel->pnext == NULL )
   {
      if ( plist->ptop->pprev == NULL )
         ListInit( plist );

      plist->ptop = plist->ptop->pprev;
      plist->ptop->pnext = NULL;
   }
   else if ( pdel->pprev == NULL )
   {
      plist->ptop = pdel->pnext;
      plist->ptop->pprev = NULL;
   }
   else
   {
      plist->ptop = plist->ptop->pprev;
      plist->ptop->pnext = pdel->pnext;
      plist->ptop->pnext->pprev = plist->ptop;

   }

   VirtualFree( pdel, 0, MEM_RELEASE );
   plist->vcount--;
}


