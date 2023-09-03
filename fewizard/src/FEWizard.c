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
BOOL IsZipFile (LPTSTR szFileName)
{
   HANDLE handle;
   char buf[3];
   DWORD dwRead;

   handle = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

   SetFilePointer(handle, 0, 0, FILE_BEGIN );
   ReadFile(handle, (LPVOID)buf, 4, &dwRead, NULL);

   if ( buf[ 0 ] == 'P' && buf[ 1 ] == 'K' && buf[ 2 ] == 0x03 && buf[ 3 ] == 0x04 )
   {
      CloseHandle( handle );
      return 1;
   }
      
   return 0;
}

/*
 
   SkipProgramName 

   Used for parsing the command line.

*/
LPTSTR SkipProgramName ( LPTSTR lpCmdLine )
{
   LPTSTR p = lpCmdLine;
   BOOL bInQuotes = FALSE;

   for ( p; *p; p = CharNext( p ) )
   {
      if ( ( *p == TEXT( ' ' ) || *p == TEXT( '\t' ) ) && !bInQuotes )
         break;

      if ( *p == TEXT( '\"' ) )
         bInQuotes = !bInQuotes;
   }

   while ( *p == TEXT( ' ' ) || *p == TEXT( '\t' ) )
      p++;

   return ( p );
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpXXXCmdLine, int nCmdShow )
{
   int slashpos = 0;
   char buf[ MAX_PATH ] = "";
   LPTSTR lpCmdLine = GetCommandLine ();

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
   if ( DoesFontExist( "Tahoma" ) )
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
   GetModuleFileName( NULL, szHelpPath, MAX_PATH );
   GetFolderFromPath( szHelpPath, buf );

   lstrcpy( szHelpPath, buf );
   lstrcpy( szINIPath, szHelpPath );

   lstrcat( szHelpPath, "\\FEHelp.chm" );
   lstrcat( szINIPath, "\\default.ini" );


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

      bDeleteFiles = ( BOOL ) GetPrivateProfileInt( "FE", "DeleteFiles", 0, szINIPath );
      bOpenFolder = ( BOOL ) GetPrivateProfileInt( "FE", "OpenFolder", 0, szINIPath );
      bAutoExtract = ( BOOL ) GetPrivateProfileInt( "FE", "AutoExtract", 0, szINIPath );

      //
      // Replace return carriages in intro text.
      //
      while ( szIntroText[ i ] != '\0' )
      {
         if ( ( ( char ) szIntroText[ i ] == '\\' ) && ( ( char ) szIntroText[ i + 1 ] == 'n' ) )
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
   lstrcpy( buf, trim( SkipProgramName( lpCmdLine ) ) );

   // remove leading and trailing quotes

   if (left(buf, 1) == "\"" && right(buf, 1) == "\"")
      lstrcpy(buf, mid(buf, 1, lstrlen(buf)-2));
      

   
   if ( lstrlen( buf ) > 0 )
   {
      if ( FileExists( buf ) )
      {
         char buf2[ MAX_PATH ] = "";
         GetFullPathName( buf, MAX_PATH, buf2, NULL );
         lstrcpy( szZipFileName, buf2 );
         if (!IsZipFile(szZipFileName))
            RaiseError ("The file you have specified is not a ZIP file.");
         Open();
      }
      
   }

   //
   // We're done initializing. Show the main dialog.
   //
   DialogBox( ghInstance, MAKEINTRESOURCE( IDD_TEMPLATE ), NULL, MainDlgProc );
   return 1;
}






BOOL CALLBACK AboutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
      
   case WM_INITDIALOG:
      {
         char buf[255];
         wsprintf(buf, "FreeExtractor %s", VERSION);
         SetDlgItemText (hDlg, IDC_TITLE, buf);
         return TRUE;
      }
      
   case WM_LBUTTONDOWN:
      {
         PostMessage( hDlg, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
         return TRUE;
      }
      
   case WM_CTLCOLORSTATIC:
      {
         if ( ( HWND ) lParam == GetDlgItem( hDlg, IDC_URL ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, 13, _TEXT_BLUE_, "MS Shell Dlg", TRUE, OPAQUE, WHITE_BRUSH );
         if ( ( HWND ) lParam == GetDlgItem( hDlg, IDC_TITLE ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_BOLD, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         if ( ( HWND ) lParam == GetDlgItem( hDlg, IDC_TEXT ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         
         return TRUE;
      }

      /*
      case 0x020A:
      {
         if ( wParam & 0x0004 )
         {
            bAboutFlag = TRUE;
            SetDlgItemText (hDlg, IDC_TEXT, HASH2);
         }
         return TRUE;
      }
      */

      case WM_PAINT:
      {
         HDC hdc, memdc;
         HBITMAP hbmp;
         PAINTSTRUCT ps;

         hdc = BeginPaint ( hDlg, &ps );

         //
         // Draw white banner background
         //
         SetROP2( hdc, R2_WHITE );
         Rectangle( hdc, 0, 0, 500, 500 );


         //
         // Draw box icon
         //
         memdc = CreateCompatibleDC( NULL );

         hbmp = LoadBitmap( ghInstance, MAKEINTRESOURCE( IDB_BITMAP1 ) );
         SelectObject ( memdc, hbmp );
         
         StretchBlt( ps.hdc,
            8, 18,
            43, 42,
            memdc,
            0, 0, 43, 42,
            SRCCOPY );
         

         ReleaseDC ( hDlg, hdc );
         EndPaint ( hDlg, &ps );

         return TRUE;
      }

      case WM_COMMAND:
      {
         switch ( LOWORD( wParam ) )
         {
            case IDOK:
            {
               EndDialog( hDlg, 1 );
               UpdateWindow( hwndStatic );
               UpdateWindow( hwndMain );
               return TRUE;
            }
            case IDC_URL:
            {
               ShellExecute( hDlg, TEXT( "open" ), TEXT( "http://www.disoriented.com/" ), NULL, NULL, SW_SHOWNORMAL );
               return TRUE;
            }

         }
      }
   }
   return FALSE;
}








/*
 
   MainDlgProc
 
   Main callback
 
*/
BOOL CALLBACK MainDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
      case WM_INITDIALOG:
      {
         hwndMain = hDlg;

         GetTempPath( MAX_PATH, szTempDir );

         SetTitle( "FreeExtractor Wizard" );

         //
         // Check to see if szZipFileName is already filled in (in the case of being passed
         // from the command line.
         //
         if ( lstrlen( szZipFileName ) > 0 )
         {
            iCurrentPage = 3;  // options page
         }

         else
         {
            iCurrentPage = 1;  // splash page
         }


         SetDialogPage( iCurrentPage );



         SetClassLong( hwndMain, GCL_HICON, ( long ) LoadIcon( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDI_SETUP1 ) ) );

         return TRUE;
      }


      case WM_LBUTTONDOWN:
      {
         PostMessage( hwndMain, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
         return TRUE;
      }


      case WM_PAINT:
      {
         if ( iCurrentPage != SPLASH_PAGE )
         {
            HDC hdc, memdc;
            HBITMAP hbmp;
            PAINTSTRUCT ps;

            hdc = BeginPaint ( hwndMain, &ps );

            SetROP2( hdc, R2_WHITE );

            Rectangle( hdc, 0, 0, DLG_X_SCALE( 600 ), 60 );

            //
            // Draw box icon in the upper right
            //
            memdc = CreateCompatibleDC( NULL );
            
            hbmp = LoadBitmap( ghInstance, MAKEINTRESOURCE( IDB_BITMAP1 ) );
            SelectObject ( memdc, hbmp );
            
            //
            // Copy it
            //
            StretchBlt( ps.hdc,
               DLG_X_SCALE( 440 ), DLG_Y_SCALE( 9 ),
               43, 42,
               memdc,
               0, 0, 43, 42,
               SRCCOPY );
            

            ReleaseDC ( hwndMain, hdc );
            EndPaint ( hwndMain, &ps );

            return TRUE;
         }

         return FALSE;
      }

      case WM_DROPFILES:
      {
         if ( iCurrentPage == ZIP_PAGE )
         {
            HDROP hDrop = ( HDROP ) wParam;
            DragQueryFile( hDrop, 0, szZipFileName, MAX_PATH );
            Open();
         }
         return TRUE;
      }

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
      {
         if ( ( HWND ) lParam == GetDlgItem( hwndMain, IDC_BANNER ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_BOLD, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         if ( ( HWND ) lParam == GetDlgItem( hwndMain, IDC_SUBBANNER ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
         return TRUE;
      }

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
            case 0x0000002:
            {
               if ( MessageBox( hwndMain, "Are you sure you want to exit?", "Confirm exit", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES ) CleanUp();
               return TRUE;
            }

            case IDM_ABOUT:
            {
               DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hwndMain, AboutDlgProc );
               return TRUE;
            }

            case IDC_NEXT:
            {
               if ( iCurrentPage == OPTIONS_PAGE )
               {
                  GetDlgItemText( hwndStatic, IDC_PACKAGENAME, szPackageName, 255 );
                  if ( lstrlen( szPackageName ) > 0 )
                     lstrcpy( szPackageName, trim( szPackageName ) );

                  GetDlgItemText( hwndStatic, IDC_URL, szURL, 128 );
                  if ( lstrlen( szURL ) > 0 )
                     lstrcpy( szURL, trim( szURL ) );

                  GetDlgItemText( hwndStatic, IDC_INTROTEXT, szIntroText, 1024 );
                  if ( lstrlen( szIntroText ) > 0 )
                     lstrcpy( szIntroText, trim( szIntroText ) );
               }


               if ( iCurrentPage == ADVANCED_OPTIONS_PAGE )
               {
                  char buf[ MAX_PATH ];

                  GetDlgItemText( hwndStatic, IDC_EXEC, buf, MAX_PATH );
                  if ( lstrlen( buf ) > 0 )
                     lstrcpy( szExecuteCommand, trim( buf ) );

                  GetDlgItemText( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH, buf, MAX_PATH );
                  if ( lstrlen( buf ) > 0 )
                     lstrcpy( szExtractionPath, trim( buf ) );

                  if ( !IsWindowEnabled( GetDlgItem( hwndStatic, IDC_DELETEFILES ) ) )
                     bDeleteFiles = FALSE;
               }


               if ( iCurrentPage == FINISHED_PAGE )
               {
                  //
                  // If the "Save Settings ..." checkbox is checked, write out
                  // the default settings to an INI file
                  //
                  CheckSaveSettings();

                  //
                  // Reset and start over
                  //
                  iCurrentPage = 1;

                  lstrcpy( szZipFileName, "" );
                  lstrcpy( szEXEOutPath, "" );

                  lstrcpy( szPackageName, "" );
                  lstrcpy( szURL, "" );
                  lstrcpy( szConfirmMessage, "" );
                  lstrcpy( szIntroText, "" );
                  lstrcpy( szExecuteCommand, "" );
                  lstrcpy( szExtractionPath, "" );

                  DestroyIcon( hIcon );
                  ListInit( &list_Shortcuts );

                  bAutoExtract = bOpenFolder = bDeleteFiles = FALSE;

                  SetDlgItemText( hwndMain, IDC_NEXT, "Next >" );
                  SetDlgItemText( hwndMain, IDC_CANCEL, "Cancel" );
                  SetDialogPage( iCurrentPage );
                  return TRUE;
               }

               iCurrentPage++;
               SetDialogPage( iCurrentPage );
               return TRUE;
            }

            case IDC_BACK:
            {
               if ( iCurrentPage == OPTIONS_PAGE )
               {
                  GetDlgItemText( hwndStatic, IDC_PACKAGENAME, szPackageName, 255 );
                  if ( lstrlen( szPackageName ) > 0 ) lstrcpy( szPackageName, trim( szPackageName ) );

                  GetDlgItemText( hwndStatic, IDC_URL, szURL, 128 );
                  if ( lstrlen( szURL ) > 0 ) lstrcpy( szURL, trim( szURL ) );

                  GetDlgItemText( hwndStatic, IDC_INTROTEXT, szIntroText, 1024 );
                  if ( lstrlen( szIntroText ) > 0 ) lstrcpy( szIntroText, trim( szIntroText ) );
               }

               if ( iCurrentPage == ADVANCED_OPTIONS_PAGE )
               {
                  char buf[ MAX_PATH ];

                  GetDlgItemText( hwndStatic, IDC_EXEC, buf, MAX_PATH );
                  if ( lstrlen( buf ) > 0 )
                     lstrcpy( szExecuteCommand, trim( buf ) );

                  GetDlgItemText( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH, buf, MAX_PATH );
                  if ( lstrlen( buf ) > 0 )
                     lstrcpy( szExtractionPath, trim( buf ) );

                  if ( !IsWindowEnabled( GetDlgItem( hwndStatic, IDC_DELETEFILES ) ) )
                     bDeleteFiles = FALSE;
               }

               iCurrentPage--;
               SetDialogPage( iCurrentPage );
               return TRUE;
            }

            case IDC_CANCEL:
            {
               CheckSaveSettings();
               CleanUp();
               return TRUE;
            }
         }
      }
   }

   return FALSE;
}




/*
 
   SetDialogPage
 
   Change the child dialog page
 
*/
void SetDialogPage( int iPageNum )
{
   LPTSTR szTemp = "";

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

   MAIN_SETFONT( IDC_BACK )
   MAIN_SETFONT( IDC_NEXT )
   MAIN_SETFONT( IDC_CANCEL )

   switch ( iCurrentPage )
   {
      case SPLASH_PAGE:
      {
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

         return ;
      }


      case ZIP_PAGE:
      {
         EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
         EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

         SetDlgItemText( hwndStatic, IDC_ZIPPATH, ( LPCTSTR ) szZipFileName );
         SetDlgItemText( hwndStatic, IDC_EXEOUT, ( LPCTSTR ) szEXEOutPath );

         if ( lstrlen( szEXEOutPath ) > 0 ) EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );

         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_ZIPLABEL )
         CHILD_SETFONT( IDC_ZIPPATH )
         CHILD_SETFONT( IDC_OUTLABEL )
         CHILD_SETFONT( IDC_OPEN )
         CHILD_SETFONT( IDC_EXEOUT )

         return ;
      }


      case OPTIONS_PAGE:
      {
         SendMessage( GetDlgItem( hwndStatic, IDC_PACKAGENAME ), EM_LIMITTEXT, ( WPARAM ) 75, 0 );
         SendMessage( GetDlgItem( hwndStatic, IDC_INTROTEXT ), EM_LIMITTEXT, ( WPARAM ) 675, 0 );
         SendMessage( GetDlgItem( hwndStatic, IDC_URL ), EM_LIMITTEXT, ( WPARAM ) 200, 0 );

         SetDlgItemText( hwndStatic, IDC_URL, ( LPCTSTR ) szURL );
         SetDlgItemText( hwndStatic, IDC_INTROTEXT, ( LPCTSTR ) szIntroText );

         if ( lstrlen( szPackageName ) == 0 ) SetDlgItemText( hwndStatic, IDC_PACKAGENAME, "Unnamed Archive" );
         else SetDlgItemText( hwndStatic, IDC_PACKAGENAME, ( LPCTSTR ) szPackageName );

         EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
         EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_NAMELABEL )
         CHILD_SETFONT( IDC_WEBSITELABEL )
         CHILD_SETFONT( IDC_INTROLABEL )

         return ;
      }


      case ADVANCED_OPTIONS_PAGE:
      {
         SendMessage( GetDlgItem( hwndStatic, IDC_EXEC ), EM_LIMITTEXT, ( WPARAM ) 200, 0 );
         SendMessage( GetDlgItem( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH ), EM_LIMITTEXT, ( WPARAM ) 200, 0 );

         SetDlgItemText( hwndStatic, IDC_DEFAULT_EXTRACTION_PATH, szExtractionPath );
         SetDlgItemText( hwndStatic, IDC_EXEC, szExecuteCommand );

         if ( bOpenFolder ) SendMessage( GetDlgItem( hwndStatic, IDC_OPENFOLDER ), BM_SETCHECK, BST_CHECKED, 0 );
         if ( bAutoExtract ) SendMessage( GetDlgItem( hwndStatic, IDC_AUTOEXTRACT ), BM_SETCHECK, BST_CHECKED, 0 );
         if ( bDeleteFiles ) SendMessage( GetDlgItem( hwndStatic, IDC_DELETEFILES ), BM_SETCHECK, BST_CHECKED, 0 );

         EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
         EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_AUTOEXTRACT )
         CHILD_SETFONT( IDC_OPENFOLDER )
         CHILD_SETFONT( IDC_DEFAULT_EXTRACTION_PATH )
         CHILD_SETFONT( IDC_EXECLABEL )
         CHILD_SETFONT( IDC_DELETEFILES )
         CHILD_SETFONT( IDC_SHORTCUTLABEL )

         return ;

      }

      case SHORTCUT_PAGE:
      {
         RefreshShortcuts();
         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_LIST )
         CHILD_SETFONT( IDC_ADD_SC )
         CHILD_SETFONT( IDC_REMOVE_SC )
         return ;
      }

      case ICON_PAGE:
      {
         HDC hdc;
         PAINTSTRUCT ps;

         hdc = BeginPaint ( hwndStatic, &ps );

         if ( lstrlen( szIconPath ) != 0 )
         {
            hIcon = ExtractIcon( ghInstance, szIconPath, 0 );

            Rectangle( hdc, 78, 82, 124, 128 );
            DrawIcon( hdc, 85, 89, hIcon );
         }

         ReleaseDC ( hwndStatic, hdc );
         EndPaint ( hwndStatic, &ps );

         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_NEWICON )
      }

      case CONFIRM_PAGE:
      {
         SetConfirmMessage();
         EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
         EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );

         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_CONFIRMMESSAGE )
         CHILD_SETFONT( IDC_TEXT2 )

         return ;
      }


      case PROGRESS_PAGE:
      {
         DWORD dwThreadID;
         char szSCToken[ 1024 ] = "";
         int i = 0;

         EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
         EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
         EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), TRUE );

         //
         // Build shortcut string before build
         //
         ListMoveFirst( &list_Shortcuts );
         while ( i < ListCount( &list_Shortcuts ) )
         {
            wsprintf( szSCToken, "Shortcut%d=%s|%s|\n", i, ListPeekShortcut( &list_Shortcuts ), ListPeekTarget( &list_Shortcuts ) );
            lstrcat( szShortcut, szSCToken );
            ListMoveNext( &list_Shortcuts );
            ++i;
         }

         h_ExtractThread = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE ) Build, NULL, 0, &dwThreadID );

         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_STATUSLABEL )
         CHILD_SETFONT( IDC_PROGRESS )

         return ;
      }

      case FINISHED_PAGE:
      {
         EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
         EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
         EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), TRUE );

         SetDlgItemText( hwndMain, IDC_NEXT, "&New SFX" );
         SetDlgItemText( hwndMain, IDC_CANCEL, "&Finish" );

         CHILD_SETFONT( IDC_TEXT )
         CHILD_SETFONT( IDC_EXECUTE )
         CHILD_SETFONT( IDC_SAVESETTINGS )

         return ;
      }

      return ;
   }
}





/*
 
   ChildDialogProc
 
   Callback for all child dialogs. I'm too lazy too build one for each dialog.
 
*/
BOOL CALLBACK ChildDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   static HCURSOR hHandCursor = NULL;
   static HCURSOR hRegularCursor = NULL;
   static BOOL underline_link;

   switch ( message )
   {
      case WM_INITDIALOG:
      {
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

         if ( iCurrentPage == SPLASH_PAGE )
         {
            underline_link = TRUE;
            hHandCursor = LoadCursor( ghInstance, MAKEINTRESOURCE( IDC_HAND1 ) );
            hRegularCursor = LoadCursor( NULL, IDC_ARROW );
         }
         return 1;
      }

      case WM_MOUSEMOVE:
      {
         POINT pt = { LOWORD( lParam ), HIWORD( lParam ) };
         HWND hChild = ChildWindowFromPoint( hwndStatic, pt );

         if ( iCurrentPage == SPLASH_PAGE && hChild == GetDlgItem( hwndStatic, IDC_URL ) )
         {
            SetCursor( hHandCursor );
         }
         return TRUE;
      }


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

      case WM_PAINT:
      {
         if ( iCurrentPage == SPLASH_PAGE )
         {
            HDC hdc, memdc;
            HBITMAP hbmp;
            PAINTSTRUCT ps;

            hdc = BeginPaint ( hwndStatic, &ps );

            SetROP2( hdc, R2_WHITE );

            Rectangle( hdc, 0, 0, DLG_X_SCALE( 600 ), DLG_X_SCALE( 350 ) );

            //
            // Draw intro bitmap
            //
            memdc = CreateCompatibleDC( NULL );
            hbmp = LoadBitmap( ghInstance, MAKEINTRESOURCE( IDB_INTRO ) );
            SelectObject ( memdc, hbmp );
            
            //
            // Copy it and scale it (if nessessary)
            //
            StretchBlt( ps.hdc,
               0, 0,
               DLG_X_SCALE( 163 ), DLG_Y_SCALE( 312 ),
               memdc,
               0, 0, 163, 312,
               SRCCOPY );
            

            ReleaseDC ( hwndStatic, hdc );
            EndPaint ( hwndStatic, &ps );

            return TRUE;
         }

         if ( iCurrentPage == ICON_PAGE )
         {
            HDC hdc;
            PAINTSTRUCT ps;

            hdc = BeginPaint ( hwndStatic, &ps );
            SetBkMode( hdc, OPAQUE );

            if ( lstrlen( szIconPath ) != 0 )
            {
               hIcon = ExtractIcon( ghInstance, szIconPath, 0 );
               Rectangle( hdc, 78, 82, 124, 128 );
               DrawIcon( hdc, 85, 89, hIcon );
            }

            ReleaseDC ( hwndStatic, hdc );
            EndPaint ( hwndStatic, &ps );

            return 0 ;
         }

         return FALSE;
      }

      case WM_DROPFILES:
      {
         if ( iCurrentPage == ZIP_PAGE )
         {
            HDROP hDrop = ( HDROP ) wParam;
            DragQueryFile( hDrop, 0, szZipFileName, MAX_PATH );
            Open();
         }
         return TRUE;
      }

      case WM_CTLCOLORSTATIC:
      {
         if ( iCurrentPage == SPLASH_PAGE )
         {
            if ( ( HWND ) lParam == GetDlgItem( hwndStatic, IDC_CLICKNEXT ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            if ( ( HWND ) lParam == GetDlgItem( hwndStatic, IDC_FENAME ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_HEAVY, 24, _TEXT_BLUE_, "Verdana", FALSE, OPAQUE, WHITE_BRUSH );
            if ( ( HWND ) lParam == GetDlgItem( hwndStatic, IDC_VERSION_DATE ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, 12, _TEXT_GRAY_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            if ( ( HWND ) lParam == GetDlgItem( hwndStatic, IDC_INTROTEXT ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLACK_, szActiveFont, FALSE, OPAQUE, WHITE_BRUSH );
            if ( ( HWND ) lParam == GetDlgItem( hwndStatic, IDC_URL ) ) return FormatControl( ( HWND ) lParam, ( HDC ) wParam, FW_MEDIUM, iFontSize, _TEXT_BLUE_, "MS Shell Dlg", TRUE, OPAQUE, WHITE_BRUSH );
         }

         return FALSE;
      }

      /*
            case WM_LBUTTONDOWN:
            {
               PostMessage( hwndMain, WM_NCLBUTTONDOWN, HTCAPTION, 0 );
               return TRUE;
            }
      */

      case WM_LBUTTONDOWN:
      {
         POINT pt = { LOWORD( lParam ), HIWORD( lParam ) };
         HWND hChild = ChildWindowFromPoint( hwndStatic, pt );
         if ( hChild == GetDlgItem( hwndStatic, IDC_URL ) )
         {
            ShellExecute( hDlg, TEXT( "open" ), TEXT( WEBSITE_URL ), NULL, NULL, SW_SHOWNORMAL );

         }
         else PostMessage( hwndMain, WM_NCLBUTTONDOWN, HTCAPTION, 0 );

         return TRUE;
      }



      case WM_COMMAND:
      {
         switch ( LOWORD( wParam ) )
         {
            case 0x0000002:
            {
               if ( MessageBox( hwndMain, "Are you sure you want to exit?", "Confirm exit", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES ) CleanUp();
               return TRUE;
            }

            
            case IDM_ABOUT:
            {
               DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ABOUT ), hwndMain, AboutDlgProc );
               return TRUE;
            }
            

            case IDC_ADD_SC:
            {
               //
               // Show 'Add Shortcut' dialog
               //
               DialogBox( ghInstance, MAKEINTRESOURCE( IDD_ADDSHORTCUT ), hwndMain, ShortcutDlgProc );
               RefreshShortcuts();
               return TRUE;
            }

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
               OPENFILENAME ofn;
               char szIconPathTemp[ MAX_PATH ] = "";

               ZeroMemory( &ofn, sizeof( ofn ) );

               EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
               EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
               EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), FALSE );
               EnableWindow( GetDlgItem( hwndStatic, IDC_NEWICON ), FALSE );

               ofn.lStructSize = sizeof( ofn );
               ofn.lpstrFile = szIconPathTemp;
               ofn.hInstance = ghInstance;
               ofn.nMaxFile = MAX_PATH;
               ofn.lpstrFilter = "Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
               ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_SHAREAWARE;
               GetOpenFileName( &ofn );

               //
               // Verify Icon
               //
               if ( lstrlen( szIconPathTemp ) != 0 )
               {
                  HDC hdc = GetDC( hwndStatic );
                  HANDLE Icon;
                  Icon = CreateFile( szIconPathTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

                  if ( GetFileSize ( Icon, NULL ) != 2238 )
                  {
                     MessageBox( hwndMain, "The specified icon is in an unsupported format. It must be a 2,238 byte, 32x32x256 color icon.\n", "Invalid Icon", 0 );
                  }
                  else
                  {
                     lstrcpy( szIconPath, szIconPathTemp );
                     bChangeIcon = TRUE;

                     hIcon = ExtractIcon( ghInstance, szIconPath, 0 );

                     Rectangle( hdc, 78, 82, 124, 128 );
                     DrawIcon( hdc, 85, 89, hIcon );

                     ReleaseDC ( hwndStatic, hdc ) ;
                  }

                  CloseHandle( Icon );
               }

               //
               // Update Icon on child dialog
               //
               UpdateWindow( hwndStatic );

               //
               // Reset UI
               //
               EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
               EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );
               EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), TRUE );
               EnableWindow( GetDlgItem( hwndStatic, IDC_NEWICON ), TRUE );

               return TRUE;
            }


            case IDC_EXEC:
            {
               LPTSTR szTemp = "";

               GetDlgItemText( hwndStatic, IDC_EXEC, szTemp, MAX_PATH );
               EnableWindow( GetDlgItem( hwndStatic, IDC_DELETEFILES ), lstrlen( trim( szTemp ) ) > 0 );

               return TRUE;
            }

            case IDC_EXECUTE:
            {
               if ( ( int ) ShellExecute( hwndMain, "open", szEXEOutPath, NULL, NULL, 0 ) < 33 ) RaiseError( "Could not execute self extractor." );
               else
               {
                  CheckSaveSettings();
                  CleanUp();
               }
               return TRUE;
            }

            case IDC_AUTOEXTRACT:
            {
               if ( BST_CHECKED == SendMessage( GetDlgItem( hwndStatic, IDC_AUTOEXTRACT ), BM_GETCHECK, 0, 0 ) ) bAutoExtract = TRUE;
               else bAutoExtract = FALSE;
               return TRUE;
            }

            case IDC_DELETEFILES:
            {
               if ( BST_CHECKED == SendMessage( GetDlgItem( hwndStatic, IDC_DELETEFILES ), BM_GETCHECK, 0, 0 ) ) bDeleteFiles = TRUE;
               else bDeleteFiles = FALSE;
               return TRUE;
            }

            case IDC_OPENFOLDER:
            {
               if ( BST_CHECKED == SendMessage( GetDlgItem( hwndStatic, IDC_OPENFOLDER ), BM_GETCHECK, 0, 0 ) ) bOpenFolder = TRUE;
               else bOpenFolder = FALSE;
               return TRUE;
            }

            case IDC_OPEN:
            {
               OPENFILENAME ofn;

               ZeroMemory( &ofn, sizeof( ofn ) );

               EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
               EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), FALSE );
               EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), FALSE );
               EnableWindow( GetDlgItem( hwndStatic, IDC_OPEN ), FALSE );

               ofn.lStructSize = sizeof( ofn );
               ofn.lpstrFile = szZipFileName;
               ofn.hInstance = ghInstance;
               ofn.nMaxFile = MAX_PATH;
               ofn.lpstrFilter = "ZIP Files (*.zip)\0*.zip\0All Files (*.*)\0*.*\0";
               ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_SHAREAWARE;
               GetOpenFileName( &ofn );

               if ( lstrlen( szZipFileName ) != 0 ) Open();

               EnableWindow( GetDlgItem( hwndMain, IDC_BACK ), TRUE );
               EnableWindow( GetDlgItem( hwndMain, IDC_CANCEL ), TRUE );
               EnableWindow( GetDlgItem( hwndStatic, IDC_OPEN ), TRUE );

            }
         }
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
   int iSuffix = 1;

   if ( DirectoryExists( szZipFileName ) )
   {
      //
      // The user drag-and-dropped a directory. Handle gracefully.
      //
      lstrcpy( szZipFileName, "" );

      MessageBox( NULL, "Directories are not supported. Select a .zip file.", "FreeExtractor Error", 0 );
      return ;
   }

   SetDlgItemText( hwndStatic, IDC_ZIPPATH, szZipFileName );

   wsprintf( szEXEOutPath, "%s.exe", left( szZipFileName, lstrlen( szZipFileName ) - 4 ) );

   //
   // If the output file already exists, generate a new output file name.
   //
   while ( FileExists( szEXEOutPath ) )
   {
      if ( FileExists( szEXEOutPath ) )
      {
         wsprintf( szEXEOutPath, "%s-%i.exe", left( szZipFileName, lstrlen( szZipFileName ) - 4 ), iSuffix );
      }

      iSuffix++;
      if ( iSuffix >= 50 ) break;
   }

   SetDlgItemText( hwndStatic, IDC_EXEOUT, szEXEOutPath );

   if ( lstrlen( szZipFileName ) > 0 ) EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), TRUE );
   else
   {
      EnableWindow( GetDlgItem( hwndMain, IDC_NEXT ), FALSE );
      SetDlgItemText( hwndStatic, IDC_EXEOUT, " " );
   }

}






/*
 
   SetConfirmMessage
 
   Based on the user's settings, format a confirmation message of the SFX's
   tasks, Not pretty, but it works....
 
*/
void SetConfirmMessage()
{
   lstrcpy( szConfirmMessage, "" );
   if ( !bAutoExtract )
   {
      lstrcat( szConfirmMessage, "The user will be given the option to enter a path to extract the files to" );

      if ( lstrlen( szExtractionPath ) > 0 )
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
      lstrcat( szConfirmMessage, "The files in this archive will automatically be extracted to " );

      if ( lstrlen( szExtractionPath ) == 0 ) lstrcat( szConfirmMessage, "the user's temp directory " );
      else
      {
         lstrcat( szConfirmMessage, "\"" );
         lstrcat( szConfirmMessage, szExtractionPath );
         lstrcat( szConfirmMessage, "\" " );
      }
      lstrcat( szConfirmMessage, "without prompting the user. " );
   }

   if ( lstrlen( szExecuteCommand ) > 0 || bOpenFolder )
   {
      lstrcat( szConfirmMessage, "When the file extraction has finished, FreeExtractor will " );

      if ( lstrlen( szExecuteCommand ) > 0 )
      {
         lstrcat( szConfirmMessage, "execute \"" );
         lstrcat( szConfirmMessage, szExecuteCommand );
         lstrcat( szConfirmMessage, "\"" );

         if ( bOpenFolder ) lstrcat( szConfirmMessage, " and " );
         else lstrcat( szConfirmMessage, "." );
      }

      if ( bOpenFolder )
      {
         lstrcat( szConfirmMessage, "automatically open the target folder" );

         if ( bAutoExtract && lstrlen( szExtractionPath ) > 0 )
         {
            lstrcat( szConfirmMessage, " (\"" );
            lstrcat( szConfirmMessage, szExtractionPath );
            lstrcat( szConfirmMessage, "\"). " );

         }
         else
            lstrcat ( szConfirmMessage, ". " );
      }
   }

   if ( bDeleteFiles ) lstrcat( szConfirmMessage, " When completed, the extracted files will be deleted." );
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




BOOL CALLBACK ShortcutDlgProc ( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
   switch ( message )
   {
      case WM_INITDIALOG:
      {
         hWndListView = GetDlgItem( hwndStatic, IDC_LIST );

         FormatText( GetDlgItem( hDlg, IDC_MAINTEXT ), szActiveFont, iFontSize, FALSE);
         FormatText( GetDlgItem( hDlg, IDC_SHORTCUTLABEL ), szActiveFont, iFontSize, FALSE );
         FormatText( GetDlgItem( hDlg, IDC_SCLOCATION ), szActiveFont, iFontSize, FALSE );
         FormatText( GetDlgItem( hDlg, IDC_LOCATIONEXAMPLE ), szActiveFont, iFontSize, FALSE );
         FormatText( GetDlgItem( hDlg, IDC_TARGETLABEL ), szActiveFont, iFontSize, FALSE );
         FormatText( GetDlgItem( hDlg, IDC_SCTARGET ), szActiveFont, iFontSize, FALSE );
         FormatText( GetDlgItem( hDlg, IDC_TARGETEXAMPLE ), szActiveFont, iFontSize, FALSE );
         FormatText( GetDlgItem( hDlg, IDOK ), szActiveFont, iFontSize, FALSE );
         FormatText( GetDlgItem( hDlg, IDCANCEL ), szActiveFont, iFontSize, FALSE );

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

               lstrcpy( szLocation, trim( szLocation ) );
               lstrcpy( szTarget, trim( szTarget ) );

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


/*
 
  CheckSaveSettings
 
  If the "Save Settings ..." checkbox is checked, write out the default settings
  to an INI file.
 
*/
void CheckSaveSettings()
{
   if ( iCurrentPage == FINISHED_PAGE )
   {
      if ( BST_CHECKED == SendMessage( GetDlgItem( hwndStatic, IDC_SAVESETTINGS ), BM_GETCHECK, 0, 0 ) )
      {


         //
         // Replace return carriages in intro text.
         //
         /*
         while ( szIntroText[ i ] != '\0' )
      {
            if ( ( ( char ) szIntroText[ i ] == '\\' ) && ( ( char ) szIntroText[ i + 1 ] == 'n' ) )
            {
               szIntroText[ i ] = ' ';
               szIntroText[ i + 1 ] = '\n';
            }
            i++;
      }

         */



         WritePrivateProfileString( "FE", "Title", szPackageName, szINIPath );
         WritePrivateProfileString( "FE", "Website", szURL, szINIPath );
         WritePrivateProfileString( "FE", "IntroText", szIntroText, szINIPath );
         WritePrivateProfileString( "FE", "ExtractPath", szExtractionPath, szINIPath );
         WritePrivateProfileString( "FE", "Execute", szExecuteCommand, szINIPath );

         if ( bDeleteFiles ) WritePrivateProfileString( "FE", "DeleteFiles", "1", szINIPath );
         if ( bOpenFolder ) WritePrivateProfileString( "FE", "OpenFolder", "1", szINIPath );
         if ( bAutoExtract ) WritePrivateProfileString( "FE", "AutoExtract", "1", szINIPath );
      }
   }
}