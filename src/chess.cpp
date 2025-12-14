#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

class Board{
    char board[8][8];
    char side;
    std::string castling;
    std::string en_passant;
    int half_move_counter;
    int move_counter;

    std::string w_die_figure;
    std::string b_die_figure;

    void set_position(std::string pos){
    }

    void do_move(std::string move){
        std::string alf="abcdefgh";

    }
};

std::string talk_with_engine(std::string message,FILE* engine){
    fputs((message+"\n").c_str(), engine);
    fflush(engine);
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
            if (line.find("uciok")==0 || line.find("readyok")==0 ||
            line.find("Unknown command")==0 || line.find("bestmove")==0 ||
            line.find("registration")==0 || line.find("copyprotection")==0){
                break;
            }
        } 
        std::cout << '\n';
        return std::string(buffer);
    }
    
}

int main(){
    FILE* engine =  popen("./stockfish","r+");
    if (!engine){
        std::cout<<"ERROR: cant start chess engine";
    }
    std::string message ;
    while(true){
        getline(std::cin,message);
        talk_with_engine(message,engine);
    }
}