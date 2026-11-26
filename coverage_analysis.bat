@echo off
echo ======================================
echo        Code Coverage Analysis
echo ======================================
echo.

REM Ensure we're in the right directory
cd /d "%~dp0"

REM Build the projects first
echo [1/4] Building projects...
msbuild projekt-obiektowe.sln /p:Configuration=Debug /p:Platform=x64 /verbosity:quiet

if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed!
    exit /b 1
)

echo     Build successful!
echo.

REM Copy required DLLs
echo [2/4] Copying required DLLs...
copy "SensorsTests\vcpkg_installed\x64-windows\debug\bin\gtest.dll" "x64\Debug\" > nul 2>&1
if not exist "x64\Debug\gmock.dll" (
  copy "SensorsTests\vcpkg_installed\x64-windows\debug\bin\gmock.dll" "x64\Debug\" > nul 2>&1
)

REM Create coverage output directory
echo [3/4] Preparing coverage analysis...
if not exist "coverage" mkdir coverage

REM Run coverage analysis
echo [4/4] Running coverage analysis...
"C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe" ^
    --modules x64\Debug\SensorsTests.exe ^
    --sources projekt-obiektowe ^
    --excluded_sources SensorsTests\vcpkg_installed ^
    --excluded_sources packages ^
    --export_type html:coverage\html ^
    --export_type cobertura:coverage\coverage.xml ^
    -- x64\Debug\SensorsTests.exe

echo.
echo ======================================
echo     Coverage Analysis Complete!
echo ======================================
echo HTML Report: coverage\html\index.html
echo XML Report:  coverage\coverage.xml
echo.
echo Opening HTML report...
start coverage\html\index.html

echo.
echo Press any key to exit...
pause > nul