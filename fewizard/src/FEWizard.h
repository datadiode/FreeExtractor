
#ifndef _wizard_h_
#define _wizard_h_

#define _WIZARD_

#include "resource.h"
#include "../../common/FECommon.h"
#include "FEHelp.h"
#include <Htmlhelp.h>
#include <shlwapi.h>


#define SPLASH_PAGE           1
#define ZIP_PAGE              2
#define OPTIONS_PAGE          3
#define ADVANCED_OPTIONS_PAGE 4
#define SHORTCUT_PAGE         5
#define ICON_PAGE             6
#define CONFIRM_PAGE          7
#define PROGRESS_PAGE         8
#define FINISHED_PAGE         9


WORD const iDialogArray[] =
{
   0,     // 1 based
   IDD_SPLASH,
   IDD_ZIPFILE,
   IDD_OPTIONS,
   IDD_ADVOPTIONS,
   IDD_SHORTCUT,
   IDD_ICON,
   IDD_CONFIRM,
   IDD_PROGRESS,
   IDD_FINISHED
};

#ifndef NO_HTML_HELP
WORD const iHelpIdArray[] =
{
   0,     // 1 based
   IDH_INTRO,
   IDH_ZIP,
   IDH_OPTIONS,
   IDH_ADVOPTIONS,
   IDH_SHORTCUTS,
   IDH_ICONSS,
   IDH_CONFIRM,
   IDH_BUILD,
   IDH_FINISH
};
#endif

LPCSTR const szBannerText[] =
{
   "",     // 1 based
   "",     // splash screen
   "Select Zip File (Step 1 of 5)",
   "Options (Step 2 of 5)",
   "Advanced Options (Step 3 of 5)",
   "Shortcuts (Step 4 of 5)",
   "Custom Icon (Step 5 of 5)",
   "Confirm Your Options",
   "Building Your Self-Extractor",
   "Finished"
};

LPCSTR const szSubBannerText[] =
{
   "",     // 1 based
   "",     // splash screen
   "Choose the source zip file for this self-extracting archive.",
   "These options will be displayed on your the splash screen.",
   "You can better tailor this self-extracting archive for your application.",
   "You may optionally create shortcuts for your end-users.",
   "You can optionally customize your self-extractor's icon.",
   "Review your options for this self-extractor.",
   "FreeExtractor is building your self extractor.",
   "You self-extracting archive was successfully built."
};

char const szWizIntroText[] = 
   "This wizard will guide you through the process of creating a self-extracting archive for Windows.\n"
   "\n"
   "For more information, consult the included documentation (by pressing F1) or visit the FreeExtractor homepage.";

char szHelpPath[ MAX_PATH ];
char szINIPath[ MAX_PATH ];

HWND hWndListView;




/*

   Prototypes (Wizard-specific)

*/
void Open();
void SetConfirmMessage();
void SetDialogPage();
void RefreshShortcuts();
INT_PTR CALLBACK ShortcutDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
void CheckSaveSettings();

#endif // _wizard_h_
