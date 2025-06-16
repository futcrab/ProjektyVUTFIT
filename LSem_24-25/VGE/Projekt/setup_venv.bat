@echo off
setlocal

set VENV_NAME=VisualizerEnv

if "%1"=="create" goto CREATE_ENV
if "%1"=="delete" goto DELETE_ENV

echo.
echo Usage:
echo   setup.bat create   - Create venv and install dependencies
echo   setup.bat delete   - Delete venv
goto END

:CREATE_ENV
echo Creating virtual environment: %VENV_NAME%
python -m venv %VENV_NAME%
call %VENV_NAME%\Scripts\activate
echo Installing packages...

:: Check if pip is installed
python -m pip --version > nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Pip is not installed. Installing pip...
    python -m ensurepip --upgrade
)
pip install open3d polyscope numpy scipy
call deactivate
echo Environment setup complete.
goto END

:DELETE_ENV
echo Removing virtual environment: %VENV_NAME%
rmdir /s /q %VENV_NAME%
echo Environment deleted.
goto END

:END
endlocal