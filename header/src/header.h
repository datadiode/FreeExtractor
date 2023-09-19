/*

   header.h

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


#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#define _HEADER_


#include "resource.h"
#include "..\..\common\FECommon.h"
#include "shell.h"


#define SPLASH_PAGE        1
#define EXTRACT_PAGE       2
#define PROGRESS_PAGE      3

char szDefaultPath[ MAX_PATH ];

LPCTSTR const szBannerText[] =
{
   "",     // 1 based
   "",     // splash screen
   "Extraction Path",
   "Extracting Files"
};

LPCTSTR const szSubBannerText[] =
{
   "",     // 1 based
   "",     // splash screen
   "Select a directory to extract files to.",
   "FreeExtractor is extracting the compressed files in this archive."
};

int iZipOffset = 0;
int iZipSize = 0;
int iDeleteFiles = 0;

WORD const iDialogArray[] = { 0, IDD_SPLASH, IDD_PATH, IDD_PROGRESS };

//BOOL bOverwriteYesToAll = FALSE;
//BOOL bOverwriteNoToAll = FALSE;

/*

   Prototypes (header-specific)

*/
void InitApp();
DWORD CALLBACK Extract( void *dummy );
void SetDialogPage();
void ExecCommand();
void CreateDirCheckError();

#endif EXTRACTOR_H