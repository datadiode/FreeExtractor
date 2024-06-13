@echo off
set ducible.exe="UnityDoorstop-BSIPA\BuildUtils\ducible.exe"

setlocal
call "%VS140COMNTOOLS%vsvars32.bat"
msbuild /t:Rebuild header\header.vcxproj /p:SolutionDir=%~dp0 /p:Platform=x64 /p:Configuration=Release
%ducible.exe% output\header64.exe
%ducible.exe% output\header64_elevated.exe
msbuild /t:Rebuild header\header.vcxproj /p:SolutionDir=%~dp0 /p:Platform=x64 /p:Configuration=Release_CAB
%ducible.exe% output\header64_cab.exe
%ducible.exe% output\header64_cab_elevated.exe
msbuild /t:Rebuild header\header.vcxproj /p:SolutionDir=%~dp0 /p:Platform=x64 /p:Configuration=Release_ZPAQ
%ducible.exe% output\header64_zpaq.exe
%ducible.exe% output\header64_zpaq_elevated.exe
msbuild /t:Rebuild header\header.vcxproj /p:SolutionDir=%~dp0 /p:Platform=Win32 /p:Configuration=Release_ZPAQ
%ducible.exe% output\header32_zpaq.exe
%ducible.exe% output\header32_zpaq_elevated.exe
endlocal

setlocal
call "%VS90COMNTOOLS%vsvars32.bat"
msbuild /t:Rebuild header\header.vcproj /p:SolutionDir=%~dp0 /p:Platform=Win32 /p:Configuration=Release
%ducible.exe% output\header32.exe
%ducible.exe% output\header32_elevated.exe
msbuild /t:Rebuild header\header.vcproj /p:SolutionDir=%~dp0 /p:Platform=Win32 /p:Configuration=Release_CAB
%ducible.exe% output\header32_cab.exe
%ducible.exe% output\header32_cab_elevated.exe
msbuild /t:Rebuild fewizard\FEWizard.vcproj /p:SolutionDir=%~dp0 /p:Platform=Win32 /p:Configuration=Release
%ducible.exe% output\FEWizard.exe
msbuild /t:Rebuild makesfx\MakeSFX.vcproj /p:SolutionDir=%~dp0 /p:Platform=Win32 /p:Configuration=Release
%ducible.exe% output\MakeSFX.exe
endlocal

call "%~dp0output\MakeSetup.bat"
