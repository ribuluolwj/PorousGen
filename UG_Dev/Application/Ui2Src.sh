#!/bin/bash
DATE=$(date +"%Y%m%d%H%M")
SHELL_PATH=$(cd "$(dirname $0)";pwd)
REMOTE_PATH="../_Source/Porous2D/Porous2D"
if [ -n "ls ./*.dll" ];then
    cp -vf *.dll ./bak
fi
if [ -n "ls ./*.dlx" ];then
    cp -vf *.dlx ./bak
    echo ".dll and .dlx has been backuped."
fi
if [ -n "${REMOTE_PATH}" ];then
    cd ${REMOTE_PATH}
    printf "Change directory to source path:"
    echo $(pwd)
    if [ -n "ls ./*.cpp" ];then
        for tfile in *.cpp
        do
            tshort=${tfile%.cpp}
            tbak="${tshort}_${DATE}.cpp"
            mv -vf ${tfile} ./bak/${tbak}
        done
    else
	echo "There is no .cpp files in source directory."
    fi
    if [ -n "ls ./*.hpp" ];then
        for tfile in *.hpp
        do
            tshort=${tfile%.hpp}
            tbak="${tshort}_${DATE}.hpp"
            mv -vf ${tfile} ./bak/${tbak}
        done
    else
	echo "There is no .hpp files in source directory."
    fi
else
    echo "Error! No source directory found!"
fi
cd ${SHELL_PATH}
printf "Change directory to shell path:"
echo "${SHELL_PATH}"
if [ -n "ls ./*.cpp" ];then
    mv -vf *.cpp ${REMOTE_PATH}
else
    echo "There is no .cpp files in Application directory."
fi
if [ -n "ls ./*.hpp" ];then
    mv -vf *.hpp ${REMOTE_PATH}
else
    echo "There is no .hpp files in Application directory."
fi
