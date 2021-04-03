rm -r ./SDK_C++/CodeCraft-2021/include
rm -r ./SDK_C++/CodeCraft-2021/src
cp -r ./include ./SDK_C++/CodeCraft-2021
cp -r ./src ./SDK_C++/CodeCraft-2021
./SDK_C++/build.sh
./SDK_C++/CodeCraft_zip.sh

