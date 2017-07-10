#!/bin/bash 

md ./bin

cp ./code/3rdparty/copy_to_bin/*.* ./bin/

sh ./code/build_tools/build_3rdparty.sh
