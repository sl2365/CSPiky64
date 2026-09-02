@echo off
setlocal EnableExtensions

if /i "%~1"=="--run-build" goto :run_build

set "RESULTS_LOG=%~dp0Results.log"

call "%~f0" --run-build > "%RESULTS_LOG%" 2>&1
set "BUILD_EXIT_CODE=%ERRORLEVEL%"

type "%RESULTS_LOG%"
echo.
echo Results saved:
echo   %RESULTS_LOG%
echo.
pause
exit /b %BUILD_EXIT_CODE%

:run_build
title CSPiky64 - Build

set "PROJECT_ROOT=%~dp0"
set "SOURCE_DIR=%PROJECT_ROOT%source"
set "BUILD_DIR=%PROJECT_ROOT%build"
set "DIST_DIR=%PROJECT_ROOT%dist"
set "CMAKE_EXE=%PROJECT_ROOT%..\_Tools\cmake\_4.4.2\bin\cmake.exe"
set "JUCE_CMAKE=%PROJECT_ROOT%..\_Tools\JUCE\_8.0.15\CMakeLists.txt"
set "BUNDLE_BINARY=%BUILD_DIR%\CSPiky64_artefacts\Release\VST3\CSPiky64.vst3\Contents\x86_64-win\CSPiky64.vst3"
set "FINAL_PLUGIN=%DIST_DIR%\CSPiky64.vst3"
set "DATA_DIR=%DIST_DIR%\Data"
set "PRESET_DIR=%DATA_DIR%\Preset"

echo.
echo CSPiky64 64-bit VST3 - Stage 0.33 Build
echo ======================================
echo.

echo Closing PolyHostInterface.exe if it is running...
taskkill.exe /F /IM "PolyHostInterface.exe" >nul 2>&1
if errorlevel 1 (
    echo       No running instance found
) else (
    echo       CLOSED
)
echo.

echo [1/5] Checking required files...

if not exist "%CMAKE_EXE%" goto :missing_cmake
if not exist "%JUCE_CMAKE%" goto :missing_juce
if not exist "%SOURCE_DIR%\CMakeLists.txt" goto :missing_source
if not exist "%SOURCE_DIR%\PluginProcessor.cpp" goto :missing_source
if not exist "%SOURCE_DIR%\PluginEditor.cpp" goto :missing_source
if not exist "%SOURCE_DIR%\Wave2Tables.h" goto :missing_source

where powershell.exe >nul 2>nul
if errorlevel 1 goto :missing_powershell

echo       PASS
echo.
echo [2/5] Configuring Visual Studio Community 2026 x64...

"%CMAKE_EXE%" -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 18 2026" -A x64
if errorlevel 1 goto :configure_failed

echo       PASS
echo.
echo [3/5] Building Release VST3...

"%CMAKE_EXE%" --build "%BUILD_DIR%" --config Release --target CSPiky64_VST3 --clean-first --parallel
if errorlevel 1 goto :build_failed

if not exist "%BUNDLE_BINARY%" goto :binary_not_found

echo       PASS
echo.
echo [4/5] Creating the portable output and data folders...

if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"
if errorlevel 1 goto :dist_failed

if exist "%FINAL_PLUGIN%\" rmdir /s /q "%FINAL_PLUGIN%"
if exist "%FINAL_PLUGIN%" del /f /q "%FINAL_PLUGIN%"

copy /y "%BUNDLE_BINARY%" "%FINAL_PLUGIN%" >nul
if errorlevel 1 goto :copy_failed

if not exist "%FINAL_PLUGIN%" goto :copy_failed

if not exist "%DATA_DIR%" mkdir "%DATA_DIR%"
if not exist "%DATA_DIR%" goto :data_failed

if not exist "%PRESET_DIR%" mkdir "%PRESET_DIR%"
if not exist "%PRESET_DIR%" goto :data_failed

echo       PASS
echo.
echo [5/5] Validating x64 format and dist contents...

powershell.exe -NoProfile -Command ^
  "$path = [IO.Path]::GetFullPath('%FINAL_PLUGIN%');" ^
  "$bytes = [IO.File]::ReadAllBytes($path);" ^
  "if ($bytes.Length -lt 64) { exit 1 };" ^
  "$pe = [BitConverter]::ToInt32($bytes, 60);" ^
  "if ($pe -lt 0 -or ($pe + 6) -gt $bytes.Length) { exit 1 };" ^
  "if ([BitConverter]::ToUInt32($bytes, $pe) -ne 0x00004550) { exit 1 };" ^
  "if ([BitConverter]::ToUInt16($bytes, $pe + 4) -ne 0x8664) { exit 1 };"
if errorlevel 1 goto :wrong_architecture

powershell.exe -NoProfile -Command ^
  "$extra = Get-ChildItem -LiteralPath '%DIST_DIR%' -Force | Where-Object { $_.Name -notin @('CSPiky64.vst3', 'Data') };" ^
  "if ($extra) { $extra | ForEach-Object { Write-Host ('Unexpected: ' + $_.FullName) }; exit 1 }"
if errorlevel 1 goto :unexpected_dist_files

if not exist "%DATA_DIR%\" goto :data_failed
if not exist "%PRESET_DIR%\" goto :data_failed

echo       PASS
echo.
echo BUILD SUMMARY
echo   - PASS: Required tools and source files found
echo   - PASS: Visual Studio Community 2026 x64 configured
echo   - PASS: Release VST3 compiled
echo   - PASS: Single-file VST3 and persistent Data folders created
echo   - PASS: Output verified as Windows x64
echo.
echo Output:
echo   %FINAL_PLUGIN%
echo   %PRESET_DIR%
echo.
echo This is the Stage 0.33 preset-management-and-size-settings build.
echo No plug-in files were installed elsewhere.
echo.
exit /b 0

:missing_cmake
echo.
echo BUILD FAILED
echo   - FAIL: CMake 4.4.2 was not found
echo.
echo Expected:
echo   %CMAKE_EXE%
goto :failed

:missing_juce
echo.
echo BUILD FAILED
echo   - FAIL: JUCE 8.0.15 was not found
echo.
echo Expected:
echo   %JUCE_CMAKE%
goto :failed

:missing_source
echo.
echo BUILD FAILED
echo   - FAIL: One or more required source files were not found
echo.
echo Expected folder:
echo   %SOURCE_DIR%
goto :failed

:missing_powershell
echo.
echo BUILD FAILED
echo   - FAIL: Windows PowerShell was not found
goto :failed

:configure_failed
echo.
echo BUILD FAILED
echo   - PASS: Required tools and source files found
echo   - FAIL: CMake could not configure Visual Studio Community 2026 x64
goto :failed

:build_failed
echo.
echo BUILD FAILED
echo   - PASS: CMake configuration completed
echo   - FAIL: Release VST3 compilation failed
goto :failed

:binary_not_found
echo.
echo BUILD FAILED
echo   - PASS: Compilation command completed
echo   - FAIL: JUCE's internal x64 VST3 binary was not found
echo.
echo Expected:
echo   %BUNDLE_BINARY%
goto :failed

:dist_failed
echo.
echo BUILD FAILED
echo   - FAIL: The dist folder could not be created
goto :failed

:copy_failed
echo.
echo BUILD FAILED
echo   - FAIL: CSPiky64.vst3 could not be copied into dist
goto :failed

:data_failed
echo.
echo BUILD FAILED
echo   - FAIL: dist\Data\Preset could not be created or verified
goto :failed

:wrong_architecture
echo.
echo BUILD FAILED
echo   - FAIL: dist\CSPiky64.vst3 is not a Windows x64 PE binary
goto :failed

:unexpected_dist_files
echo.
echo BUILD FAILED
echo   - FAIL: dist contains an unexpected file or folder
echo.
echo Allowed:
echo   CSPiky64.vst3
echo   Data
goto :failed

:failed
echo.
echo Please send Results.log.
echo.
exit /b 1
