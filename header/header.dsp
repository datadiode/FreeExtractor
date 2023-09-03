# Microsoft Developer Studio Project File - Name="header" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=header - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "header.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "header.mak" CFG="header - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "header - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "header - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "header - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\output\"
# PROP Intermediate_Dir "obj\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ole32.lib advapi32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /pdb:none /map /machine:I386 /OPT:NOWIN98 /OPT:REF /WS:AGGRESSIVE
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Compressing header.exe and building header include files
PostBuild_Cmds=upx.exe --best -q -f ..\output\header.exe	bin2h.exe ..\output\header.exe ..\common\stub.h STUB	bin2h.exe res\setup.ico ..\common\icon.h ICON
# End Special Build Tool

!ELSEIF  "$(CFG)" == "header - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\output\"
# PROP Intermediate_Dir "obj\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ole32.lib advapi32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Building header include files
PostBuild_Cmds=bin2h.exe ..\output\header.exe ..\common\stub.h STUB	bin2h.exe res\setup.ico ..\common\icon.h ICON
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "header - Win32 Release"
# Name "header - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "zLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\zLib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\deflate.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\infblock.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\infblock.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\infcodes.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\inffast.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\infutil.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\infutil.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\trees.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\unzip.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\unzip.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\src\zLib\zutil.c
# End Source File
# Begin Source File

SOURCE=.\src\zLib\zutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\header.c
# End Source File
# Begin Source File

SOURCE=.\src\shell.cpp
# End Source File
# Begin Source File

SOURCE=.\src\stack.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\FEcommon.h
# End Source File
# Begin Source File

SOURCE=.\src\header.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\shell.h
# End Source File
# Begin Source File

SOURCE=.\src\stack.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\box.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cbg.bin
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\src\header.rc
# End Source File
# Begin Source File

SOURCE=.\res\intro.bmp
# End Source File
# Begin Source File

SOURCE=.\res\setup.ico
# End Source File
# End Group
# End Target
# End Project
