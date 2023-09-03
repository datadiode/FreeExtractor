# Microsoft Developer Studio Generated NMAKE File, Based on GetVersion.dsp
!IF "$(CFG)" == ""
CFG=GetVersion - Win32 Release
!MESSAGE No configuration specified. Defaulting to GetVersion - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "GetVersion - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GetVersion.mak" CFG="GetVersion - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GetVersion - Win32 Release" (based on "Win32 (x86) Console Application")
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
OUTDIR=.\..\output
INTDIR=.\obj
# Begin Custom Macros
OutDir=.\..\output\ 
# End Custom Macros

ALL : "$(OUTDIR)\GetVersion.exe"


CLEAN :
	-@erase "$(INTDIR)\getversion.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\GetVersion.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\GetVersion.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oleaut32.lib uuid.lib winspool.lib shell32.lib user32.lib comctl32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"$(OUTDIR)\GetVersion.exe" 
LINK32_OBJS= \
	"$(INTDIR)\getversion.obj"

"$(OUTDIR)\GetVersion.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

CPP_PROJ=/nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\GetVersion.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
!IF EXISTS("GetVersion.dep")
!INCLUDE "GetVersion.dep"
!ELSE 
!MESSAGE Warning: cannot find "GetVersion.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "GetVersion - Win32 Release"
SOURCE=.\src\getversion.c

"$(INTDIR)\getversion.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

