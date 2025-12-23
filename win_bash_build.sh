cd ./Stockfish/src
mingw32-make -j profile-build
cd ../..
cmake .
cmake --build .

