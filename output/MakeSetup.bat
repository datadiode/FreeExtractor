@echo off
pushd "%~dp0"
for /f "tokens=*" %%G in ('where git.exe') do set git.exe="%%G"
set touch.exe=%git.exe:cmd\git.exe=usr\bin\touch.exe%
REM Read the version number from FEcommon.h
for /F "tokens=1,2,3" %%G in (..\common\FEcommon.h) do if "%%G,%%H" == "#define,VERSION" set VERSION=%%I
echo VERSION=%VERSION:"=%
REM Set the mtime of all bundled files to the time of the latest commit
for /f "delims=: tokens=1,*" %%G in ('git log -1 --date=iso') do if "%%G"=="Date" set GITDATE=%%H
echo GITDATE=%GITDATE%
for /f delims^=^"^ tokens^=2 %%G in (FESetup.ddf) do %touch.exe% -d "%GITDATE%" "%~dp0%%G"
REM Create the CAB
makecab /f FESetup.ddf
REM Create the SFX
makesfx.exe ^
/zip="disk1\FESetup.cab" ^
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
