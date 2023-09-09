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

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
   int slashpos = 0;
   char buf[ MAX_PATH ] = "";

   ghInstance = hInstance;

   //
   // Initialize shortcut list
   //
   ListInit( &list_Shortcuts );

   //
   // Determine font to use
   //
   // If Tahoma exists, use it. Else, default to "MS Shell Dlg"
   //
   if ( DoesFontExist( "Segoe UI" ) )
   {
      lstrcpy( szActiveFont, "Segoe UI" );
      iFontSize = 13;
   }
   else if ( DoesFontExist( "Tahoma" ) )
   {
      lstrcpy( szActiveFont, "Tahoma" );
      iFontSize = 13;
   }
   else
   {
      lstrcpy( szActiveFont, "MS Shell Dlg" );
      iFontSize = 13;
   }

   //
   // Build .ini and help file path strings
   //
   GetModuleFileName( NULL, buf, MAX_PATH );
   PathRemoveFileSpec( buf );
   PathCombine( szHelpPath, buf, "FEHelp.chm" );
   PathCombine( szINIPath, buf, "default.ini" );


   //
   // Load the default settings, if default.ini already exists.
   //
   if ( FileExists( szINIPath ) )
   {
      int i = 0;

      GetPrivateProfileString( "FE", "Title", "", szPackageName, 255, szINIPath );
      GetPrivateProfileString( "FE", "Website", "", szURL, 128, szINIPath );
      GetPrivateProfileString( "FE", "IntroText", "", szIntroText, 650, szINIPath );
      GetPrivateProfileString( "FE", "ExtractPath", "", szExtractionPath, MAX_PATH, szINIPath );
      GetPrivateProfileString( "FE", "Execute", "", szExecuteCommand, MAX_PATH, szINIPath );

      bRunElevated = GetPrivateProfileInt( "FE", "RunElevated", 0, szINIPath );
      bSubsystem64 = GetPrivateProfileInt( "FE", "Subsystem64", 0, szINIPath );
      bAutoExtract = GetPrivateProfileInt( "FE", "AutoExtract", 0, szINIPath );
      bOpenFolder = GetPrivateProfileInt( "FE", "OpenFolder", 0, szINIPath );
      bDeleteFiles = GetPrivateProfileInt( "FE", "DeleteFiles", 0, szINIPath );

      //
      // Replace return carriages in intro text.
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

   }


   //
   // Check to see if a filename was passed from the command
   // line. If so, skip the splash screen.
   //
   lstrcpy( buf, lpCmdLine );
   StrTrim( buf, " \t\r\n" );

   // remove leading and trailing quotes

   PathUnquoteSpaces(buf);

   if ( *buf && FileExists( buf ) )
   {
      char buf2[ MAX_PATH ] = "";
      GetFullPathName( buf, MAX_PATH, buf2, NULL );
      lstrcpy( szZipFileName, buf2 );
      if (!IsZipFile(szZipFileName))
         RaiseError ("The file you have specified is not a ZIP file.");
      Open();
   }

   //
   // We're done initializing. Show the main dialog.
   //
   DialogBox( ghInstance, MAKEINTRESOURCE( IDD_TEMPLATE ), NULL, MainDlgProc );
   return 1;
}






INT_PTR CALLBACK AboutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
   case WM_INITDIALOG:
      SetDlgItemText(hDlg, IDC_TITLE, "FreeExtractor " VERSION);
      return TRUE;

   case WM_LBUTTONDOWN:
      PostMessage( hDlg, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
      return TRUE;

   case WM_CTLCOLORSTATIC:
      switch ( GetDlgCtrlID( ( HWND ) lParam ) )
      {
      case IDC_URL:
         {
            static HFONT hFont = NULL;
            return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, 13, _TEXT_BLUE_, "MS Shell Dlg", TRUE, OPAQUE, WHITE_BRUSH );
         }
      case IDC_TITLE:
         {
            static HFONT hFont = NULL;
            return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_BOLD, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         }
      case IDC_TEXT:
         {
            static HFONT hFont = NULL;
            return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         }
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
         DeleteDC( memdc );
         EndPaint( hDlg, &ps );
         return TRUE;
      }

   case WM_SETCURSOR:
      if ( GetDlgCtrlID( ( HWND ) wParam ) == IDC_URL )
      {
         SetCursor( LoadCursor( ghInstance, MAKEINTRESOURCE( IDC_HAND1 ) ) );
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
         ShellExecute( hDlg, TEXT( "open" ), TEXT( "http://www.disoriented.com/" ), NULL, NULL, SW_SHOWNORMAL );
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
   switch ( message )
   {
   case WM_INITDIALOG:
      hwndMain = hDlg;

      GetTempPath( MAX_PATH, szTempDir );

      SetTitle( "FreeExtractor Wizard" );

      //
      // Check to see if szZipFileName is already filled in (in the case of being passed
      // from the command line.
      //
      if ( *szZipFileName )
      {
         iCurrentPage = 3;  // options page
      }
      else
      {
         iCurrentPage = 1;  // splash page
      }

      SetDialogPage( iCurrentPage );

      SetClassLongPtr( hwndMain, GCLP_HICON, ( LONG_PTR ) LoadIcon( ghInstance, MAKEINTRESOURCE( IDI_SETUP1 ) ) );

      return TRUE;

   case WM_LBUTTONDOWN:
      PostMessage( hwndMain, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
      return TRUE;

   case WM_PAINT:
      if ( iCurrentPage != SPLASH_PAGE )
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint( hwndMain, &ps );
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
         DeleteDC( memdc );
         EndPaint( hwndMain, &ps );

         return TRUE;
      }
      return FALSE;

   case WM_DROPFILES:
      if ( iCurrentPage == ZIP_PAGE )
      {
         HDROP hDrop = ( HDROP ) wParam;
         DragQueryFile( hDrop, 0, szZipFileName, MAX_PATH );
         Open();
      }
      return TRUE;

#ifndef NO_HTML_HELP
   case WM_HELP:
      {
         int iHelpPage;
         if ( iCurrentPage == SPLASH_PAGE ) iHelpPage = IDH_INTRO;
         else if ( iCurrentPage == ZIP_PAGE ) iHelpPage = IDH_ZIP;
         else if ( iCurrentPage == OPTIONS_PAGE ) iHelpPage = IDH_OPTIONS;
         else if ( iCurrentPage == ADVANCED_OPTIONS_PAGE ) iHelpPage = IDH_ADVOPTIONS;
         else if ( iCurrentPage == SHORTCUT_PAGE ) iHelpPage = IDH_SHORTCUTS;
         else if ( iCurrentPage == ICON_PAGE ) iHelpPage = IDH_ICONSS;
         else if ( iCurrentPage == CONFIRM_PAGE ) iHelpPage = IDH_CONFIRM;
         else if ( iCurrentPage == PROGRESS_PAGE ) iHelpPage = IDH_BUILD;
         else if ( iCurrentPage == FINISHED_PAGE ) iHelpPage = IDH_FINISH;

         if ( HtmlHelp( hwndMain, szHelpPath, HH_HELP_CONTEXT, iHelpPage ) == NULL )
            MessageBox( hwndMain, "FEHelp.chm could not be found or loaded.", "Cannot load help", 0 );

         return TRUE;
      }
#endif

   case WM_CTLCOLORSTATIC:
      switch ( GetDlgCtrlID( ( HWND ) lParam ) )
      {
      case IDC_BANNER:
         {
            static HFONT hFont = NULL;
            return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_BOLD, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         }
      case IDC_SUBBANNER:
         {
            static HFONT hFont = NULL;
            return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         }
      }
      return FALSE;

   case WM_QUIT:
   case WM_CLOSE:
      PostMessage( hwndMain, WM_COMMAND, IDC_CANCEL, 0 );
      return TRUE;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDCANCEL:
         if ( MessageBox( hwndMain, "Are you sure you want to exit?", "Confirm exit", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES )
            CleanUp();
         return TRUE;

      case IDM_ABOUT:
         DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hwndMain, AboutDlgProc );
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
            *szShortcut = '\0';

            DestroyIcon( hIcon );
            ListInit( &list_Shortcuts );

            bAutoExtract = bOpenFolder = bDeleteFiles = FALSE;

            SetDlgItemText( hwndMain, IDC_NEXT, "Next >" );
            SetDlgItemText( hwndMain, IDC_CANCEL, "Cancel" );
			break;
         }

         SetDialogPage( iCurrentPage );
         return TRUE;

      case IDC_CANCEL:
         CheckSaveSettings();
         CleanUp();
         return TRUE;
      }
      return TRUE;
   }

   return FALSE;
}




/*

   SetDialogPage

   Change the child dialog page

*/
void SetDialogPage( int iPageNum )
{
   int i;

   if ( hwndStatic != NULL ) DestroyWindow( hwndStatic );

   LoadDialog( iDialogArray[ iPageNum ] );

   SetBannerText( szBannerText[ iCurrentPage ] );
   SetSubBannerText( szSubBannerText[ iCurrentPage ] );

   SetWindowPos( GetDlgItem( hwndMain, IDC_GLYPH ), HWND_TOP, DLG_X_SCALE( 440 ), 9, 0, 0, SWP_NOSIZE );
   SetWindowPos( GetDlgItem( hwndMain, IDC_INTROBMP ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
   SetWindowPos( GetDlgItem( hwndMain, IDC_TOPFRAME ), HWND_TOP, 0, 59, 0, 0, SWP_NOSIZE );
   SetWindowPos( GetDlgItem( hwndMain, IDC_WHITEBANNER ), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE );
   SetWindowPos( GetDlgItem( hwndMain, IDC_BANNER ), HWND_TOP, 22, 12, 0, 0, SWP_NOSIZE );
   SetWindowPos( GetDlgItem( hwndMain, IDC_SUBBANNER ), HWND_TOP, 44, 27, 0, 0, SWP_NOSIZE );

   DLGITEM_SETFONT( hwndMain, IDC_BACK )
   DLGITEM_SETFONT( hwndMain, IDC_NEXT )
   DLGITEM_SETFONT( hwndMain, IDC_CANCEL )

   switch ( iCurrentPage )
   {
   case SPLASH_PAGE:
      SetBannerText( szBannerText[ iCurrentPage ] );
      SetSubBannerText( szSubBannerText[ iCurrentPage ] );

      SetWindowPos( GetDlgItem( hwndStatic, IDC_URL ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
      SetWindowPos( GetDlgItem( hwndStatic, IDC_INTROTEXT ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );

      SetWindowPos( GetDlgItem( hwndStatic, IDC_FENAME ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
      SetWindowPos( GetDlgItem( hwndStatic, IDC_VERSION_DATE ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
      SetWindowPos( GetDlgItem( hwndStatic, IDC_URL ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );

      SetWindowPos( GetDlgItem( hwndMain, IDC_WHITEBANNER ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
      SetWindowPos( GetDlgItem( hwndMain, IDC_BANNER ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
      SetWindowPos( GetDlgItem( hwndMain, IDC_SUBBANNER ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
      SetWindowPos( GetDlgItem( hwndMain, IDC_TOPFRAME ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
      SetWindowPos( GetDlgItem( hwndMain, IDC_GLYPH ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
      SetWindowPos( GetDlgItem( hwndMain, IDC_INTROBMP ), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE );

      SetWindowPos( hwndStatic, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE );

      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );

      SetDlgItemText( hwndStatic, IDC_VERSION_DATE, VERSIONDATE );
      SetDlgItemText( hwndStatic, IDC_INTROTEXT, szWizIntroText );
      break;

   case ZIP_PAGE:
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

      SetDlgItemText( hwndStatic, IDC_ZIPPATH, szZipFileName );
      SetDlgItemText( hwndStatic, IDC_EXEOUT, szEXEOutPath );

      if (*szEXEOutPath ) EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_ZIPLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_ZIPPATH )
      DLGITEM_SETFONT( hwndStatic, IDC_OUTLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_OPEN )
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

      if ( bRunElevated ) CheckDlgButton( hwndStatic, IDC_RUNELEVATED, BST_CHECKED );
      if ( bSubsystem64 ) CheckDlgButton( hwndStatic, IDC_SUBSYSTEM64, BST_CHECKED );
      if ( bAutoExtract ) CheckDlgButton( hwndStatic, IDC_AUTOEXTRACT, BST_CHECKED );
      if ( bOpenFolder ) CheckDlgButton( hwndStatic, IDC_OPENFOLDER, BST_CHECKED );
      if ( bDeleteFiles ) CheckDlgButton( hwndStatic, IDC_DELETEFILES, BST_CHECKED );

      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_AUTOEXTRACT )
      DLGITEM_SETFONT( hwndStatic, IDC_OPENFOLDER )
      DLGITEM_SETFONT( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH )
      DLGITEM_SETFONT( hwndStatic, IDC_EXECLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_DELETEFILES )
      DLGITEM_SETFONT( hwndStatic, IDC_SHORTCUTLABEL )
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
      EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), TRUE );

      //
      // Build shortcut string before build
      //
      ListMoveFirst( &list_Shortcuts );
      for ( i = 0 ; i < ListCount( &list_Shortcuts ) ; ++i )
      {
         char szSCToken[ 1024 ];
         wsprintf( szSCToken, "Shortcut=%s|%s|\n", ListPeekShortcut( &list_Shortcuts ), ListPeekTarget( &list_Shortcuts ) );
         lstrcat( szShortcut, szSCToken );
         ListMoveNext( &list_Shortcuts );
      }

      hExtractThread = CreateThread( NULL, 0, Build, NULL, 0, NULL );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_STATUSLABEL )
      DLGITEM_SETFONT( hwndStatic, IDC_PROGRESS )
      break;

   case FINISHED_PAGE:
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
      EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), TRUE );

      SetDlgItemText( hwndMain, IDC_NEXT, "&New SFX" );
      SetDlgItemText( hwndMain, IDC_CANCEL, "&Finish" );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_EXECUTE )
      DLGITEM_SETFONT( hwndStatic, IDC_SAVESETTINGS )
      break;
   }
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
      SetFocus( GetDlgItem( hwndMain, IDC_NEXT ) );

      if ( iCurrentPage == SHORTCUT_PAGE )
      {
         LV_COLUMN lvColumn;

         hWndListView = GetDlgItem( hwndStatic, IDC_LIST );

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

   case WM_SETCURSOR:
      if ( iCurrentPage == SPLASH_PAGE && GetDlgCtrlID( ( HWND ) wParam ) == IDC_URL )
      {
         SetCursor( LoadCursor( ghInstance, MAKEINTRESOURCE( IDC_HAND1 ) ) );
         SetWindowLongPtr( hDlg, DWLP_MSGRESULT, 1 );
         return TRUE;
      }
      return FALSE;

   case WM_NOTIFY:
      {
         NMHDR* notify = ( NMHDR* ) lParam;

         if ( notify->hwndFrom == hWndListView )
         {
            NM_LISTVIEW * nmlv = ( NM_LISTVIEW* ) notify;

            if ( notify->code == NM_CLICK )
            {
               LV_HITTESTINFO hti;
               POINT pt;


               GetCursorPos( &pt );
               ScreenToClient( hWndListView, &pt );

               hti.pt.x = pt.x;
               hti.pt.y = pt.y;

               iSelectedIndex = ListView_HitTest( hWndListView, &hti );

               if ( iSelectedIndex >= 0 )
               {
                  LV_ITEM lv_item;

                  lv_item.mask = LVIF_PARAM;
                  lv_item.iItem = iSelectedIndex;
                  lv_item.iSubItem = 0;

                  if ( ListView_GetItem( hWndListView, &lv_item ) )
                  {
                     //
                     // Enable the 'Remove' button
                     //
                     EnableWindow( GetDlgItem( hwndStatic, IDC_REMOVE_SC ), TRUE );

                     //
                     // Redraw
                     //
                     ListView_RedrawItems( hWndListView, iSelectedIndex, iSelectedIndex );
                  }
               }
               else
               {
                  //
                  // Disable the 'Remove' button
                  //
                  EnableWindow( GetDlgItem( hwndStatic, IDC_REMOVE_SC ), FALSE );

               }
            }
         }
         return TRUE;
      }

   case WM_RBUTTONDOWN:
      {
         POINT pt = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
         HMENU hRightClick = LoadMenu( ghInstance, MAKEINTRESOURCE( IDR_MENU1 ) );
         HMENU hmenuTrackPopup = GetSubMenu( hRightClick, 0 );
         ClientToScreen( hwndStatic, &pt );
         TrackPopupMenu( hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndStatic, NULL );
         DestroyMenu( hRightClick );
         return TRUE;
      }

   case WM_PAINT:
      if ( iCurrentPage == SPLASH_PAGE )
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint( hwndStatic, &ps );
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
         DeleteDC( memdc );
         EndPaint( hwndStatic, &ps );

         return TRUE;
      }

      if ( iCurrentPage == ICON_PAGE )
      {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint ( hwndStatic, &ps );
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

         EndPaint ( hwndStatic, &ps );

         return TRUE;
      }

      return FALSE;

   case WM_DROPFILES:
      if ( iCurrentPage == ZIP_PAGE )
      {
         HDROP hDrop = ( HDROP ) wParam;
         DragQueryFile( hDrop, 0, szZipFileName, MAX_PATH );
         Open();
      }
      return TRUE;

   case WM_CTLCOLORSTATIC:
      if ( iCurrentPage == SPLASH_PAGE )
      {
         switch ( GetDlgCtrlID( ( HWND ) lParam ) )
         {
         case IDC_CLICKNEXT:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            }
         case IDC_FENAME:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_HEAVY, 24, _TEXT_BLUE_, "Verdana", FALSE, OPAQUE, WHITE_BRUSH );
            }
         case IDC_VERSION_DATE:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, 12, _TEXT_GRAY_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            }
         case IDC_INTROTEXT:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            }
         case IDC_URL:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLUE_, "MS Shell Dlg", TRUE, OPAQUE, WHITE_BRUSH );
            }
         }
      }
      return FALSE;

   case WM_LBUTTONDOWN:
      PostMessage( hwndMain, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
      return TRUE;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDCANCEL:
         if ( MessageBox( hwndMain, "Are you sure you want to exit?", "Confirm exit", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES )
            CleanUp();
         return TRUE;

      case IDM_ABOUT:
         DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hwndMain, AboutDlgProc );
         return TRUE;

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
            int i = 0;
            int x = 4;
            ListMoveFirst( &list_Shortcuts );

            while ( i < ListCount( &list_Shortcuts ) )
            {
               if ( iSelectedIndex == ListPeekUID( &list_Shortcuts ) )
               {
                  ListDeleteNode( &list_Shortcuts );
                  break;
               }
               ListMoveNext( &list_Shortcuts );
               i++;
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
                  InvalidateRect( hwndStatic, &rect, FALSE );
               }
            }
            return TRUE;
         }

      case IDC_EXEC:
         {
            TCHAR szTemp[MAX_PATH];
            GetDlgItemText( hwndStatic, IDC_EXEC, szTemp, _countof(szTemp) );
            StrTrim( szTemp, " \t\r\n" );
            EnableWindow( GetDlgItem( hwndStatic, IDC_DELETEFILES ), *szTemp != '\0' );
            return TRUE;
         }

      case IDC_EXECUTE:
         if ( ( INT_PTR ) ShellExecute( hwndMain, "open", szEXEOutPath, NULL, NULL, 0 ) < 33 )
            RaiseError( "Could not execute self extractor." );
         CheckSaveSettings();
         CleanUp();
         return TRUE;

      case IDC_RUNELEVATED:
         bRunElevated = IsDlgButtonChecked( hwndStatic, IDC_RUNELEVATED );
         return TRUE;

      case IDC_SUBSYSTEM64:
         bSubsystem64 = IsDlgButtonChecked( hwndStatic, IDC_SUBSYSTEM64 );
         return TRUE;

      case IDC_AUTOEXTRACT:
         bAutoExtract = IsDlgButtonChecked( hwndStatic, IDC_AUTOEXTRACT );
         return TRUE;

      case IDC_OPENFOLDER:
         bOpenFolder = IsDlgButtonChecked( hwndStatic, IDC_OPENFOLDER );
         return TRUE;

      case IDC_DELETEFILES:
         bDeleteFiles = IsDlgButtonChecked( hwndStatic, IDC_DELETEFILES );
         return TRUE;

      case IDC_OPEN:
         {
            OPENFILENAME ofn = { sizeof ofn };

            ofn.hwndOwner = hwndMain;
            ofn.lpstrFile = szZipFileName;
            ofn.hInstance = ghInstance;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = "ZIP Files (*.zip)\0*.zip\0All Files (*.*)\0*.*\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_SHAREAWARE;
            GetOpenFileName( &ofn );

            if ( *szZipFileName ) Open();

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
   if ( DirectoryExists( szZipFileName ) )
   {
      //
      // The user drag-and-dropped a directory. Handle gracefully.
      //
      *szZipFileName = '\0';

      MessageBox( NULL, "Directories are not supported. Select a .zip file.", "FreeExtractor Error", 0 );
      return;
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
   if ( !bAutoExtract )
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

         if ( bAutoExtract && *szExtractionPath )
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

   if ( ListCount( &list_Shortcuts ) > 0 )
   {
      if ( ListCount( &list_Shortcuts ) == 1 )
      {
         lstrcat( szConfirmMessage, " Then, the shortcut you specified will be created." );
      }
      else
      {
         char buf[ 255 ] = "";
         wsprintf( buf, " Then, the %d shortcuts you specified will be created.", ListCount( &list_Shortcuts ) );
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
   int i;
   LV_ITEM lvI;

   //
   // Empty the list in list view.
   //
   ListView_DeleteAllItems ( hWndListView );

   lvI.mask = LVIF_TEXT;
   lvI.iSubItem = 0;
   lvI.state = 0;
   lvI.pszText = "";

   ListMoveFirst( &list_Shortcuts );
   i = 0;
   while ( i < ListCount( &list_Shortcuts ) )
   {
      lvI.iItem = i;
      lvI.pszText = ListPeekShortcut( &list_Shortcuts );

      ListView_InsertItem( hWndListView, &lvI );
      ListSetUID( &list_Shortcuts, i );
      ListMoveNext( &list_Shortcuts );
      i++;
   }

   ListMoveFirst( &list_Shortcuts );
   i = 0;

   while ( i < ListCount( &list_Shortcuts ) )
   {
      ListView_SetItemText( hWndListView, i, 1, ListPeekTarget( &list_Shortcuts ) );
      ListMoveNext( &list_Shortcuts );
      i++;
   }
}




INT_PTR CALLBACK ShortcutDlgProc ( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
   case WM_INITDIALOG:
      {
         hWndListView = GetDlgItem( hwndStatic, IDC_LIST );

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
      }

#ifndef NO_HTML_HELP
   case WM_HELP:
      {
         if ( HtmlHelp( hwndMain, szHelpPath, HH_HELP_CONTEXT, IDH_SHORTCUTS ) == NULL )
            MessageBox( hwndMain, "FEHelp.chm could not be found or loaded.", "Cannot load help", 0 );
         return TRUE;
      }
#endif

   case WM_QUIT:
   case WM_CLOSE:
      {
         PostMessage( hwndMain, WM_COMMAND, IDC_CANCEL, 0 );
         return TRUE;
      }

   case WM_COMMAND:
      {
         switch ( LOWORD( wParam ) )
         {
         case IDOK:
            {
               char szLocation[ MAX_PATH ] = "";
               char szTarget[ MAX_PATH ] = "";

               //
               // Save Settings
               //
               GetDlgItemText( hDlg, IDC_SCLOCATION, szLocation, MAX_PATH );
               GetDlgItemText( hDlg, IDC_SCTARGET, szTarget, MAX_PATH );

               StrTrim( szLocation, " \t\r\n" );
               StrTrim( szTarget, " \t\r\n" );

               ListMoveLast( &list_Shortcuts );

               if ( lstrlen( szLocation ) != 0 && lstrlen( szTarget ) != 0 )
               {
                  ListPush( &list_Shortcuts, ( char * ) szLocation, ( char * ) szTarget, 0 );
               }

               //
               // Close dialog
               //
               EndDialog( hDlg, 0 );
               return TRUE;
            }

         case IDCANCEL:
            {
               //
               // Close Dialog
               //
               EndDialog( hDlg, 0 );
               return TRUE;
            }
         }
      }
   }

   return FALSE;
}

BOOL WINAPI WritePrivateProfileQuoted(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName)
{
   LPTSTR lpQuoted = _alloca((lstrlen(lpString) + 3) * sizeof(TCHAR));
   wsprintf(lpQuoted, TEXT("\"%s\""), lpString);
   return WritePrivateProfileString(lpAppName, lpKeyName, lpQuoted, lpFileName);
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
         WritePrivateProfileString( "FE", "Title", szPackageName, szINIPath );
         WritePrivateProfileString( "FE", "Website", szURL, szINIPath );
         WritePrivateProfileString( "FE", "IntroText", szIntroText, szINIPath );
         WritePrivateProfileString( "FE", "ExtractPath", szExtractionPath, szINIPath );
         WritePrivateProfileQuoted( "FE", "Execute", szExecuteCommand, szINIPath );

         WritePrivateProfileString( "FE", "RunElevated", bRunElevated ? "1" : NULL, szINIPath );
         WritePrivateProfileString( "FE", "Subsystem64", bSubsystem64 ? "1" : NULL, szINIPath );
         WritePrivateProfileString( "FE", "AutoExtract", bAutoExtract ? "1" : NULL, szINIPath );
         WritePrivateProfileString( "FE", "OpenFolder", bOpenFolder ? "1" : NULL, szINIPath );
         WritePrivateProfileString( "FE", "DeleteFiles", bDeleteFiles ? "1" : NULL, szINIPath );
      }
   }
}
