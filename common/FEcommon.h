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

#define VERSION            "v1.48"
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

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif


#define OpenExplorerFolder(TheDir)  ShellExecute( NULL, "explore", TheDir, NULL, NULL, SW_SHOW )

#define DLGITEM_SETFONT(hDlg, ControlID) \
{ \
   static HFONT hFont = NULL; \
   FormatText( &hFont, GetDlgItem( hDlg, ControlID ), szActiveFont, iFontSize, FALSE ); \
}

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
char szIconPath[ MAX_PATH ] = "";
char szExtractionPath[ MAX_PATH ] = "";
char szPackageName[ 255 ] = "";
char szExecuteCommand[ MAX_PATH ] = "";
char szURL[ 255 ] = "";
char szConfirmMessage[ 1024 ] = "";
char szIntroText[ 1024 ];
char szINIPath[ MAX_PATH ] = "";
char szShortcut[ 10240 ] = "";
char szTargetDirectory[ MAX_PATH ];

int iCurrentPage = 1;

BOOL bRunElevated = FALSE;
BOOL bSubsystem64 = FALSE;
BOOL bAutoExtract = FALSE;
BOOL bOpenFolder = FALSE;
BOOL bDeleteFiles = FALSE;
BOOL bNoGUI = FALSE;
BOOL bChangeIcon = FALSE;
BOOL isDebug = FALSE;

char szActiveFont[ LF_FACESIZE ];
int const iFontSize = 13;


/*
 
   Prototypes (common)
 
*/
#ifndef _CONSOLE
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow );
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
   SetWindowPos( hwndStatic, HWND_TOP, 44, 70, 0, 0, SWP_NOSIZE );
   SetFocus( hwndStatic );
}
#endif




#ifndef _CONSOLE
/*
 
   FormatControl
 
   Formats a static text control that needs a white background (e.g. the splash screen)
 
*/
INT_PTR FormatControl( HFONT *phf, HWND handle, HDC hDC, LONG lFontWeight, LONG lFontHeight, COLORREF FontColor, LPTSTR szFontFace, BOOL Underline, int iBackgroundMode, int StockObject )
{
   LOGFONT lf;
   if ( ( *phf == NULL ) && GetObject( GetWindowFont( handle ), sizeof lf, &lf ) )
   {
      lf.lfWeight = lFontWeight;
      lf.lfHeight = lFontHeight;
      lf.lfUnderline = Underline;
      lstrcpy( lf.lfFaceName, szFontFace );
      *phf = CreateFontIndirect( &lf );
   }
   SelectObject( hDC, *phf );
   SetBkMode( hDC, iBackgroundMode );
   SetTextColor( hDC, FontColor );
   return (INT_PTR) GetStockObject( StockObject );
}


/*
 
   FormatText
 
   Sets the font and size of a static text control.
 
*/
void FormatText( HFONT *phf, HWND handle, LPTSTR szFontName, int iFontSize, BOOL bIsBold )
{
   LOGFONT lf;
   if ( ( *phf == NULL ) && GetObject( GetWindowFont( handle ), sizeof lf, &lf ) )
   {
      lf.lfWeight = FW_REGULAR;
      lf.lfHeight = ( LONG ) iFontSize;
      if ( bIsBold ) lf.lfWeight = FW_BOLD;
      lstrcpy( lf.lfFaceName, szFontName );
      *phf = CreateFontIndirect( &lf );
   }
   if ( *phf )
   {
      SendMessage( handle, WM_SETFONT, ( WPARAM ) *phf, TRUE );
   }
}


#endif // _CONSOLE





/*
 
  IsSpace
 
*/
BOOL IsSpace( int in )
{
   if ( in == 0x20 || in >= 0x09 && in <= 0x0D ) return TRUE;
   return FALSE;
}



#ifndef _CONSOLE
void __cdecl WinMainCRTStartup( void )
{
   int mainret;
   char *lpszCommandLine;
   STARTUPINFO StartupInfo;

   lpszCommandLine = GetCommandLine();

   if ( *lpszCommandLine == '"' )
   {
      do ++lpszCommandLine; while ( *lpszCommandLine && ( *lpszCommandLine != '"' ) );
      if ( *lpszCommandLine == '"' ) lpszCommandLine++;
   }
   else while ( *lpszCommandLine > ' ' ) lpszCommandLine++;

   while ( *lpszCommandLine && ( *lpszCommandLine <= ' ' ) ) lpszCommandLine++;

   StartupInfo.dwFlags = 0;
   GetStartupInfo( &StartupInfo );

   mainret = WinMain( GetModuleHandle( NULL ), NULL, lpszCommandLine,
                      StartupInfo.dwFlags & STARTF_USESHOWWINDOW ? StartupInfo.wShowWindow : SW_SHOWDEFAULT );

   ExitProcess( mainret );
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
BOOL IsExtension(char *exec, char *exten)
{
   char *p = exten + lstrlen(exten);
   char *q = *exec == '"' ? lstrstr( exec + 1, "\"" ) : exec + lstrlen(exec);
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
      int l = MAX_PATH;
      int t = REG_SZ;
      RegQueryValueEx( hKey, name, NULL, &t, out, &l );
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
void ParsePath( char *input )
{
   int tokennum = 0;
   char *delimiter;

   char szExpandedInput[ MAX_PATH ];
   char szCurToken[ MAX_PATH ];
   char output[ MAX_PATH ];

   *szExpandedInput = *szCurToken = *output = '\0';

   //
   // Replace all occurances of a dollar sign with a percent sign
   //
   while ( ( delimiter = lstrstr( input, "$" ) ) != NULL ) *delimiter = '%';

   ExpandEnvironmentStrings( input, szExpandedInput, MAX_PATH );

   do
   {
      //
      // Get the next token (delimited by a '\')
      //
      delimiter = gettoken( szExpandedInput, "\\", tokennum++, szCurToken );

      //
      // Start Menu (for this user)
      //
      if ( !lstrcmpi( szCurToken, "%startmenu%" ) )
      {
         queryShellFolders( "Programs", output );
      }

      //
      // Desktop
      //
      else if ( !lstrcmpi( szCurToken, "%desktop%" ) )
      {
         queryShellFolders( "Desktop", output );
      }

      //
      // Sendto Directory
      //
      else if ( !lstrcmpi( szCurToken, "%sendto%" ) )
      {
         queryShellFolders( "SendTo", output );
      }

      //
      // Favorites
      //
      else if ( !lstrcmpi( szCurToken, "%favorites%" ) )
      {
         queryShellFolders( "Favorites", output );
      }

      //
      // Start Up Folder in the Start Menu for this user
      //
      else if ( !lstrcmpi( szCurToken, "%startup%" ) )
      {
         queryShellFolders( "Startup", output );
      }

      //
      // Output directory
      //
      else if ( !lstrcmpi( szCurToken, "%targetdir%" ) )
      {
         lstrcpy( output, szTargetDirectory );
      }

      //
      // Current Directory
      //
      else if ( !lstrcmpi( szCurToken, "%curdir%" ) )
      {
         GetCurrentDirectory( MAX_PATH, output );
      }

      //
      // Quicklaunch directory: If the OS doesn't support
      // it, this defaults to the temp dir.
      //
      else if ( !lstrcmpi( szCurToken, "%quicklaunch%" ) )
      {
         queryShellFolders( "AppData", output );

         if ( lstrlen( output ) > 0 )
         {
            lstrcat( output, "\\Microsoft\\Internet Explorer\\Quick Launch" );
         }

         if ( !DirectoryExists( output ) )
         {
            GetTempPath( MAX_PATH, output );
         }
      }

      //
      // Program Files Directory
      //
      else if ( !lstrcmpi( szCurToken, "%programfiles%" ) )
      {
         HKEY hKey;

         lstrcpy( output, "C:\\Program Files" );
         if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
         {
            int l = sizeof( output );
            int t = REG_SZ;
            RegQueryValueEx( hKey, "ProgramFilesDir", NULL, &t, output, &l );
            RegCloseKey( hKey );
         }
      }

      //
      // System Directory
      //
      else if ( !lstrcmpi( szCurToken, "%sysdir%" ) )
      {
         GetSystemDirectory( output, MAX_PATH );
      }

      //
      // The token isn't a variable, so append it.
      //
      else
      {
         lstrcat( output, szCurToken );
      }
      lstrcat( output, delimiter );
   } while ( *delimiter );

   lstrcpy( input, output );
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
   LPVOID IoBuffer;

   char szINIFileContents[ 16384 ];
   int iINIFileSize;
   int iZipFileSize;
   int i = 0;
   DWORD dwDummy, dwBytesRead, dwBytesWritten;

   char *p, *q;
   HRSRC const hICON = FindResource(NULL, "setup.ico", RT_RCDATA);
   DWORD const cchICON = SizeofResource(NULL, hICON);
   char* const pchICON = (char*)LoadResource(NULL, hICON);
   HRSRC const hSTUB = FindResource(NULL, bSubsystem64 ?
      (bRunElevated ? "header64_elevated.exe" : "header64.exe") :
      (bRunElevated ? "header32_elevated.exe" : "header32.exe"),
      RT_RCDATA);
   DWORD const cchSTUB = SizeofResource(NULL, hSTUB);
   char* const pchSTUB = (char*)memcpy(_alloca(cchSTUB), LoadResource(NULL, hSTUB), cchSTUB);

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
      if ( ( szIntroText[ i ] == 0x0D ) && ( szIntroText[ i + 1 ] == 0x0A ) )
      {
         szIntroText[ i ] = '\\';
         szIntroText[ i + 1 ] = 'n';
      }
      i++;
   }

   iZipFileSize = GetFileSize( hSourceZip, NULL );


   //
   // Build the metadata string
   //
   p = szINIFileContents;
   p += wsprintf(p, "Name=%s", szPackageName) + 1;
   p += wsprintf(p, "ZipSize=%d", iZipFileSize) + 1;
   p += wsprintf(p, "Exec=%s", szExecuteCommand) + 1;
   p += wsprintf(p, "DefaultPath=%s", szExtractionPath) + 1;
   p += wsprintf(p, "Intro=%s", szIntroText) + 1;
   p += wsprintf(p, "URL=%s", szURL) + 1;

   if ( bAutoExtract ) p += wsprintf(p, "AutoExtract=1" ) + 1;
   if ( bOpenFolder ) p += wsprintf(p, "OpenFolder=1" ) + 1;
   if ( bDeleteFiles ) p += wsprintf(p, "Delete=1" ) + 1;
   if ( bNoGUI ) p += wsprintf(p, "NoGUI=1" ) + 1;
   //if ( isDebug ) lstrcat( szINIFileContents, "Debug=1\n" );          // not used yet.

   for ( q = szShortcut ; *q ; q += lstrlen(q) + 1 )
      p += wsprintf( p, "%s", q ) + 1;
   iINIFileSize = ( int ) ( p - szINIFileContents );

   //
   // Write the metadata out
   //
   WriteFile( hFEHeaderOut, szINIFileContents, iINIFileSize, &dwDummy, NULL );


   //
   // Append the zip file to the SFX
   //
   IoBuffer = VirtualAlloc( NULL, BUFFER_SIZE, MEM_COMMIT, PAGE_READWRITE );

   for ( NULL; iZipFileSize > 0; iZipFileSize = iZipFileSize - BUFFER_SIZE )
   {
      ReadFile ( hSourceZip, IoBuffer, BUFFER_SIZE, &dwBytesRead, NULL );
      WriteFile( hFEHeaderOut, IoBuffer, dwBytesRead, &dwBytesWritten, NULL );

      if ( dwBytesWritten = 0 ) break;
   }

   //
   // Clean up
   //
   SetStatus( "Finishing up ..." );

   VirtualFree( IoBuffer, 0, MEM_RELEASE );
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
