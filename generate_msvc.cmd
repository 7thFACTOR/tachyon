call ./code/build_tools/extract_git_rev.cmd
cd code
set VSVER=vs2017
IF EXIST vs2015 set VSVER=vs2015
premake5 %VSVER%
cd ..