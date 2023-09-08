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


#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <process.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <olectl.h>

#ifdef _WIZARD_
#define INCLUDE_HEADER
#ifndef NO_HTML_HELP
#include <Htmlhelp.h>
#endif
#endif


#ifdef _MAKESFX_
#define INCLUDE_HEADER
#endif // _MAKESFX_


#ifdef _CONSOLE
#include <stdio.h>
#include <conio.h>
#endif



#define _TEXT_BLUE_        0x00AA0000
#define _TEXT_BLACK_       0x00000000
#define _TEXT_GRAY_        0x00BBBBBB

#define _CRITICAL_         MB_ICONSTOP

#define VERSION            "v1.45"
#define VERSIONDATE        VERSION" ("__DATE__")"
#define WEBSITE_URL        "http://www.disoriented.com"
#define CASESENSITIVITY    0
#define BUFFER_SIZE        16384

#define DLG_SCALE_X        8
#define DLG_SCALE_Y        16

#define DLG_X_SCALE(pixels)      (pixels * LOWORD(GetDialogBaseUnits())) / DLG_SCALE_X
#define DLG_Y_SCALE(pixels)      (pixels * HIWORD(GetDialogBaseUnits())) / DLG_SCALE_Y


#ifdef _DEBUG
#define DISABLE_PAYLOAD_CHECK
#endif

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
HWND hwndMain, hwndStatic;

HANDLE hFile;
HANDLE hExtractThread;

HINSTANCE ghInstance;

char szZipFileName[ MAX_PATH ] = "";
char szEXEOutPath[ MAX_PATH ] = "";
char szIconPath[ MAX_PATH ] = "";
char szTempDir[ MAX_PATH ] = "";
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
int iFontSize;


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
   if ( hwndStatic != NULL ) DestroyWindow( hwndStatic );
   if ( hwndMain != NULL ) DestroyWindow( hwndMain );
   if ( hFile != NULL ) CloseHandle( hFile );

#ifdef _WIZARD_
   if ( hExtractThread ) CloseHandle ( hExtractThread );
#endif // _WIZARD_

#endif // _CONSOLE

   ExitProcess( 1 );
}


/*
 
   RaiseError
 
   Called when there's an error.
 
*/
void RaiseError( LPTSTR szTheErrorText )
{
   char szErrorBase[ 1024 ];
   LPTSTR szWinError = "";

   if ( GetLastError() != ERROR_SUCCESS )
   {
      FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
         NULL,
         GetLastError(),
         MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
         ( LPTSTR ) & szWinError,
         0,
         NULL );
   }
   wsprintf( szErrorBase, "An error prevents this program from continuing:\n\n %s %s", szTheErrorText, szWinError );

#ifdef _CONSOLE
   WriteConsoleOut( "ERROR: %s\n", szErrorBase );
#else
   MessageBox( NULL, szErrorBase, "FreeExtractor Error", _CRITICAL_ );
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
   hwndStatic = CreateDialog( ghInstance, MAKEINTRESOURCE( Resource ), hwndMain, ( DLGPROC ) ChildDialogProc );
   SetWindowPos( hwndStatic, HWND_TOP, 44, 70, 0, 0, SWP_NOSIZE );

   return ;
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
   HDC hDC = GetDC( handle );
   if ( hDC )
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
         SetBkMode( hDC, OPAQUE );     //TRANSPARENT
         SendMessage( handle, WM_SETFONT, ( WPARAM ) *phf, TRUE );
      }
      ReleaseDC( handle, hDC );
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
   strnsub

   Replaces a substring in pszString

*/
char *strnsub( char *pszString, char *pszPattern, char *pszReplacement, int iMaxLength )
{
   char * pszSubstring, *pszTmpSubstring ;

   int iPatternLength, iReplacementLength ;

   pszTmpSubstring = pszSubstring = pszString ;
   iPatternLength = lstrlen( pszPattern ) ;
   iReplacementLength = lstrlen( pszReplacement ) ;

   if ( !lstrcmp( pszPattern, pszReplacement ) ) return 0;

   //
   // No match found
   //
   if ( NULL == ( pszSubstring = lstrstr( pszString, pszPattern ) ) ) return NULL;


   //
   // Is there enough space for replacement?
   //
   if ( ( int ) ( lstrlen( pszString ) + ( iReplacementLength - iPatternLength ) ) >= iMaxLength ) return NULL;


   //
   // Allocate memory
   //
   pszTmpSubstring = ( char * ) VirtualAlloc( NULL, iMaxLength * sizeof( char ), MEM_COMMIT, PAGE_READWRITE );


   //
   // Couldn't allocate memory
   //
   if ( pszTmpSubstring == NULL ) return NULL;

   //
   // Copy replacement
   //
   lstrcpy( pszTmpSubstring, pszSubstring + iPatternLength ) ;
   while ( iReplacementLength-- ) * pszSubstring++ = *pszReplacement++ ;
   lstrcpy( pszSubstring, pszTmpSubstring ) ;

   VirtualFree( pszTmpSubstring, 0, MEM_RELEASE );

   return ( pszSubstring - iPatternLength ) ;

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


char *lstrncat( char *front, char *back, size_t count )
{
   char * start = front;

   while ( *front++ );
   front--;

   while ( count-- ) if ( !( *front++ = *back++ ) ) return ( start );

   *front = '\0';
   return ( start );
}

/*
 
   lstrrev
 
   Reverses a string. Used to remove dependance on the C runtime.
 
*/
char *lstrrev( char *str )
{
   char * p1, *p2;

   if ( ! str || ! *str )
      return str;
   for ( p1 = str, p2 = str + lstrlen( str ) - 1; p2 > p1; ++p1, --p2 )
   {
      *p1 ^= *p2;
      *p2 ^= *p1;
      *p1 ^= *p2;
   }
   return str;
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
   // Remove leading and trailing quotes, if they exist
   strnsub( szPathAndFile, "\"", "", MAX_PATH );
   strnsub( szPathAndFile, "\"", "", MAX_PATH );

#ifdef _WIN32
   if ( GetFileAttributes( szPathAndFile ) == -1 ) return FALSE;
   return TRUE;
#else
   if ( fopen( szPathAndFile, "r" ) == NULL ) return FALSE;
   return TRUE;
#endif 
   return FALSE;
}

/*
 
   DirectoryExists
 
*/
BOOL DirectoryExists( LPTSTR szDirName )
{
   unsigned int nAttributes = 0;
   if ( ( nAttributes = GetFileAttributes( szDirName ) ) == -1 ) return FALSE;
   if ( ( nAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) return TRUE;
   return FALSE;
}

/*
 
   queryShellFolders
 
   Grabs this user's shell folder variables from the registry.
 
*/
#ifndef _GETVERSION_
void queryShellFolders( char *name, char *out )
{
   HKEY hKey;
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

   char szExpandedInput[ MAX_PATH ] = "";
   char szCurToken[ MAX_PATH ] = "";
   char output[ MAX_PATH ] = "";

   //
   // Replace all occurances of a dollar sign with a percent sign
   //
   while ( lstrstr( input, "$" ) )
      strnsub( input, "$", "%", MAX_PATH );

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
         char buf[ MAX_PATH ] = "";
         queryShellFolders( "Programs", buf );
         lstrcpy( output, buf );
      }

      //
      // Desktop
      //
      else if ( !lstrcmpi( szCurToken, "%desktop%" ) )
      {
         char buf[ MAX_PATH ];
         queryShellFolders( "Desktop", buf );
         lstrcpy( output, buf );
      }

      //
      // Sendto Directory
      //
      else if ( !lstrcmpi( szCurToken, "%sendto%" ) )
      {
         char buf[ MAX_PATH ];
         queryShellFolders( "SendTo", buf );
         lstrcpy( output, buf );
      }

      //
      // Favorites
      //
      else if ( !lstrcmpi( szCurToken, "%favorites%" ) )
      {
         char buf[ MAX_PATH ];
         queryShellFolders( "Favorites", buf );
         lstrcpy( output, buf );
      }

      //
      // Start Up Folder in the Start Menu for this user
      //
      else if ( !lstrcmpi( szCurToken, "%startup%" ) )
      {
         char buf[ MAX_PATH ];
         queryShellFolders( "Startup", buf );
         lstrcpy( output, buf );
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
         char buf[ MAX_PATH ];
         GetCurrentDirectory( MAX_PATH, buf );
         lstrcpy( output, buf );
      }

      //
      // Quicklaunch directory: If the OS doesn't support
      // it, this defaults to the temp dir.
      //
      else if ( !lstrcmpi( szCurToken, "%quicklaunch%" ) )
      {
         char buf[ MAX_PATH ] = "";
         queryShellFolders( "AppData", buf );

         if ( lstrlen( buf ) > 0 )
         {
            lstrcat( buf, "\\Microsoft\\Internet Explorer\\Quick Launch" );
         }

         if ( !DirectoryExists( buf ) )
         {
            GetTempPath( MAX_PATH, buf );
         }
         lstrcpy( output, buf );
      }

      //
      // Program Files Directory
      //
      else if ( !lstrcmpi( szCurToken, "%programfiles%" ) )
      {
         HKEY hKey;
         char buf[ MAX_PATH ];

         lstrcpy( buf, "C:\\Program Files" );
         if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
         {
            int l = sizeof( buf );
            int t = REG_SZ;
            RegQueryValueEx( hKey, "ProgramFilesDir", NULL, &t, buf, &l );
            RegCloseKey( hKey );
         }
         lstrcpy( output, buf );
      }

      //
      // System Directory
      //
      else if ( !lstrcmpi( szCurToken, "%sysdir%" ) )
      {
         char buf[ MAX_PATH ];
         GetSystemDirectory( buf, MAX_PATH );
         lstrcpy( output, buf );
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

   char szFullPath[ MAX_PATH + 3 ] = "";
   char szTempPath[ MAX_PATH + 3 ] = "";
   char szCurToken[ MAX_PATH + 3 ] = "";

   BOOL successful = TRUE;

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
void replace_data( char *hdr, int hdr_len, char *srch, int srchlen, char *filename )
{
   char temp[ 32 ];
   DWORD dummy;

   HANDLE fp = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   SetFilePointer( fp, 0, 0 , FILE_END );

   hdr_len -= srchlen - 32;
   while ( hdr_len >= 0 )
   {
      if ( !memcmp( hdr, srch + 32, srchlen - 32 ) ) break;
      hdr++;
      hdr_len--;
   }

   if ( hdr_len < 0 )
   {
      RaiseError( "Could not set new icon." );
      CloseHandle( fp );
      return ;
   }

   SetFilePointer( fp, 0, 0 , FILE_BEGIN );
   ReadFile( fp, temp, 32, &dummy, NULL );
   ReadFile( fp, srch + 32, srchlen - 32, &dummy, NULL );
   CloseHandle( fp );

   memcpy( hdr, srch + 32, srchlen - 32 );
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

   HRSRC const hICON = FindResource(NULL, "setup.ico", RT_RCDATA);
   char* const pchICON = (char*)LoadResource(NULL, hICON);
   DWORD const cchICON = SizeofResource(NULL, hICON);
   HRSRC const hSTUB = FindResource(NULL, bSubsystem64 ?
      (bRunElevated ? "header64_elevated.exe" : "header64.exe") :
      (bRunElevated ? "header32_elevated.exe" : "header32.exe"),
      RT_RCDATA);
   char* const pchSTUB = (char*)LoadResource(NULL, hSTUB);
   DWORD const cchSTUB = SizeofResource(NULL, hSTUB);

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
   wsprintf( szINIFileContents,
             "[FE]\n"
             "Name=%s\n"
             "ZipSize=%d\n"
             "Exec=\"%s\"\n"
             "DefaultPath=%s\n"
             "Intro=%s\n"
             "URL=%s\n",
             szPackageName, iZipFileSize,
             szExecuteCommand, szExtractionPath,
             szIntroText, szURL );

   if ( bAutoExtract ) lstrcat( szINIFileContents, "AutoExtract=1\n" );
   if ( bOpenFolder ) lstrcat( szINIFileContents, "OpenFolder=1\n" );
   if ( bDeleteFiles ) lstrcat( szINIFileContents, "Delete=1\n" );
   if ( bNoGUI ) lstrcat( szINIFileContents, "NoGUI=1\n" );
   //if ( isDebug ) lstrcat( szINIFileContents, "Debug=1\n" );          // not used yet.
   lstrcat( szINIFileContents, szShortcut );
   iINIFileSize = lstrlen( szINIFileContents );


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
   BOOL bIsUNCPath = FALSE;
   int offset = 0;
   char * szTemp = szPath;

   //
   // Handle UNC paths.
   //
   if ( szPath[ 0 ] == 0x5C && szPath[ 1 ] == 0x5C ) bIsUNCPath = TRUE;

   //
   // Replaces all occurances of a double slash with a single.
   //
   while ( lstrstr( szPath, "\\\\" ) )
   {
      strnsub( szPath, "\\\\", "\\", MAX_PATH );
   }

   //
   // If it was a UNC path, replace the double splash back
   //
   if ( bIsUNCPath )
   {
      strnsub( szPath, "\\", "\\\\", MAX_PATH );
   }
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
