/*
 
   shell.cpp
 
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
#include <shlobj.h>

extern "C"
{
   void CreateShortCut( LPCSTR pszShortcutFile, LPCSTR pszFolder, LPCSTR pszExe )
   {
      HRESULT hres;
      IShellLink* psl;

      CoInitialize( 0 );

      hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, ( void ** ) & psl );

      if ( SUCCEEDED( hres ) )
      {
         IPersistFile * ppf;

         hres = psl->QueryInterface( IID_IPersistFile, ( void ** ) & ppf );

         if ( SUCCEEDED( hres ) )
         {
            WCHAR wsz[ MAX_PATH ];
            MultiByteToWideChar( CP_ACP, 0, pszShortcutFile, -1, wsz, MAX_PATH );

            hres = psl->SetPath( pszExe );
            hres = psl->SetWorkingDirectory( pszFolder );

            if ( SUCCEEDED( hres ) ) ppf->Save( wsz, TRUE );
            ppf->Release();
         }
         psl->Release();
      }
      CoUninitialize();
   }
}
