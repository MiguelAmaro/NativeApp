@echo off

set a='"adb shell pidof com.code.NativeExample"'
for /f "delims=" %%c in (%a%) do (set PROCESSID=%%c)
echo 

echo starting logcat for process[%PROCESSID%]...

adb logcat --pid=%PROCESSID% -v brief, --format=color descriptive
