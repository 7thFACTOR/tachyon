@echo off

echo Building 3rdParty libraries...

if "%VS140COMNTOOLS%"=="" goto VSNotInstalledError
call "%VS140COMNTOOLS%\vsvars32.bat"

echo Building Freetype [Release]...
MSBuild ".\code\3rdparty\freetype\builds\windows\vc2015\freetype.vcxproj" /t:clean,build /p:Configuration="Release Multithreaded";Platform=x64

echo Building ZLib [Release]
MSBuild "code\3rdparty\zlib\contrib\vstudio\vc14\zlibstat.vcxproj" /t:clean,build /p:Configuration="ReleaseWithoutAsm";Platform=x64

echo Building Squish [Release]
MSBuild "code\3rdparty\squish\vs2015\squish\squish.vcxproj" /t:clean,build /p:Configuration="Debug";Platform=x64
MSBuild "code\3rdparty\squish\vs2015\squish\squish.vcxproj" /t:clean,build /p:Configuration="Release";Platform=x64

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

xcopy /y .\code\3rdparty\Cg\bin\*.dll .\bin\
xcopy /y .\code\3rdparty\Cg\bin\*.exe .\bin\
xcopy /y .\code\3rdparty\physx\PhysX_3.4\Bin\vc14win64\*.dll .\bin\

echo Deploying 3rdParty libraries...
echo Installing OpenAL...
.\code\3rdparty\SFML\oalinst.exe /silent