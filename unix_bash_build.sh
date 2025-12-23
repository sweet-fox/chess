git clone https://github.com/official-stockfish/Stockfish
cd ./Stockfish/src
make help
make -j profile-build
cd ../..
cmake .
cmake --build .
