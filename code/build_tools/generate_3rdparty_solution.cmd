cls
set CRTDIR=%CD%
set BUILD_PATH=%CD%\build\3rdparty
set ROOT_PATH=%CD%
REM TODO: hack, VS2017 doesnt set VS150COMNTOOLS anymore! so we do it brute force
set VS150COMNTOOLS="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\"

if "%VS140COMNTOOLS%"=="" goto VS2015NotInstalledError
call "%VS140COMNTOOLS%\vsvars32.bat"

%CRTDIR:~0,2%
cd "%CRTDIR%"
mkdir "%BUILD_PATH%"
cd "%BUILD_PATH%"

cmake -G "Visual Studio 14 Win64" "../3rdparty"
goto end

:VS2015NotInstalledError
echo ERROR: Visual Studio 14 [2015] is not installed properly!
goto end

:end
%CRTDIR:~0,2%
cd %CRTDIR%