git clone https://github.com/official-stockfish/Stockfish
cd ./Stockfish/src
mingw32-make -j profile-build
cd ../..
cmake .
cmake --build .

