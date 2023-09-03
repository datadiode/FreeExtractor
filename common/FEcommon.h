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



#ifdef INCLUDE_HEADER
#include "..\..\common\stub.h"
#include "..\..\common\icon.h"
#endif // INCLUDE_HEADER



#define _TEXT_BLUE_        0x00AA0000
#define _TEXT_BLACK_       0x00000000
#define _TEXT_GRAY_        0x00BBBBBB

#define _CRITICAL_         MB_OK | MB_ICONSTOP

#define VERSION            "v1.44"
#define VERSIONDATE        VERSION" ("__DATE__")"
#define WEBSITE_URL        "http://www.disoriented.com"
#define CASESENSITIVITY    0
#define BUFFER_SIZE        65535

#define DLG_SCALE_X        8
#define DLG_SCALE_Y        16

#define DLG_X_SCALE(pixels)      (pixels * LOWORD(GetDialogBaseUnits())) / DLG_SCALE_X
#define DLG_Y_SCALE(pixels)      (pixels * HIWORD(GetDialogBaseUnits())) / DLG_SCALE_Y


#ifdef _DEBUG
#define DISABLE_PAYLOAD_CHECK
#endif

#ifdef _CONSOLE
#define SetStatus(TheText)          WriteConsoleOut("%s\n", TheText);
#else
#define SetBannerText(TheText)      SetDlgItemText(hwndMain, IDC_BANNER, TheText);
#define SetSubBannerText(TheText)   SetDlgItemText(hwndMain, IDC_SUBBANNER, TheText);
#define SetTitle(TheTitle)          SetWindowText(hwndMain, TheTitle);
#define SetStatus(TheText)          SetDlgItemText(hwndStatic, IDC_PROGRESS, TheText);
#endif

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif


#define CleanupPath(Path)           lstrcpy(Path, CleanPath(Path) );
#define OpenExplorerFolder(TheDir)  ShellExecute( NULL, "explore", TheDir, NULL, NULL, SW_SHOW | SW_SHOWNORMAL );
#define IsExtension(exec, exten)    !lstrcmpCI(exten, right(exec, 3))

#define MAIN_SETFONT(ControlID)    FormatText( GetDlgItem( hwndMain, ControlID), szActiveFont, iFontSize, FALSE);
#define CHILD_SETFONT(ControlID)   FormatText( GetDlgItem( hwndStatic, ControlID), szActiveFont, iFontSize, FALSE);



/*
 
   Global Variables (common)
 
*/
HWND hwndMain, hwndStatic;

HANDLE hFile;
HANDLE h_ExtractThread;

HINSTANCE ghInstance;

char szZipFileName[ MAX_PATH ] = "",
                                 szEXEOutPath[ MAX_PATH ] = "",
                                                            szIconPath[ MAX_PATH ] = "",
                                                                                     szTempDir[ MAX_PATH ] = "",
                                                                                                             szExtractionPath[ MAX_PATH ] = "",
                                                                                                                                            szPackageName[ 255 ] = "",
                                                                                                                                                                   szExecuteCommand[ MAX_PATH ] = "",
                                                                                                                                                                                                  szURL[ 255 ] = "",
                                                                                                                                                                                                                 szConfirmMessage[ 1024 ] = "",
                                                                                                                                                                                                                                            szAuthor[ 255 ] = "",
                                                                                                                                                                                                                                                              szIntroText[ 1024 ],
                                                                                                                                                                                                                                                              szINIPath[ MAX_PATH ] = "",
                                                                                                                                                                                                                                                                                      szShortcut[ 10240 ] = "",
                                                                                                                                                                                                                                                                                                            szTargetDirectory[ MAX_PATH ];

int iZipFileSize,
iCurrentPage = 1;

BOOL bAutoExtract = FALSE,
                    bOpenFolder = FALSE,
                                  bDeleteFiles = FALSE,
                                                 bNoGUI = FALSE,
                                                          bChangeIcon = FALSE,
                                                                        isDebug = FALSE;

LPVOID IoBuffer;

char szActiveFont[ 255 ] = "";
int iFontSize;


/*
 
   Prototypes (common)
 
*/
#ifndef _CONSOLE
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow );
BOOL CALLBACK MainDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK ChildDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
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
   if ( h_ExtractThread ) CloseHandle ( h_ExtractThread );
#endif // _WIZARD_

#endif // _CONSOLE

   ExitProcess( 1 );
}





/*
 
   WriteConsoleOut
 
   A printf replacement; helps removes dependancy on MSVCRT.dll for console apps
 
*/
#ifdef _CONSOLE
int WriteConsoleOut( const char * format, ... )
{
   char szBuff[ 8096 ];
   int retValue;
   DWORD cbWritten;
   va_list argptr;

   va_start( argptr, format );
   retValue = wvsprintf( szBuff, format, argptr );
   va_end( argptr );

   WriteFile( GetStdHandle( STD_OUTPUT_HANDLE ), szBuff, retValue, &cbWritten, 0 );

   return retValue;
}
#endif


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
 
   ShowAboutBox
 
   Shows the about box.
 
*/
#ifndef _CONSOLE
void ShowAboutBox()
{
   MessageBox( NULL,
               "FreeExtractor - "VERSIONDATE
               "\n"WEBSITE_URL
               "\n\nWritten by Andrew Fawcett (andrewfawcett@users.sourceforge.net).\n"
               , "About FreeExtractor", 0 );

}
#endif // _CONSOLE





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
int FormatControl( HWND handle, HDC hDC, LONG lFontWeight, LONG lFontHeight, COLORREF FontColor, LPTSTR szFontFace, BOOL Underline, int iBackgroundMode, int StockObject )
{
   LOGFONT lfTemp;
   static HFONT hfTemp;

   GetObject( GetWindowFont( handle ), sizeof( lfTemp ), &lfTemp );
   lfTemp.lfWeight = lFontWeight;
   lfTemp.lfHeight = lFontHeight;
   lfTemp.lfUnderline = Underline;

   lstrcpy( lfTemp.lfFaceName, szFontFace );

   hfTemp = CreateFontIndirect( &lfTemp );

   SelectObject( hDC, hfTemp );
   SetBkMode( hDC, iBackgroundMode );
   SetTextColor( hDC, FontColor );
   DeleteObject( hfTemp );

   return ( int ) GetStockObject( StockObject );
}


/*
 
   FormatText
 
   Sets the font and size of a static text control.
 
*/
void FormatText( HWND handle, LPTSTR szFontName, int iFontSize, BOOL bIsBold )
{
   LOGFONT lf = {0};

   HDC hDC = GetDC( handle );

   if ( hDC )
   {
      HFONT hfontNew;

      GetObject( GetWindowFont( handle ), sizeof( lf ), &lf );
      lf.lfWeight = FW_REGULAR;
      lf.lfHeight = ( LONG ) iFontSize;
      if ( bIsBold ) lf.lfWeight = FW_BOLD;
      lstrcpy( lf.lfFaceName, szFontName );

      hfontNew = CreateFontIndirect( &lf );

      if ( hfontNew )
      {
         SetBkMode( hDC, OPAQUE );     //TRANSPARENT
         SendMessage( handle, WM_SETFONT, ( WPARAM ) hfontNew, TRUE );
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
      while ( *lpszCommandLine && ( *lpszCommandLine != '"' ) ) lpszCommandLine++;
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
 
   left
 
   Port of the Visual Basic function of the same name.
 
*/
char *left( char *in, int size )
{
   int i;
   char *buf2 = ( char * ) VirtualAlloc( NULL, ( size * sizeof( char ) ) + 1, MEM_COMMIT, PAGE_READWRITE );

   for ( i = 0; i < size && in[ i ] != '\0' ; i++ ) buf2[ i ] = in[ i ];
   buf2[ i++ ] = '\0';

   return buf2;
}


/*
 
   right
 
   Port of the Visual Basic function of the same name.
 
*/
char *right( char *in, int size )
{
   char * p = in;

   int tl = lstrlen( in ) ;

   size = tl - size;

   while ( *p && size > 0 )
   {
      p++;
      size--;
   }

   return p;
}


/*
 
   instr ("In String")
 
   Port of the Visual Basic function of the same name.
 
*/
int instr( int offset, char *src, char *query )
{
   char * s1, *s2;

   int c;

   if ( !*query ) return 0;
   if ( offset > lstrlen( src ) ) return 0;
   if ( offset < 0 ) offset = 0;

   c = offset;

   while ( c > 0 )
   {
      src++;
      c--;
   }

   while ( *src )
   {
      s1 = src;
      s2 = ( char * ) query;

      while ( *s1 && *s2 && !( *s1 - *s2 ) ) s1++, s2++;

      if ( !*s2 ) break;
      c++;
      src++;
   }

   offset += c + 1;

   if ( !*src || offset < 0 ) return 0;

   return offset;
}


/*
 
   mid
 
   Port of the Visual Basic function of the same name.
 
*/
char *mid( char *c, int offset, int len )
{
   char * p = c;

   while ( offset - 1 > 0 )
   {
      *p++;
      --offset;
   }
   return left( p, len );
}




/*
 
   ltrim (Left Trim)
 
   Port of the Visual Basic function of the same name.
 
*/
char *ltrim( char *in )
{
   char * buf = in;

   for ( NULL; *buf && IsSpace( *buf ); buf++ );
   return buf;
}


/*
 
   rtrim
 
   Port of the Visual Basic function of the same name.
 
*/
char *rtrim( char *in )
{
   int i = lstrlen( in );

   if ( in ) while ( --i >= 0 ) if ( !IsSpace( in[ i ] ) ) break;

   return left( in, ++i );
}


/*
 
   trim
 
   Port of the Visual Basic function of the same name.
 
*/
char *trim( char *in )
{
   in = ltrim( in );
   return rtrim( in );
}




char *lstrstr( const char * str1, const char * str2 )
{
   char * cp = ( char * ) str1;
   char *s1, *s2;

   if ( !*str2 )
      return ( ( char * ) str1 );

   while ( *cp )
   {
      s1 = cp;
      s2 = ( char * ) str2;

      while ( *s1 && *s2 && !( *s1 - *s2 ) )
         s1++, s2++;

      if ( !*s2 )
         return ( cp );

      cp++;
   }

   return ( NULL );
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
   lstrcmpCI
 
   Case-insensitive string compare.
*/
int lstrcmpCI( const char* fileName1, const char* fileName2 )
{
   for ( ; ; )
   {
      char c1 = *( fileName1++ );
      char c2 = *( fileName2++ );
      if ( ( c1 >= 'a' ) && ( c1 <= 'z' ) ) c1 -= 0x20;
      if ( ( c2 >= 'a' ) && ( c2 <= 'z' ) ) c2 -= 0x20;
      if ( c1 == '\0' ) return ( ( c2 == '\0' ) ? 0 : -1 );
      if ( c2 == '\0' ) return 1;
      if ( c1 < c2 ) return -1;
      if ( c1 > c2 ) return 1;
   }
}


/*
 
   gettoken
 
   Retrieves the tokennum-th token in string in, delimited by delimiter.
 
*/
char *gettoken( char *in, char *delimiter, int tokennum )
{
   char * p = in;
   int curpos = 0;
   int endpos = 0;

   if ( tokennum > lstrlen( in ) ) return "";

   while ( tokennum > 0 )
   {
      curpos = instr( curpos, p, delimiter );
      if ( !curpos ) return "";
      --tokennum;
   }
   curpos++;

   endpos = instr( curpos, p, delimiter );

   if ( endpos <= 0 ) return "";

   return mid( in, curpos, endpos - curpos );
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
 
   GetFolderFromPath
 
   Given a path, extract the folder name (remove the filename, if any)
 
*/
void GetFolderFromPath( char *in, char *out )
{
   int slashpos = 0;

   lstrcpy( out, lstrrev( in ) );
   slashpos = instr( 0, out, "\\" );
   lstrcpy( out, lstrrev( out ) );
   lstrcpy( out, left( out, lstrlen( out ) - slashpos ) );
   lstrrev( in );
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
BOOL DoesFontExist ( LPTSTR szName )
{
   char buf[ MAX_PATH ] = "";

   queryShellFolders( "Fonts", buf );
   lstrcat( buf, "\\" );
   lstrcat( buf, szName );
   lstrcat( buf, ".ttf" );

   return FileExists( buf );
}
#endif // _GETVERSION_


/*
 
  ParsePath
 
  Expands path variables in a path string
 
*/
#ifdef _HEADER_
char *ParsePath( char *input )
{
   int iToken = 0, i = 0;

   BOOL bIsFolder = TRUE;

   char szExpandedInput[ MAX_PATH ] = "",
                                      szCurToken[ MAX_PATH ] = "",
                                                               szTempDir[ MAX_PATH ] = "",
                                                                                       szTempINIFile[ MAX_PATH ] = "",
                                                                                                                   szExpandDefaultPath[ MAX_PATH ] = "";

   char *output = ( char * ) VirtualAlloc( NULL, ( MAX_PATH * sizeof( char ) ) + 1, MEM_COMMIT, PAGE_READWRITE );

   if ( left( input, 1 ) == "\\" )
   {
      bIsFolder = TRUE;
   }
   else
   {
      bIsFolder = FALSE;
      lstrcat( input, "\\" );
   }

   //
   // Replace all occurances of a dollar sign with a percent sign
   //
   while ( instr( 0, input, "$" ) )
      strnsub( input, "$", "%", MAX_PATH );

   ExpandEnvironmentStrings( ( LPCTSTR ) input, ( LPTSTR ) szExpandedInput, MAX_PATH );

   while ( TRUE )
   {
      //
      // Get the next token (delimited by a '\')
      //
      lstrcpy( szCurToken, gettoken( szExpandedInput, "\\", iToken ) );

      if ( lstrcmpCI( szCurToken, "" ) != 0 )
      {
         //
         // Start Menu (for this user)
         //
         if ( !lstrcmpCI( szCurToken, "%startmenu%" ) )
         {
            char buf[ MAX_PATH ] = "";
            queryShellFolders( "Programs", buf );
            lstrcpy( output, buf );
         }

         //
         // Desktop
         //
         else if ( !lstrcmpCI( szCurToken, "%desktop%" ) )
         {
            char buf[ MAX_PATH ];
            queryShellFolders( "Desktop", buf );
            lstrcpy( output, buf );
         }

         //
         // Sendto Directory
         //
         else if ( !lstrcmpCI( szCurToken, "%sendto%" ) )
         {
            char buf[ MAX_PATH ];
            queryShellFolders( "SendTo", buf );
            lstrcpy( output, buf );
         }

         //
         // Favorites
         //
         else if ( !lstrcmpCI( szCurToken, "%favorites%" ) )
         {
            char buf[ MAX_PATH ];
            queryShellFolders( "Favorites", buf );
            lstrcpy( output, buf );
         }

         //
         // Start Up Folder in the Start Menu for this user
         //
         else if ( !lstrcmpCI( szCurToken, "%startup%" ) )
         {
            char buf[ MAX_PATH ];
            queryShellFolders( "Startup", buf );
            lstrcpy( output, buf );
         }

         //
         // Output directory
         //
         else if ( !lstrcmpCI( szCurToken, "%targetdir%" ) )
         {
            lstrcpy( output, szTargetDirectory );
         }

         //
         // Current Directory
         //
         else if ( !lstrcmpCI( szCurToken, "%curdir%" ) )
         {
            char buf[ MAX_PATH ];
            GetCurrentDirectory( MAX_PATH, buf );
            lstrcpy( output, buf );
         }

         //
         // Quicklaunch directory: If the OS doesn't support
         // it, this defaults to the temp dir.
         //
         else if ( !lstrcmpCI( szCurToken, "%quicklaunch%" ) )
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
         else if ( !lstrcmpCI( szCurToken, "%programfiles%" ) )
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
         else if ( !lstrcmpCI( szCurToken, "%sysdir%" ) )
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
         lstrcat( output, "\\" );
      }
      else break;

      iToken++;
   }

   if ( !bIsFolder )
   {
      lstrcpy( output, left( output, lstrlen( output ) - 1 ) );
   }

   return output;
}
#endif //_HEADER_



/*
 
  CreateDirectoryRecursively
 
   Creates a directory recursively. If parent folders do not already exist, they are created.
 
*/
BOOL CreateDirectoryRecursively( LPTSTR szPath )
{
   char szFullPath[ MAX_PATH + 3 ] = "";
   char szTempPath[ MAX_PATH + 3 ] = "";
   char *curtoken;
   int tokennum = 0;
   BOOL successful;

   GetFullPathName( szPath, MAX_PATH, szFullPath, NULL );
   lstrcat( szFullPath, "\\" );

   while ( TRUE )
   {
      curtoken = gettoken( szFullPath, "\\", tokennum );
      if ( !lstrlen( curtoken ) ) break;

      if ( lstrlen ( szTempPath ) == 0 )
      {
         lstrcpy( szTempPath, curtoken );
         lstrcat( szTempPath, "\\" );
      }
      else
      {
         lstrcat( szTempPath, curtoken );
         lstrcat( szTempPath, "\\" );
      }

      GetFullPathName( szTempPath, MAX_PATH, szTempPath, NULL );

      if ( !DirectoryExists( szTempPath ) ) successful = CreateDirectory( szTempPath, NULL );

      tokennum++;
   }

   return successful;
}

void *memcpy ( void * dst, const void * src, size_t count )
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

int memcmp ( const void * buf1, const void * buf2, size_t count )
{
   if ( !count ) return ( 0 );

   while ( --count && *( char * ) buf1 == *( char * ) buf2 )
   {
      buf1 = ( char * ) buf1 + 1;
      buf2 = ( char * ) buf2 + 1;
   }

   return ( *( ( unsigned char * ) buf1 ) - *( ( unsigned char * ) buf2 ) );
}


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
void Build()
{
   HANDLE hFEHeaderOut, hSourceZip;
   //   HKEY hKey;

   char szINIFileContents[ 4096 ], szAuthor[ 64 ];
   int iINIFileSize, i = 0;
   DWORD dwDummy, dwBytesRead, dwBytesWritten;


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
      replace_data( ( char* ) STUB, sizeof( STUB ), ( char* ) ICON, sizeof( ICON ), szIconPath );
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

   WriteFile( hFEHeaderOut, STUB, sizeof( STUB ), &dwDummy, NULL );


   //
   // Build and append the settings to this SFX.
   //
   SetStatus( "Saving archive settings..." );

   //
   // Replace return carriages in the intro text.
   //
   while ( szIntroText[ i ] != '\0' )
   {
      if ( ( ( char ) szIntroText[ i ] == 0x0D ) && ( ( char ) szIntroText[ i + 1 ] == 0x0A ) )
      {
         szIntroText[ i ] = '\\';
         szIntroText[ i + 1 ] = 'n';
      }
      i++;
   }

   //
   // Embed the author name. Read it from the registry.
   //
   /*
      if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
      {
         int l = MAX_PATH;
         int t = REG_SZ;
         RegQueryValueEx( hKey, "RegisteredOwner", NULL, &t, szAuthor, &l );
         RegCloseKey( hKey );
      }
    
      if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
      {
         int l = MAX_PATH;
         int t = REG_SZ;
         RegQueryValueEx( hKey, "RegisteredOwner", NULL, &t, szAuthor, &l );
         RegCloseKey( hKey );
      }
   */
   iZipFileSize = GetFileSize( hSourceZip, NULL );


   //
   // Build the metadata string
   //
   wsprintf( szINIFileContents,
             "[FE]\n"
             "Name=%s\n"
             "ZipSize=%d\n"
             "Exec=%s\n"
             "DefaultPath=%s\n"
             "Intro=%s\n"
             "URL=%s\n"
             "Author=%s\n",
             szPackageName, iZipFileSize,
             szExecuteCommand, szExtractionPath,
             szIntroText, szURL, szAuthor );

   if ( bAutoExtract ) lstrcat( szINIFileContents, "AutoExtract=1\n" );
   if ( bOpenFolder ) lstrcat( szINIFileContents, "OpenFolder=1\n" );
   if ( bDeleteFiles ) lstrcat( szINIFileContents, "Delete=1\n" );
   if ( bNoGUI ) lstrcat( szINIFileContents, "NoGUI=1\n" );
   //if ( isDebug ) lstrcat( szINIFileContents, "Debug=1\n" );          // not used yet.
   if ( lstrlen( szShortcut ) > 0 ) lstrcat( szINIFileContents, szShortcut );
   iINIFileSize = lstrlen( szINIFileContents );


   //
   // Write the metadata out
   //
   WriteFile( hFEHeaderOut, ( LPVOID ) szINIFileContents, iINIFileSize, &dwDummy, NULL );


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
}

#endif // _HEADER_
#endif // _GETVERSION_




/*
   CleanPath
 
   Remove all instances of a double slash.
*/
char *CleanPath( char *szPath )
{
   BOOL bIsUNCPath = FALSE;
   int offset = 0;
   char * szTemp = szPath;

   //
   // Handle UNC paths.
   //
   if ( szPath[ 0 ] == szPath[ 1 ] == 0x5C ) bIsUNCPath = TRUE;

   //
   // Replaces all occurances of a double slash with a single.
   //
   while ( instr( 0, szPath, "\\\\" ) )
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

   return szPath;
}




#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr) + (addValue) )



/*
 
   ExeType
   
   Determines the executable type (32 bit PE or other)
   
 
   return values:
      1 = Win32 PE
      0 = 16 bit exe, or unknown
 
*/
int ExeType( LPSTR filename )
{
   HANDLE hFile;
   HANDLE hFileMapping;
   LPVOID lpFileBase;
   PIMAGE_DOS_HEADER dosHeader;

   hFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

   if ( hFile == INVALID_HANDLE_VALUE ) return 0;

   hFileMapping = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );

   if ( hFileMapping == 0 )
   {
      CloseHandle( hFile );
      return 0;
   }

   lpFileBase = MapViewOfFile( hFileMapping, FILE_MAP_READ, 0, 0, 0 );

   if ( lpFileBase == 0 )
   {
      CloseHandle( hFileMapping );
      CloseHandle( hFile );
      return 0;
   }

   dosHeader = ( PIMAGE_DOS_HEADER ) lpFileBase;

   if ( dosHeader->e_magic == IMAGE_DOS_SIGNATURE )
   {
      PIMAGE_NT_HEADERS pNTHeader;
      DWORD base = ( DWORD ) dosHeader;

      pNTHeader = MakePtr( PIMAGE_NT_HEADERS, dosHeader, dosHeader->e_lfanew );

      if ( IsBadReadPtr( pNTHeader, sizeof( IMAGE_NT_HEADERS ) ) || pNTHeader->Signature != IMAGE_NT_SIGNATURE )
      {
         return 0;
      }
      else return 1;
   }

   return 0;
}

