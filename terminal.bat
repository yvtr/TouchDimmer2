echo %USERPROFILE%| C:\MinGW\msys\1.0\bin\tr "C\\" "c/" > temp.tmp
<temp.tmp (set /p USERPROFILE=)
del temp.tmp
SET HOME=.
call C:\MinGW\msys\1.0\msys.bat
