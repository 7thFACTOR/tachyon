@echo Preparing the bin folder
md .\bin

xcopy /y .\code\3rdparty\copy_to_bin\*.* .\bin\

call code\build_tools\build_3rdparty.cmd
