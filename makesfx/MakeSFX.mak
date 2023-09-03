# Microsoft Developer Studio Generated NMAKE File, Based on MakeSFX.dsp
!IF "$(CFG)" == ""
CFG=MakeSFX - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MakeSFX - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MakeSFX - Win32 Release" && "$(CFG)" != "MakeSFX - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakeSFX.mak" CFG="MakeSFX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakeSFX - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MakeSFX - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakeSFX - Win32 Release"

OUTDIR=.\..\output
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

ALL : "$(OUTDIR)\MakeSFX.exe"


CLEAN :
	-@erase "$(INTDIR)\MakeSFX.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MakeSFX.exe"
	-@erase ".\obj\MakeSFX.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MakeSFX.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\MakeSFX.pdb" /map:"$(INTDIR)\MakeSFX.map" /machine:I386 /out:"$(OUTDIR)\MakeSFX.exe" /OPT:REF 
LINK32_OBJS= \
	"$(INTDIR)\MakeSFX.obj"

"$(OUTDIR)\MakeSFX.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Compressing MakeSFX.exe
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\MakeSFX.exe"
   upx.exe -9 -q -f ..\output\MakeSFX.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MakeSFX - Win32 Debug"

OUTDIR=.\..\output
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

ALL : "$(OUTDIR)\MakeSFX.exe"


CLEAN :
	-@erase "$(INTDIR)\MakeSFX.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MakeSFX.exe"
	-@erase "$(OUTDIR)\MakeSFX.ilk"
	-@erase "$(OUTDIR)\MakeSFX.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MakeSFX.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\MakeSFX.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MakeSFX.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\MakeSFX.obj"

"$(OUTDIR)\MakeSFX.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("MakeSFX.dep")
!INCLUDE "MakeSFX.dep"
!ELSE 
!MESSAGE Warning: cannot find "MakeSFX.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MakeSFX - Win32 Release" || "$(CFG)" == "MakeSFX - Win32 Debug"
SOURCE=.\src\MakeSFX.c

"$(INTDIR)\MakeSFX.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

