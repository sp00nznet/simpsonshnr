@echo off
cd /d C:\simpsonshnr\game\build\ps3
set PATH=C:\simpsonshnr\game\libs\radbuild\GlobalTools;%PATH%
echo Starting make...
C:\msys64\usr\bin\make.exe CONFIG=DEBUG 2>&1
echo Exit code: %ERRORLEVEL%
