@echo OFF
set InName=%1In
set OutName=%1Out
set SOURCE_DIR=%~f2
set DEST_DIR=%~f3

if exist "%SOURCE_DIR%\%InName%" copy "%SOURCE_DIR%\%InName%" "%DEST_DIR%"
if exist "%SOURCE_DIR%\%OutName%" copy "%SOURCE_DIR%\%OutName%" "%DEST_DIR%"

