/*

   FEWizard.c

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

#define _FEWIZARD_

#include "FEWizard.h"

/*

   Windows version numbers to choose from to set a minimum requirement to run
   the SFX, as listed on https://www.gaijin.at/de/infos/windows-versionsnummern

*/
static const WORD rgTargetVer[] =
{
   0x0400, // 95
   0x0410, // 98
   0x0490, // ME
   0x0500, // 2000
   0x0501, // XP
   0x0502, // Server 2003
   0x0600, // Vista
   0x0601, // 7
   0x0602, // 8
   0x0603, // 8.1
   0x1000, // 10+
};

/*

   IsZipFile

   Determines if a file is a real zip file by checking the first four
   bytes of a file and checking for the ZIP signature header.

*/
BOOL IsZipFile( LPTSTR szFileName )
{
   char buf[4];
   DWORD cb = 0;
   HANDLE handle = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if ( handle != INVALID_HANDLE_VALUE )
   {
      ReadFile( handle, buf, 4, &cb, NULL );
      CloseHandle( handle );
   }
   return cb == 4 && buf[ 0 ] == 'P' && buf[ 1 ] == 'K' && buf[ 2 ] == 0x03 && buf[ 3 ] == 0x04;
}

/*

   IsCabFile

   Determines if a file is a real cab file by checking the first four
   bytes of a file and checking for the CAB signature header.

*/
BOOL IsCabFile( LPTSTR szFileName )
{
   char buf[4];
   DWORD cb = 0;
   HANDLE handle = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if ( handle != INVALID_HANDLE_VALUE )
   {
      ReadFile( handle, buf, 4, &cb, NULL );
      CloseHandle( handle );
   }
   return cb == 4 && buf[ 0 ] == 'M' && buf[ 1 ] == 'S' && buf[ 2 ] == 'C' && buf[ 3 ] == 'F';
}

void OpenINI()
{
   int i, n;

   GetFullPathName( szINIPath, MAX_PATH, szINIPath, NULL );
   SetDlgItemText( hwndStatic, IDC_INIPATH, szINIPath );

   GetPrivateProfileString( "FE", "Title", "", szPackageName, _countof(szPackageName), szINIPath );
   GetPrivateProfileString( "FE", "Website", "", szURL, _countof(szURL), szINIPath );
   GetPrivateProfileString( "FE", "ExtractPath", "", szExtractionPath, MAX_PATH, szINIPath );
   GetPrivateProfileString( "FE", "Execute", "", szExecuteCommand, MAX_PATH, szINIPath );
   GetPrivateProfileString( "FE", "OSVersion", "4.0", szTargetVer, _countof(szTargetVer), szINIPath );

   bRunElevated = GetPrivateProfileInt( "FE", "RunElevated", 0, szINIPath );
   bSubsystem64 = GetPrivateProfileInt( "FE", "Subsystem64", 0, szINIPath );
   uAutoExtract = GetPrivateProfileInt( "FE", "AutoExtract", 0, szINIPath );
   bOpenFolder = GetPrivateProfileInt( "FE", "OpenFolder", 0, szINIPath );
   bDeleteFiles = GetPrivateProfileInt( "FE", "DeleteFiles", 0, szINIPath );

   //
   // Replace return carriages in intro text.
   //
   n = GetPrivateProfileSection( "IntroText", szIntroText, _countof(szIntroText), szINIPath );
   for ( i = 0 ; i < n ; ++i )
   {
      if ( ( szIntroText[ i ] == '\\' ) && ( szIntroText[ i + 1 ] == '\0' ) )
      {
         szIntroText[ i ] = 0x0D;
         szIntroText[ i + 1 ] = 0x0A;
      }
   }

   GetPrivateProfileSection( "Shortcuts", szShortcut, _countof(szShortcut), szINIPath );

   DestroyMenu( hShortcuts );
   hShortcuts = CreatePopupMenu();

   for ( i = 0 ; szShortcut[ i ] != '\0' ; i += lstrlen( szShortcut + i ) + 1 )
   {
      char buf[ MAX_PATH * 2 ];
      if ( *gettoken( buf + i, "=", 0, buf ) && lstrcmpi( buf, "Shortcut" ) == 0 )
      {
         gettoken( szShortcut + i, "=", 1, buf );
         AppendMenu( hShortcuts, MF_STRING, 0, buf );
      }
   }
}

void WinMainCRTStartup()
{
   char buf[ MAX_PATH ];

   ghInstance = GetModuleHandle( NULL );

   //
   // Determine font to use
   //
   // If Tahoma exists, use it. Else, default to "MS Shell Dlg"
   //
   if ( DoesFontExist( "Segoe UI" ) )
   {
      lstrcpy( szActiveFont, "Segoe UI" );
   }
   else if ( DoesFontExist( "Tahoma" ) )
   {
      lstrcpy( szActiveFont, "Tahoma" );
   }
   else
   {
      lstrcpy( szActiveFont, "MS Shell Dlg" );
   }

   //
   // Build .ini and help file path strings
   //
   GetModuleFileName( NULL, buf, _countof(buf) );
   PathRemoveFileSpec( buf );
   PathCombine( szHelpPath, buf, "FEHelp.chm" );
   PathCombine( szINIPath, buf, "default.ini" );

   //
   // Load the default settings, if default.ini already exists.
   //
   if ( !FileExists( szINIPath ) )
      *szINIPath = '\0';

   //
   // We're done initializing. Show the main dialog.
   //
   DialogBox( ghInstance, MAKEINTRESOURCE( IDD_TEMPLATE ), NULL, MainDlgProc );
   ExitProcess( 0 );
}


INT_PTR CALLBACK AboutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
   case WM_INITDIALOG:
      SetDlgItemText( hDlg, IDC_TITLE, "FreeExtractor " VERSION );
      SetDlgItemText( hDlg, IDC_URL, WEBSITE_URL );
      return TRUE;

   case WM_NCHITTEST:
      SetWindowLongPtr( hDlg, DWLP_MSGRESULT, HTCAPTION );
      return TRUE;

   case WM_CTLCOLORSTATIC:
      switch ( GetDlgCtrlID( ( HWND ) lParam ) )
      {
      case IDC_URL:
         return FormatControl( hActiveFontURL, ( HDC ) wParam, _TEXT_BLUE_ );
      case IDC_TITLE:
         return FormatControl( hActiveFontBanner, ( HDC ) wParam, _TEXT_BLACK_ );
      case IDC_TEXT:
         return FormatControl( hActiveFontSubBanner, ( HDC ) wParam, _TEXT_BLACK_ );
      }
      return FALSE;

   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint( hDlg, &ps );
         HDC memdc = CreateCompatibleDC( NULL );
         HBITMAP hbmp = LoadBitmap( ghInstance, MAKEINTRESOURCE( IDB_BITMAP1 ) );
         HGDIOBJ hbmpUnselected = SelectObject( memdc, hbmp );

         //
         // Draw white banner background
         //
         SetROP2( hdc, R2_WHITE );
         Rectangle( hdc, 0, 0, 500, 500 );

         //
         // Draw box icon
         //
         StretchBlt( ps.hdc, 8, 18, 43, 42, memdc, 0, 0, 43, 42, SRCCOPY );

         SelectObject( memdc, hbmpUnselected );
         DeleteObject( hbmp );
         DeleteDC( memdc );
         EndPaint( hDlg, &ps );
         return TRUE;
      }

   case WM_SETCURSOR:
      if ( GetDlgCtrlID( ( HWND ) wParam ) == IDC_URL )
      {
         SetCursor( LoadCursor( NULL, IDC_HAND ) );
         SetWindowLongPtr( hDlg, DWLP_MSGRESULT, 1 );
         return TRUE;
      }
      return FALSE;

   case WM_COMMAND:
      switch ( wParam )
      {
      case IDOK:
      case IDCANCEL:
         EndDialog( hDlg, 1 );
         return TRUE;

      case MAKEWPARAM( IDC_URL, STN_CLICKED ):
         ShellExecute( hDlg, TEXT( "open" ), TEXT( WEBSITE_URL ), NULL, NULL, SW_SHOWNORMAL );
         return TRUE;
      }
      return TRUE;
   }
   return FALSE;
}


/*

   MainDlgProc

   Main callback

*/
INT_PTR CALLBACK MainDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   char *p, *q;

   switch ( message )
   {
   case WM_SETFONT:
      {
         LOGFONT lf;
         if ( GetObject( ( HFONT ) wParam, sizeof lf, &lf ) )
         {
            lstrcpy( lf.lfFaceName, szActiveFont );
            lf.lfHeight = iFontSize;
            lf.lfWeight = FW_REGULAR;
            hActiveFont = CreateFontIndirect( &lf );
            lf.lfWeight = FW_MEDIUM;
            hActiveFontSubBanner = CreateFontIndirect( &lf );
            lf.lfWeight = FW_BOLD;
            hActiveFontBanner = CreateFontIndirect( &lf );
            lf.lfHeight = 22;
            hActiveFontIntroBanner = CreateFontIndirect( &lf );
            lf.lfHeight = 12;
            lf.lfWeight = FW_MEDIUM;
            hActiveFontVersionDate = CreateFontIndirect( &lf );
            lstrcpy( lf.lfFaceName, "Verdana" );
            lf.lfHeight = 24;
            lf.lfWeight = FW_HEAVY;
            hActiveFontProductName = CreateFontIndirect( &lf );
            lstrcpy( lf.lfFaceName, "MS Shell Dlg" );
            lf.lfHeight = 13;
            lf.lfWeight = FW_MEDIUM;
            lf.lfUnderline = TRUE;
            hActiveFontURL = CreateFontIndirect( &lf );
         }
      }
      break;

   case WM_NCDESTROY:
      DeleteObject( hActiveFont );
      DeleteObject( hActiveFontBanner );
      DeleteObject( hActiveFontSubBanner );
      DeleteObject( hActiveFontIntroBanner );
      DeleteObject( hActiveFontVersionDate );
      DeleteObject( hActiveFontProductName );
      break;

   case WM_INITDIALOG:
      hwndMain = hDlg;

      SetTitle( "FreeExtractor Wizard" );
      SetClassLongPtr( hDlg, GCLP_HICON, ( LONG_PTR ) LoadIcon( ghInstance, MAKEINTRESOURCE( IDI_SETUP1 ) ) );

      //
      // Check to see if a filename was passed from the command
      // line. If so, skip the splash screen.
      //
      p = GetCommandLine();

      for ( ; ( q = PathGetArgs( p ) ) > p ; p = q )
      {
         PathRemoveArgs( p );
         PathRemoveBlanks( p );
         PathUnquoteSpaces( p );
         if ( PathMatchSpec( p, "*.zip" ) )
         {
            lstrcpy( szZipFileName, p );
            if ( !IsZipFile( szZipFileName ) )
               RaiseError( "The file you have specified is not a ZIP file." );
         }
         else if ( PathMatchSpec( p, "*.cab" ) )
         {
            lstrcpy( szZipFileName, p );
            if ( !IsCabFile( szZipFileName ) )
               RaiseError( "The file you have specified is not a CAB file." );
         }
         else if ( PathMatchSpec( p, "*.ini" ) )
         {
            lstrcpy( szINIPath, p );
         }
      }

      if ( *szZipFileName )
      {
          iCurrentPage = ZIP_PAGE;
          SetDialogPage();
          Open();
      }
      else
      {
          iCurrentPage = SPLASH_PAGE;
          SetDialogPage();
      }

      if ( *szINIPath )
         OpenINI();

      return TRUE;

   case WM_NCHITTEST:
      SetWindowLongPtr( hDlg, DWLP_MSGRESULT, HTCAPTION );
      return TRUE;

   case WM_NCRBUTTONDOWN:
      {
         HMENU hmenuTrackPopup = CreatePopupMenu();
         AppendMenu( hmenuTrackPopup, MF_STRING, IDM_ABOUT, "&About..." );
         TrackPopupMenu( hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ), 0, hDlg, NULL );
         DestroyMenu( hmenuTrackPopup );
         return TRUE;
      }

   case WM_PAINT:
      if ( iCurrentPage != SPLASH_PAGE )
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint( hDlg, &ps );
         HDC memdc = CreateCompatibleDC( NULL );
         HBITMAP hbmp = LoadBitmap( ghInstance, MAKEINTRESOURCE( IDB_BITMAP1 ) );
         HGDIOBJ hbmpUnselected = SelectObject( memdc, hbmp );

         //
         // Draw white background
         //
         SetROP2( hdc, R2_WHITE );
         Rectangle( hdc, 0, 0, DLG_X_SCALE( 600 ), 60 );

         //
         // Draw box icon in the upper right
         //
         StretchBlt( ps.hdc,
                     DLG_X_SCALE( 440 ), DLG_Y_SCALE( 9 ),
                     43, 42,
                     memdc,
                     0, 0, 43, 42,
                     SRCCOPY );

         SelectObject( memdc, hbmpUnselected );
         DeleteObject( hbmp );
         DeleteDC( memdc );
         EndPaint( hDlg, &ps );

         return TRUE;
      }
      return FALSE;

   case WM_DROPFILES:
      {
         HDROP hDrop = ( HDROP ) wParam;
         UINT n = DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 );
         UINT i;
         for ( i = 0 ; i < n ; ++i )
         {
            char szFileName[ MAX_PATH ];
            if ( DragQueryFile( hDrop, i, szFileName, _countof(szFileName) ) )
            {
               if ( PathMatchSpec( szFileName, "*.zip" ) )
               {
                  lstrcpy( szZipFileName, szFileName );
                  Open();
               }
               else if (PathMatchSpec(szFileName, "*.cab"))
               {
                  lstrcpy(szZipFileName, szFileName);
                  Open();
               }
               else if ( PathMatchSpec( szFileName, "*.ini" ) )
               {
                  lstrcpy( szINIPath, szFileName );
                  OpenINI();
               }
            }
         }
         DragFinish( hDrop );
      }
      return TRUE;

#ifndef NO_HTML_HELP
   case WM_HELP:
      if ( HtmlHelp( hDlg, szHelpPath, HH_HELP_CONTEXT, iHelpIdArray[ iCurrentPage ] ) == NULL )
         MessageBox( hDlg, "FEHelp.chm could not be found or loaded.", "Cannot load help", 0 );

      return TRUE;
#endif

   case WM_CTLCOLORSTATIC:
      switch ( GetDlgCtrlID( ( HWND ) lParam ) )
      {
      case IDC_BANNER:
         return FormatControl( hActiveFontBanner, ( HDC ) wParam, _TEXT_BLACK_ );
      case IDC_SUBBANNER:
         return FormatControl( hActiveFontSubBanner, ( HDC ) wParam, _TEXT_BLACK_ );
      }
      return FALSE;

   case WM_CLOSE:
      PostMessage( hDlg, WM_COMMAND, IDC_CANCEL, 0 );
      return TRUE;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDCANCEL:
         if ( MessageBox( hDlg, "Are you sure you want to exit?", "Confirm exit", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES )
            CleanUp();
         return TRUE;

      case IDM_ABOUT:
         DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hDlg, AboutDlgProc );
         return TRUE;

      case IDC_NEXT:
      case IDC_BACK:
         switch ( LOWORD( wParam ) == IDC_NEXT ? iCurrentPage++ : iCurrentPage-- )
         {
         case OPTIONS_PAGE:
            GetDlgItemText( hwndStatic, IDC_PACKAGENAME, szPackageName, _countof(szPackageName) );
            StrTrim( szPackageName, " \t\r\n" );

            GetDlgItemText( hwndStatic, IDC_URL, szURL, _countof(szURL) );
            StrTrim( szURL, " \t\r\n" );

            GetDlgItemText( hwndStatic, IDC_INTROTEXT, szIntroText, _countof(szIntroText) );
            StrTrim( szIntroText, " \t\r\n" );
            break;

         case ADVANCED_OPTIONS_PAGE:
            GetDlgItemText( hwndStatic, IDC_EXEC, szExecuteCommand, _countof(szExecuteCommand) );
            StrTrim( szExecuteCommand, " \t\r\n" );

            GetDlgItemText( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH, szExtractionPath, _countof(szExtractionPath) );
            StrTrim( szExtractionPath, " \t\r\n" );

            GetDlgItemText( hwndStatic, IDC_TARGETVER, szTargetVer, _countof(szTargetVer) );

            if ( !IsWindowEnabled( GetDlgItem( hwndStatic, IDC_DELETEFILES ) ) )
               bDeleteFiles = FALSE;
            break;

         case FINISHED_PAGE:
            //
            // If the "Save Settings ..." checkbox is checked, write out
            // the default settings to an INI file
            //
            CheckSaveSettings();

            //
            // Reset and start over
            //
            iCurrentPage = 1;

            *szZipFileName = '\0';
            *szEXEOutPath = '\0';
            *szPackageName = '\0';
            *szURL = '\0';
            *szConfirmMessage = '\0';
            *szIntroText = '\0';
            *szExecuteCommand = '\0';
            *szExtractionPath = '\0';
            szShortcut[0] = szShortcut[1] = '\0';

            DestroyMenu( hShortcuts );
            hShortcuts = CreatePopupMenu();

            uAutoExtract = 0;
            bOpenFolder = FALSE;
            bDeleteFiles = FALSE;

            SetDlgItemText( hDlg, IDC_NEXT, "Next >" );
            SetDlgItemText( hDlg, IDC_CANCEL, "Cancel" );
            break;
         }

         SetDialogPage();
         return TRUE;

      case IDC_CANCEL:
         CheckSaveSettings();
         CleanUp();
         return TRUE;

      default:
         PostMessage( hwndStatic, message, wParam, lParam );
         break;
      }
      return TRUE;
   }

   return FALSE;
}


/*

   SetDialogPage

   Change the child dialog page

*/
void SetDialogPage()
{
   int i, n;
   char *p;
   HWND hwndFocus;

   UINT flags = iCurrentPage != SPLASH_PAGE ? SWP_NOSIZE | SWP_SHOWWINDOW : SWP_NOSIZE | SWP_HIDEWINDOW;

   if ( hwndStatic != NULL ) DestroyWindow( hwndStatic );

   SetWindowPos( GetDlgItem( hwndMain, IDC_TOPFRAME ), HWND_TOP, 0, 59, 0, 0, flags );
   SetWindowPos( GetDlgItem( hwndMain, IDC_BANNER ), HWND_TOP, 22, 12, 0, 0, flags );
   SetWindowPos( GetDlgItem( hwndMain, IDC_SUBBANNER ), HWND_TOP, 44, 27, 0, 0, flags );

   LoadDialog( iDialogArray[ iCurrentPage ] );
   if ( iCurrentPage != SPLASH_PAGE )
      SetWindowPos( hwndStatic, HWND_TOP, 44, 70, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW );

   DragAcceptFiles( hwndMain, iCurrentPage == ZIP_PAGE );

   SetBannerText( szBannerText[ iCurrentPage ] );
   SetSubBannerText( szSubBannerText[ iCurrentPage ] );

   DLGITEM_SETFONT( hwndMain, IDC_BACK )
   DLGITEM_SETFONT( hwndMain, IDC_NEXT )
   DLGITEM_SETFONT( hwndMain, IDC_CANCEL )

   switch ( iCurrentPage )
   {
   case SPLASH_PAGE:
      SetBannerText( szBannerText[ iCurrentPage ] );
      SetSubBannerText( szSubBannerText[ iCurrentPage ] );

      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );

      SetDlgItemText( hwndStatic, IDC_VERSION_DATE, VERSIONDATE );
      SetDlgItemText( hwndStatic, IDC_INTROTEXT, szWizIntroText );
      SetDlgItemText( hwndStatic, IDC_URL, WEBSITE_URL );
      break;

   case ZIP_PAGE:
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), *szEXEOutPath != '\0' );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

      SetDlgItemText( hwndStatic, IDC_INIPATH, szINIPath );
      SetDlgItemText( hwndStatic, IDC_ZIPPATH, szZipFileName );
      SetDlgItemText( hwndStatic, IDC_EXEOUT, szEXEOutPath );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_INILABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_INIPATH )
      DLGITEM_SETFONT( hwndStatic, IDC_OPENINI )
      DLGITEM_SETFONT( hwndStatic, IDC_ZIPLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_ZIPPATH )
      DLGITEM_SETFONT( hwndStatic, IDC_OPEN )
      DLGITEM_SETFONT( hwndStatic, IDC_OUTLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_EXEOUT )
      break;

   case OPTIONS_PAGE:
      SendDlgItemMessage( hwndStatic, IDC_PACKAGENAME, EM_LIMITTEXT, 75, 0 );
      SendDlgItemMessage( hwndStatic, IDC_INTROTEXT, EM_LIMITTEXT, 675, 0 );
      SendDlgItemMessage( hwndStatic, IDC_URL, EM_LIMITTEXT, 200, 0 );

      SetDlgItemText( hwndStatic, IDC_URL, szURL );
      SetDlgItemText( hwndStatic, IDC_INTROTEXT, szIntroText );

      SetDlgItemText( hwndStatic, IDC_PACKAGENAME, *szPackageName ? szPackageName : "Unnamed Archive");

      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_NAMELABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_WEBSITELABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_INTROLABEL )
      break;

   case ADVANCED_OPTIONS_PAGE:
      SendDlgItemMessage( hwndStatic, IDC_EXEC, EM_LIMITTEXT, 200, 0 );
      SendDlgItemMessage( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH, EM_LIMITTEXT, 200, 0 );

      SetDlgItemText( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH, szExtractionPath );
      SetDlgItemText( hwndStatic, IDC_EXEC, szExecuteCommand );

      CheckDlgButton( hwndStatic, IDC_RUNELEVATED, bRunElevated );
      CheckDlgButton( hwndStatic, IDC_SUBSYSTEM64, bSubsystem64 );
      CheckDlgButton( hwndStatic, IDC_AUTOEXTRACT, uAutoExtract );
      CheckDlgButton( hwndStatic, IDC_OPENFOLDER, bOpenFolder );
      CheckDlgButton( hwndStatic, IDC_DELETEFILES, bDeleteFiles );

      for ( i = 0 ; i < _countof(rgTargetVer) ; ++i )
      {
         char sz[ 6 ];
         wsprintf( sz, "%x.%02x", HIBYTE( rgTargetVer[ i ] ), LOBYTE( rgTargetVer[ i ] ) );
         sz[4] = '\0'; // Cosmetic: 10.00 -> 10.0
         SendDlgItemMessage( hwndStatic, IDC_TARGETVER, CB_ADDSTRING, 0, ( LPARAM ) sz );
      }
      SendDlgItemMessage( hwndStatic, IDC_TARGETVER, CB_SELECTSTRING, 0, ( LPARAM ) szTargetVer );

      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_RUNELEVATED )
      DLGITEM_SETFONT( hwndStatic, IDC_SUBSYSTEM64 )
      DLGITEM_SETFONT( hwndStatic, IDC_AUTOEXTRACT )
      DLGITEM_SETFONT( hwndStatic, IDC_OPENFOLDER )
      DLGITEM_SETFONT( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH )
      DLGITEM_SETFONT( hwndStatic, IDC_TARGETVER )
      DLGITEM_SETFONT( hwndStatic, IDC_EXECLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_DELETEFILES )
      DLGITEM_SETFONT( hwndStatic, IDC_SHORTCUTLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_TARGETVERLABEL )
      break;

   case SHORTCUT_PAGE:
      RefreshShortcuts();

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_LIST )
      DLGITEM_SETFONT( hwndStatic, IDC_ADD_SC )
      DLGITEM_SETFONT( hwndStatic, IDC_REMOVE_SC )
      break;

   case ICON_PAGE:
      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_NEWICON )
      break;

   case CONFIRM_PAGE:
      SetConfirmMessage();
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_CONFIRMMESSAGE )
      DLGITEM_SETFONT( hwndStatic, IDC_TEXT2 )
      break;

   case PROGRESS_PAGE:
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );

      //
      // Build shortcut string before build
      //
      p = szShortcut;
	   n = GetMenuItemCount( hShortcuts );
      for ( i = 0 ; i < n ; ++i )
      {
         char buf[MAX_PATH * 2];
         GetMenuString( hShortcuts, i, buf, _countof(buf), MF_BYPOSITION );
         p += wsprintf( p, "Shortcut=%s", buf ) + 1;
      }
      *p++ = '\0';

      CloseHandle( CreateThread( NULL, 0, Build, NULL, 0, NULL ) );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_STATUSLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_PROGRESS )
      break;

   case FINISHED_PAGE:
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );

      SetDlgItemText( hwndMain, IDC_NEXT, "&New SFX" );
      SetDlgItemText( hwndMain, IDC_CANCEL, "&Finish" );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_EXECUTE )
      DLGITEM_SETFONT( hwndStatic, IDC_SAVESETTINGS )
      break;
   }

   SendDlgItemMessage( hwndMain, IDC_BACK, BM_SETSTYLE, BS_PUSHBUTTON, TRUE );
   SendDlgItemMessage( hwndMain, IDC_NEXT, BM_SETSTYLE, BS_PUSHBUTTON, TRUE );
   SetFocus( hwndMain );
   hwndFocus = GetFocus();
   if ( SendMessage( hwndFocus, WM_GETDLGCODE, 0, 0 ) & DLGC_UNDEFPUSHBUTTON )
      SendMessage( hwndFocus, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE );
}


/*

   ChildDialogProc

   Callback for all child dialogs. I'm too lazy too build one for each dialog.

*/
INT_PTR CALLBACK ChildDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
   case WM_INITDIALOG:
      hwndStatic = hDlg;

      if ( iCurrentPage == SHORTCUT_PAGE )
      {
         LV_COLUMN lvColumn;

         hWndListView = GetDlgItem( hDlg, IDC_LIST );
         ListView_SetExtendedListViewStyle( hWndListView, LVS_EX_FULLROWSELECT );

         lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
         lvColumn.fmt = LVCFMT_LEFT;

         lvColumn.cx = 120;
         lvColumn.pszText = "Shortcut";
         ListView_InsertColumn ( hWndListView, 0, &lvColumn );

         lvColumn.cx = 290;
         lvColumn.pszText = "Target";
         ListView_InsertColumn ( hWndListView, 1, &lvColumn );
      }
      return TRUE;

   case WM_NCHITTEST:
      SetWindowLongPtr( hDlg, DWLP_MSGRESULT, HTTRANSPARENT );
      return TRUE;

   case WM_SETCURSOR:
      if ( iCurrentPage == SPLASH_PAGE && GetDlgCtrlID( ( HWND ) wParam ) == IDC_URL )
      {
         SetCursor( LoadCursor( NULL, IDC_HAND ) );
         SetWindowLongPtr( hDlg, DWLP_MSGRESULT, 1 );
         return TRUE;
      }
      return FALSE;

   case WM_PAINT:
      if ( iCurrentPage == SPLASH_PAGE )
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint( hDlg, &ps );
         HDC memdc = CreateCompatibleDC( NULL );
         HBITMAP hbmp = LoadBitmap( ghInstance, MAKEINTRESOURCE( IDB_INTRO ) );
         HGDIOBJ hbmpUnselected = SelectObject( memdc, hbmp );

         //
         // Draw white background
         //
         SetROP2( hdc, R2_WHITE );
         Rectangle( hdc, 0, 0, DLG_X_SCALE( 600 ), DLG_X_SCALE( 350 ) );

         //
         // Draw intro bitmap
         //
         StretchBlt( ps.hdc,
                     0, 0,
                     DLG_X_SCALE( 163 ), DLG_Y_SCALE( 312 ),
                     memdc,
                     0, 0, 163, 312,
                     SRCCOPY );

         SelectObject( memdc, hbmpUnselected );
         DeleteObject( hbmp );
         DeleteDC( memdc );
         EndPaint( hDlg, &ps );

         return TRUE;
      }

      if ( iCurrentPage == ICON_PAGE )
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint ( hDlg, &ps );
         SetBkMode( hdc, OPAQUE );

         if ( *szIconPath )
         {
            HICON hIcon = ExtractIcon( ghInstance, szIconPath, 0 );
            Rectangle( hdc, 78, 82, 124, 128 );
            if ( hIcon )
            {
               DrawIcon( hdc, 85, 89, hIcon );
               DestroyIcon( hIcon );
            }
         }

         EndPaint ( hDlg, &ps );

         return TRUE;
      }

      return FALSE;

   case WM_CTLCOLORSTATIC:
      if ( iCurrentPage == SPLASH_PAGE )
      {
         switch ( GetDlgCtrlID( ( HWND ) lParam ) )
         {
         case IDC_INTROTEXT:
         case IDC_CLICKNEXT:
            return FormatControl( hActiveFontSubBanner, ( HDC ) wParam, _TEXT_BLACK_ );
         case IDC_FENAME:
            return FormatControl( hActiveFontProductName, ( HDC ) wParam, _TEXT_BLUE_ );
         case IDC_VERSION_DATE:
            return FormatControl( hActiveFontVersionDate, ( HDC ) wParam, _TEXT_GRAY_ );
         case IDC_URL:
            return FormatControl( hActiveFontURL, ( HDC ) wParam, _TEXT_BLUE_ );
         }
      }
      return FALSE;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDC_ADD_SC:
         //
         // Show 'Add Shortcut' dialog
         //
         DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ADDSHORTCUT ), hwndMain, ShortcutDlgProc );
         RefreshShortcuts();
         return TRUE;

      case IDC_REMOVE_SC:
         {
            //
            // Remove selected entry from the stack
            //
            int iItem = ListView_GetItemCount( hWndListView );
            while ( iItem-- )
            {
               if ( ListView_GetItemState( hWndListView, iItem, LVIS_SELECTED ) )
                  DeleteMenu( hShortcuts, iItem, MF_BYPOSITION );
            }
            RefreshShortcuts();
            return TRUE;
         }

      case IDC_NEWICON:
         {
            //
            // Show "open" dialog
            //
            OPENFILENAME ofn = { sizeof ofn };
            char szIconPathTemp[ MAX_PATH ] = "";

            ofn.hwndOwner = hwndMain;
            ofn.lpstrFile = szIconPathTemp;
            ofn.hInstance = ghInstance;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = "Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_SHAREAWARE;

            //
            // Verify Icon
            //
            if ( GetOpenFileName( &ofn ) )
            {
               DWORD dwFileSize = 0;
               HANDLE hFile = CreateFile( szIconPathTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
               if ( hFile != INVALID_HANDLE_VALUE )
               {
                  dwFileSize = GetFileSize( hFile, NULL );
                  CloseHandle( hFile );
               }
               if ( dwFileSize != 2238 )
               {
                  MessageBox( hwndMain, "The specified icon is in an unsupported format. It must be a 2,238 byte, 32x32x256 color icon.\n", "Invalid Icon", 0 );
               }
               else
               {
                  RECT const rect = { 78, 82, 124, 128 };
                  lstrcpy( szIconPath, szIconPathTemp );
                  bChangeIcon = TRUE;
                  InvalidateRect( hDlg, &rect, FALSE );
               }
            }
            return TRUE;
         }

      case IDC_EXEC:
         {
            TCHAR szTemp[MAX_PATH];
            GetDlgItemText( hDlg, IDC_EXEC, szTemp, _countof(szTemp) );
            StrTrim( szTemp, " \t\r\n" );
            EnableWindow( GetDlgItem( hDlg, IDC_DELETEFILES ), *szTemp != '\0' );
            return TRUE;
         }

      case IDC_EXECUTE:
         if ( ( INT_PTR ) ShellExecute( hwndMain, "open", szEXEOutPath, NULL, NULL, 0 ) < 33 )
            RaiseError( "Could not execute self extractor." );
         CheckSaveSettings();
         CleanUp();
         return TRUE;

      case IDC_RUNELEVATED:
         bRunElevated = IsDlgButtonChecked( hDlg, IDC_RUNELEVATED );
         return TRUE;

      case IDC_SUBSYSTEM64:
         bSubsystem64 = IsDlgButtonChecked( hDlg, IDC_SUBSYSTEM64 );
         return TRUE;

      case IDC_AUTOEXTRACT:
         uAutoExtract = IsDlgButtonChecked( hDlg, IDC_AUTOEXTRACT );
         return TRUE;

      case IDC_OPENFOLDER:
         bOpenFolder = IsDlgButtonChecked( hDlg, IDC_OPENFOLDER );
         return TRUE;

      case IDC_DELETEFILES:
         bDeleteFiles = IsDlgButtonChecked( hDlg, IDC_DELETEFILES );
         return TRUE;

      case IDC_OPEN:
         {
            OPENFILENAME ofn = { sizeof ofn };

            ofn.hwndOwner = hwndMain;
            ofn.lpstrFile = szZipFileName;
            ofn.hInstance = ghInstance;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = "ZIP Files (*.zip)\0*.zip\0CAB Files (*.cab)\0*.cab\0All Files (*.*)\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_SHAREAWARE;

            if ( GetOpenFileName( &ofn ) ) Open();

            return TRUE;
         }

      case IDC_OPENINI:
         {
            OPENFILENAME ofn = { sizeof ofn };

            ofn.hwndOwner = hwndMain;
            ofn.lpstrFile = szINIPath;
            ofn.hInstance = ghInstance;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = "INI Files (*.ini)\0*.ini\0All Files (*.*)\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_SHAREAWARE;

            if ( GetOpenFileName( &ofn ) ) OpenINI();

            return TRUE;
         }

      case IDC_URL:
         if (HIWORD(wParam) == STN_CLICKED)
         {
            ShellExecute( hDlg, TEXT( "open" ), TEXT( WEBSITE_URL ), NULL, NULL, SW_SHOWNORMAL );
         }
         return TRUE;
      }
   }
   return FALSE;
}


/*

   Open

   Called after a source ZIP file has been chosen. This verifies the file and creates a filename
   for the output .EXE (by default, <filename>.exe).

*/
void Open()
{
   GetFullPathName( szZipFileName, MAX_PATH, szZipFileName, NULL );
   if ( DirectoryExists( szZipFileName ) )
   {
      //
      // The user drag-and-dropped a directory. Handle gracefully.
      //
      *szZipFileName = '\0';
      MessageBox( hwndMain, "Directories are not supported. Select a .zip or .cab file.", "FreeExtractor Error", 0 );
   }

   SetDlgItemText( hwndStatic, IDC_ZIPPATH, szZipFileName );
   lstrcpy( szEXEOutPath, szZipFileName );
   if ( *szZipFileName )
   {
      int iSuffix = 0;
      char *pSuffix = PathFindExtension( szEXEOutPath );
      //
      // If the output file already exists, generate a new output file name.
      //
      do
      {
         wsprintf( pSuffix, iSuffix ? "-%i.exe" : ".exe", iSuffix );
      } while ( FileExists( szEXEOutPath ) && ( ++iSuffix < 50 ) );
   }

   EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), *szZipFileName != '\0' );
   SetDlgItemText( hwndStatic, IDC_EXEOUT, szEXEOutPath );
}


/*

   SetConfirmMessage

   Based on the user's settings, format a confirmation message of the SFX's
   tasks, Not pretty, but it works....

*/
void SetConfirmMessage()
{
   int n;

   if ( uAutoExtract == 0 )
   {
      lstrcpy( szConfirmMessage, "The user will be given the option to enter a path to extract the files to" );

      if ( *szExtractionPath )
      {
         lstrcat( szConfirmMessage, " (by default, \"" );
         lstrcat( szConfirmMessage, szExtractionPath );
         lstrcat( szConfirmMessage, "\" ). " );
      }
      else
         lstrcat( szConfirmMessage, ". " );
   }
   else
   {
      lstrcpy( szConfirmMessage, "The files in this archive will automatically be extracted to " );

      if ( *szExtractionPath )
      {
         lstrcat( szConfirmMessage, "\"" );
         lstrcat( szConfirmMessage, szExtractionPath );
         lstrcat( szConfirmMessage, "\" " );
      }
      else
         lstrcat( szConfirmMessage, "the user's temp directory " );

      lstrcat( szConfirmMessage, "without prompting the user. " );
   }

   if ( *szExecuteCommand || bOpenFolder )
   {
      lstrcat( szConfirmMessage, "When the file extraction has finished, FreeExtractor will " );

      if ( *szExecuteCommand )
      {
         lstrcat( szConfirmMessage, "execute \"" );
         lstrcat( szConfirmMessage, szExecuteCommand );
         lstrcat( szConfirmMessage, "\"" );

         if ( bOpenFolder )
            lstrcat( szConfirmMessage, " and " );
         else
            lstrcat( szConfirmMessage, "." );
      }

      if ( bOpenFolder )
      {
         lstrcat( szConfirmMessage, "automatically open the target folder" );

         if ( uAutoExtract == 1 && *szExtractionPath )
         {
            lstrcat( szConfirmMessage, " (\"" );
            lstrcat( szConfirmMessage, szExtractionPath );
            lstrcat( szConfirmMessage, "\"). " );

         }
         else
            lstrcat ( szConfirmMessage, ". " );
      }
   }

   if ( bDeleteFiles )
      lstrcat( szConfirmMessage, " When completed, the extracted files will be deleted." );

   n = GetMenuItemCount( hShortcuts );
   if ( n > 0 )
   {
      if ( n == 1 )
      {
         lstrcat( szConfirmMessage, " Then, the shortcut you specified will be created." );
      }
      else
      {
         char buf[ 255 ];
         wsprintf( buf, " Then, the %d shortcuts you specified will be created.", n );
         lstrcat( szConfirmMessage, buf );
      }
   }

   if ( iCurrentPage == CONFIRM_PAGE )
      SetDlgItemText( hwndStatic, IDC_CONFIRMMESSAGE, szConfirmMessage );
}


/*

   RefreshShortcuts

   Redraws the shortcut list box

*/
void RefreshShortcuts()
{
   int i, n;
   LV_ITEM lvI;

   //
   // Empty the list in list view.
   //
   ListView_DeleteAllItems ( hWndListView );

   lvI.mask = LVIF_TEXT;
   lvI.iSubItem = 0;
   lvI.state = 0;
   lvI.pszText = "";

   i = 0;
   n = GetMenuItemCount( hShortcuts );
   for ( i = 0 ; i < n ; ++i )
   {
      char buf[MAX_PATH * 2];
      char location[ MAX_PATH ];
      char target[ MAX_PATH ];

      GetMenuString( hShortcuts, i, buf, _countof(buf), MF_BYPOSITION );

      //
      // Expand the path variables for the shortcut
      //
      gettoken( buf, "|", 0, location );
      gettoken( buf, "|", 1, target );
      lvI.iItem = i;
      lvI.pszText = location;

      ListView_InsertItem( hWndListView, &lvI );
      ListView_SetItemText( hWndListView, i, 1, target );
   }
}




INT_PTR CALLBACK ShortcutDlgProc ( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
   case WM_INITDIALOG:
      DLGITEM_SETFONT( hDlg, IDC_MAINTEXT );
      DLGITEM_SETFONT( hDlg, IDC_SHORTCUTLABEL );
      DLGITEM_SETFONT( hDlg, IDC_SCLOCATION );
      DLGITEM_SETFONT( hDlg, IDC_LOCATIONEXAMPLE );
      DLGITEM_SETFONT( hDlg, IDC_TARGETLABEL );
      DLGITEM_SETFONT( hDlg, IDC_SCTARGET );
      DLGITEM_SETFONT( hDlg, IDC_TARGETEXAMPLE );
      DLGITEM_SETFONT( hDlg, IDOK );
      DLGITEM_SETFONT( hDlg, IDCANCEL );
      return TRUE;

#ifndef NO_HTML_HELP
   case WM_HELP:
      if ( HtmlHelp( hwndMain, szHelpPath, HH_HELP_CONTEXT, IDH_SHORTCUTS ) == NULL )
         MessageBox( hwndMain, "FEHelp.chm could not be found or loaded.", "Cannot load help", 0 );
      return TRUE;
#endif

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDOK:
         {
            char szLocation[ MAX_PATH ];
            char szTarget[ MAX_PATH ];

            //
            // Save Settings
            //
            GetDlgItemText( hDlg, IDC_SCLOCATION, szLocation, MAX_PATH );
            GetDlgItemText( hDlg, IDC_SCTARGET, szTarget, MAX_PATH );

            StrTrim( szLocation, " \t\r\n" );
            StrTrim( szTarget, " \t\r\n" );

            if ( *szLocation && *szTarget )
            {
               char buf[ MAX_PATH * 2 ];
               wsprintf( buf, "%s|%s", szLocation, szTarget );
               AppendMenu( hShortcuts, MF_STRING, 0, buf );
            }

            //
            // Close dialog
            //
            EndDialog( hDlg, 0 );
            return TRUE;
         }

      case IDCANCEL:
         //
         // Close Dialog
         //
         EndDialog( hDlg, 0 );
         return TRUE;
      }
   }

   return FALSE;
}

BOOL WINAPI WritePrivateProfileQuoted(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName)
{
   LPTSTR lpQuoted = _alloca(IS_INTRESOURCE(lpString) ? 12 : (lstrlen(lpString) + 3) * sizeof(TCHAR));
   wsprintf(lpQuoted, IS_INTRESOURCE(lpString) ? TEXT("%u") : TEXT("\"%s\""), lpString);
   return WritePrivateProfileString(lpAppName, lpKeyName, lpString ? lpQuoted : NULL, lpFileName);
}

/*

  CheckSaveSettings

  If the "Save Settings ..." checkbox is checked, write out the default settings
  to an INI file.

*/
void CheckSaveSettings()
{
   if ( iCurrentPage == FINISHED_PAGE )
   {
      if ( IsDlgButtonChecked( hwndStatic, IDC_SAVESETTINGS ) )
      {
         int i;

         WritePrivateProfileString( "FE", "Title", szPackageName, szINIPath );
         WritePrivateProfileString( "FE", "Website", szURL, szINIPath );
         WritePrivateProfileString( "FE", "ExtractPath", szExtractionPath, szINIPath );
         WritePrivateProfileQuoted( "FE", "Execute", szExecuteCommand, szINIPath );
         WritePrivateProfileString( "FE", "OSVersion", szTargetVer, szINIPath );

         WritePrivateProfileQuoted( "FE", "RunElevated", MAKEINTRESOURCE( bRunElevated ), szINIPath );
         WritePrivateProfileQuoted( "FE", "Subsystem64", MAKEINTRESOURCE( bSubsystem64 ), szINIPath );
         WritePrivateProfileQuoted( "FE", "AutoExtract", MAKEINTRESOURCE( uAutoExtract ), szINIPath );
         WritePrivateProfileQuoted( "FE", "OpenFolder", MAKEINTRESOURCE( bOpenFolder ), szINIPath );
         WritePrivateProfileQuoted( "FE", "DeleteFiles", MAKEINTRESOURCE( bDeleteFiles ), szINIPath );

         WritePrivateProfileSection( "Shortcuts", szShortcut, szINIPath );

         //
         // Replace return carriages in intro text.
         //
         for ( i = 0 ; szIntroText[ i ] ; ++i )
         {
            if ( szIntroText[ i ] == 0x0D ) 
               szIntroText[ i ] = '\\';
            else if ( szIntroText[ i ] == 0x0A )
               szIntroText[ i ] = '\0';
         }
         szIntroText[ ++i ] = '\0';

         WritePrivateProfileSection( "IntroText", NULL, szINIPath );
         WritePrivateProfileSection( "IntroText", szIntroText, szINIPath );
      }
   }
}
