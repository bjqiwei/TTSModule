@echo off

set _task=TTSD.exe
set _svr=F:\CCP\TTSModule\work\BIN\TTSD.exe
set _des=start.bat
set _kill=kill.bat
set _basemem=300000

:checkstart
for /f "tokens=1" %%n in ('tasklist ^| find "%_task%" ') do (
if %%n==%_task% (goto checkmemery) else goto startsvr
)



:startsvr
echo %time% 
echo ********����ʼ����********
echo �������������� %date%%time% ,����ϵͳ��־ >> monitorlog.txt
echo start %_svr% > %_des%
echo exit >> %_des%
start %_des%
set/p=.<nul
for /L %%i in (1 1 10) do set /p a=.<nul&ping.exe /n 2 127.0.0.1>nul
echo .
echo Wscript.Sleep WScript.Arguments(0) >%tmp%\delay.vbs 
cscript //b //nologo %tmp%\delay.vbs 10000 
del %_des% /Q
echo ********�����������********
goto checkstart


:checkmemery
echo ********��ʼ����ڴ�******** >> monitorlog.txt
for /f "tokens=5" %%n in ('tasklist ^| find "%_task%" ') do (
set _mem=%%n
)
echo %date%%time% ��ǰ�ڴ�ռ���� %_mem% K >> monitorlog.txt
for /f "tokens=1,2,3 delims=, " %%a in ("%_mem%") do (
set para1=%%a
set para2=%%b
set para3=%%c
)
set _newmem=%para1%%para2%%para3%

if %_newmem% gtr %_basemem% (
echo goto checktime >> monitorlog.txt
goto checktime
) else (
echo goto sleepwait >> monitorlog.txt
goto sleepcheck
)



:checktime

for /f "tokens=1 delims=: " %%a in ('time /t') do (
set hhtime=%%a
)
if "%hhtime%"=="01" (
echo goto killtts >> monitorlog.txt
goto killtts
) else (
if "%hhtime%"=="1" (
echo goto killtts >> monitorlog.txt
goto killtts
) else (
echo goto sleepwait >> monitorlog.txt
goto sleepwait
)
)


:killtts
echo %time% �ڴ�ռ�ù�����Ҫ������ >> monitorlog.txt
echo ����ɱ���� %time% ,����ϵͳ��־  >> monitorlog.txt
echo taskkill /f /t /im %_task% > %_kill%
echo exit >> %_kill%
start %_kill%
set/p=.<nul
for /L %%i in (1 1 10) do set /p a=.<nul&ping.exe /n 2 127.0.0.1>nul
echo .
echo Wscript.Sleep WScript.Arguments(0) >%tmp%\delay.vbs 
cscript //b //nologo %tmp%\delay.vbs 3000 
del %_kill% /Q
goto checkstart



:sleepwait
echo %time% ������������,5���������.. 
echo Wscript.Sleep WScript.Arguments(0) >%tmp%\delay.vbs 
cscript //b //nologo %tmp%\delay.vbs 5000 
goto checkstart