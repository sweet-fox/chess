# CHESS (⌐■_■)
---
---
## How to install
---
### Unix
#### long
`git clone https://github.com/sweet-fox/chess`
`cd chess`
`cd ./Stockfish/src`
`make help`
`make make -j profile-build`
`cd ..`
`cd ..`
`cmake .`
`cmake --build .`
`./chess` (use arguments)
#### short
1. `git clone https://github.com/sweet-fox/chess && cd chess && git clone https://github.com/official-stockfish/Stockfish && cd ./Stockfish/src && make help && make -j profile-build && cd ../.. && cmake . && cmake --build .`
    `./chess` (use arguments)

##### or

2. `git clone https://github.com/sweet-fox/chess && cd chess && chmod +x unix_bash_build.sh && bash unix_bash_build.sh`
    `./chess` (use arguments)
    
---
### Windows
#### long
W+R
`cmd`
Enter
`winget install MSYS2.MSYS2 && start C:\msys64\mingw64.exe`
in new window (shift+ins or ins) after press y
`pacman -Syu`
and open again in cmd `start C:\msys64\mingw64.exe`
`pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make git`
`git clone https://github.com/sweet-fox/chess`
`cd chess`
`git clone https://github.com/official-stockfish/Stockfish`
`cd ./Stockfish/src`
`mingw32-make -j profile-build`
`cd ..`
`cd ..`
`cmake .`
`cmake --build .`
`./chess.exe` (use arguments)
#### short
W+R
`cmd`
Enter
`winget install MSYS2.MSYS2 && start C:\msys64\mingw64.exe`
in new window (shift+ins or ins) after press y
`pacman -Syu`
and open again in cmd `start C:\msys64\mingw64.exe`

##### after 

1. `pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make && git clone https://github.com/sweet-fox/chess && cd chess && git clone https://github.com/official-stockfish/Stockfish && cd ./Stockfish/src && mingw32-make -j profile-build && cd ../.. && cmake . && cmake --build .`
    `./chess.exe` (use arguments)

##### or

2. `git clone https://github.com/sweet-fox/chess && cd chess && chmod +x win_bash_build.sh && bash win_bash_build.sh`
    `./chess.exe` (use arguments)

 ## How to use:

ARGUMENTS:

| Argument          | Short  | Description                                                      | Default Value                    |
| ----------------- | ------ | ---------------------------------------------------------------- | -------------------------------- |
| `--game-mode`     | `-gm`  | Game mode: human vs human, human vs engine, or engine vs engine  | `human-vs-human`                 |
|                   |        | Possible values: `hvh`, `hve`, `eve`                             |                                  |
| `--engine1-path`  | `-ep1` | Path to the first chess engine executable                        | `./stockfish`                    |
| `--engine2-path`  | `-ep2` | Path to the second chess engine executable (only for `eve` mode) | —                                |
| `--engine1-depth` | `-ed1` | Search depth for the first engine                                | `1`                              |
| `--engine2-depth` | `-ed2` | Search depth for the second engine                               | —                                |
| `--engine1-side`  | `-es1` | Side (white/black) for the first engine (only for `eve` mode)    | —                                |
| `--player-side`   | `-ps`  | Side (white/black) for the human player                          | `w`                              |
| `--position`      | `-p`   | Starting position in FEN format                                  | standard chess starting position |
| `--time-control`  | `-tc`  | Time control for each player (in seconds)                        | unlimited                        |

You can use https://mutsuntsai.github.io/fen-tool/ or https://www.redhotpawn.com/chess/chess-fen-viewer.php for FEN strings