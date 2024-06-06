/*
 
   FECommon.h
 
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


#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>

#ifdef _WIZARD_
#ifndef NO_HTML_HELP
#include <Htmlhelp.h>
#endif
#endif


#ifdef _CONSOLE
#include <stdio.h>
#include <conio.h>
#endif


#define _TEXT_BLUE_        0x00B75700
#define _TEXT_BLACK_       0x00000000
#define _TEXT_GRAY_        0x00BBBBBB

#define _CRITICAL_         MB_ICONSTOP

#define VERSION            "v1.54"
#define VERSIONDATE        VERSION" ("__DATE__")"
#define WEBSITE_URL        "http://www.disoriented.com"
#define CASESENSITIVITY    0
#define BUFFER_SIZE        16384

#define DLG_SCALE_X        8
#define DLG_SCALE_Y        16

#define DLG_X_SCALE(pixels)      (pixels * LOWORD(GetDialogBaseUnits())) / DLG_SCALE_X
#define DLG_Y_SCALE(pixels)      (pixels * HIWORD(GetDialogBaseUnits())) / DLG_SCALE_Y


#ifdef _CONSOLE
#define WriteConsoleOut             printf
#define SetStatus(TheText)          puts(TheText)
#else
#define SetBannerText(TheText)      SetDlgItemText(hwndMain, IDC_BANNER, TheText)
#define SetSubBannerText(TheText)   SetDlgItemText(hwndMain, IDC_SUBBANNER, TheText)
#define SetTitle(TheTitle)          SetWindowText(hwndMain, TheTitle)
#define SetStatus(TheText)          SetDlgItemText(hwndStatic, IDC_PROGRESS, TheText)
#endif

#define OpenExplorerFolder(TheDir)  ShellExecute( NULL, "explore", TheDir, NULL, NULL, SW_SHOW )

#define DLGITEM_SETFONT(hDlg, nID)  SendDlgItemMessage( hDlg, nID, WM_SETFONT, ( WPARAM ) hActiveFont, TRUE );

/*
 
   Global Variables (common)
 
*/
HWND hwndMain = NULL;
HWND hwndStatic = NULL;
HMENU hShortcuts = NULL;
HANDLE hFile = NULL;

HINSTANCE ghInstance;

char szZipFileName[ MAX_PATH ] = "";
char szEXEOutPath[ MAX_PATH ] = "";
char szTargetVer[ 6 ] = "";
char szIconPath[ MAX_PATH ] = "";
char szExtractionPath[ MAX_PATH ] = "";
char szPackageName[ 255 ] = "";
char szExecuteCommand[ MAX_PATH ] = "";
char szURL[ 255 ] = "";
char szConfirmMessage[ 1024 ] = "";
char szIntroText[ 1024 ];
char szShortcut[ 10240 ] = "";
char szTargetDirectory[ MAX_PATH ];

int iCurrentPage = 1;

BOOL bRunElevated = FALSE;
BOOL bSubsystem64 = FALSE;
UINT uAutoExtract = 0;
BOOL bOpenFolder = FALSE;
BOOL bDeleteFiles = FALSE;
BOOL bNoGUI = FALSE;
BOOL bChangeIcon = FALSE;
BOOL isDebug = FALSE;

HFONT hActiveFont;
HFONT hActiveFontBanner;
HFONT hActiveFontSubBanner;
HFONT hActiveFontIntroBanner;
HFONT hActiveFontURL;
#ifdef _WIZARD_
HFONT hActiveFontProductName;
HFONT hActiveFontVersionDate;
#endif
char szActiveFont[ LF_FACESIZE ];
int const iFontSize = 13;


/*
 
   Prototypes (common)
 
*/
#ifndef _CONSOLE
INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK ChildDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
#endif

/*
 
   CleanUp
 
   Free any open handles and exit.
 
*/
void CleanUp()
{
#ifndef _CONSOLE
   DestroyWindow( hwndStatic );
   DestroyWindow( hwndMain );
   DestroyMenu( hShortcuts );
   CloseHandle( hFile );
#endif // _CONSOLE

   ExitProcess( 0 );
}


/*
 
   RaiseError
 
   Called when there's an error.
 
*/
void RaiseError( LPTSTR szTheErrorText )
{
   char buf[ 1536 ];
   char szWinError[ 1024 ];

   *szWinError = '\0';
   if ( GetLastError() != ERROR_SUCCESS )
   {
      FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL,
                     GetLastError(),
                     MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                     szWinError,
                     _countof(szWinError),
                     NULL );
   }
   wsprintf( buf, "An error prevents this program from continuing:\n\n %s %s", szTheErrorText, szWinError );

#ifdef _CONSOLE
   WriteConsoleOut( "ERROR: %s\n", buf );
#else
   MessageBox( hwndMain, buf, "FreeExtractor Error", _CRITICAL_ );
#endif // _CONSOLE

   CleanUp();
}



/*
 
   LoadDialog
 
   Loads a client dialog into IDD_TEMPLATE
 
*/
#ifndef _CONSOLE
void LoadDialog( int Resource )
{
   hwndStatic = CreateDialog( ghInstance, MAKEINTRESOURCE( Resource ), hwndMain, ChildDialogProc );
}
#endif




#ifndef _CONSOLE
/*
 
   FormatControl
 
   Formats a static text control that needs a white background (e.g. the splash screen)
 
*/
INT_PTR FormatControl( HFONT hf, HDC hDC, COLORREF FontColor )
{
   SelectObject( hDC, hf );
   SetTextColor( hDC, FontColor );
   return ( INT_PTR ) GetStockObject( NULL_BRUSH );
}


#endif // _CONSOLE


/********************************************************************************************
*********************************************************************************************
 
   String parsing and manipulation functions.
 
*********************************************************************************************
********************************************************************************************/

/*

   lstrstr

   Finds a substring in str1

*/
char *lstrstr( const char * str1, const char * str2 )
{
   const char *cp = str1;

   if ( !*str2 )
      return ( char * ) str1;

   while ( *cp )
   {
      const char *s1 = cp;
      const char *s2 = str2;

      while ( *s1 && *s2 && *s1 == *s2 )
         s1++, s2++;

      if ( !*s2 )
         return ( char * ) cp;

      cp++;
   }

   return NULL;
}

/*
 
   gettoken
 
   Retrieves the tokennum-th token in string in, delimited by delimiter.
 
*/
char *gettoken( char *in, char *delimiter, int tokennum, char *out )
{
   char *curpos = in;
   char *endpos;
   int len = lstrlen( in );

   *out = '\0';

   if ( tokennum > len ) return "";

   while ( tokennum > 0 )
   {
      curpos = lstrstr( curpos, delimiter );
      if ( !curpos ) return "";
      --tokennum;
      ++curpos;
   }

   endpos = lstrstr( curpos, delimiter );

   if ( !endpos )
   {
      endpos = in + len;
      delimiter = "";
   }

   lstrcpyn( out, curpos, (int) (endpos - curpos) + 1 );
   return delimiter;
}

/*
 
   IsExtension
 
*/
BOOL IsExtension(const char *exec, const char *exten)
{
   const char *p = exten + lstrlen(exten);
   const char *q = *exec == '"' ? lstrstr( exec + 1, "\"" ) : exec + lstrlen(exec);
   while ((p > exten) && (q > exec))
   {
      if ((*--p & ~0x20) != (*--q & ~0x20))
         return FALSE;
   }
   return (p == exten) && (q > exec) && (*--q == '.');
}

/*
 
   FileExists
 
*/
BOOL FileExists( LPTSTR szPathAndFile )
{
   return ( GetFileAttributes( szPathAndFile ) & FILE_ATTRIBUTE_DIRECTORY ) == 0;
}

/*
 
   DirectoryExists
 
*/
BOOL DirectoryExists( LPTSTR szDirName )
{
   return ( GetFileAttributes( szDirName ) & ( FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY ) ) == FILE_ATTRIBUTE_DIRECTORY;
}

/*
 
   queryShellFolders
 
   Grabs this user's shell folder variables from the registry.
 
*/
#ifndef _GETVERSION_
void queryShellFolders( char *name, char *out )
{
   HKEY hKey;
   *out = '\0';
   if ( RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
   {
      DWORD l = MAX_PATH;
      DWORD t = REG_SZ;
      RegQueryValueEx( hKey, name, NULL, &t, ( LPBYTE ) out, &l );
      RegCloseKey( hKey );
   }
}
#endif // _GETVERSION_

/*
 
   DoesFontExist
 
   A cheap hack. Check to see if a TrueType font exists.
 
*/
#ifndef _GETVERSION_
int CALLBACK FontEnumProc(LOGFONT const *lplf, void const *lptm, DWORD dwStyle, LPARAM lParam)
{
   return 0xC00L;
}

BOOL DoesFontExist ( LPTSTR szName )
{
   HDC hdc = GetDC(NULL);
   int found = EnumFontFamilies(hdc, szName, FontEnumProc, 0);
   ReleaseDC(NULL, hdc);
   return found == 0xC00L;
}
#endif // _GETVERSION_


/*
 
  ParsePath
 
  Expands path variables in a path string
 
*/
#ifdef _HEADER_
void ParsePath( char *output, int size )
{
   int offset = 0;
   int tokennum = 0;
   char *delimiter;

   char input[ 4096 ];
   char token[ 4096 ];
   char tmp[ MAX_PATH ];

   ExpandEnvironmentStrings( output, input, _countof(input) );

   do
   {
      //
      // Get the next token (delimited by a '$')
      //
      delimiter = gettoken( input, "$", tokennum, token );

      if ( ++tokennum & 1 )
      {
         // Leave it alone - only every second token qualifies for replacement.
      }
      else if ( *token == '\0' )
      {
         lstrcpy( token, "$$" );
      }
      //
      // Start Menu (for this user)
      //
      else if ( !lstrcmpi( token, "startmenu" ) )
      {
         queryShellFolders( "Programs", token );
      }

      //
      // Desktop
      //
      else if ( !lstrcmpi( token, "desktop" ) )
      {
         queryShellFolders( "Desktop", token );
      }

      //
      // Sendto Directory
      //
      else if ( !lstrcmpi( token, "sendto" ) )
      {
         queryShellFolders( "SendTo", token );
      }

      //
      // Favorites
      //
      else if ( !lstrcmpi( token, "favorites" ) )
      {
         queryShellFolders( "Favorites", token );
      }

      //
      // Start Up Folder in the Start Menu for this user
      //
      else if ( !lstrcmpi( token, "startup" ) )
      {
         queryShellFolders( "Startup", token );
      }

      //
      // Output directory
      //
      else if ( !lstrcmpi( token, "targetdir" ) )
      {
         lstrcpy( token, szTargetDirectory );
      }

      //
      // Current Directory
      //
      else if ( !lstrcmpi( token, "curdir" ) )
      {
         GetCurrentDirectory( MAX_PATH, token );
      }

      //
      // Quicklaunch directory: If the OS doesn't support
      // it, this defaults to the temp dir.
      //
      else if ( !lstrcmpi( token, "quicklaunch" ) )
      {
         queryShellFolders( "AppData", token );

         if ( *token )
         {
            lstrcat( token, "\\Microsoft\\Internet Explorer\\Quick Launch" );
         }

         if ( !DirectoryExists( token ) )
         {
            GetTempPath( MAX_PATH, token );
         }
      }

      //
      // Program Files Directory
      //
      else if ( !lstrcmpi( token, "programfiles" ) )
      {
         HKEY hKey;

         lstrcpy( token, "C:\\Program Files" );
         if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
         {
            int l = MAX_PATH;
            int t = REG_SZ;
            RegQueryValueEx( hKey, "ProgramFilesDir", NULL, &t, token, &l );
            RegCloseKey( hKey );
         }
      }

      //
      // System Directory
      //
      else if ( !lstrcmpi( token, "sysdir" ) )
      {
         GetSystemDirectory( token, MAX_PATH );
      }

      //
      // Command line
      //
      else if ( !lstrcmpi( token, "cmdline" ) )
      {
         lstrcpyn( token, GetCommandLine(), _countof(token) );
      }

      //
      // Dialog placement in the form expected by IHTMLWindow2::showModalDialog()
      //
      else if ( !lstrcmpi( token, "dialogplacement" ) )
      {
         RECT rcWindow = { 0, 0, 0, 0 };
         RECT rcClient = { 0, 0, 0, 0 };
         GetWindowRect( hwndMain, &rcWindow );
         GetClientRect( hwndMain, &rcClient );
         wsprintf( token,
            "dialogLeft:%dpx;dialogTop:%dpx;dialogWidth:%dpx;dialogHeight:%dpx",
            rcWindow.left, rcWindow.top, rcClient.right, rcClient.bottom);
      }

      //
      // Registry location in the form registry:Hive\MyKey\MySubKey@Value
      //
      else if ( lstrlen( token ) < sizeof tmp && lstrcpyn( tmp, token, sizeof "registry:" ) && !lstrcmpi( tmp, "registry:" ) )
      {
        HKEY hKey = lstrcpyn( tmp, token + sizeof "registry", sizeof "HKCR\\" ) && !lstrcmpi( tmp, "HKCR\\" ) ? HKEY_CLASSES_ROOT :
                    lstrcpyn( tmp, token + sizeof "registry", sizeof "HKCU\\" ) && !lstrcmpi( tmp, "HKCU\\" ) ? HKEY_CURRENT_USER :
                    lstrcpyn( tmp, token + sizeof "registry", sizeof "HKLM\\" ) && !lstrcmpi( tmp, "HKLM\\" ) ? HKEY_LOCAL_MACHINE :
                    NULL;
        gettoken( token, "@", 0, tmp );
        if ( hKey && RegOpenKeyEx( hKey, tmp + sizeof "registry:????", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
        {
          int l = MAX_PATH;
          int t = REG_SZ;
          gettoken( token, "@", 1, tmp );
          RegQueryValueEx( hKey, tmp, NULL, &t, token, &l );
          RegCloseKey( hKey );
        }
      }

      //
      // The token isn't a variable, so append it.
      //
      lstrcpyn( output + offset, token, size - offset );
      offset += lstrlen( output + offset );
   } while ( *delimiter );
}
#endif //_HEADER_



/*
 
  CreateDirectoryRecursively
 
   Creates a directory recursively. If parent folders do not already exist, they are created.
 
*/
BOOL CreateDirectoryRecursively( LPTSTR szPath )
{
   int tokennum = 0;
   char *delimiter = NULL;

   char szFullPath[ MAX_PATH + 3 ];
   char szTempPath[ MAX_PATH + 3 ];
   char szCurToken[ MAX_PATH + 3 ];

   BOOL successful = TRUE;

   *szFullPath = *szTempPath = *szCurToken = '\0';

   GetFullPathName( szPath, MAX_PATH, szFullPath, &delimiter );
   if (delimiter)
      *delimiter = '\0';

   if ( !DirectoryExists( szFullPath ) ) do
   {
      delimiter = gettoken( szFullPath, "\\", tokennum++, szCurToken );
      lstrcat( szTempPath, szCurToken );
      lstrcat( szTempPath, delimiter );

      GetFullPathName( szTempPath, MAX_PATH, szTempPath, NULL );

      if ( !DirectoryExists( szTempPath ) )
         successful = CreateDirectory( szTempPath, NULL );
   } while ( *delimiter );

   return successful;
}


#ifndef _CONSOLE
void *memset( void * dst, int value, size_t count )
{
   void * ret = dst;

   while ( count-- )
   {
      *( char * ) dst = ( char ) value;
      dst = ( char * ) dst + 1;
   }

   return ( ret );
}

void *memcpy( void * dst, const void * src, size_t count )
{
   void * ret = dst;

   while ( count-- )
   {
      *( char * ) dst = *( char * ) src;
      dst = ( char * ) dst + 1;
      src = ( char * ) src + 1;
   }

   return ( ret );
}

int memcmp( const void * buf1, const void * buf2, size_t count )
{
   if ( !count ) return ( 0 );

   while ( --count && *( char * ) buf1 == *( char * ) buf2 )
   {
      buf1 = ( char * ) buf1 + 1;
      buf2 = ( char * ) buf2 + 1;
   }

   return ( *( ( unsigned char * ) buf1 ) - *( ( unsigned char * ) buf2 ) );
}
#endif


/*
 
   replace_data
 
   Used for finding and replacing the icon in an SFX.
 
*/
void replace_data( char *hdr, int hdr_len, const char *srch, int srchlen, const char *filename )
{
   HANDLE fp;
   DWORD dummy;
   char temp[32];

   srch += 32;
   srchlen -= 32;

   while ( hdr_len >= srchlen  && memcmp( hdr, srch, srchlen ) )
   {
      hdr++;
      hdr_len--;
   }

   if ( hdr_len < 0 )
      RaiseError( "Could not set new icon." );

   fp = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if ( fp == INVALID_HANDLE_VALUE )
      RaiseError( "Could not set new icon." );

   ReadFile( fp, temp, 32, &dummy, NULL );
   ReadFile( fp, hdr, srchlen, &dummy, NULL );

   CloseHandle( fp );
}



/*
 
   Build
 
   This function actually builds the EXE. This is shared between
   MakeSFX and FEWizard.
 
*/
#ifndef _GETVERSION_
#ifndef _HEADER_
DWORD CALLBACK Build( void *dummy )
{
   HANDLE hFEHeaderOut, hSourceZip;

   char IoBuffer[ BUFFER_SIZE ];
   int iINIFileSize;
   int iZipFileSize;
   int i = 0;
   DWORD dwDummy, dwBytesRead, dwBytesWritten;

   char *p, *q;
   HRSRC const hICON = FindResource(NULL, "setup.ico", RT_RCDATA);
   DWORD const cchICON = SizeofResource(NULL, hICON);
   char* const pchICON = (char*)LoadResource(NULL, hICON);
   HRSRC const hSTUB = FindResource(NULL,
      IsExtension(szZipFileName, "cab") ? (
         bSubsystem64 ? (
            bRunElevated ? "header64_cab_elevated" : "header64_cab"
         ) : (
            bRunElevated ? "header32_cab_elevated" : "header32_cab"
         )
      ) : (
         bSubsystem64 ? (
            bRunElevated ? "header64_elevated" : "header64"
         ) : (
            bRunElevated ? "header32_elevated" : "header32"
         )
      ),
      RT_RCDATA);
   DWORD const cchSTUB = SizeofResource(NULL, hSTUB);
   char* const pchSTUB = (char*)memcpy(_alloca(cchSTUB), LoadResource(NULL, hSTUB), cchSTUB);

   //
   // Patch OS version
   //
   IMAGE_DOS_HEADER *const mz = (IMAGE_DOS_HEADER *)pchSTUB;
   IMAGE_NT_HEADERS *const nt = (IMAGE_NT_HEADERS *)(pchSTUB + mz->e_lfanew);

   // Assert some assumptions about the memory layout of the involved structs
   C_ASSERT(FIELD_OFFSET(IMAGE_NT_HEADERS32, OptionalHeader.MajorOperatingSystemVersion) == FIELD_OFFSET(IMAGE_NT_HEADERS64, OptionalHeader.MajorOperatingSystemVersion));
   C_ASSERT(FIELD_OFFSET(IMAGE_NT_HEADERS32, OptionalHeader.MinorOperatingSystemVersion) == FIELD_OFFSET(IMAGE_NT_HEADERS64, OptionalHeader.MinorOperatingSystemVersion));
   C_ASSERT(FIELD_OFFSET(IMAGE_NT_HEADERS32, OptionalHeader.MajorSubsystemVersion) == FIELD_OFFSET(IMAGE_NT_HEADERS64, OptionalHeader.MajorSubsystemVersion));
   C_ASSERT(FIELD_OFFSET(IMAGE_NT_HEADERS32, OptionalHeader.MinorSubsystemVersion) == FIELD_OFFSET(IMAGE_NT_HEADERS64, OptionalHeader.MinorSubsystemVersion));

   WORD wMajor;
   WORD wMinor = 0;

   p = szTargetVer;
   do
   {
      wMajor = wMinor;
      for (wMinor = 0; *p && *p != '.'; wMinor += *p++ & 0xF) wMinor *= 10;
   } while (*p++);

   if (wMajor != 0)
   {
      nt->OptionalHeader.MajorOperatingSystemVersion = wMajor;
      nt->OptionalHeader.MinorOperatingSystemVersion = wMinor;
      nt->OptionalHeader.MajorSubsystemVersion = wMajor;
      nt->OptionalHeader.MinorSubsystemVersion = wMinor;
   }

   //
   // Read source zip file
   //
   SetStatus( "Reading source ZIP file..." );

   hSourceZip = CreateFile( szZipFileName,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL );

   //
   // Change icon, if requested
   //
   if ( bChangeIcon )
   {
      replace_data( pchSTUB, cchSTUB, pchICON, cchICON, szIconPath );
   }

   //
   // Create output SFX
   //
   hFEHeaderOut = CreateFile( szEXEOutPath,
                              GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,
                              CREATE_ALWAYS,
                              0,
                              NULL );

   WriteFile( hFEHeaderOut, pchSTUB, cchSTUB, &dwDummy, NULL );


   //
   // Build and append the settings to this SFX.
   //
   SetStatus( "Saving archive settings..." );

   //
   // Replace return carriages in the intro text.
   //
   while ( szIntroText[ i ] != '\0' )
   {
      if ( ( szIntroText[ i ] == '\\' ) && ( szIntroText[ i + 1 ] == 'n' ) )
      {
         szIntroText[ i ] = 0x0D;
         szIntroText[ i + 1 ] = 0x0A;
      }
      i++;
   }

   iZipFileSize = GetFileSize( hSourceZip, NULL );


   //
   // Build the metadata string
   //
   p = IoBuffer;
   p += wsprintf(p, "Name=%s", szPackageName) + 1;
   p += wsprintf(p, "Exec=%s", szExecuteCommand) + 1;
   p += wsprintf(p, "DefaultPath=%s", szExtractionPath) + 1;
   p += wsprintf(p, "Intro=%s", szIntroText) + 1;
   p += wsprintf(p, "URL=%s", szURL) + 1;

   if ( uAutoExtract ) p += wsprintf(p, "AutoExtract=%u", uAutoExtract) + 1;
   if ( bOpenFolder ) p += wsprintf(p, "OpenFolder=1" ) + 1;
   if ( bDeleteFiles ) p += wsprintf(p, "Delete=1" ) + 1;
   if ( bNoGUI ) p += wsprintf(p, "NoGUI=1" ) + 1;
   //if ( isDebug ) lstrcat( szINIFileContents, "Debug=1\n" );          // not used yet.

   for ( q = szShortcut ; *q ; q += lstrlen(q) + 1 )
      p += wsprintf( p, "%s", q ) + 1;
   iINIFileSize = ( int ) ( p - IoBuffer );

   //
   // Write the metadata out
   //
   WriteFile( hFEHeaderOut, IoBuffer, iINIFileSize, &dwDummy, NULL );


   //
   // Append the zip file to the SFX
   //
   while ( ReadFile( hSourceZip, IoBuffer, sizeof IoBuffer, &dwBytesRead, NULL ) && dwBytesRead != 0 )
   {
      WriteFile( hFEHeaderOut, IoBuffer, dwBytesRead, &dwBytesWritten, NULL );
   }

   //
   // Clean up
   //
   SetStatus( "Finishing up ..." );

   CloseHandle( hFEHeaderOut );
   CloseHandle( hSourceZip );

   Sleep( 300 );


#ifndef _MAKESFX_
   PostMessage( hwndMain, WM_COMMAND, IDC_NEXT, 0 );
#endif // _MAKESFX_
   return 0;
}

#endif // _HEADER_
#endif // _GETVERSION_




/*

   CleanPath
 
   Remove all instances of a double slash.

*/
void CleanPath( char *szPath )
{
   // For UNC paths, intentionally overlook the first double slash.
   char *q = szPath[ 0 ] == '\\' && szPath[ 1 ] == '\\' ? szPath + 1 : szPath;
   char *p = q;
   while ( (*p = *q++) != '\0' )
      if ( *p != '\\' || *q != '\\' ) ++p;
}




/*
 
   ExeType
   
   Determines the executable type (32 bit PE or other)
   
 
   return values:
      1 = Win32 PE
      0 = 16 bit exe, or unknown
 
*/
int ExeType( LPSTR filename )
{
   int type = 0;
   PIMAGE_DOS_HEADER lpFileBase = NULL;
   HANDLE hFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
   if ( hFile != INVALID_HANDLE_VALUE )
   {
      HANDLE hFileMapping = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
      if ( hFileMapping )
      {
         lpFileBase = ( PIMAGE_DOS_HEADER ) MapViewOfFile( hFileMapping, FILE_MAP_READ, 0, 0, 0 );
         CloseHandle( hFileMapping );
      }
      CloseHandle( hFile );
   }
   if ( lpFileBase )
   {
      if ( lpFileBase->e_magic == IMAGE_DOS_SIGNATURE )
      {
         PIMAGE_NT_HEADERS pNTHeader = ( PIMAGE_NT_HEADERS ) ( ( DWORD_PTR ) lpFileBase + lpFileBase->e_lfanew );
         if ( !IsBadReadPtr( pNTHeader, sizeof( IMAGE_NT_HEADERS ) ) && pNTHeader->Signature == IMAGE_NT_SIGNATURE )
         {
            type = 1;
         }
      }
      UnmapViewOfFile(lpFileBase);
   }
   return type;
}
