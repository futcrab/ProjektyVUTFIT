@echo off
setlocal

if "%1"=="install" goto INSTALL_SPECTRAL
if "%1"=="delete" goto DELETE_SPECTRAL

echo.
echo Usage:
echo   spectral_simpl.bat install  - downloads and compiles the project
echo   spectral_simpl.bat delete   - Deletes the project directory
goto END


:INSTALL_SPECTRAL
:: Clone the repository with submodules
:: echo Cloning repository...
git clone --recurse-submodules https://gitlab.com/Synxis/spectral-mesh-simplification.git
cd spectral-mesh-simplification

:: Create the build directory
mkdir build
cd build

:: Run CMake with 64-bit architecture
cmake .. -A x64

:: Build the project using MSBuild
msbuild SpectralCollapsing.sln /p:Configuration=Release

:: Finished
echo Spectral simplification setup complete!
goto END


:DELETE_SPECTRAL
echo Removing spectral simplification directory
rmdir /s /q spectral-mesh-simplification
echo Directory deleted.
goto END

:END
endlocal