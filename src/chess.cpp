//======= File info =======//
// File: chess.cpp

//========ARGUMENTS:========//
//--game-mode(-gm) [human-vs-human/human-vs-engine/engine-vs-engine](hvh,hve,eve) : choose game mode (default: human-vs-human)
//--engine1-path(-ep1) [path to engine executable] : set the path to the first engine executable (default: ./stockfish)
//--engine2-path(-ep2) [path to engine executable] : set the path to the second engine executable (for engine-vs-engine mode)
//--engine1-depth(-ed1) [search depth] : set the search depth for the first engine (default: 1)
//--engine2-depth(-ed2) [search depth] : set the search depth for the second
//--engine1-side(-es1) [w/b] : set the side for the first engine (for engine-vs-engine mode)
//--player-side(-ps) [w/b] : set the side for the human player (default: w)
//--position(-p) [FEN string] : set the starting position using a FEN string (default: standard chess starting position)
/*you can use https://mutsuntsai.github.io/fen-tool/ or https://www.redhotpawn.com/chess/chess-fen-viewer.php for FEN strings*/
//--time-control(-tc) [time in seconds] : set the time control for each player (default: unlimited)


//======= Includes =======//
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>

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
    long long time_control1; //Time control in milliseconds, but write it in seconds (default: 2**64 for unlimited)
    long long time_control2; //Time control in milliseconds, but write it in seconds (default: 2**64 for unlimited)
    std::string engine1_path; //Path to the engine executable (default: ./stockfish)
    std::string engine2_path; //Path to the second engine executable (for engine-vs-engine mode)
    char engine1_side; //Side for engine 1 (for engine-vs-engine mode)
    char engine2_side; //Side for engine 2 (for engine-vs-engine mode)
//======= Board constructor =======//
    Board(){
        //Default constructor initializing standard chess starting position
        fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        set_position(fen,stdout);
        player_side='w';
        side = 'w';
        game_mode="human-vs-human";
        engine1_depth=10*100000;
        time_control1=10*100000;
        time_control2=10*100000;
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
        if (side=='w'){
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

//======= Game mode functions =======//
void HumanVSHuman(Board* board){
    std::cout<<"Human vs Human mode selected.\n";
    board->set_position(board->fen,stdout);
    while (true){
        board->print_board();
        std::cout << "Enter your move in algebraic notation (or 'quit' to exit): ";
        std::string input;
        std::cin>>input;
        if (input=="quit"){
            return;
        }
        board->do_move(input);
    }
}

void HumanVSEngine(Board* board){
    std::cout <<board->time_control1<<'\n';
    FILE* engine = popen(board->engine1_path.c_str(),"r+");
        if (!engine){
            std::cout<<"ERROR: cant start chess engine";
            return;
        }
        send_message("uci",engine);
        read_response(engine);
        send_message("isready",engine);
        read_response(engine);
        board->set_position(board->fen,engine);
        if (board->engine1_side==board->side){
            board->print_board();
            send_message("position fen "+ board->get_uci_line(),engine);
            send_message("go depth "+ std::to_string(board->engine1_depth),engine);
            board->do_move(read_response(engine).substr(9,4));
        }
        while (true){
            if (board->time_control1<=0){
                return;
            }
            board->print_board();
            std::cout << "Enter your move in algebraic notation (or 'quit' to exit): ";
            std::string input;
            auto start_time = std::chrono::high_resolution_clock::now();
            std::cin>>input;
            auto end_time = std::chrono::high_resolution_clock::now();
            board->time_control1 -= std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            if (input=="quit"){
                return;
            }
            board->do_move(input);
            send_message("position fen "+ board->get_uci_line(),engine);
            send_message("go depth "+ std::to_string(board->engine1_depth),engine);
            board->do_move(read_response(engine).substr(9,4));
        }
}

void EngineVSEngine(Board* board){
    std::cout<<"Engine vs Engine mode selected. Not implemented yet.\n";
}

//======= Argument parsing functions =======//
bool parse_game_mode(int argc, char* argv[],Board* board){
    for (int i=0;i<argc;i++){
        std::string arg=argv[i];
        if ((arg=="--game-mode" || arg == "-gm") && i+1<argc){
            board->game_mode=argv[i+1];
        }
    }
    return 0;
}

bool parse_engine1_path(int argc, char* argv[],Board* board){
    for (int i=0;i<argc;i++){
        std::string arg=argv[i];
        if ((arg=="--engine1-path" || arg == "-ep1") && i+1<argc){
            board->engine1_path=argv[i+1];
        }
        else{
            board->engine1_path="./stockfish";
        }
    }
    return 0;
}

bool parse_engine2_path(int argc, char* argv[],Board* board){
    for (int i=0;i<argc;i++){
        std::string arg=argv[i];
        if ((arg=="--engine2-path" || arg == "-ep2") && i+1<argc){
            board->engine2_path=argv[i+1];
        }
        else{
            board->engine2_path="./stockfish";
        }
    }
    return 0;
}

bool parse_engine1_depth(int argc, char* argv[],Board* board){
    for (int i=0;i<argc;i++){
        std::string arg=argv[i];
        if ((arg=="--engine1-depth" || arg == "-ed1") && i+1<argc){
            board->engine1_depth=std::stoi(argv[i+1]);
        }
        else{
            board->engine1_depth=1;
        }

    }
    return 0;
}

bool parse_engine2_depth(int argc, char* argv[],Board* board){
    for (int i=0;i<argc;i++){
        std::string arg=argv[i];
        if ((arg=="--engine2-depth" || arg == "-ed2") && i+1<argc){
            board->engine2_depth=std::stoi(argv[i+1]);
        }
        else{
            board->engine2_depth=1;
        }
    }
    return 0;
}

bool parse_sides(int argc, char* argv[],Board* board){
    for (int i=0;i<argc;i++){
        std::string arg=argv[i];
        if ((arg=="--player-side" || arg=="-ps") && i+1<argc){
            if (argv[i+1][0]=='w'){
                board->player_side='w';
                board->engine1_side='b';
            }
            else{
                board->player_side='b';
                board->engine1_side='w';
            }
            return 1;
        }
        else if((arg=="--engine1-side" || arg=="-es1") && i+1<argc){
            if (argv[i+1][0]=='w'){
                board->engine1_side='w';
                board->engine2_side='b';
            }
            else{
                board->engine1_side='b';
                board->engine2_side='w';
            }
            return 1;
        }
    }
    return 0;
}

bool parse_position(int argc, char* argv[],Board* board){
    for (int i=0;i<argc;i++){
        std::string arg = argv[i];
        if ((arg=="--position" || arg=="-p") && i+1<argc){
            board->fen = argv[i+1];
            return 1;
        }
    }
    return 0;
}

bool parse_time_control(int argc, char* argv[],Board* board){
    for (int i;i<argc;i++){
        std::string arg=argv[i];
        if ((arg=="--time-control" || arg=="-tc") && i+1<argc){
            board->time_control1=std::stoi(argv[i+1])*1000;
            board->time_control2=std::stoi(argv[i+1])*1000;
            return 1;
        }
        else{
            board->time_control1=pow(2,64);
            board->time_control2=pow(2,64);
        }
    }
    return 0;
}

bool arg_to_board(int argc, char* argv[],Board* board){
    parse_game_mode(argc,argv,board);
    parse_engine1_path(argc,argv,board);
    parse_engine2_path(argc,argv,board);
    parse_engine1_depth(argc,argv,board);
    parse_engine2_depth(argc,argv,board);
    parse_sides(argc,argv,board);
    parse_position(argc,argv,board);
    parse_time_control(argc,argv,board);
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

   if (board.game_mode=="human-vs-human" || board.game_mode=="hvh"){
       HumanVSHuman(&board);
   }
   else if (board.game_mode=="human-vs-engine" || board.game_mode=="hve"){
       HumanVSEngine(&board);
   }
   else if (board.game_mode=="engine-vs-engine" || board.game_mode=="eve"){
       EngineVSEngine(&board);
   }
   else{
       std::cout<<"Invalid game mode. Exiting.\n";
       return 1;
   }
}

//ДОПИСАТЬ ФУНКЦИЮ main, ДОБАВИТЬ ТАЙМ-КОНТРОЛЬ, ДОБАВИТЬ ОБРАБОТКУ ХОДОВ ЧЕЛОВЕКА
//СДЕЛАТЬ БАЗОВУЮ ПРОЫЕРКУ НА ХОД ЦВЕТОМ ФИГУРЫ, КОТОРАЯ ХОДИТ
//ПЕРЕПИСАТЬ В ФУНКЦИИ РЕЖИМЫ ИГР
//переписать инициализатоор класса
