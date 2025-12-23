# CHESS (⌐■_■)

 ## How to use:
1. Download and compile any engine that supports the UCI protocol.
2. Compile the project: cd /chess/scr, g++(gcc) chess.cpp
3. Play
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