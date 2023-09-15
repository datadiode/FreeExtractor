/*

   header.c

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
#include "header.h"
#include "resource.h"



/*

   WinMain

*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
   ghInstance = hInstance;

   //
   // Initialize the stacks
   //
   hShortcuts = CreatePopupMenu();

   //
   // Initialize the app
   //
   InitApp();

   //
   // Handle GUI-less mode
   //
   if ( bNoGUI )
   {
      //
      // Be sure output directory exists
      //
      if ( !CreateDirectoryRecursively( szTargetDirectory ) )
         RaiseError( "Couldn't create output directory." );

      //
      // Extract
      //
      Extract( NULL );
   }
   else
   {
      //
      // Determine font to use
      //
      // If Tahoma exists, use it. Else, default to "MS Shell Dlg"
      //
      if (DoesFontExist("Segoe UI"))
      {
         lstrcpy(szActiveFont, "Segoe UI");
      }
      else if (DoesFontExist("Tahoma"))
      {
         lstrcpy(szActiveFont, "Tahoma");
      }
      else
      {
         lstrcpy(szActiveFont, "MS Shell Dlg");
      }

      //
      // Normal (GUI'ed) usage: Show the main dialog box
      //
      DialogBox(ghInstance, MAKEINTRESOURCE(IDD_TEMPLATE), NULL, MainDlgProc);
   }
   return 0;
}



INT_PTR CALLBACK AboutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
   case WM_INITDIALOG:
      SetDlgItemText( hDlg, IDC_TITLE, "FreeExtractor " VERSION );
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
         DeleteObject( hbmp );
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
   switch ( message )
   {
   case WM_INITDIALOG:
      hwndMain = hDlg;

      //
      // Set the correct (child) dialog page
      //
      SetDialogPage();

      //
      // Set the window title
      //
      SetTitle( szPackageName );

      //
      // Set the banner and sub-banner text
      //
      SetBannerText( szBannerText[ iCurrentPage ] );
      SetSubBannerText( szSubBannerText[ iCurrentPage ] );

      //
      // Set the window icon
      //
      SetClassLongPtr( hDlg, GCLP_HICON, ( LONG_PTR ) LoadIcon( ghInstance, MAKEINTRESOURCE( IDI_SETUP1 ) ) );

      return TRUE;

   case WM_MOUSEACTIVATE:
      if (wParam != 1 && HIWORD(lParam) != WM_LBUTTONDOWN)
         break;
      // fall through
   case WM_LBUTTONDOWN:
      PostMessage( hDlg, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
      return TRUE;

   case WM_CONTEXTMENU:
      {
         HMENU hRightClick = LoadMenu( ghInstance, MAKEINTRESOURCE( IDR_MENU1 ) );
         HMENU hmenuTrackPopup = GetSubMenu( hRightClick, 0 );
         TrackPopupMenu( hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ), 0, hDlg, NULL );
         DestroyMenu( hRightClick );
         return TRUE;
      }

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

   case WM_PAINT:
      if ( iCurrentPage != SPLASH_PAGE )
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
         Rectangle( hdc, 0, 0, DLG_X_SCALE( 550 ), 60 );

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

   case WM_QUIT:
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

      case IDC_NEXT:
         switch ( iCurrentPage )
         {
         case SPLASH_PAGE:
            //
            // If bAutoExtract, skip the path dialog
            //
            if ( !bAutoExtract )
               break;
            iCurrentPage = EXTRACT_PAGE;
            // fall through
         case EXTRACT_PAGE:
            //
            // They've chosen an output directory, and they're ready to extract.
            //
            CleanPath( szTargetDirectory );

            if ( bAutoExtract )
            {
               lstrcat( szTargetDirectory, "\\" );
               CreateDirectoryRecursively( szTargetDirectory );
            }
            else
            {
               GetDlgItemText( hwndStatic, IDC_EXTRACTPATH, szTargetDirectory, MAX_PATH );
               lstrcat( szTargetDirectory, "\\" );
            }

            //
            // If directory doesn't already exist ...
            //
            if ( !DirectoryExists( szTargetDirectory ) && !bAutoExtract )
            {
               //
               // .. and "auto create dir" checkbox isn't checked
               //
               if ( IsDlgButtonChecked( hwndStatic, IDC_AUTOCREATEDIR ) == BST_UNCHECKED )
               {
                  //
                  // Ask the user via MessageBox.
                  //
                  if ( MessageBox( hDlg, "The target directory doesn't exist. Create it?", "Create Directory?", MB_YESNO | MB_ICONQUESTION ) != IDYES )
                     return TRUE;
               }
               //
               // The "auto create dir" checkbox *is* checked. If it doesn't exist, try to create it.
               //
               CreateDirCheckError( szTargetDirectory );
            }
            break;
         }

         iCurrentPage++;
         SetDialogPage();
         return TRUE;

      case IDM_ABOUT:
         DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hDlg, AboutDlgProc );
         return TRUE;

      case IDC_BACK:
         iCurrentPage--;
         SetDialogPage();
         return TRUE;

      case IDC_CANCEL:
         CleanUp();
         return TRUE;
      }
   }
   return FALSE;
}



/*

   ChildDialogProc

   Callback for all client dialogs.

*/
INT_PTR CALLBACK ChildDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
   case WM_INITDIALOG:
      hwndStatic = hDlg;
      ShowWindow( hwndStatic, SW_SHOW );
      return TRUE;

   case WM_SETCURSOR:
      if ( iCurrentPage == SPLASH_PAGE && GetDlgCtrlID( ( HWND ) wParam ) == IDC_URL && *szURL != '\0' )
      {
         SetCursor( LoadCursor( ghInstance, MAKEINTRESOURCE( IDC_HAND1 ) ) );
         SetWindowLongPtr( hDlg, DWLP_MSGRESULT, 1 );
         return TRUE;
      }
      return FALSE;

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
         Rectangle( hdc, 0, 0, DLG_X_SCALE( 550 ), DLG_Y_SCALE( 313 ) );

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
         EndPaint( hwndStatic, &ps );

         return TRUE;
      }
      return FALSE;

   case WM_CTLCOLORSTATIC:
      //
      // Look pretty
      //
      if ( iCurrentPage == SPLASH_PAGE )
      {
         switch ( GetDlgCtrlID( ( HWND ) lParam ) )
         {
         case IDC_SUBBANNER:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            }
         case IDC_INTROBANNER:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_BOLD, 22, _TEXT_BLUE_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            }
         case IDC_URL:
            {
               static HFONT hFont = NULL;
               return FormatControl( &hFont, ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, 13, _TEXT_BLUE_, "MS Shell Dlg", TRUE, OPAQUE, WHITE_BRUSH );
            }
         }
      }
      return FALSE;

   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDC_GLYPHBROWSE:
         {
            //
            // Browse for folder
            //
            BROWSEINFO biFindFolder;
            ITEMIDLIST *idlist;

            biFindFolder.hwndOwner = hwndMain;
            biFindFolder.pidlRoot = NULL;
            biFindFolder.pszDisplayName = "Browse";
            biFindFolder.lpszTitle = "Choose an Extraction Folder";
            biFindFolder.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;
            biFindFolder.lpfn = NULL;

            idlist = SHBrowseForFolder( &biFindFolder );

            if ( idlist )
            {
               char szFolderName[ MAX_PATH ];
               SHGetPathFromIDList( idlist, szFolderName );

               SetDlgItemText( hwndStatic, IDC_EXTRACTPATH, szFolderName );
            }

            return TRUE;
         }

      case IDC_URL:
         if ( HIWORD(wParam) == STN_CLICKED && *szURL != '\0' )
         {
            ShellExecute( hDlg, TEXT( "open" ), szURL, NULL, NULL, SW_SHOWNORMAL );
         }
         return TRUE;
      }
   }
   return FALSE;
}


/*

Miniz callbacks

*/
static size_t file_read_func(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n)
{
   HANDLE hFile = (HANDLE)pOpaque;
   DWORD dw;
   LARGE_INTEGER li;
   li.QuadPart = iZipOffset + file_ofs;
   dw = SetFilePointer(hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
   if (dw == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
      return 0;
   if (!ReadFile(hFile, pBuf, (DWORD)n, &dw, NULL))
      return 0;
   return dw;
}

static size_t file_write_func(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n)
{
   HANDLE hFile = (HANDLE)pOpaque;
   DWORD dw;
   LARGE_INTEGER li;
   li.QuadPart = file_ofs;
   dw = SetFilePointer(hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
   if (dw == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
      return 0;
   if (!WriteFile(hFile, pBuf, (DWORD)n, &dw, NULL))
      return 0;
   return dw;
}


/*

   Extract

   Open the ZIP file, enumerate the files and write them out.

*/
DWORD CALLBACK Extract( void *dummy )
{
   mz_zip_archive zip_archive;
   mz_uint i, n;

   memset( &zip_archive, 0, sizeof zip_archive );
   zip_archive.m_pIO_opaque = hFile;
   zip_archive.m_pRead = file_read_func;

   //
   // Attempt to get global (zip) info
   //
   if ( !mz_zip_reader_init( &zip_archive, iZipSize, 0 ) )
      RaiseError( "Could not read SFX info. It's likely corrupt." );

   n = mz_zip_reader_get_num_files( &zip_archive );

   //
   // Open this self extractor
   //
   GetFullPathName( szTargetDirectory, MAX_PATH, szTargetDirectory, NULL );

   //
   // Prepare progress bar
   //
   SendDlgItemMessage( hwndStatic, IDC_PROGRESSBAR, PBM_SETSTEP, 1, 0 );
   SendDlgItemMessage( hwndStatic, IDC_PROGRESSBAR, PBM_SETRANGE, 0, MAKELPARAM( 0, n ) );

   //
   // Enumerate files
   //
   for ( i = 0 ; i < n ; ++i )
   {
      char szTempDirBuffer[ MAX_PATH * 2 ];
      char szStatusMessage[ MAX_PATH + 30 ];

      mz_zip_archive_file_stat file_stat;
      if ( !mz_zip_reader_file_stat( &zip_archive, i, &file_stat ) )
         RaiseError( "Could not get file info. This archive is likely corrupted." );

      //
      // Display which file is being extracted
      //
      wsprintf( szStatusMessage, "Extracting %s ...", file_stat.m_filename );
      SetDlgItemText( hwndStatic, IDC_STATUS, szStatusMessage );

      //
      // Create the full path
      //
      wsprintf( szTempDirBuffer, "%s%s", szTargetDirectory, file_stat.m_filename );
      CleanPath( szTempDirBuffer );
      CreateDirectoryRecursively( szTempDirBuffer );
      if ( mz_zip_reader_is_file_a_directory( &zip_archive, i) )
      {
         CreateDirectory( szTempDirBuffer, NULL );
      }
      else
      {
         FILETIME ft = { 0, 0 };
         HANDLE hCurrentFile = CreateFile( szTempDirBuffer, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
         if ( hCurrentFile == INVALID_HANDLE_VALUE )
            RaiseError( "Could not extract the current file." );

         if ( !mz_zip_reader_extract_to_callback( &zip_archive, i, file_write_func, hCurrentFile, 0 ) )
         {
            SetFilePointer( hCurrentFile, 0, NULL, FILE_BEGIN );
            SetEndOfFile( hCurrentFile );
            RaiseError( "Could not extract the current file." );
         }

         LocalFileTimeToFileTime( &file_stat.m_time, &ft );
         SetFileTime( hCurrentFile, NULL, NULL, &ft );
         CloseHandle( hCurrentFile );
      }

      //
      // Increment progress bar
      //
      SendDlgItemMessage( hwndStatic, IDC_PROGRESSBAR, PBM_STEPIT, 0, 0 );
#ifdef _DEBUG
      Sleep( 500 );
#endif
   }

   //
   // Look pretty
   //
   Sleep( 300 );

   //PostMessage( hwndMain, WM_COMMAND, IDC_NEXT, 0 );
   //
   // If we're on the last dialog page, execute the command (if any)
   //
   ExecCommand();

   //
   // Open the folder in Explorer
   //
   if ( bOpenFolder )
      OpenExplorerFolder( szTargetDirectory );

   if ( iDeleteFiles )
   {
      ShowWindow( hwndMain, SW_SHOW );

      //
      // Look pretty
      //
      SetBannerText( "File Cleanup" );
      SetSubBannerText( "Removing temp files" );
      SetDlgItemText( hwndStatic, IDC_STATUS, "Cleaning up ..." );

      SetDlgItemText( hwndStatic, IDC_TEXT, "The extracted files are being removed." );

      //
      // Prepare progress bar
      //
      SendDlgItemMessage(hwndStatic, IDC_PROGRESSBAR, PBM_SETPOS, 0, 0);

      //
      // Dramatic pause
      //
      Sleep( 500 );

      //
      // Enumerate files in reverse order
      //
      while ( i-- )
      {
         char szTempDirBuffer[ MAX_PATH * 2 ];
         char szStatusMessage[ MAX_PATH + 30 ];

         mz_zip_archive_file_stat file_stat;
         if ( !mz_zip_reader_file_stat( &zip_archive, i, &file_stat ) )
            RaiseError( "Could not get file info. This archive is likely corrupted." );

         //
         // Display which file is being deleted
         //
         wsprintf( szStatusMessage, "Deleting %s ...", file_stat.m_filename );
         SetDlgItemText( hwndStatic, IDC_STATUS, szStatusMessage );

         //
         // Create the full path
         //
         wsprintf( szTempDirBuffer, "%s%s", szTargetDirectory, file_stat.m_filename );
         CleanPath( szTempDirBuffer );

         //
         // Delete the file (or directory)
         //
         if ( mz_zip_reader_is_file_a_directory( &zip_archive, i) )
            RemoveDirectory( szTempDirBuffer );
         else
            DeleteFile( szTempDirBuffer );

         //
         // Increment progress bar
         //
         SendDlgItemMessage( hwndStatic, IDC_PROGRESSBAR, PBM_STEPIT, 0, 0 );
#ifdef _DEBUG
         Sleep( 500 );
#endif
      }

      //
      // If there are any remaining files in this directory, this will fail.
      // This is intended, becase we don't want to accidentally delete
      // a file or directory underneath szTargetDirectory that existed
      // before we started.
      //
      RemoveDirectory( szTargetDirectory );

      Sleep( 500 );
   }

   mz_zip_reader_end( &zip_archive );
   CleanUp();
   return 0;
}



/*

   SetDialogPage

   Change the dialog page

*/
void SetDialogPage()
{
   char szTmp[ _countof(szURL) ];

   if ( hwndStatic != NULL ) DestroyWindow( hwndStatic );

   ShowWindow( hwndMain, SW_SHOW );

   LoadDialog( iDialogArray[ iCurrentPage ] );

   EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
   EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

   SetBannerText( szBannerText[ iCurrentPage ] );
   SetSubBannerText( szSubBannerText[ iCurrentPage ] );

   ShowWindow( GetDlgItem( hwndMain, IDC_WATERMARK ), SW_SHOW );

   //
   // Set the default location of IDD_TEMPLATE UI elements
   //
   SetWindowPos( GetDlgItem( hwndMain, IDC_TOPFRAME ), HWND_TOP, 0, 59, 0, 0, SWP_NOSIZE );
   SetWindowPos( GetDlgItem( hwndMain, IDC_BANNER ), HWND_TOP, 22, 12, 0, 0, SWP_NOSIZE );
   SetWindowPos( GetDlgItem( hwndMain, IDC_SUBBANNER ), HWND_TOP, 44, 27, 0, 0, SWP_NOSIZE );

   //
   // Set Fonts
   //
   DLGITEM_SETFONT( hwndMain, IDC_BACK )
   DLGITEM_SETFONT( hwndMain, IDC_NEXT )
   DLGITEM_SETFONT( hwndMain, IDC_CANCEL )
   DLGITEM_SETFONT( hwndMain, IDC_WATERMARK )

   switch ( iCurrentPage )
   {
   case SPLASH_PAGE:
      //
      // Move the UI elements off screen
      //
      SetWindowPos( GetDlgItem( hwndMain, IDC_BANNER ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
      SetWindowPos( GetDlgItem( hwndMain, IDC_SUBBANNER ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );
      SetWindowPos( GetDlgItem( hwndMain, IDC_TOPFRAME ), HWND_BOTTOM, 1000, 1000, 0, 0, SWP_NOSIZE );

      SetWindowPos( hwndStatic, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE );

      ShowWindow( GetDlgItem( hwndMain, IDC_WATERMARK ), SW_HIDE );

      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );

      SetDlgItemText( hwndStatic, IDC_INTROBANNER, szPackageName );
      SetDlgItemText( hwndStatic, IDC_SUBBANNER, szIntroText );

      //
      // If the URL is a mailto, parse it and display only the email address
      //
      if ( lstrcpyn( szTmp, szURL, sizeof "mailto:" ) && !lstrcmpi( szTmp, "mailto:" ) )
      {
         gettoken( szURL + 7, "?", 0, szTmp );
         SetDlgItemText( hwndStatic, IDC_URL, szTmp );
      }
      else
      {
         SetDlgItemText( hwndStatic, IDC_URL, szURL );
      }

      if ( bAutoExtract )
         SetDlgItemText( hwndMain, IDC_NEXT, "&Extract" );

      break;

   case EXTRACT_PAGE:
      //
      // Set default target directory
      //
      SetDlgItemText( hwndStatic, IDC_EXTRACTPATH, *szDefaultPath ? szDefaultPath : szTargetDirectory );

      SendDlgItemMessage( hwndStatic, IDC_EXTRACTPATH, EM_LIMITTEXT, 150, 0 );
      SendDlgItemMessage( hwndStatic, IDC_AUTOCREATEDIR, BM_SETCHECK, BST_CHECKED, 0 );

      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
      EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), TRUE );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_EXTRACTPATH )
      DLGITEM_SETFONT( hwndStatic, IDC_GLYPHBROWSE )
      DLGITEM_SETFONT( hwndStatic, IDC_AUTOCREATEDIR )

      break;

   case PROGRESS_PAGE:
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
      EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );

      DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
      DLGITEM_SETFONT( hwndStatic, IDC_STATUS )

      CloseHandle( CreateThread( NULL, 0, Extract, NULL, 0, NULL ) );

      break;
   }
}



/*

   InitApp

   Open self extractor, reads the embedded variables and sets the global options for this SFX.

*/
void InitApp()
{
   int i;

   DWORD MetaDataOffset = 0;
   DWORD EndOffset = 0;
   DWORD MetaDataSize = 0;

   IMAGE_DOS_HEADER mz;
   IMAGE_NT_HEADERS nt;

   DWORD dwDummy;

   char szThisEXE[ MAX_PATH ];
   char szINIFileContents[ 16384 ];

#ifdef _DEBUG
   lstrcpy( szThisEXE, "FESetup.zip" );
   lstrcpy( szDefaultPath, "$curdir$\\ExtractionTestFolder" );
   iDeleteFiles = TRUE;
#else
   //
   // Have the SFX read itself
   //
   GetModuleFileName(NULL, szThisEXE, MAX_PATH);
#endif

   hFile = CreateFile( szThisEXE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

#ifdef _DEBUG
   iZipSize = GetFileSize( hFile, NULL );
#else
   //
   // Look for start of the metadata
   //
   if ( !ReadFile( hFile, &mz, sizeof mz, &dwDummy, NULL ) || ( dwDummy != sizeof mz ) )
      RaiseError( "Could not read the source SFX." );
   MetaDataOffset = SetFilePointer( hFile, mz.e_lfanew, NULL, FILE_BEGIN );
   if ( MetaDataOffset == INVALID_SET_FILE_POINTER )
      RaiseError( "Could not read the source SFX." );
   if ( !ReadFile( hFile, &nt, sizeof nt, &dwDummy, NULL ) || ( dwDummy != sizeof nt ) )
      RaiseError( "Could not read the source SFX." );
   if ( nt.Signature != IMAGE_NT_SIGNATURE )
      RaiseError( "Could not read the source SFX." );
   if ( nt.FileHeader.SizeOfOptionalHeader != sizeof nt.OptionalHeader )
      RaiseError( "Could not read the source SFX." );
   for ( i = 0 ; i < nt.FileHeader.NumberOfSections ; ++i )
   {
      IMAGE_SECTION_HEADER sh;
      if ( !ReadFile( hFile, &sh, sizeof sh, &dwDummy, NULL ) || ( dwDummy != sizeof sh ) )
         RaiseError( "Could not read the source SFX." );
      EndOffset = ( DWORD ) ( sh.PointerToRawData + sh.SizeOfRawData );
      if ( MetaDataOffset < EndOffset )
         MetaDataOffset = EndOffset;
   }

   EndOffset = SetFilePointer( hFile, MetaDataOffset, NULL, FILE_BEGIN );
   if ( EndOffset == INVALID_SET_FILE_POINTER )
      RaiseError( "Could not read the source SFX." );

   if ( !ReadFile( hFile, szINIFileContents, _countof(szINIFileContents) - 1, &dwDummy, NULL ) )
      RaiseError( "Could not read the source SFX." );

   szINIFileContents[ dwDummy ] = '\0';

   lstrcpy( szPackageName, "Unnamed Archive" );

   while ( MetaDataSize + 4 <= dwDummy && memcmp(szINIFileContents + MetaDataSize, "PK\x03\x04", 4 ) != 0 )
   {
      char token[1024];
      char *const argument = szINIFileContents + MetaDataSize;
      MetaDataSize += lstrlen(argument) + 1;
      if ( *gettoken( argument, "=", 0, token ) )
      {
         if ( !lstrcmpi( token, "ZipSize" ) )
         {
            char *p = token;
            gettoken(argument, "=", 1, token);
            iZipSize = 0;
            while ( *p )
            {
               iZipSize *= 10;
               iZipSize += *p++ - '0';
            }
         }
      else
         if ( !lstrcmpi( token, "Delete" ) ) iDeleteFiles = lstrstr( argument, "=1" ) != 0;
      else
         if ( !lstrcmpi( token, "NoGUI" ) ) bNoGUI = lstrstr( argument, "=1" ) != 0;
      else
         if ( !lstrcmpi( token, "Debug" ) ) isDebug = lstrstr( argument, "=1" ) != 0;
      else
         if ( !lstrcmpi( token, "Name" ) ) gettoken( argument, "=", 1, szPackageName );
      else
         if ( !lstrcmpi( token, "Exec" ) ) gettoken( argument, "=", 1, szExecuteCommand );
      else
         if ( !lstrcmpi( token, "DefaultPath" ) ) gettoken( argument, "=", 1, szDefaultPath );
      else
         if ( !lstrcmpi( token, "Intro" ) ) gettoken( argument, "=", 1, szIntroText );
      else
         if ( !lstrcmpi( token, "AutoExtract" ) ) bAutoExtract = lstrstr( argument, "=1" ) != 0;
      else
         if ( !lstrcmpi( token, "OpenFolder" ) ) bOpenFolder = lstrstr( argument, "=1" ) != 0;
      else
         if ( !lstrcmpi( token, "URL" ) ) gettoken( argument, "=", 1, szURL );
      else
         if ( !lstrcmpi( token, "Shortcut" ) )
         {
            gettoken( argument, "=", 1, token );
            AppendMenu( hShortcuts, MF_STRING, 0, token );
         }
      }
   }

   //
   // Search for the start of the zip file
   //
   if ( MetaDataSize + 4 > dwDummy )
      RaiseError( "Could not get file info. This archive is likely corrupted." );

   iZipOffset = EndOffset + MetaDataSize;
#endif

   //
   // Replace return carriages in intro text.
   //
   for ( i = 0 ; szIntroText[ i ] != '\0' ; ++i )
   {
      if ( ( szIntroText[ i ] == '\\' ) && ( szIntroText[ i + 1 ] == 'n' ) )
      {
         szIntroText[ i ] = 0x0D;
         szIntroText[ i + 1 ] = 0x0A;
      }
   }

   //
   // Parse and expand the default extraction folder
   //
   if ( *szDefaultPath == '\0' )
   {
      if ( bAutoExtract )
      {
         //
         // AutoExtract is enabled and no default path was specified, so use $temp$.
         //
         lstrcpy( szDefaultPath, "$temp$" );
      }
      else
      {
         //
         // Otherwise, use the current directory
         //
         lstrcpy( szDefaultPath, "$curdir$" );
      }
   }

   ParsePath( szDefaultPath );
   CleanPath( szDefaultPath );

   lstrcpy( szTargetDirectory, szDefaultPath );

}


void ExecCommand()
{
   int n = GetMenuItemCount( hShortcuts );
   int i;

   //
   // Create shortcuts
   //
   for ( i = 0 ; i < n ; ++i )
   {
      char buf[ MAX_PATH * 2 ];
      char location[ MAX_PATH ];
      char target[ MAX_PATH ];

      GetMenuString( hShortcuts, i, buf, _countof(buf), MF_BYPOSITION );

      //
      // Expand the path variables for the shortcut
      //
      gettoken( buf, "|", 0, location );
      gettoken( buf, "|", 1, target );

      ParsePath( location );
      ParsePath( target );

      CleanPath( location );
      CleanPath( target );

      //
      // If the target or shortcut location directories don't exist, create them.
      //
      CreateDirectoryRecursively( location );
      CreateDirectoryRecursively( target );

      //
      // Create Shortcut
      //
      CreateShortCut( location, NULL, target );
   }

   //
   // If there's a command to execute...
   //
   if ( *szExecuteCommand )
   {
      char szExecCommandAndPath[ MAX_PATH ];

      CleanPath( szTargetDirectory );

      lstrcpy( szExecCommandAndPath, szExecuteCommand );
      ParsePath( szExecCommandAndPath );
      CleanPath( szExecCommandAndPath );

      //
      // Execute command
      //
      if ( IsExtension( szExecCommandAndPath, "exe" ) ||
           IsExtension( szExecCommandAndPath, "com" ) ||
           IsExtension( szExecCommandAndPath, "bat" ) )
      {
         if ( ( *szExecCommandAndPath == '"' ) || ExeType( szExecCommandAndPath ) )
         {
            //
            // It's a 32-bit PE.
            //
            PROCESS_INFORMATION ProcessInfo;
            STARTUPINFO StartupInfo = { sizeof StartupInfo };

            if ( CreateProcess( NULL, szExecCommandAndPath, NULL, NULL, FALSE, 0, NULL, szTargetDirectory, &StartupInfo, &ProcessInfo ) )
            {
               //
               // It was successfully started. Hide FreeExtractor, wait for the process to
               // complete, then show FreeExtractor.
               //
               ShowWindow( hwndMain, SW_HIDE );
               CloseHandle( ProcessInfo.hThread );
               WaitForSingleObject( ProcessInfo.hProcess, INFINITE );
               CloseHandle( ProcessInfo.hProcess );
            }
            else
            {
               //
               // The command was not successfully executed. Display an error.
               //
               char szErrorMessage[ 1024 ];

               wsprintf( szErrorMessage, "%s could not be executed.", szExecCommandAndPath );
               MessageBox( hwndMain, szErrorMessage, "Error", _CRITICAL_ );
            }
         }
         else
         {
            //
            // It's not a Win32 PE. (Probably a 16-bit DOS exe)
            //
            iDeleteFiles = FALSE;
            SetCurrentDirectory( szTargetDirectory );
            WinExec( szExecCommandAndPath, SW_SHOW );
         }
      }
      else
      {
         //
         // If the exec command contains a supported document, shellexecute it.
         //
         if ( IsExtension( szExecCommandAndPath, "doc" ) ||           // Word Doc
              IsExtension( szExecCommandAndPath, "xls" ) ||           // Excel Doc
              IsExtension( szExecCommandAndPath, "prc" ) ||           // Palm App
              IsExtension( szExecCommandAndPath, "pdf" ) ||           // Adobe Acrobat
              IsExtension( szExecCommandAndPath, "ppt" ) ||           // Powerpoint Doc
              IsExtension( szExecCommandAndPath, "txt" ) ||           // Text File
              IsExtension( szExecCommandAndPath, "htm" ) ||           // HTML File
              IsExtension( szExecCommandAndPath, "tml" ) ||           // HTML File (with ".html" extension)
              IsExtension( szExecCommandAndPath, "xml" ) ||           // XML File
              IsExtension( szExecCommandAndPath, "mp3" ) ||           // mp3 File
              IsExtension( szExecCommandAndPath, "chm" ) )            // Microsoft Compiled HTML Help
         {
            ShellExecute( hwndMain, "open", szExecCommandAndPath, NULL, szTargetDirectory, SW_SHOWDEFAULT );

            //
            // BUGBUG: If the document is successfully executed, it becomes locked and can't be
            //         deleted. An attempt to delete it will fail. However, header.exe will continue
            //         deleting the other extracted files.
         }
      }
   }
}



void CreateDirCheckError()
{
   if ( !CreateDirectoryRecursively( szTargetDirectory ) )
   {
      //
      // Directory couldn't be created.
      //
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
      wsprintf( buf, "Could not create the target directory %s. %s\n"
                "Be sure that it does not contain an invalid character: \n"
                "/ \\ : * ? \" < > | ", szTargetDirectory, szWinError );

      MessageBox( hwndMain, buf, "Couldn't create directory", _CRITICAL_ );
   }
}
