#!/bin/bash

while :
do
  curl --insecure --data ccpheartbeat http://192.168.178.46:8090/CheckHeartBeat  > ./TTS_check.txt  2>&1 
  #cat ./TTS_check.txt
  sleep 2
  stillRunning=`cat ./TTS_check.txt|grep 404|wc -l`
  
  #echo $stillRunning

  if [[ $stillRunning  -ge 1 ]] ; then
    echo `date "+%Y-%m-%d %H:%M:%S"` "ttsD.exe is running "
  else
    echo `date "+%Y-%m-%d %H:%M:%S"` "ttsD.exe is not running"
    ttspid=`cat tts.pid`
    killall -9 $ttspid
    echo "Restarting ttsD.exe ..."
    cd  ./
    ./ttsD.exe start
  fi
  sleep 8 
done

