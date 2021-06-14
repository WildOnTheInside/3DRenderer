@echo off
cd ../build
cmake -G"Unix Makefiles" ../source
make
cmake --install . --prefix C:/Users/stmat/CS/3DRenderer/project/source
cd ../source