// Board.cpp
#include "Board.h"

std::array<char, 64> Board = {
    'r','n','b','q','k','b','n','r',
    'p','p','p','p','p','p','p','p',
    '.','.','.','.','.','.','.','.',
    '.','.','.','.','.','.','.','.',
    '.','.','.','.','.','.','.','.',
    '.','.','.','.','.','.','.','.',
    'P','P','P','P','P','P','P','P',
    'R','N','B','Q','K','B','N','R'
};

std::string showBoard() {
    std::string s;
    int rowLabel = 8;

    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) {
            s += std::to_string(rowLabel);
            s += "| ";
        }

        s += Board[i];
        s += ' ';

        if ((i + 1) % 8 == 0) {
            s += '\n';
            rowLabel--;
        }
    }

    s += "   - - - - - - - - \n";
    s += "   a b c d e f g h\n";
    return s;
}
