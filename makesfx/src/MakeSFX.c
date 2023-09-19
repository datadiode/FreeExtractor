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
BOOL bDisableSplashScreen = FALSE;
BOOL isWarning = FALSE;
BOOL isError = FALSE;
BOOL bOverWrite = FALSE;
BOOL isHelp = FALSE;

int iErrorCount = 0;
int iWarningCount = 0;

int main( int argc, char *argv[] )
{
   int i = 0;
   char token[ 1024 ];
   char *p = szShortcut;

   __int64 liStart;
   __int64 liEnd;
   __int64 liFreq;

   double fCompileTime;
   double fBandwidth = 0;

   HANDLE hFile;

   QueryPerformanceFrequency((LARGE_INTEGER*)&liFreq);

   //
   // Parse command line arguments
   //
   while ( argc != 0 )
   {
      char *const argument = argv[ --argc ];

      //
      // String variables
      //
      if ( *gettoken( argument, "=", 0, token ) )
      {
         if ( !_strcmpi( token, "/title" ) ) gettoken( argument, "=", 1, szPackageName );
      else
         if ( !_strcmpi( token, "/exec" ) ) gettoken( argument, "=", 1, szExecuteCommand );
      else
         if ( !_strcmpi( token, "/website" ) ) gettoken( argument, "=", 1, szURL );
      else
         if ( !_strcmpi( token, "/intro" ) ) gettoken( argument, "=", 1, szIntroText );
      else
         if ( !_strcmpi( token, "/defaultpath" ) ) gettoken( argument, "=", 1, szExtractionPath );
      else
         if ( !_strcmpi( token, "/zip" ) ) gettoken( argument, "=", 1, szZipFileName );
      else
         if ( !_strcmpi( token, "/sfx" ) ) gettoken( argument, "=", 1, szEXEOutPath );
      else
         if ( !_strcmpi( token, "/icon" ) )
         {
            bChangeIcon = TRUE;
            gettoken( argument, "=", 1, szIconPath );
         }
      else
         //
         // Parse the shortcut in the following format:
         //
         //    /shortcut="Shortcut|Directory"
         //
         if ( !_strcmpi( token, "/shortcut" ) && i < 16 )
         {
            gettoken( argument, "=", 1, token );
            p += wsprintf( p, "Shortcut=%s", token ) + 1;
            ++i;
         }
      }
   else
      //
      // Boolean variables
      //
      {
         if ( !_strcmpi( token, "/runelevated" ) ) bRunElevated = TRUE;
      else
         if ( !_strcmpi( token, "/subsystem64" ) ) bSubsystem64 = TRUE;
      else
         if ( !_strcmpi( token, "/autoextract" ) ) bAutoExtract = TRUE;
      else
         if ( !_strcmpi( token, "/openexplorerwindow" ) ) bOpenFolder = TRUE;
      else
         if ( !_strcmpi( token, "/overwrite" ) ) bOverWrite = TRUE;
      else
         if ( !_strcmpi( token, "/delete" ) ) bDeleteFiles = TRUE;
      else
         if ( !_strcmpi( token, "/debug" ) ) isDebug = TRUE;          // Not used yet.
      else
         if ( !_strcmpi( token, "/nogui" ) )
         {
            bAutoExtract = TRUE;
            bDisableSplashScreen = TRUE;
            bNoGUI = TRUE;
            strcpy( szPackageName, "[GUI-less archive]" );
         }
      else
         if ( !_strcmpi( token, "/?" ) || !_strcmpi( token, "-?" ) || !_strcmpi( token, "/help" ) || !_strcmpi( token, "-help" ) ) isHelp = TRUE;
      }
   }

   *p++ = '\0';

   if ( isHelp )
   {
      puts( "MakeSFX "VERSIONDATE"\n"
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
            "  /icon=\"MyIcon.ico\"                  Changes the SFX icon. This must be a\n"
            "                                      2,238 byte, 32x32, 256 color icon.\n"
            "more ..." );

      _getch();

      puts( "  /exec=\"setup.exe\"                   Command to execute after extraction\n"
            "  /defaultpath=\"$desktop$\\Folder\"     Default extraction path. Valid variables\n"
            "                                      are: $desktop$, $temp$, $programfiles$,\n"
            "                                      $windir$, $targetdir$ and $sysdir$. You\n"
            "                                      may also hard-code a path.\n\n"
            "  /runelevated                        Run as administrator.\n"
            "  /subsystem64                        Run in 64 bit subsystem.\n"
            "  /autoextract                        Automatically extract the contents to\n"
            "                                      the default path. Don't prompt the user.\n"
            "  /openexplorerwindow                 Open the target directory in Windows\n"
            "                                      Explorer.\n"
            "  /delete                             Deletes extracted files after program\n"
            "                                      specified in /exec is completed.\n"
            "  /shortcut=\"shortcut.lnk|target.ext\" Creates a shortcut to target. Each may\n"
            "                                      include scriptable paths.\n"
            "more ..." );
      _getch();

      puts( "® MakeSFX Options ¯\n\n"
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
            "MakeSFX is open source. Visit "WEBSITE_URL" for updates." );

      return 1;
   }

   if ( strlen( szPackageName ) == 0 && !bNoGUI ) strcpy( szPackageName, "Unnamed Archive" );

   if ( isDebug )
   {
      printf( "----------------Compiling: %s (Debug) ----------------\n", szPackageName );
      strcat( szPackageName, " (DEBUG)" );
   }
   else
   {
      printf( "--------------------Compiling: %s---------------------\n", szPackageName );
   }



   //
   // Check for errors
   //
   if ( strlen( szZipFileName ) == 0 )
   {
      printf( "¯ ERROR: The source zip file name not specified.\n" );
      isError = TRUE;
      iErrorCount++;
   }
   else if ( !FileExists( szZipFileName ) )
   {
      printf( "¯ ERROR: The source zip file could not be found.\n" );
      isError = TRUE;
      iErrorCount++;
   }

   if ( strlen( szEXEOutPath ) == 0 )
   {
      printf( "¯ ERROR: The output self-extractor file name was not specified.\n" );
      isError = TRUE;
      iErrorCount++;
   }
   else if ( !bOverWrite && FileExists( szEXEOutPath ) )
   {
      printf( "¯ ERROR: The output EXE already exists. Use the /overwrite option to overwrite.\n" );
      isError = TRUE;
      iErrorCount++;
   }

   if ( bChangeIcon )
   {
      HANDLE hIcon = CreateFile( szIconPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
      if ( hIcon == INVALID_HANDLE_VALUE )
      {
         printf( "¯ ERROR: The specified icon could not be found.\n" );
         isError = TRUE;
         iErrorCount++;
      }
      else
      {
         if ( GetFileSize ( hIcon, NULL ) != 2238 )
         {
            printf( "¯ ERROR: The specified icon is in an unsupported size or format. It must be a 2,238 byte, 32x32x256 color icon.\n" );
            isError = TRUE;
            iErrorCount++;
         }
         CloseHandle( hIcon );
      }
   }

   if ( !_strcmpi( szPackageName, "Unnamed Archive" ) && !bNoGUI && !isError )
   {
      printf( "¯ WARNING: Self-extractor title not specified. Using 'Unnamed Archive'.\n" );
      iWarningCount++;
   }

   if ( !strlen( szExtractionPath ) && bNoGUI )
   {
      printf( "¯ WARNING: GUI-less option specified without specifying default extraction path. Using temp directory.\n" );
      iWarningCount++;
   }

   if ( bNoGUI )
   {
      printf( "¯ WARNING: GUI-less SFX specified. /title, /website, and /intro variables ignored. \n" );
      iWarningCount++;
      strcpy( szPackageName, "" );
      strcpy( szURL, "" );
      strcpy( szIntroText, "" );
   }

   if ( isError )
   {
      printf( "\n%s - %i error(s), %i warning(s) - ", szPackageName, iErrorCount, iWarningCount );
      printf( "Run \"MakeSFX.exe /?\" for help.\n" );
      return 1;
   }

   //
   // We're cool. Build it.
   //
   QueryPerformanceCounter((LARGE_INTEGER*)&liStart);
   Build(NULL);
   QueryPerformanceCounter((LARGE_INTEGER*)&liEnd);
   printf( "\n\n%s - %i error(s), %i warning(s)\n\n", szPackageName, iErrorCount, iWarningCount );

   fCompileTime = (float)(liEnd - liStart) / liFreq;

   hFile = CreateFile(  szEXEOutPath,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

   if ( hFile != INVALID_HANDLE_VALUE )
   {
      fBandwidth = (float)GetFileSize( hFile, NULL) / (float)fCompileTime / 1024 / 1024;
      CloseHandle( hFile );
   }

   printf( "Build time: %1.2f seconds (%.2f MB/S)\n", fCompileTime, fBandwidth );

   return 0;
}
