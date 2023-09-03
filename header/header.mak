# Microsoft Developer Studio Generated NMAKE File, Based on header.dsp
!IF "$(CFG)" == ""
CFG=header - Win32 Debug
!MESSAGE No configuration specified. Defaulting to header - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "header - Win32 Release" && "$(CFG)" != "header - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "header - Win32 Release"

OUTDIR=.\..\output
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

ALL : "$(OUTDIR)\header.exe"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\header.obj"
	-@erase "$(INTDIR)\header.res"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\unzip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\header.exe"
	-@erase ".\obj\header.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O1 /Ob0 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\header.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\header.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ole32.lib advapi32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /pdb:none /map:"$(INTDIR)\header.map" /machine:I386 /out:"$(OUTDIR)\header.exe" /OPT:NOWIN98 /OPT:REF /WS:AGGRESSIVE 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\unzip.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\header.obj" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\header.res"

"$(OUTDIR)\header.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Compressing header.exe and building header include files
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\header.exe"
   upx.exe --best -q -f ..\output\header.exe
	bin2h.exe ..\output\header.exe ..\common\stub.h STUB
	bin2h.exe res\setup.ico ..\common\icon.h ICON
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "header - Win32 Debug"

OUTDIR=.\..\output
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

ALL : "$(OUTDIR)\header.exe" "$(OUTDIR)\header.bsc"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\adler32.sbr"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crc32.sbr"
	-@erase "$(INTDIR)\header.obj"
	-@erase "$(INTDIR)\header.res"
	-@erase "$(INTDIR)\header.sbr"
	-@erase "$(INTDIR)\infblock.obj"
	-@erase "$(INTDIR)\infblock.sbr"
	-@erase "$(INTDIR)\infcodes.obj"
	-@erase "$(INTDIR)\infcodes.sbr"
	-@erase "$(INTDIR)\inffast.obj"
	-@erase "$(INTDIR)\inffast.sbr"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\inflate.sbr"
	-@erase "$(INTDIR)\inftrees.obj"
	-@erase "$(INTDIR)\inftrees.sbr"
	-@erase "$(INTDIR)\infutil.obj"
	-@erase "$(INTDIR)\infutil.sbr"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\shell.sbr"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\stack.sbr"
	-@erase "$(INTDIR)\unzip.obj"
	-@erase "$(INTDIR)\unzip.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(INTDIR)\zutil.sbr"
	-@erase "$(OUTDIR)\header.bsc"
	-@erase "$(OUTDIR)\header.exe"
	-@erase "$(OUTDIR)\header.ilk"
	-@erase "$(OUTDIR)\header.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\header.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\header.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adler32.sbr" \
	"$(INTDIR)\crc32.sbr" \
	"$(INTDIR)\infblock.sbr" \
	"$(INTDIR)\infcodes.sbr" \
	"$(INTDIR)\inffast.sbr" \
	"$(INTDIR)\inflate.sbr" \
	"$(INTDIR)\inftrees.sbr" \
	"$(INTDIR)\infutil.sbr" \
	"$(INTDIR)\unzip.sbr" \
	"$(INTDIR)\zutil.sbr" \
	"$(INTDIR)\header.sbr" \
	"$(INTDIR)\shell.sbr" \
	"$(INTDIR)\stack.sbr"

"$(OUTDIR)\header.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ole32.lib advapi32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\header.pdb" /debug /machine:I386 /out:"$(OUTDIR)\header.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\infblock.obj" \
	"$(INTDIR)\infcodes.obj" \
	"$(INTDIR)\inffast.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\inftrees.obj" \
	"$(INTDIR)\infutil.obj" \
	"$(INTDIR)\unzip.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\header.obj" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\header.res"

"$(OUTDIR)\header.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Building header include files
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\header.exe" "$(OUTDIR)\header.bsc"
   bin2h.exe ..\output\header.exe ..\common\stub.h STUB
	bin2h.exe res\setup.ico ..\common\icon.h ICON
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("header.dep")
!INCLUDE "header.dep"
!ELSE 
!MESSAGE Warning: cannot find "header.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "header - Win32 Release" || "$(CFG)" == "header - Win32 Debug"
SOURCE=.\src\zLib\adler32.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\adler32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\adler32.obj"	"$(INTDIR)\adler32.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\crc32.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\crc32.obj"	"$(INTDIR)\crc32.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\infblock.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\infblock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\infblock.obj"	"$(INTDIR)\infblock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\infcodes.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\infcodes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\infcodes.obj"	"$(INTDIR)\infcodes.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\inffast.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\inffast.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\inffast.obj"	"$(INTDIR)\inffast.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\inflate.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\inflate.obj"	"$(INTDIR)\inflate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\inftrees.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\inftrees.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\inftrees.obj"	"$(INTDIR)\inftrees.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\infutil.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\infutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\infutil.obj"	"$(INTDIR)\infutil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\unzip.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\unzip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\unzip.obj"	"$(INTDIR)\unzip.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\zLib\zutil.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\zutil.obj"	"$(INTDIR)\zutil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\header.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\header.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\header.obj"	"$(INTDIR)\header.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\shell.cpp

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\shell.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\shell.obj"	"$(INTDIR)\shell.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\stack.c

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\stack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\stack.obj"	"$(INTDIR)\stack.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\header.rc

!IF  "$(CFG)" == "header - Win32 Release"


"$(INTDIR)\header.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\header.res" /i "src" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "header - Win32 Debug"


"$(INTDIR)\header.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\header.res" /i "src" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

