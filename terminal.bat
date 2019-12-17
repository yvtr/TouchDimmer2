echo %USERPROFILE%| C:\MinGW\msys\1.0\bin\tr "C\\" "c/" > temp.tmp
<temp.tmp (set /p USERPROFILE=)
del temp.tmp
SET HOME=.
SET PATH=%PATH%;C:\Program Files\Git\usr\bin
call C:\MinGW\msys\1.0\msys.bat
