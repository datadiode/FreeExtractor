@echo off
pushd "%~dp0"
for /f "tokens=*" %%G in ('where git.exe') do set git.exe="%%G"
set touch.exe=C:\msys64\usr\bin\touch.exe
REM Read the version number from FEcommon.h
for /F "tokens=1,2,3" %%G in (..\common\FEcommon.h) do if "%%G,%%H" == "#define,VERSION" set VERSION=%%I
echo VERSION=%VERSION:"=%
REM Set the mtime of all bundled files to the time of the latest commit
for /f "delims=: tokens=1,*" %%G in ('git log -1 --date=iso') do if "%%G"=="Date" set GITDATE=%%H
echo GITDATE=%GITDATE%
for /f delims^=^"^ tokens^=2 %%G in (FESetup.ddf) do %touch.exe% -d "%GITDATE%" "%~dp0%%G"
REM Create the ZIP
del "FESetup.zip"
"%ProgramFiles%\7-zip\7z.exe" a -mx9 "FESetup.zip" "..\docs\Icons\" "FEHelp.chm" "FEWizard.exe" "MakeSFX.exe"
REM Create the SFX
makesfx.exe ^
/zip="FESetup.zip" ^
/sfx="FESetup.exe" ^
/title="FreeExtractor Setup %VERSION:"=%" ^
/website="https://github.com/datadiode/FreeExtractor" ^
/intro="This self-extracting archive will extract and setup FreeExtractor to run on your machine." ^
/defaultpath="$programfiles$\FreeExtractor" ^
/runelevated ^
/overwrite ^
/openexplorerwindow ^
/shortcut="$sendto$\Convert Zip to SFX.lnk|$targetdir$\FEWizard.exe" ^
/shortcut="$desktop$\FreeExtractor Wizard.lnk|$targetdir$\FEWizard.exe"
popd
