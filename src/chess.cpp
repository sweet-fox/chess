#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

/*======= Engine communication =======*/
/*This function sends a message to the chess engine
and reads its response. It returns the main line of the response
as a string.*/
std::string talk_with_engine(std::string message,FILE* engine){
    fputs((message+"\n").c_str(), engine);
    fflush(engine);
    std::string main_line;
    std::cout<<'\n';
    if (message.find("position")==0 || message.find("ucinewgame")==0 ||
    message.find("setoption")==0){
        return "";
    }
    else{
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), engine)) {
            std::string line(buffer);
            std::cout << line;
            main_line = main_line + line + " ";
            if (line.find("uciok")==0 || line.find("readyok")==0 ||
            line.find("Unknown command")==0 || line.find("bestmove")==0 ||
            line.find("registration")==0 || line.find("copyprotection")==0 ||
            line.find("Nodes")==0){
                break;
            }
        } 
        std::cout << '\n';
        return std::string(main_line);
    }
    
}

/*======= Board class =======*/
/*This is where all the board data is stored: 
its state, number of moves, and so on. 
It also contains all the main functions for interacting with it.*/

class Board{
    public: 
    char player_side;
    char board[8][8];
    char side;
    std::string castling;
    std::string en_passant;
    int half_move_counter;
    int move_counter;
//======= Board functions =======

//Выдаёт строку типа "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 с ттекущей позиции на доске
    std::string get_uci_line(){
        std::string pos="";
        for (int i=0;i<8;i++){
            int empty_count=0;
            for (int j=0;j<8;j++){
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
        pos+=' ';
        pos+=side;
        pos+=' ';
        pos+=castling;
        pos+=' ';
        pos+=en_passant;
        pos+=' ';
        pos+=std::to_string(half_move_counter);
        pos+=' ';
        pos+=std::to_string(move_counter);
        return pos;
    }

    /*Получает строку типа "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    <доска> <ход стороны> <рокировка> <enpasant> <половинные ходы> <ходы(с 1)>"
    и задаёт из неё позицию на оске и в движке перед следующем ходе*/
    void set_position(std::string pos, FILE* engine){
        talk_with_engine(("position fen " + pos),engine);
        int i = 0;
        int row = 0;
        int col = 0;
        std::string alf_num = "0123456789";
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

//Проверяет легальность хода с помощью движка
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

//Делает ход на доске и меняет сторону
    void do_move(std::string move, FILE* engine){
        std::string alf="abcdefgh";
        if (/*move_check(move,engine)*/true){
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

//Печатает доску в консоль
    void print_board(){
        if (player_side=='w'){
            for (int i=7;i>=0;i--){
                std::cout<<i+1<<" ";
                for (int j=0;j<8;j++){
                    std::cout<<board[i][j]<<" ";
                }
                std::cout<<'\n';
            }
            std::cout<<"  a b c d e f g h\n";
        }
        else{
            for (int i=0;i<8;i++){
                std::cout<<i+1<<" ";
                for (int j=7;j>=0;j--){
                    std::cout<<board[i][j]<<" ";
                }
                std::cout<<'\n';
            }
            std::cout<<"  h g f e d c b a\n";
        }
    }
};

//======= Main function =======

int main(){
    FILE* engine = popen("./stockfish","r+");
    if (!engine){
        std::cout<<"ERROR: cant start chess engine";
    }
    //Initial board setup
    Board board;
    //Initial engine setup
    talk_with_engine("uci",engine);
    talk_with_engine("isready",engine);
    board.set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",engine);
    
    std::cout << "Choose your side (w/b): ";
    std::cin >> board.player_side;

    std::string move;
//Game loop
    while (true){
        //Print board and ask for move
        board.print_board();
        std::cout<<"Enter your move: ";
        std::cin>>move;
        std::cout<<board.get_uci_line()<<'\n';
        //Make move on board and in engine
        board.do_move(move,engine);
        std::cout<<board.get_uci_line()<<'\n';
        //Get engine's response
        board.set_position(board.get_uci_line(),engine);
        std::cout<<board.get_uci_line()<<'\n';
        //Ask engine for its move
        talk_with_engine("go depth 3",engine);
        std::cout<<'\n';
        std::cout<<board.board<<'\n';
    }
}