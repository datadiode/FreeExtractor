# Microsoft Developer Studio Generated NMAKE File, Based on FEWizard.dsp
!IF "$(CFG)" == ""
CFG=FEWizard - Win32 Debug
!MESSAGE No configuration specified. Defaulting to FEWizard - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FEWizard - Win32 Release" && "$(CFG)" != "FEWizard - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FEWizard.mak" CFG="FEWizard - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FEWizard - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FEWizard - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "FEWizard - Win32 Release"

OUTDIR=.\..\output
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

ALL : "$(OUTDIR)\FEWizard.exe"


CLEAN :
	-@erase "$(INTDIR)\FEWizard.obj"
	-@erase "$(INTDIR)\FEWizard.res"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\FEWizard.exe"
	-@erase ".\obj\FEWizard.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /vmg /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\FEWizard.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FEWizard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=HTMLHelp.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /version:1.0 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\FEWizard.pdb" /map:"$(INTDIR)\FEWizard.map" /machine:I386 /out:"$(OUTDIR)\FEWizard.exe" /OPT:REF /OPT:NOWIN98 
LINK32_OBJS= \
	"$(INTDIR)\FEWizard.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\FEWizard.res"

"$(OUTDIR)\FEWizard.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Compressing FEWizard.exe
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\FEWizard.exe"
   upx.exe -9 -q -f ..\output\FEWizard.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "FEWizard - Win32 Debug"

OUTDIR=.\..\output
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

ALL : "$(OUTDIR)\FEWizard.exe"


CLEAN :
	-@erase "$(INTDIR)\FEWizard.obj"
	-@erase "$(INTDIR)\FEWizard.res"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\FEWizard.exe"
	-@erase "$(OUTDIR)\FEWizard.ilk"
	-@erase "$(OUTDIR)\FEWizard.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /Zm500 /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\FEWizard.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FEWizard.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Htmlhelp.lib comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\FEWizard.pdb" /debug /machine:I386 /out:"$(OUTDIR)\FEWizard.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\FEWizard.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\FEWizard.res"

"$(OUTDIR)\FEWizard.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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
!IF EXISTS("FEWizard.dep")
!INCLUDE "FEWizard.dep"
!ELSE 
!MESSAGE Warning: cannot find "FEWizard.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FEWizard - Win32 Release" || "$(CFG)" == "FEWizard - Win32 Debug"
SOURCE=.\src\FEWizard.c

"$(INTDIR)\FEWizard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\list.c

"$(INTDIR)\list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\FEWizard.rc

!IF  "$(CFG)" == "FEWizard - Win32 Release"


"$(INTDIR)\FEWizard.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\FEWizard.res" /i "src" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "FEWizard - Win32 Debug"


"$(INTDIR)\FEWizard.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\FEWizard.res" /i "src" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

