@echo off

echo.
echo Building Binaries (VS 2017)...
echo.
set TOOL_DIR=%VS150COMNTOOLS%..\IDE

call :Exec .\sdk\sln\Ne1.sln "Debug|x64"
call :Exec .\sdk\sln\Ne1.sln "Release|x64"
call :Exec .\sdk\sln\Ne1.sln "Master|x64"

:Success
exit /b 0

:Fail
echo Build failed!
exit /b 1

:Exec
echo %2
"%TOOL_DIR%\devenv.com" %1 /build %2 1<NUL
if ERRORLEVEL 1 goto Fail
exit /b 0
