@echo off
cd ../build
cmake -G"Unix Makefiles" ../source
make
cmake --install . --prefix ../source
cd ../source
