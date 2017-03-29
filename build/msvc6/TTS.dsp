# Microsoft Developer Studio Project File - Name="T_TTS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=T_TTS - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TTS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TTS.mak" CFG="T_TTS - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "T_TTS - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "T_TTS - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "T_TTS - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zd /O2 /I "../../main/win32" /I "../../ttsapi/include" /I "../../common" /I "../../public" /I "../../ttsmodule" /I "../../httpmodule" /I "../../alarm" /I "../../encodetrans" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "IF2" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../../lib/iFlyTTS.lib Ws2_32.lib ../../lib/iconv.lib /nologo /subsystem:windows /incremental:yes /map:"..\..\WORK\BIN\TTS.map" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"libc.lib" /out:"..\..\WORK\BIN\TTS.exe" /libpath:"../../ttsapi/lib" /libpath:"../../../../pub_file/rtcs/lib" /libpath:"../../../../pub_file/lib" /libpath:"../../../../pub_file/rtp/lib" /EDITANDCONTINUE /mapinfo:lines
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "T_TTS - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "../../main/win32" /I "../../ttsapi/include" /I "../../common" /I "../../public" /I "../../ttsmodule" /I "../../httpmodule" /I "../../alarm" /I "../../encodetrans" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_MYDEBUG" /FR /YX /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /fo"Debug/RTPTTS.res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/RTPTTS.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../lib/iFlyTTS.lib Ws2_32.lib ../../lib/iconv.lib /nologo /subsystem:windows /pdb:"Release/T_TTS.pdb" /map:"../../work/bin/TTSD.map" /debug /machine:I386 /out:"..\..\WORK\BIN\TTSD.exe" /libpath:"../../ttsapi/lib" /libpath:"../../../../pub_file/rtcs/lib" /libpath:"../../../../pub_file/lib" /libpath:"../../../../pub_file/rtp/lib" /mapinfo:lines
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ENDIF 

# Begin Target

# Name "T_TTS - Win32 Release"
# Name "T_TTS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\main\win32\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\messageview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\splasher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\stationview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\t_tts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\t_tts.rc
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\t_ttsdoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\t_ttsview.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\main\win32\mainfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\messageview.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\splasher.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\stationview.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\t_tts.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\t_ttsdoc.h
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\t_ttsview.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\main\win32\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\res\splash.bmp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\res\station.bmp
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\res\t_tts.ico
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\res\t_tts.rc2
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\res\t_ttsdoc.ico
# End Source File
# Begin Source File

SOURCE=..\..\main\win32\res\toolbar.bmp
# End Source File
# End Group
# Begin Group "TTS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\ttsmodule\msgdefine.h
# End Source File
# Begin Source File

SOURCE=..\..\ttsmodule\threadmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ttsmodule\threadmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\ttsmodule\tts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ttsmodule\tts.h
# End Source File
# Begin Source File

SOURCE=..\..\ttsmodule\ttsthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ttsmodule\ttsthread.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\clist.h
# End Source File
# Begin Source File

SOURCE=..\..\common\cmap.h
# End Source File
# Begin Source File

SOURCE=..\..\common\cobject.h
# End Source File
# Begin Source File

SOURCE=..\..\common\comm.h
# End Source File
# Begin Source File

SOURCE=..\..\common\cqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\common\cthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\cthread.h
# End Source File
# Begin Source File

SOURCE=..\..\common\ctime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\ctime.h
# End Source File
# Begin Source File

SOURCE=..\..\common\ctimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\ctimer.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Define.h
# End Source File
# Begin Source File

SOURCE=..\..\common\interface.h
# End Source File
# Begin Source File

SOURCE=..\..\common\logtrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\logtrace.h
# End Source File
# Begin Source File

SOURCE=..\..\common\typedef.h
# End Source File
# End Group
# Begin Group "public"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\public\deelx.h
# End Source File
# Begin Source File

SOURCE=..\..\encodetrans\encodetransfer.h
# End Source File
# Begin Source File

SOURCE=..\..\public\http.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\http.h
# End Source File
# Begin Source File

SOURCE=..\..\public\PathDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\PathDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\public\SocketClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\SocketClass.h
# End Source File
# Begin Source File

SOURCE=..\..\public\Xml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\Xml.h
# End Source File
# Begin Source File

SOURCE=..\..\public\XmlObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\XmlObject.h
# End Source File
# End Group
# Begin Group "httpmodule"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\httpmodule\HttpClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\httpmodule\HttpClient.h
# End Source File
# Begin Source File

SOURCE=..\..\httpmodule\httpserver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\httpmodule\httpserver.h
# End Source File
# End Group
# Begin Group "alarm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\alarm\alarm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\alarm\alarm.h
# End Source File
# Begin Source File

SOURCE=..\..\alarm\alarmerrordef.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project
