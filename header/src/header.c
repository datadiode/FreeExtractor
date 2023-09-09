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
   stkInit( &stk_ExtractedFiles );
   stkInit( &stk_Shortcuts );

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
      {
         RaiseError( "Couldn't create output directory." );
         return 1;
      }

      //
      // Extract
      //
      Extract( NULL );

      //
      // Execute post-extraction command
      //
      ExecCommand();

      //
      // Open Folder
      //
      if ( bOpenFolder )
         OpenExplorerFolder( szTargetDirectory );

      //
      // We're done.
      //
      CleanUp();
      return 1;
   }

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
   // Normal (GUI'ed) usage: Show the main dialog box
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

      //
      // Set the correct (child) dialog page
      //
      SetDialogPage( 1 );

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
      SetClassLongPtr( hwndMain, GCLP_HICON, ( LONG_PTR ) LoadIcon( ghInstance, MAKEINTRESOURCE( IDI_SETUP1 ) ) );

      ShowWindow( hwndMain, SW_SHOW );

      return TRUE;

   case WM_LBUTTONDOWN:
      PostMessage( hwndMain, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
      return TRUE;

   case WM_RBUTTONDOWN:
      {
		 POINT pt = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
         HMENU hRightClick = LoadMenu( ghInstance, MAKEINTRESOURCE( IDR_MENU1 ) );
         HMENU hmenuTrackPopup = GetSubMenu( hRightClick, 0 );
         ClientToScreen( hwndMain, &pt );
         TrackPopupMenu( hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndMain, NULL );
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
         HDC hdc = BeginPaint( hwndMain, &ps );
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
         DeleteDC( memdc );
         EndPaint( hwndMain, &ps );
         return TRUE;
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

      case IDC_NEXT:
         //
         // If bAutoExtract, skip the path dialog
         //
         if ( iCurrentPage == SPLASH_PAGE && bAutoExtract ) iCurrentPage++;


         //
         // They've chosen an output directory, and they're ready to extract.
         //
         if ( iCurrentPage == EXTRACT_PAGE )
         {
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
         }

         //
         // If we're on the last dialog page ...
         //
         if ( iCurrentPage == PROGRESS_PAGE )
         {
            //
            // Execute the command (if any)
            //
            ExecCommand();

            //
            // Open the folder in Explorer
            //
            if ( bOpenFolder )
               OpenExplorerFolder( szTargetDirectory );

            CleanUp();
         }


         iCurrentPage++;
         SetDialogPage( iCurrentPage );
         return TRUE;

      case IDM_ABOUT:
         DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hwndMain, AboutDlgProc );
         return TRUE;

      case IDC_BACK:
         iCurrentPage--;
         SetDialogPage( iCurrentPage );
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

   case WM_LBUTTONDOWN:
      PostMessage( hwndMain, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
      return TRUE;

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
         DeleteDC( memdc );
         EndPaint( hwndStatic, &ps );

         return TRUE;
      }
      return FALSE;

   case WM_RBUTTONDOWN:
      {
         //
         // Show about menu
         //
         POINT pt;
         HMENU hmenuTrackPopup;
         HMENU hRightClick = LoadMenu( ghInstance, MAKEINTRESOURCE( IDR_MENU1 ) );

         pt.x = LOWORD( lParam );
         pt.y = HIWORD( lParam );

         ClientToScreen( hwndStatic, ( LPPOINT ) & pt );

         hmenuTrackPopup = GetSubMenu( hRightClick, 0 );
         TrackPopupMenu( hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndStatic, NULL );

         DestroyMenu( hmenuTrackPopup );
         DestroyMenu( hRightClick );

         return TRUE;
      }

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
      case IDM_ABOUT:
         DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hwndMain, AboutDlgProc );
         return TRUE;

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
   HWND hProgress;
   unsigned int i, n;
   char szTempDirBuffer[MAX_PATH * 2];
   char szStatusMessage[ 255 ];

   mz_zip_archive zip_archive;
   memset(&zip_archive, 0, sizeof zip_archive);

   //
   // Open this self extractor
   //
   GetFullPathName(szTargetDirectory, MAX_PATH, szTargetDirectory, NULL);

   zip_archive.m_pIO_opaque = hFile;
   zip_archive.m_pRead = file_read_func;

   //
   // Attempt to get global (zip) info
   //
   if (!mz_zip_reader_init(&zip_archive, iZipSize, 0))
      RaiseError("Could not read SFX info. It's likely corrupt.");

   n = mz_zip_reader_get_num_files(&zip_archive);

   //
   // Prepare progress bar
   //
   hProgress = GetDlgItem(hwndStatic, IDC_PROGRESSBAR);
   SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, n));

   //
   // Enumerate files
   //
   for (i = 0; i < n; ++i)
   {
      mz_zip_archive_file_stat file_stat;
      if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
         RaiseError("Could not get file info. This archive is likely corrupted.");

      //
      // Display which file is being extracted
      //
      wsprintf(szStatusMessage, "Extracting %s ...", file_stat.m_filename);
      SetDlgItemText(hwndStatic, IDC_STATUS, szStatusMessage);

      //
      // Append the internal dir name to the target directory and create the full path
      //
      wsprintf(szTempDirBuffer, "%s%s", szTargetDirectory, file_stat.m_filename);
      CleanPath(szTempDirBuffer);
      CreateDirectoryRecursively(szTempDirBuffer);
      if (mz_zip_reader_is_file_a_directory(&zip_archive, i))
      {
         CreateDirectory(szTempDirBuffer, NULL);
      }
      else
      {
         FILETIME ft = { 0, 0 };
         HANDLE hCurrentFile = CreateFile(szTempDirBuffer, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
         if (hCurrentFile == INVALID_HANDLE_VALUE)
            RaiseError("Could not extract the current file.");

         if (!mz_zip_reader_extract_to_callback(&zip_archive, i, file_write_func, hCurrentFile, 0))
         {
            SetFilePointer(hCurrentFile, 0, NULL, FILE_BEGIN);
            SetEndOfFile(hCurrentFile);
            RaiseError("Could not extract the current file.");
         }

         LocalFileTimeToFileTime(&file_stat.m_time, &ft);
         SetFileTime(hCurrentFile, NULL, NULL, &ft);
         CloseHandle(hCurrentFile);
      }
      //
      // Increment progress bar
      //
      SendMessage(hProgress, PBM_SETPOS, i + 1, 0);
   }

   //
   // Clean up
   //
   mz_zip_reader_end(&zip_archive);

   //
   // Look pretty
   //
   SendMessage( hProgress, PBM_SETPOS, n, 0 );
   Sleep( 300 );
   PostMessage( hwndMain, WM_COMMAND, IDC_NEXT, 0 );

   return 0;
}



/*

   SetDialogPage

   Change the dialog page

*/
void SetDialogPage( int iPageNum )
{
   if ( hwndStatic != NULL ) DestroyWindow( hwndStatic );

   ShowWindow( hwndMain, SW_SHOW );

   LoadDialog( iDialogArray[ iPageNum ] );

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
      {
	     char szTmp[ _countof(szURL) ];

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
      }

   case EXTRACT_PAGE:
      {
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
      }

   case PROGRESS_PAGE:
      {
         DWORD dwThreadID;

         EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
         EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );

         DLGITEM_SETFONT( hwndStatic, IDC_TEXT )
         DLGITEM_SETFONT( hwndStatic, IDC_STATUS )

         CreateThread( NULL, 0, Extract, NULL, 0, &dwThreadID );

         break;
      }
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

   char szTempDir[ MAX_PATH ] = "";
   char *lpHeader;
   char szINIFileContents[ 16384 ];

/*
#ifdef _DEBUG // _DEBUG

   //
   // For debug builds, skip loading of metadata and display test text.
   //
   lstrcpy( szPackageName, "DEBUG SFX" );
   lstrcpy( szIntroText, "This is a debug SFX for testing purposes. There are no files in this SFX." );
   lstrcpy( szURL, "http://www.disoriented.com" );
   lstrcpy( szDefaultPath, "*** no files here (DEBUG) *** " );
   return ;

#endif // _DEBUG
*/

   //
   // Get the temp and current directories
   //
   GetCurrentDirectory( MAX_PATH, szCurrentDirectory );


   //
   // Have the SFX read itself
   //
   hThisProcess = GetCurrentProcess();
   GetModuleFileName( NULL, szThisEXE, MAX_PATH );

   hFile = CreateFile( szThisEXE,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL );

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

   lpHeader = szINIFileContents;
   while ( memcmp( lpHeader, "PK\x03\x04", 4 ) != 0 )
   {
      char token[1024];
      char *const argument = lpHeader;
      lpHeader = lstrstr( lpHeader, "\n" );
      if ( lpHeader == NULL )
         RaiseError("Could not get file info. This archive is likely corrupted.");
      *lpHeader++ = '\0';
      //
      // Load SFX vars to memory
      //
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
         if ( !lstrcmpi( token, "NoGUI" ) ) bNoGUI = lstrstr(argument, "=1") != 0;
      else
         if ( !lstrcmpi( token, "Debug" ) ) isDebug = lstrstr(argument, "=1") != 0;
      else
         if ( !lstrcmpi( token, "Name" ) ) gettoken( argument, "=", 1, szPackageName );
      else
         if ( !lstrcmpi( token, "Exec" ) ) gettoken( argument, "=", 1, szExecuteCommand );
      else
         if ( !lstrcmpi( token, "DefaultPath" ) ) gettoken( argument, "=", 1, szDefaultPath );
      else
         if ( !lstrcmpi( token, "Intro" ) ) gettoken( argument, "=", 1, szIntroText );
      else
         if ( !lstrcmpi( token, "AutoExtract" ) ) bAutoExtract = lstrstr(argument, "=1") != 0;
      else
         if ( !lstrcmpi( token, "OpenFolder" ) ) bOpenFolder = lstrstr(argument, "=1") != 0;
      else
         if ( !lstrcmpi( token, "URL" ) ) gettoken(argument, "=", 1, szURL);
      else
         if ( !lstrcmpi( token, "Shortcut" ) )
         {
            gettoken(argument, "=", 1, token);
            stkPush(&stk_Shortcuts, token);
         }
      }
   }

   //
   // Search for the start of the zip file
   //
   if ( lpHeader == NULL )
      RaiseError("Could not get file info. This archive is likely corrupted.");

   MetaDataSize = (DWORD) ( lpHeader - szINIFileContents );
   iZipOffset = EndOffset + MetaDataSize;

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


/*

   DeleteFiles

   Deletes the extract files. This is broken into another function so we can call it
   from another thread and still have the UI be responsive.

*/
DWORD CALLBACK DeleteFiles( void *dummy )
{
   HWND hProgress = GetDlgItem( hwndStatic, IDC_PROGRESSBAR );
   int iTotalFiles = stkCount( &stk_ExtractedFiles );


   //
   // Look pretty
   //
   ShowWindow( hwndMain, SW_SHOW );

   SetBannerText( "File Cleanup" );
   SetSubBannerText( "Removing temp files" );
   SetDlgItemText( hwndStatic, IDC_STATUS, "Cleaning up ..." );

   SendMessage( hProgress, PBM_SETRANGE, 0, MAKELPARAM( 0, stkCount( &stk_ExtractedFiles ) ) );
   SetDlgItemText( hwndStatic, IDC_TEXT, "The extracted files are being removed." );

   //
   // Dramatic pause
   //
   Sleep( 500 );

   //
   // Enumerate the extracted files stack
   //
   while ( stkCount( &stk_ExtractedFiles ) > 0 )
   {
      int i;
      char szCurrentFile[ MAX_PATH ],
      szDeleteMessage[ MAX_PATH + 30 ];

      //
      // Format strings
      //
      wsprintf( szCurrentFile, "%s%s", szTargetDirectory, stkPeek( &stk_ExtractedFiles ) );
      wsprintf( szDeleteMessage, "Deleting %s ...", stkPeek( &stk_ExtractedFiles ) );

      SetDlgItemText( hwndStatic, IDC_STATUS, szDeleteMessage );

      //
      // Internally, zip files delimit folders with forward slashes. So,
      // convert all forward slashes to backwards slashes.
      //
      for ( i = 0 ; szCurrentFile[ i ] != '\0' ; ++i )
      {
         if ( szCurrentFile[ i ] == '/' ) szCurrentFile[ i ] = '\\';
      }

      //
      // Delete the file (or directory)
      //
      if ( DirectoryExists( szCurrentFile ) )
         RemoveDirectory( szCurrentFile );

      if ( FileExists( szCurrentFile ) )
         DeleteFile( szCurrentFile );

      //
      // Pop the stack
      //
      stkPop( &stk_ExtractedFiles );

      //
      // Update the progress bar
      //
      SendMessage( hProgress, PBM_SETPOS, iTotalFiles - stkCount( &stk_ExtractedFiles ), 0 );

   }

   //
   // If there are any remaining files in this directory, this will fail.
   // This is intended, becase we don't want to accidentally delete
   // a file or directory underneath szTargetDirectory that existed
   // before we started.
   //
   RemoveDirectory( szTargetDirectory );

   Sleep( 500 );
   CleanUp();
   return 0;
}



void ExecCommand()
{
   BOOL bNoDelete = FALSE;

   //
   // Create shortcuts
   //
   while ( stkCount( &stk_Shortcuts ) > 0 )
   {
      char location[ MAX_PATH ];
      char target[ MAX_PATH ];

      //
      // Expand the path variables for the shortcut
      //
      gettoken( stkPeek( &stk_Shortcuts ), "|", 0, location );
      gettoken( stkPeek( &stk_Shortcuts ), "|", 1, target );

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

      //
      // Pop the stack
      //
      stkPop( &stk_Shortcuts );
   }

   //
   // If there's a command to execute...
   //
   if ( *szExecuteCommand )
   {
      char szExecCommandAndPath[ MAX_PATH ];

      CleanPath( szTargetDirectory );
      SetCurrentDirectory( szTargetDirectory );

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

            if ( CreateProcess( NULL, szExecCommandAndPath, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo ) )
            {
               //
               // It was successfully started. Hide FreeExtractor, wait for the process to
               // complete, then show FreeExtractor.
               //
               ShowWindow( hwndMain, SW_HIDE );
               WaitForSingleObject( ProcessInfo.hProcess, INFINITE );
               ShowWindow( hwndMain, SW_SHOW );
            }
            else
            {
               //
               // The command was not successfully executed. Display an error.
               //
               char szErrorMessage[ 1024 ];

               wsprintf( szErrorMessage, "%s could not be executed.", szExecCommandAndPath );
               MessageBox( NULL, szErrorMessage, "Error", _CRITICAL_ );
            }
         }
         else
         {
            //
            // It's not a Win32 PE. (Probably a 16-bit DOS exe)
            //
            bNoDelete = TRUE;
            WinExec(szExecCommandAndPath, SW_SHOW);
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
            ShellExecute( hwndMain, "open", szExecCommandAndPath, NULL, NULL, SW_SHOWDEFAULT );

            //
            // BUGBUG: If the document is successfully executed, it becomes locked and can't be
            //         deleted. An attempt to delete it will fail. However, header.exe will continue
            //         deleting the other extracted files.
         }
      }
   }

   //
   // Optionally delete the files and directories created by this SFX.
   //
   if ( iDeleteFiles && !bNoDelete )
   {
      HANDLE hDelete = CreateThread( NULL, 0, DeleteFiles, NULL, 0, NULL );
      CloseHandle( hDelete );
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
      char szWinError[ 1024 ] = "";

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
