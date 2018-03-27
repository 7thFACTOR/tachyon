set cdb = %CD%
cd ..\
echo Deleting old version...
del engine\core\revision_number.h
echo Extracting Mercurial last revision number and generating header...
echo %CD%
hg parent --template "#pragma once \r\nconst const char* repositoryChangelist= \"{node}\";" >> engine\core\revision_number.h
cd %cdb%
exit 0