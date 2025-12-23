pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make git -y
git clone https://github.com/sweet-fox/chess
cd chess
git clone https://github.com/official-stockfish/Stockfish
cd chess/Stockfish/src
mingw32-make -j profile-build
cd ../..
cmake .
cmake --build .

