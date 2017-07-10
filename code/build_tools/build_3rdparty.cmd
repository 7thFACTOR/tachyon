echo Building 3rdParty libraries...
echo 	Building [Release]...
MSBuild "%basePath%\build\3rdparty\ALL_BUILD.vcxproj" /t:clean,build /p:Configuration=Release;Platform=x64
echo 	Building [Debug]...
MSBuild "%basePath%\build\3rdparty\ALL_BUILD.vcxproj" /t:clean,build /p:Configuration=Debug;Platform=x64

REM ===========================================================================
REM Check for build errors
REM ===========================================================================

if errorlevel 1 (
	echo Error building 3rdParty libraries!
	exit /B 1
)

REM ===========================================================================
REM Deploy the files
REM ===========================================================================

echo Deploying 3rdParty libraries...
echo Installing OpenAL...
%basePath%/code/3rdparty/SFML/oalinst.exe /silent