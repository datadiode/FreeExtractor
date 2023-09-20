@echo off
REM Read the version number from FEcommon.h
for /F "tokens=1,2,3" %%G in (%~dp0..\common\FEcommon.h) do if "%%G,%%H" == "#define,VERSION" set VERSION=%%I
echo VERSION=%VERSION:"=%
REM Create the ZIP
del "%~dp0FESetup.zip"
"%ProgramFiles%\7-zip\7z.exe" a -mx9 "%~dp0FESetup.zip" "%~dp0..\docs\Icons\" "%~dp0FEHelp.chm" "%~dp0FEWizard.exe" "%~dp0MakeSFX.exe"
REM Create the SFX
makesfx.exe ^
/zip="%~dp0FESetup.zip" ^
/sfx="%~dp0FESetup.exe" ^
/title="FreeExtractor Setup %VERSION:"=%" ^
/website="https://github.com/datadiode/FreeExtractor" ^
/intro="This self-extracting archive will extract and setup FreeExtractor to run on your machine." ^
/defaultpath="$programfiles$\FreeExtractor" ^
/runelevated ^
/overwrite ^
/openexplorerwindow ^
/shortcut="$sendto$\Convert Zip to SFX.lnk|$targetdir$\FEWizard.exe" ^
/shortcut="$desktop$\FreeExtractor Wizard.lnk|$targetdir$\FEWizard.exe"
