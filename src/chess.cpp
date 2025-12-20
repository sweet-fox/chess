//======= File info =======//
// File: chess.cpp


//========ARGUMENTS:========//
//--game-mode [human-vs-human/human-vs-engine/engine-vs-engine] : choose game mode (default: human-vs-engine)
/*[human-vs-human] : --game-mode human-vs-human */
/*[human-vs-engine] : --game-mode human-vs-engine [engine-path] (default: ./stockfish)[engine-depth] (default: 1) [player-side] (default: w)*/
/*[engine-vs-engine] : --game-mode engine-vs-engine [engine1-path] [engine2-path] [engine1-depth] (default: 1) 
[engine2-depth] (default: 1) [side1] (default: w)*/
//--position [FEN string] : set the starting position using a FEN string (default: standard chess starting position)
/*you can use https://mutsuntsai.github.io/fen-tool/ or https://www.redhotpawn.com/chess/chess-fen-viewer.php for FEN strings*/
//--time-control [time in seconds] : set the time control for each player (default: unlimited)


//======= Includes =======//
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

/*======= Engine communication =======*/

/*This function sends a message to the chess engine
and reads its response. It returns the main line of the response
as a string.*/

void send_message(std::string message,FILE* engine){
    fputs((message+"\n").c_str(), engine);
    fflush(engine);
}
//Reads engine response until a known terminating line is found
std::string read_response(FILE* engine){
    //Reads engine response until a known terminating line is found
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), engine)) {
        std::string line(buffer);
        //std::cout << line;
        //Check for known terminating lines
        if (line.find("uciok")==0 || line.find("readyok")==0 ||
        line.find("Unknown command")==0 || line.find("bestmove")==0 ||
        line.find("registration")==0 || line.find("copyprotection")==0 ||
        line.find("Nodes")==0){
            break;
        }
    } 
    std::cout << '\n';
    std::string line(buffer);
    return line;
}

std::string talk_with_engine(std::string message, FILE* engine){
    send_message(message,engine);
    return read_response(engine);
}

/*======= Board class =======*/
/*This is where all the board data is stored: 
its state, number of moves, and so on. 
It also contains all the main functions for interacting with it.*/

class Board{
    public: 

    //======= Board data =======//
    char board[8][8]; //8x8 board representation
    char side; //Side to move
    std::string castling; //Castling rights
    std::string en_passant; //En passant target square
    int half_move_counter; //Half move counter
    int move_counter; //Full move counter
    std::string fen; //FEN string representation

    std::vector<std::string> move_history; //Move history

    //======= Game settings =======//
    char player_side; //Side of the human player (default: w)
    std::string game_mode; //Game mode (default: human-vs-engine)
    int engine1_depth; //Engine search depth (default: 1)
    int engine2_depth; //Engine search depth (default: 1)
    int time_control; //Time control in seconds (default: -1 for unlimited)
    std::string engine1_path; //Path to the engine executable (default: ./stockfish)
    std::string engine2_path; //Path to the second engine executable (for engine-vs-engine mode)
    char engine1_side; //Side for engine 1 (for engine-vs-engine mode)
    char engine2_side; //Side for engine 2 (for engine-vs-engine mode

    Board(){
        //Default constructor initializing standard chess starting position
        fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        set_position(fen,stdout);
        player_side='w';
        game_mode="human-vs-engine";
        engine1_depth=1;
        time_control=-1;
    }
    
//======= Board functions =======//

//Gets the current position in UCI format
    std::string get_uci_line(){
        std::string pos="";
        //FEN generation
        for (int i=0;i<8;i++){
            int empty_count=0;
            for (int j=0;j<8;j++){
                //Counting empty squares
                if (board[i][j]=='.'){
                    empty_count++;
                }
                else{
                    if (empty_count>0){
                        pos+=std::to_string(empty_count);
                        empty_count=0;
                    }
                    pos+=board[i][j];
                }
            }
            if (empty_count>0){
                pos+=std::to_string(empty_count);
            }
            if (i!=7){
                pos+='/';
            }
        }
    //Other FEN data
        pos = pos +' ' + side + ' ' + castling + ' ' + en_passant + ' ' + 
        std::to_string(half_move_counter) + ' ' + std::to_string(move_counter);
        return pos;
    }

//Sets the board position from a FEN string
    void set_position(std::string pos, FILE* engine){
        //Send position to engine
        int i = 0;
        int row = 0;
        int col = 0;
        std::string alf_num = "0123456789";
        //Parsing FEN string
        while (pos[i]!=' '){
            if (pos[i]=='/'){
                i++;
                row ++;
                col = 0;
            }
            if (alf_num.find(pos[i])==std::string::npos){
                board[row][col]=pos[i];
                col++;
                i++;
            }
            else{
                for (int j=0;j<int(pos[i]-'0');j++){
                    board[row][col]='.';
                    col++;
                }
                i++;
            }
        }
        std::cout << "Board position set.\n";
        i++;
        side=pos[i];
        i+=2;
        castling="";
        while (pos[i]!=' '){
            castling+=pos[i];
            i++;
        }
        i++;
        en_passant="";
        while (pos[i]!=' '){
            en_passant+=pos[i];
            i++;
        }
        i++;
        std::string half_move_str="";
        while (pos[i]!=' '){
            half_move_str+=pos[i];
            i++;
        }
        half_move_counter=std::stoi(half_move_str);
        i++;
        std::string move_str="";
        while (i<int(pos.size())){
            move_str+=pos[i];
            i++;
        }
        move_counter=std::stoi(move_str);
    }

//Checks if a move is legal by asking the engine (not used currently)
    bool move_check(std::string move,FILE* engine){
        std::string right_moves = talk_with_engine("go perft 2",engine);
        if (right_moves.find(move)!=std::string::npos){
                std::cout<<"Move is legal.\n";
                return true;
            }
            else{
                std::cout<<"Move is not legal.\n";
                return false;
            }
    }

//Makes a move on the board
    void do_move(std::string move){
        std::string alf="abcdefgh";
        if (/*move_check(move,engine)*/true){
            //Доска в board лежит в прпвильном порядке 
            //но нумерайия строк обратная
            board[int(7-move[3]+'1')][alf.find(move[2])]=board[int(7-move[1]+'1')][alf.find(move[0])];
            board[int(7-move[1]+'1')][alf.find(move[0])]='.';
            if (side=='w'){
            side='b';
            }
            else{
                side='w';
            }
            move_counter++;
        }
        
    }

//Prints the board to console
    void print_board(){
        //Print board depending on player side
        if (player_side=='w'){
            for (int i=0;i<8;i++){
                std::cout<<8-i<<" ";
                for (int j=0;j<8;j++){
                    std::cout<<board[i][j]<<" ";
                }
                std::cout<<'\n';
            }
            std::cout<<"  a b c d e f g h\n";
        }
        else{
            for (int i=7;i>=0;i--){
                std::cout<<8-i<<" ";
                for (int j=7;j>=0;j--){
                    std::cout<<board[i][j]<<" ";
                }
                std::cout<<'\n';
            }
            std::cout<<"  h g f e d c b a\n";
        }
    }
};

bool arg_to_board(int argc, char* argv[], Board* board){
    for (int i=0;i<argc;i++){

        //Parsing command line arguments
        if (std::string(argv[i])=="--game-mode"){

            //Check if mode argument is provided
            if (std::string(argv[i+1])=="human-vs-human" || std::string(argv[i+1])=="human-vs-engine" || std::string(argv[i+1])=="engine-vs-engine"){
                board->game_mode = argv[i+1];
                i++;
            }
            else{
                std::cout<<"ERROR: no game mode specified after --game-mode argument.\n";
                return 0;
            }

            if (std::string(argv[i])=="human-vs-human"){
                continue;    
            }

            if (std::string(argv[i])=="human-vs-engine"){

                if (argv[i+1][0]!='.' and argv[i+1][0]!='/'){
                    std::cout<<argv[i+1][0] << ' ' << argv[i+1];
                    std::cout<<"ERROR: no engine path specified after --game-mode human-vs-engine argument.\n";
                    return 0;
                }
                else{
                    board->engine1_path = argv[i+1];
                    i++;
                }

                if (strlen(argv[i+1])<3){
                    board->engine1_depth = std::stoi(argv[i+1]);
                    i++;
                }
                else{
                    std::cout<<"WARNING: no engine depth specified after engine path. Using default depth 1.\n";
                    board->engine1_depth = 1;
                }

                if (std::string(argv[i+1])=="w" || std::string(argv[i+1])=="b"){
                    board->player_side = std::string(argv[i+1])[0];
                    i++;
                }
                else{
                    std::cout<<"WARNING: no player side specified after engine depth. Using default side 'w'.\n";
                    board->player_side = 'w';
                }
            }

            if (std::string(argv[i])=="engine-vs-engine"){

                if (argv[i+1][0]!='"'){
                    std::cout<<"ERROR: no engine path specified after --game-mode engine-vs-engine argument.\n";
                    return 0;
                }
                else{
                    board->engine1_path = argv[i+1];
                    i++;
                }

                if (argv[i+1][0]!='"'){
                    std::cout<<"ERROR: no engine path specified after --game-mode human-vs-engine argument.\n";
                    return 0;
                }
                else{
                    board->engine2_path = argv[i+1];
                    i++;
                }

                if (strlen(argv[i+1])<3){
                    board->engine1_depth = std::stoi(argv[i+1]);
                    i++;
                }
                else{
                    std::cout<<"WARNING: no engine depth specified after engine path for engine 1. Using default depth 1.\n";
                    board->engine1_depth = 1;
                }

                if (strlen(argv[i+1])<3){
                    board->engine2_depth = std::stoi(argv[i+1]);
                    i++;
                }
                else{
                    std::cout<<"WARNING: no engine depth specified after engine path for engine 2. Using default depth 1.\n";
                    board->engine2_depth = 1;
                }

                if (std::string(argv[i+1])=="w" || std::string(argv[i+1])=="b"){
                    if (std::string(argv[i+1])[0]=='w'){
                        board->engine1_side='w';
                        board->engine2_side='b';
                    }
                    else{
                        board->engine1_side='b';
                        board->engine2_side='w';
                    }
                    i++;
                }
                else{
                    std::cout<<"WARNING: no engine 1 side specified after engine depth. Using default side 'w'.\n";
                    board->engine1_side = 'w';
                    board->engine2_side = 'b';
                }
            }

        }
                  
        if (std::string(argv[i])=="--position"){
            if (argv[i+1][0]!='"'){
                std::cout<<"ERROR: no FEN string specified after --position argument.\n";
                return 0;
            }
            else{
                board->fen = argv[i+1];
                i++;
            }
        }
    }
    return 1;
}
//======= Main function =======//

int main(int argc, char* argv[]){
    Board board;

   if(arg_to_board(argc,argv,&board)){
       std::cout<<"Arguments parsed successfully.\n";
   }
   else{
       std::cout<<"Error parsing arguments. Exiting.\n";
       return 1;
   }
   std::cout<<board.game_mode<<"\n";

//Starting engine process
    if (board.game_mode=="human-vs-human"){
        std::cout<<"Human vs Human mode selected.\n";
        while (true){
            board.print_board();
            std::cout << "Enter your move in algebraic notation (or 'quit' to exit): ";
            std::string input;
            std::cin>>input;
            if (input=="quit"){
                return 0;
            }
            board.do_move(input);
        }
    }

    else if (board.game_mode=="human-vs-engine"){
        FILE* engine = popen(board.engine1_path.c_str(),"r+");
        if (!engine){
            std::cout<<"ERROR: cant start chess engine";
            return 1;
        }
        send_message("uci",engine);
        read_response(engine);
        send_message("isready",engine);
        read_response(engine);
        board.set_position(board.fen,engine);
        while (true){
            board.print_board();
            std::cout << "Enter your move in algebraic notation (or 'quit' to exit): ";
            std::string input;
            std::cin>>input;
            if (input=="quit"){
                return 0;
            }
            board.do_move(input);
            send_message("position fen "+ board.get_uci_line(),engine);
            send_message("go depth "+ std::to_string(board.engine1_depth),engine);
            board.do_move(read_response(engine).substr(9,4));
        }
    }
    else if (board.game_mode=="engine-vs-engine"){
        std::cout<<"Engine vs Engine mode selected. Not implemented yet.\n";
    }
    return 0;
}

//ДОПИСАТЬ ФУНКЦИЮ main, ДОБАВИТЬ ТАЙМ-КОНТРОЛЬ, ДОБАВИТЬ ОБРАБОТКУ ХОДОВ ЧЕЛОВЕКА
//СДЕЛАТЬ БАЗОВУЮ ПРОЫЕРКУ НА ХОД ЦВЕТОМ ФИГУРЫ, КОТОРАЯ ХОДИТ