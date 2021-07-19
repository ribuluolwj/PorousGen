@echo off

set UGII_PATH=%UGII_BASE_DIR%
@echo =======UG Base Directory is:=======
@echo %UGII_PATH%
@echo ==============================

XCOPY "Application\*.*" "%UGII_PATH%\MYPROGRAM\Application\*.*" /E /Y
@echo ====Applications has been copied!====
XCOPY "Startup\*.*" "%UGII_PATH%\MYPROGRAM\Startup\*.*" /E /Y
@echo =====Startup has been copied!======
@echo =========Completed!===========


pause
