/*
 
   MakeSFX.c
 
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
#define _MAKESFX_


#include "../../common/FECommon.h"


/*
 
   Globals
 
*/
BOOL bAutoExtract,
bDisableSplashScreen,
isWarning = FALSE,
            isError = FALSE,
                      bOverWrite = FALSE,
                                   isHelp = FALSE;

int iErrorCount = 0,
                  iWarningCount = 0,
                                  iZipFileSize;

int main( int argc, char *argv[] )
{
   int i = 0;
   char token[ 1024 ];

   __int64 liStart;
   __int64 liEnd;
   __int64 liFreq;

   double fCompileTime;
   double fBandwidth;

   HANDLE hFile;

   QueryPerformanceFrequency((LARGE_INTEGER*)&liFreq);

   //
   // Parse command line arguments
   //
   while ( argc != 0 )
   {
      strcpy( token, argv[ argc - 1 ] );

      //
      // String variables
      //
      if ( instr( 0, token, "=" ) > 0 )
      {
         char szSCToken[ 1024 ] = "";

         if ( !lstrcmpCI( left( token, 6 ), "/title" ) ) strcpy( szPackageName, mid( token, 8, lstrlen( token ) - 6 ) );
         if ( !lstrcmpCI( left( token, 5 ), "/exec" ) ) strcpy( szExecuteCommand, mid( token, 7, lstrlen( token ) - 5 ) );
         if ( !lstrcmpCI( left( token, 8 ), "/website" ) ) strcpy( szURL, mid( token, 10, lstrlen( token ) - 8 ) );
         if ( !lstrcmpCI( left( token, 6 ), "/intro" ) ) strcpy( szIntroText, mid( token, 8, lstrlen( token ) - 6 ) );
         if ( !lstrcmpCI( left( token, 12 ), "/defaultpath" ) ) strcpy( szExtractionPath, mid( token, 14, lstrlen( token ) - 12 ) );
         if ( !lstrcmpCI( left( token, 4 ), "/zip" ) ) strcpy( szZipFileName, mid( token, 6, lstrlen( token ) - 4 ) );
         if ( !lstrcmpCI( left( token, 4 ), "/sfx" ) ) strcpy( szEXEOutPath, mid( token, 6, lstrlen( token ) - 4 ) );
         if ( !lstrcmpCI( left( token, 5 ), "/icon" ) )
         {
            bChangeIcon = TRUE;
            strcpy( szIconPath, mid( token, 7, lstrlen( token ) - 4 ) );
         }

         //
         // Parse the shortcut in the following format:
         //
         //    /shortcut="Shortcut|Directory"
         //
         if ( !lstrcmpCI( left( token, 9 ), "/shortcut" ) && i < 16 )
         {
            wsprintf( szSCToken, "Shortcut%d=%s|\n", i, mid( token, 11, lstrlen( token ) - 9 ) );
            lstrcat( szShortcut, szSCToken );
            ++i;
         }
      }

      //
      // Boolean variables
      //
      else
      {
         if ( !lstrcmpCI( token, "/autoextract" ) ) bAutoExtract = TRUE;
         if ( !lstrcmpCI( token, "/openexplorerwindow" ) ) bOpenFolder = TRUE;
         if ( !lstrcmpCI( token, "/overwrite" ) ) bOverWrite = TRUE;
         if ( !lstrcmpCI( token, "/delete" ) ) bDeleteFiles = TRUE;
         if ( !lstrcmpCI( token, "/debug" ) ) isDebug = TRUE;          // Not used yet.
         if ( !lstrcmpCI( token, "/nogui" ) )
         {
            bAutoExtract = TRUE;
            bDisableSplashScreen = TRUE;
            bNoGUI = TRUE;
            strcpy( szPackageName, "[GUI-less archive]" );
         }

         if ( !lstrcmpCI( token, "/?" ) || !lstrcmp( token, "-?" ) || !lstrcmp( token, "/help" ) || !lstrcmp( token, "-help" ) ) isHelp = TRUE;
      }
      --argc;
   }

   if ( isHelp )
   {

      WriteConsoleOut( "MakeSFX "VERSIONDATE"\n"
                       "\n"
                       "Converts a zip file into a 32-bit GUI Windows self-extractor.\n\n"
                       "® Example usage ¯\n\n"
                       "  makesfx.exe /zip=\"source.zip\" /sfx=\"output.exe\" [/title=\"Your Title\"]\n"
                       "  [/website=\"http://www.example.com\"] [/intro=\"This is a test self extractor\"]\n"
                       "  [/defaultpath=\"$desktop$\\My Files\"] [/autoextract] [/openexplorerwindow]\n"
                       "  [/shortcut=\"$desktop$\\Program Shortcut.lnk|$targetdir$\\Program.exe]\n"
                       "  [/delete] [/icon=\"MyIcon.ico\"] [/overwrite] [/?]\n\n"
                       "® Self-extractor options ¯\n\n"
                       "  /zip=\"file.zip\"                     The source zip file (Required)\n"
                       "  /sfx=\"out.exe\"                      The output self-extractor (Required)\n"
                       "  /title=\"Your Title\"                 The user-friendly name\n"
                       "  /website=\"http://www.example.com\"   The embedded URL\n"
                       "  /intro=\"This is the intro.\"         The splash screen intro text\n"
                       "  /icon=\"MyIcon.ico\"                  Changes the SFX icon. This must be a \n"
                       "                                      2,238 byte, 32x32, 256 color icon.\n"
                       "more ...\n" );

      getch();

      WriteConsoleOut( "  /exec=\"setup.exe\"                   Command to execute after extraction\n"
                       "  /defaultpath=\"$desktop$\\Folder\"     Default extraction path. Valid variables\n"
                       "                                      are: $desktop$, $temp$, $programfiles$,\n"
                       "                                      $windir$, $targetdir$ and $sysdir$. You \n"
                       "                                      may also hard-code a path.\n\n"
                       "  /autoextract                        Automatically extract the contents to\n"
                       "                                      the default path. Don't prompt the user.\n"
                       "  /openexplorerwindow                 Open the target directory in Windows\n"
                       "                                      Explorer.\n"
                       "  /delete                             Deletes extracted files after program\n"
                       "                                      specified in /exec is completed.\n"
                       "  /shortcut=\"shortcut.lnk|target.ext\" Creates a shortcut to target. Each may\n"
                       "                                      include scriptable paths.\n"
                       "more ...\n" );
      getch();

      WriteConsoleOut(
         "® MakeSFX Options ¯\n\n"
         "   /overwrite                         Overwrite the output EXE, if it exists.\n"
         "   /?                                 Show this help message\n\n"
         "® Path Variables ¯\n\n"
         "  The following variables can be used in paths:\n"
         "    $desktop$           The user's desktop folder\n"
         "    $programfiles$      Computer's program files folder\n"
         "    $temp$              System Temp Directory \n"
         "    $windir$            Windows directory (i.e. \"c:\\windows\") \n"
         "    $sysdir$            Windows System Directory (i.e. \"c:\\windows\\system\") \n"
         "    $curdir$            The directory where the self-extractor is run from\n"
         "    $targetdir$         The extraction directory \n"
         "    $favorites$         Internet Explorer Favorites Folder \n"
         "    $quicklaunch$       Quick Launch Directory (IE4+ only)\n"
         "    $startup$           The User's \"Startup\" Folder \n"
         "    $startmenu$         The User's \"Programs\" Folder in the Start Menu \n\n"
         "MakeSFX is open source. Visit "WEBSITE_URL" for updates.\n" );

      return 1;
   }

   if ( lstrlen( szPackageName ) == 0 && !bNoGUI ) strcpy( szPackageName, "Unnamed Archive" );

   if ( isDebug )
   {
      WriteConsoleOut( "----------------Compiling: %s (Debug) ----------------\n", szPackageName );
      lstrcat( szPackageName, " (DEBUG)" );
   }
   else
   {
      WriteConsoleOut( "--------------------Compiling: %s---------------------\n", szPackageName );
   }

   

   //
   // Check for errors
   //
   if ( lstrlen( szZipFileName ) == 0 )
   {
      WriteConsoleOut( "¯ ERROR: The source zip file name not specified.\n" );
      isError = TRUE;
      iErrorCount++;
   }

   if ( !FileExists( szZipFileName ) && lstrlen( szZipFileName ) != 0 )
   {
      WriteConsoleOut( "¯ ERROR: The source zip file could not be found.\n" );
      isError = TRUE;
      iErrorCount++;
   }

   if ( lstrlen( szEXEOutPath ) == 0 )
   {
      WriteConsoleOut( "¯ ERROR: The output self-extractor file name was not specified.\n" );
      isError = TRUE;
      iErrorCount++;
   }


   if ( !bOverWrite && FileExists( szEXEOutPath ) )
   {
      WriteConsoleOut( "¯ ERROR: The output EXE already exists. Use the /overwrite option to overwrite.\n" );
      isError = TRUE;
      iErrorCount++;
   }

   if ( bChangeIcon )
   {
      HANDLE hIcon;

      if ( !FileExists( szIconPath ) )
      {
         WriteConsoleOut( "¯ ERROR: The specified icon could not be found.\n" );
         isError = TRUE;
         iErrorCount++;
      }

      hIcon = CreateFile( szIconPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

      if ( GetFileSize ( hIcon, NULL ) != 2238 )
      {
         WriteConsoleOut( "¯ ERROR: The specified icon is in an unsupported size or format. It must be a 2,238 byte, 32x32x256 color icon.\n" );
         isError = TRUE;
         iErrorCount++;
      }

      CloseHandle( hIcon );
   }

   if ( !lstrcmpCI( szPackageName, "Unnamed Archive" ) && !bNoGUI && !isError )
   {
      WriteConsoleOut( "¯ WARNING: Self-extractor title not specified. Using 'Unnamed Archive'.\n" );
      iWarningCount++;
   }

   if ( !lstrlen( szExtractionPath ) && bNoGUI )
   {
      WriteConsoleOut( "¯ WARNING: GUI-less option specified without specifying default extraction path. Using temp directory.\n" );
      iWarningCount++;
   }

   if ( bNoGUI )
   {
      WriteConsoleOut( "¯ WARNING: GUI-less SFX specified. /title, /website, and /intro variables ignored. \n" );
      iWarningCount++;
      strcpy( szPackageName, "" );
      strcpy( szURL, "" );
      strcpy( szIntroText, "" );
   }

   if ( isError )
   {
      WriteConsoleOut( "\n%s - %i error(s), %i warning(s) - ", szPackageName, iErrorCount, iWarningCount );
      WriteConsoleOut( "Run \"MakeSFX.exe /?\" for help.\n" );
      return 1;
   }

   //
   // We're cool. Build it.
   //
   QueryPerformanceCounter((LARGE_INTEGER*)&liStart);
   Build();
   QueryPerformanceCounter((LARGE_INTEGER*)&liEnd);
   WriteConsoleOut( "\n\n%s - %i error(s), %i warning(s)\n\n", szPackageName, iErrorCount, iWarningCount );
   

   hFile = CreateFile(  szEXEOutPath,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

   fCompileTime = (float)(liEnd - liStart) / liFreq;
   fBandwidth = (float)GetFileSize( hFile, NULL) / (float)fCompileTime / 1024 / 1024;
   CloseHandle(hFile);
   
   printf("Build time: %1.2f seconds (%.2f MB/S)\n", fCompileTime, fBandwidth );
   
   return 1;
}
