// Engine.cpp
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include "Board.h"
#include "Move.h"
#include "Pieces.h"

using namespace std;

void newGame() {
    isWhiteTurn = true;

    Board = {
        'r','n','b','q','k','b','n','r',
        'p','p','p','p','p','p','p','p',
        '.','.','.','.','.','.','.','.',
        '.','.','.','.','.','.','.','.',
        '.','.','.','.','.','.','.','.',
        '.','.','.','.','.','.','.','.',
        'P','P','P','P','P','P','P','P',
        'R','N','B','Q','K','B','N','R'
    };

    whiteKingMoved = false;
    whiteLeftRookMoved = false;
    whiteRightRookMoved = false;

    blackKingMoved = false;
    blackLeftRookMoved = false;
    blackRightRookMoved = false;

    moveHistory.clear();
    stateHistory.clear();
}

bool tryMoveUCI(const string& uci) {
    if (uci.size() < 4) return false;

    char fFile = uci[0];
    char fRank = uci[1];
    char tFile = uci[2];
    char tRank = uci[3];

    if (fFile < 'a' || fFile > 'h') return false;
    if (tFile < 'a' || tFile > 'h') return false;
    if (fRank < '1' || fRank > '8') return false;
    if (tRank < '1' || tRank > '8') return false;

    int inCol = fFile - 'a';
    int fiCol = tFile - 'a';
    int inRow = fRank - '0';
    int fiRow = tRank - '0';

    char promo = '\0';
    if (uci.size() >= 5) {
        promo = uci[4];
        if (promo >= 'A' && promo <= 'Z') promo = char(promo - 'A' + 'a');
        if (!(promo == 'q' || promo == 'r' || promo == 'b' || promo == 'n')) promo = '\0';
    }

    return finaliseMove(inCol, inRow, fiCol, fiRow, promo);
}

string board64() {
    string b;
    b.reserve(64);
    for (int i = 0; i < 64; i++) b += Board[i];
    return b;
}

char sideToMove() {
    return isWhiteTurn ? 'w' : 'b';
}

vector<string> legalMovesFrom(const string& square) {
    vector<string> legalMoves;
    if (square.size() != 2) return legalMoves;

    int fromCol = square[0] - 'a';
    int fromRow = square[1] - '0';
    if (fromCol < 0 || fromCol > 7 || fromRow < 1 || fromRow > 8) return legalMoves;

    int fromIndex = calculateIndex(fromCol, fromRow);
    char piece = Board[fromIndex];
    if (piece == '.') return legalMoves;

    if (isWhiteTurn && piece >= 'a' && piece <= 'z') return legalMoves;
    if (!isWhiteTurn && piece >= 'A' && piece <= 'Z') return legalMoves;

    for (int i = 0; i < 64; i++) {
        int toCol, toRow;
        indexToColRow(i, toCol, toRow);

        if (!isMoveValid(fromCol, fromRow, toCol, toRow)) continue;

        TempState s = makeMoveTemp(fromCol, fromRow, toCol, toRow);

        bool illegal = false;
        if (isWhiteTurn) {
            if (isWhiteInCheck()) illegal = true;
        } else {
            if (isBlackInCheck()) illegal = true;
        }

        undoMoveTemp(s);
        if (illegal) continue;

        string base;
        base.reserve(4);
        base += char('a' + fromCol);
        base += char('0' + fromRow);
        base += char('a' + toCol);
        base += char('0' + toRow);

        
        if (piece == 'P' && toRow == 8) {
            legalMoves.push_back(base + "q");
            legalMoves.push_back(base + "r");
            legalMoves.push_back(base + "b");
            legalMoves.push_back(base + "n");
        } else if (piece == 'p' && toRow == 1) {
            legalMoves.push_back(base + "q");
            legalMoves.push_back(base + "r");
            legalMoves.push_back(base + "b");
            legalMoves.push_back(base + "n");
        } else {
            legalMoves.push_back(base);
        }
    }

    return legalMoves;
}

bool isInCheck(char side) {
    if (side == 'w') return isWhiteInCheck();
    if (side == 'b') return isBlackInCheck();
    return false;
}

bool isCheckmate() {
    return isWhiteCheckmated() || isBlackCheckmated();
}

bool isStalemate() {
    return isWhiteStalemated() || isBlackStalemated();
}

bool isInsufficientMaterial() {
    int wBishops = 0, wKnights = 0, wOther = 0;
    int bBishops = 0, bKnights = 0, bOther = 0;
    int wBishopColor = -1, bBishopColor = -1;

    for (int i = 0; i < 64; i++) {
        char c = Board[i];
        if (c == '.' || c == 'K' || c == 'k') continue;

        int squareCol = (i / 8 + i % 8) % 2;

        switch (c) {
            case 'B': wBishops++; wBishopColor = squareCol; break;
            case 'N': wKnights++; break;
            case 'b': bBishops++; bBishopColor = squareCol; break;
            case 'n': bKnights++; break;
            case 'P': case 'R': case 'Q': wOther++; break;
            case 'p': case 'r': case 'q': bOther++; break;
        }
    }

    if (wOther > 0 || bOther > 0) return false;

    int wMinors = wBishops + wKnights;
    int bMinors = bBishops + bKnights;

    if (wMinors == 0 && bMinors == 0) return true;
    if (wMinors == 1 && bMinors == 0) return true;
    if (bMinors == 1 && wMinors == 0) return true;

    if (wBishops == 1 && wKnights == 0 &&
        bBishops == 1 && bKnights == 0 &&
        wBishopColor == bBishopColor) return true;

    return false;
}

bool isThreefoldRepetition() {
    if (stateHistory.size() < 4) return false;

    const auto& curBoard = Board;
    bool curTurn = isWhiteTurn;
    bool curWKM  = whiteKingMoved,  curWLRM = whiteLeftRookMoved,  curWRRM = whiteRightRookMoved;
    bool curBKM  = blackKingMoved,  curBLRM = blackLeftRookMoved,  curBRRM = blackRightRookMoved;

    int count = 1;
    for (const FullState& s : stateHistory) {
        if (s.board               == curBoard  &&
            s.isWhiteTurn         == curTurn   &&
            s.whiteKingMoved      == curWKM    &&
            s.whiteLeftRookMoved  == curWLRM   &&
            s.whiteRightRookMoved == curWRRM   &&
            s.blackKingMoved      == curBKM    &&
            s.blackLeftRookMoved  == curBLRM   &&
            s.blackRightRookMoved == curBRRM) {
            count++;
            if (count >= 3) return true;
        }
    }
    return false;
}

const std::vector<Move>& getMoveHistory() {
    return moveHistory;
}
