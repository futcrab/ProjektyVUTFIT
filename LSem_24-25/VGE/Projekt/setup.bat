@echo off
setlocal

set VENV_NAME=VisualizerEnv
set PROJECT_DIR=spectral-mesh-simplification

:: Main logic
if "%1"=="full" goto FULL_SETUP
if "%1"=="only_venv" goto INSTALL_VENV
if "%1"=="delete_venv" goto DELETE_VENV
if "%1"=="delete_spectral" goto DELETE_PROJECT
if "%1"=="delete" goto DELETE_ALL

:: Help message
echo.
echo Usage:
echo   setup.bat full             - Set up venv and project (if not already present)
echo   setup.bat only_venv        - Set up only virtual environment (if not present)
echo   setup.bat delete_venv      - Delete the virtual environment (if exists)
echo   setup.bat delete_spectral  - Delete the spectral project (if exists)
echo   setup.bat delete           - Delete both venv and project (if exist)
goto END

:FULL_SETUP
if exist %VENV_NAME% (
    echo Virtual environment already exists.
) else (
    call setup_venv.bat create
)

if exist %PROJECT_DIR% (
    echo Spectral project already exists.
) else (
    call spectral_simpl.bat install
)
goto END

:INSTALL_VENV
if exist %VENV_NAME% (
    echo Virtual environment already exists.
) else (
    call setup_venv.bat create
)
goto END

:DELETE_VENV
if exist %VENV_NAME% (
    call setup_venv.bat delete
) else (
    echo Virtual environment not found.
)
goto END

:DELETE_PROJECT
if exist %PROJECT_DIR% (
    call spectral_simpl.bat delete
) else (
    echo Spectral project not found.
)
goto END

:DELETE_ALL
call %~n0 delete_venv
call %~n0 delete_spectral
goto END

:END
endlocal
