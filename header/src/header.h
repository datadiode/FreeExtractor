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
//#define PERF


#include "resource.h"
#include "..\..\common\FECommon.h"
#include "zLib/zlib.h"
#include "zLib/unzip.h"
#include "stack.h"
#include "shell.h"


#ifdef PERF
 #include <stdio.h>
#endif

#define SPLASH_PAGE        1
#define EXTRACT_PAGE       2
#define PROGRESS_PAGE      3


HWND hwndInfo;

HANDLE hThisProcess,
hINITemp;

LPVOID IoBuffer;

LPTSTR szCurrentDirectory[ MAX_PATH ],
szNoSplashScreen[ 6 ],
szThisEXE[ MAX_PATH ];

char szDefaultPath[ MAX_PATH*2 ];

LPTSTR szBannerText[] = {"",
                         "",
                         "Extraction Path",
                         "Extracting Files"},

                        szSubBannerText[] = { "",
                                              "",
                                              "Select a directory to extract files to.",
                                              "FreeExtractor is extracting the compressed files in this archive."};

int iEXEFileSize,
INIOffset,
iZipOffset = 0,
             iZipSize = 0,
                        iDeleteFiles = 0,
                                       iDialogArray[] = {0, IDD_SPLASH, IDD_PATH, IDD_PROGRESS };


BOOL bDelete = FALSE, bAboutFlag;

BOOL bOverwriteYesToAll = FALSE,
                          bOverwriteNoToAll = FALSE;

Stk stk_ExtractedFiles;
Stk stk_Shortcuts;

unsigned char HASH2[] = { 87, 111, 114, 115, 116, 32, 112, 114, 111, 103, 114, 97, 109, 32, 101, 118, 101, 114, 46 };


/*
 
   Prototypes (header-specific)
 
*/
void __cdecl InitApp( void *dummy );
void __cdecl Extract( void *dummy );
void DeleteFiles();
void SetDialogPage( int iPageNum );
int ExtractCurrentFile( unzFile uf, LPTSTR szTargetDirectory );
void ExecCommand();
BOOL CreateDirCheckError();

#endif EXTRACTOR_H