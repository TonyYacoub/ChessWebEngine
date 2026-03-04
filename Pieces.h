// Pieces.h
#ifndef PIECES_H
#define PIECES_H

#include <cmath>
#include <iostream>
#include <vector>
#include <array>
#include "Board.h"
#include "Move.h"

using namespace std;

struct FullState {
    array<char, 64> board;
    bool isWhiteTurn;

    bool whiteKingMoved;
    bool whiteLeftRookMoved;
    bool whiteRightRookMoved;

    bool blackKingMoved;
    bool blackLeftRookMoved;
    bool blackRightRookMoved;

    size_t moveHistorySize;
};

extern bool isWhiteTurn;

extern bool whiteKingMoved;
extern bool whiteLeftRookMoved;
extern bool whiteRightRookMoved;

extern bool blackKingMoved;
extern bool blackLeftRookMoved;
extern bool blackRightRookMoved;

extern std::vector<Move> moveHistory;
extern std::vector<FullState> stateHistory;




inline bool undoLastMove() {
    if (stateHistory.empty()) return false;

    FullState s = stateHistory.back();
    stateHistory.pop_back();

    Board = s.board;
    isWhiteTurn = s.isWhiteTurn;

    whiteKingMoved = s.whiteKingMoved;
    whiteLeftRookMoved = s.whiteLeftRookMoved;
    whiteRightRookMoved = s.whiteRightRookMoved;

    blackKingMoved = s.blackKingMoved;
    blackLeftRookMoved = s.blackLeftRookMoved;
    blackRightRookMoved = s.blackRightRookMoved;

    moveHistory.resize(s.moveHistorySize);
    return true;
}

inline int calculateIndex(int col, int row){
    int r = 8 - row;
    return r * 8 + col;
}

inline void indexToColRow(int index, int& col, int& row) {
    col = index % 8;
    row = 8 - (index / 8);
}


inline void makeMove(int inCol, int inRow, int fiCol, int fiRow){
    int a = calculateIndex(inCol,inRow);
    int b = calculateIndex(fiCol,fiRow);
    Board[b] = Board[a];
    Board[a] = '.';
}

inline bool isCapturing(int inCol, int inRow, int fiCol, int fiRow){
    int a = calculateIndex(inCol, inRow);
    int b = calculateIndex(fiCol, fiRow);
    if (Board[b] == '.') return false;

    if (Board[a] >= 'A' && Board[a] <= 'Z' && Board[b] >= 'a' && Board[b] <= 'z') return true;
    if (Board[a] >= 'a' && Board[a] <= 'z' && Board[b] >= 'A' && Board[b] <= 'Z') return true;
    return false;
}

inline bool capturingOwnPiece(int inCol, int inRow, int fiCol, int fiRow){
    int a = calculateIndex(inCol, inRow);
    int b = calculateIndex(fiCol, fiRow);
    if (Board[b] == '.') return false;

    if (Board[a] >= 'A' && Board[a] <= 'Z' && Board[b] >= 'A' && Board[b] <= 'Z') return true;
    if (Board[a] >= 'a' && Board[a] <= 'z' && Board[b] >= 'a' && Board[b] <= 'z') return true;
    return false;
}

// FIX: added bounds checks on all ray loops to prevent out-of-bounds array access
// which previously could produce garbage attack values and corrupt move generation.
inline bool isSquareAttacked(bool byWhite, int col, int row){
    for (int i = 0; i < 64; i++){
        int r = 8 - (i / 8);
        int c = i % 8;

        if (Board[i] == '.') continue;

        if (byWhite){
            if (Board[i] < 'A' || Board[i] > 'Z') continue;
        } else {
            if (Board[i] < 'a' || Board[i] > 'z') continue;
        }

        if (r == row && c == col) continue;

        if (Board[i] == 'r'||Board[i]=='R'){
            if (c == col){
                if (r < row){
                    int rr = r + 1;
                    while (rr < row){
                        if (Board[calculateIndex(c, rr)] != '.') break;
                        rr++;
                    }
                    if (rr == row) return true;
                } else {
                    int rr = r - 1;
                    while (rr > row){
                        if (Board[calculateIndex(c, rr)] != '.') break;
                        rr--;
                    }
                    if (rr == row) return true;
                }
            }

            if (r == row){
                if (c < col){
                    int cc = c + 1;
                    while (cc < col){
                        if (Board[calculateIndex(cc, r)] != '.') break;
                        cc++;
                    }
                    if (cc == col) return true;
                } else {
                    int cc = c - 1;
                    while (cc > col){
                        if (Board[calculateIndex(cc, r)] != '.') break;
                        cc--;
                    }
                    if (cc == col) return true;
                }
            }
        }

        if (Board[i] == 'b'||Board[i]=='B') {
            if (abs(row - r) != abs(col - c)) continue;

            int rr = r;
            int cc = c;

            int dr = (row - r) / abs(row - r);
            int dc = (col - c) / abs(col - c);

            while (true) {
                rr += dr;
                cc += dc;

                if (rr < 1 || rr > 8 || cc < 0 || cc > 7) break;

                if (rr == row && cc == col) return true;
                if (Board[calculateIndex(cc, rr)] != '.') break;
            }
        }

        if(Board[i]=='n'||Board[i]=='N'){
            if((abs(r-row)==1&&abs(c-col)==2)||(abs(r-row)==2&&abs(c-col)==1)) return true;
        }

        if(Board[i]=='q'||Board[i]=='Q'){

            if (abs(row - r) == abs(col - c) && row != r) {
                int rr = r;
                int cc = c;

                int dr = (row - r) / abs(row - r);
                int dc = (col - c) / abs(col - c);

                while (true) {
                    rr += dr;
                    cc += dc;

                    if (rr < 1 || rr > 8 || cc < 0 || cc > 7) break;

                    if (rr == row && cc == col) return true;
                    if (Board[calculateIndex(cc, rr)] != '.') break;
                }
            }

            if (c == col){
                if (r < row){
                    int rr = r + 1;
                    while (rr < row){
                        if (Board[calculateIndex(c, rr)] != '.') break;
                        rr++;
                    }
                    if (rr == row) return true;
                } else {
                    int rr = r - 1;
                    while (rr > row){
                        if (Board[calculateIndex(c, rr)] != '.') break;
                        rr--;
                    }
                    if (rr == row) return true;
                }
            }

            if (r == row){
                if (c < col){
                    int cc = c + 1;
                    while (cc < col){
                        if (Board[calculateIndex(cc, r)] != '.') break;
                        cc++;
                    }
                    if (cc == col) return true;
                } else {
                    int cc = c - 1;
                    while (cc > col){
                        if (Board[calculateIndex(cc, r)] != '.') break;
                        cc--;
                    }
                    if (cc == col) return true;
                }
            }
        }

        if (Board[i] == 'P') {
            if (row == r + 1 && (col == c - 1 || col == c + 1))
                return true;
        }
        if (Board[i] == 'p') {
            if (row == r - 1 && (col == c - 1 || col == c + 1))
                return true;
        }

        if(Board[i]=='K'||Board[i]=='k'){
            if(abs(row-r)<=1 && abs(col-c)<=1 && !(row==r && col==c))
                return true;
        }
    }
    return false;
}

inline bool isWhiteInCheck(){
    int king = -1;
    for(int i = 0; i < 64; i++){
        if(Board[i] == 'K'){ king = i; break; }
    }
    if(king < 0) return false;
    int col = 0, row = 0;
    indexToColRow(king, col, row);
    return isSquareAttacked(false, col, row);
}

inline bool isBlackInCheck(){
    int king = -1;
    for(int i = 0; i < 64; i++){
        if(Board[i] == 'k'){ king = i; break; }
    }
    if(king < 0) return false;
    int col = 0, row = 0;
    indexToColRow(king, col, row);
    return isSquareAttacked(true, col, row);
}


inline bool isCastlingMove(int inCol, int inRow, int fiCol, int fiRow){
    int i = calculateIndex(inCol, inRow);
    char k = Board[i];

    if (inRow != fiRow) return false;
    if (abs(fiCol - inCol) != 2) return false;

    if (k == 'K'){
        if (isSquareAttacked(false, 4, 1)) return false;
        if (whiteKingMoved) return false;
        if (inCol != 4 || inRow != 1) return false;

        if (fiCol == 6){
            if (whiteRightRookMoved) return false;
            if (Board[calculateIndex(5,1)] != '.') return false;
            if (Board[calculateIndex(6,1)] != '.') return false;
            if (Board[calculateIndex(7,1)] != 'R') return false;
            return true;
        }

        if (fiCol == 2){
            if (whiteLeftRookMoved) return false;
            if (Board[calculateIndex(1,1)] != '.') return false;
            if (Board[calculateIndex(2,1)] != '.') return false;
            if (Board[calculateIndex(3,1)] != '.') return false;
            if (Board[calculateIndex(0,1)] != 'R') return false;
            return true;
        }
        return false;
    }

    if (k == 'k'){
        if (isSquareAttacked(true, 4, 8)) return false;
        if (blackKingMoved) return false;
        if (inCol != 4 || inRow != 8) return false;

        if (fiCol == 6){
            if (blackRightRookMoved) return false;
            if (Board[calculateIndex(5,8)] != '.') return false;
            if (Board[calculateIndex(6,8)] != '.') return false;
            if (Board[calculateIndex(7,8)] != 'r') return false;
            return true;
        }

        if (fiCol == 2){
            if (blackLeftRookMoved) return false;
            if (Board[calculateIndex(1,8)] != '.') return false;
            if (Board[calculateIndex(2,8)] != '.') return false;
            if (Board[calculateIndex(3,8)] != '.') return false;
            if (Board[calculateIndex(0,8)] != 'r') return false;
            return true;
        }
        return false;
    }
    return false;
}

inline bool canEnPassant(int inCol, int inRow, int fiCol, int fiRow){
    if(moveHistory.empty()) return false;

    char piece = Board[calculateIndex(inCol, inRow)];

    if(piece=='p'){
        if(inRow!=4 || fiRow!=3 || abs(fiCol-inCol)!=1) return false;
        if(Board[calculateIndex(fiCol, fiRow)]!='.') return false;
        if(Board[calculateIndex(fiCol, inRow)]!='P') return false;

        Move last = moveHistory.back();
        return (last.from==calculateIndex(fiCol, 2) && last.to==calculateIndex(fiCol, 4));
    }

    if(piece=='P'){
        if(inRow!=5 || fiRow!=6 || abs(fiCol-inCol)!=1) return false;
        if(Board[calculateIndex(fiCol, fiRow)]!='.') return false;
        if(Board[calculateIndex(fiCol, inRow)]!='p') return false;

        Move last = moveHistory.back();
        return (last.from==calculateIndex(fiCol, 7) && last.to==calculateIndex(fiCol, 5));
    }

    return false;
}

inline bool enPassant(int inCol, int inRow, int fiCol, int fiRow){
    if(!canEnPassant(inCol,inRow,fiCol,fiRow)) return false;

    char piece = Board[calculateIndex(inCol, inRow)];

    Board[calculateIndex(fiCol, inRow)]='.';
    Board[calculateIndex(inCol, inRow)]='.';
    Board[calculateIndex(fiCol, fiRow)]=piece;
    return true;
}

inline bool isMoveValid(int inCol, int inRow, int fiCol, int fiRow){
    int i = calculateIndex(inCol, inRow);
    char p = Board[i];

    if (capturingOwnPiece(inCol,inRow,fiCol,fiRow)) return false;

    if (p == 'K' || p == 'k'){
        if (isCastlingMove(inCol,inRow,fiCol,fiRow)) return true;
        if (abs(fiCol - inCol) <= 1 && abs(fiRow - inRow) <= 1){
            if(p=='K'){
                if(!isSquareAttacked(false, fiCol, fiRow)) return true;
            }
            if(p=='k'){
                if(!isSquareAttacked(true, fiCol, fiRow)) return true;
            }
        }
        return false;
    }

    if (p == 'R' || p == 'r'){
        if(inCol!=fiCol&& inRow!=fiRow) return false;
        if(inCol<fiCol && inRow==fiRow){
            for (int j=inCol+1;j<fiCol;j++){
                if(Board[calculateIndex(j, inRow)]!='.') return false;
            }
        }
        if(inCol>fiCol && inRow==fiRow){
            for (int j=fiCol+1;j<inCol;j++){
                if(Board[calculateIndex(j, inRow)]!='.') return false;
            }
        }
        if(inRow<fiRow && inCol==fiCol){
            for (int j=inRow+1;j<fiRow;j++){
                if(Board[calculateIndex(inCol, j)]!='.') return false;
            }
        }
        if(inRow>fiRow && inCol==fiCol){
            for (int j=fiRow+1;j<inRow;j++){
                if(Board[calculateIndex(inCol, j)]!='.') return false;
            }
        }
        return true;
    }

    if (p == 'B' || p == 'b'){
        if (abs(fiCol - inCol) != abs(fiRow - inRow)) return false;
        int colDiff=inCol-fiCol;
        int rowDiff=inRow-fiRow;
        if (colDiff<0&&rowDiff<0){
            int j=inRow+1;
            for(int c=inCol+1;c<fiCol;c++){
                if(Board[calculateIndex(c, j)]!='.') return false;
                j++;
            }
        }

        if (colDiff<0&&rowDiff>0){
            int j=inRow-1;
            for(int c=inCol+1;c<fiCol;c++){
                if(Board[calculateIndex(c, j)]!='.') return false;
                j--;
            }
        }

        if (colDiff>0&&rowDiff<0){
            int j=inRow+1;
            for(int c=inCol-1;c>fiCol;c--){
                if(Board[calculateIndex(c, j)]!='.') return false;
                j++;
            }
        }

        if (colDiff>0&&rowDiff>0){
            int j=inRow-1;
            for(int c=inCol-1;c>fiCol;c--){
                if(Board[calculateIndex(c, j)]!='.') return false;
                j--;
            }
        }

        return true;
    }

    if (p == 'Q' || p == 'q') {
        int dc = fiCol - inCol;
        int dr = fiRow - inRow;

        bool straight = (dc == 0 || dr == 0);
        bool diagonal = (abs(dc) == abs(dr));
        if (!straight && !diagonal) return false;

        int stepC = (dc == 0) ? 0 : (dc > 0 ? 1 : -1);
        int stepR = (dr == 0) ? 0 : (dr > 0 ? 1 : -1);

        int c = inCol + stepC;
        int r = inRow + stepR;

        while (c != fiCol || r != fiRow) {
            if (Board[calculateIndex(c, r)] != '.') return false;
            c += stepC;
            r += stepR;
        }
        return true;
    }

    if (p == 'N' || p == 'n'){
        int dx = abs(fiCol - inCol);
        int dy = abs(fiRow - inRow);
        return ((dx == 2 && dy == 1) || (dx == 1 && dy == 2));
    }

    if (p == 'P'){
        if (abs(fiCol - inCol) == 1 && fiRow - inRow == 1 &&
            (isCapturing(inCol,inRow,fiCol,fiRow) || canEnPassant(inCol,inRow,fiCol,fiRow))) return true;

        if (inCol == fiCol && fiRow - inRow == 1 && Board[calculateIndex(fiCol, fiRow)] == '.') return true;
        if (inCol == fiCol && inRow == 2 && fiRow == 4 && Board[calculateIndex(fiCol, fiRow)] == '.' && Board[calculateIndex(inCol, 3)]=='.') return true;
        return false;
    }

    if (p == 'p'){
        if (abs(fiCol - inCol) == 1 && fiRow - inRow == -1 &&
            (isCapturing(inCol,inRow,fiCol,fiRow) || canEnPassant(inCol,inRow,fiCol,fiRow))) return true;

        if (inCol == fiCol && fiRow - inRow == -1 && Board[calculateIndex(fiCol, fiRow)] == '.') return true;
        if (inCol == fiCol && inRow == 7 && fiRow == 5 && Board[calculateIndex(fiCol, fiRow)] == '.'&& Board[calculateIndex(inCol, 6)]=='.') return true;
        return false;
    }

    return false;
}

struct TempState{
    int from;
    int to;
    int epIndex;
    char fromPiece;
    char toPiece;
    char epPiece;
    bool didEP;

    int rookFrom;
    int rookTo;
    char rookToPiece;
    bool didCastle;
};

inline TempState makeMoveTemp(int inCol, int inRow, int fiCol, int fiRow){
    TempState s;
    s.from = calculateIndex(inCol,inRow);
    s.to   = calculateIndex(fiCol,fiRow);

    s.fromPiece = Board[s.from];
    s.toPiece   = Board[s.to];

    s.didEP = false;
    s.epIndex = -1;
    s.epPiece = '\0';

    s.didCastle = false;
    s.rookFrom = -1;
    s.rookTo = -1;
    s.rookToPiece = '\0';

    bool castling = isCastlingMove(inCol,inRow,fiCol,fiRow);
    bool willEP   = canEnPassant(inCol,inRow,fiCol,fiRow);

    if (willEP){
        s.didEP = true;
        s.epIndex = calculateIndex(fiCol, inRow);
        s.epPiece = Board[s.epIndex];
        enPassant(inCol,inRow,fiCol,fiRow);
    } else {
        makeMove(inCol,inRow,fiCol,fiRow);
    }

    if (castling){
        s.didCastle = true;
        if (fiCol == 6){
            s.rookFrom = calculateIndex(7, fiRow);
            s.rookTo   = calculateIndex(5, fiRow);
        }
        if (fiCol == 2){
            s.rookFrom = calculateIndex(0, fiRow);
            s.rookTo   = calculateIndex(3, fiRow);
        }
        s.rookToPiece = Board[s.rookTo];
        makeMove((s.rookFrom%8), (8-(s.rookFrom/8)), (s.rookTo%8), (8-(s.rookTo/8)));
    }

    return s;
}

inline void undoMoveTemp(TempState s){
    if (s.didCastle){
        Board[s.rookFrom] = Board[s.rookTo];
        Board[s.rookTo] = s.rookToPiece;
    }

    Board[s.from] = s.fromPiece;
    Board[s.to]   = s.toPiece;

    if (s.didEP){
        Board[s.epIndex] = s.epPiece;
    }
}

inline bool isWhiteCheckmated(){
    if(!isWhiteInCheck()) return false;

    int inCol=0, inRow=0, fiCol=0, fiRow=0;

    for(int i=0;i<64;i++){
        if(Board[i]>='A'&&Board[i]<='Z'){
            indexToColRow(i, inCol, inRow);

            for(int j=0;j<64;j++){
                indexToColRow(j, fiCol, fiRow);

                if (isMoveValid(inCol, inRow, fiCol, fiRow)){
                    TempState s = makeMoveTemp(inCol, inRow, fiCol, fiRow);

                    if(!isWhiteInCheck()){
                        undoMoveTemp(s);
                        return false;
                    }

                    undoMoveTemp(s);
                }
            }
        }
    }
    return true;
}

inline bool isBlackCheckmated(){
    if(!isBlackInCheck()) return false;

    int inCol=0, inRow=0, fiCol=0, fiRow=0;

    for(int i=0;i<64;i++){
        if(Board[i]>='a'&&Board[i]<='z'){
            indexToColRow(i, inCol, inRow);

            for(int j=0;j<64;j++){
                indexToColRow(j, fiCol, fiRow);

                if (isMoveValid(inCol, inRow, fiCol, fiRow)){
                    TempState s = makeMoveTemp(inCol, inRow, fiCol, fiRow);

                    if(!isBlackInCheck()){
                        undoMoveTemp(s);
                        return false;
                    }

                    undoMoveTemp(s);
                }
            }
        }
    }
    return true;
}

inline bool isWhiteStalemated(){
    if(isWhiteInCheck()) return false;

    int inCol=0, inRow=0, fiCol=0, fiRow=0;

    for(int i=0;i<64;i++){
        if(Board[i]>='A'&&Board[i]<='Z'){
            indexToColRow(i, inCol, inRow);

            for(int j=0;j<64;j++){
                indexToColRow(j, fiCol, fiRow);

                if (isMoveValid(inCol, inRow, fiCol, fiRow)){
                    TempState s = makeMoveTemp(inCol, inRow, fiCol, fiRow);

                    if(!isWhiteInCheck()){
                        undoMoveTemp(s);
                        return false;
                    }

                    undoMoveTemp(s);
                }
            }
        }
    }
    return true;
}

inline bool isBlackStalemated(){
    if(isBlackInCheck()) return false;

    int inCol=0, inRow=0, fiCol=0, fiRow=0;

    for(int i=0;i<64;i++){
        if(Board[i]>='a'&&Board[i]<='z'){
            indexToColRow(i, inCol, inRow);

            for(int j=0;j<64;j++){
                indexToColRow(j, fiCol, fiRow);

                if (isMoveValid(inCol, inRow, fiCol, fiRow)){
                    TempState s = makeMoveTemp(inCol, inRow, fiCol, fiRow);

                    if(!isBlackInCheck()){
                        undoMoveTemp(s);
                        return false;
                    }

                    undoMoveTemp(s);
                }
            }
        }
    }
    return true;
}

inline bool finaliseMove(int inCol, int inRow, int fiCol, int fiRow, char promo = '\0'){
    int from = calculateIndex(inCol, inRow);
    int to   = calculateIndex(fiCol, fiRow);
    char p   = Board[from];

    if (isWhiteTurn && (p < 'A' || p > 'Z')) return false;
    if (!isWhiteTurn && (p < 'a' || p > 'z')) return false;

    if (capturingOwnPiece(inCol, inRow, fiCol, fiRow)) return false;
    if (!isMoveValid(inCol, inRow, fiCol, fiRow)) return false;

    bool wasCapture = isCapturing(inCol, inRow, fiCol, fiRow);
    char capturedPiece = Board[to];

    TempState s = makeMoveTemp(inCol, inRow, fiCol, fiRow);

    bool illegal = false;
    if (isWhiteTurn) {
        if (isWhiteInCheck()) illegal = true;
    } else {
        if (isBlackInCheck()) illegal = true;
    }

    undoMoveTemp(s);
    if (illegal) return false;

    FullState snap;
    snap.board = Board;
    snap.isWhiteTurn = isWhiteTurn;

    snap.whiteKingMoved = whiteKingMoved;
    snap.whiteLeftRookMoved = whiteLeftRookMoved;
    snap.whiteRightRookMoved = whiteRightRookMoved;

    snap.blackKingMoved = blackKingMoved;
    snap.blackLeftRookMoved = blackLeftRookMoved;
    snap.blackRightRookMoved = blackRightRookMoved;

    snap.moveHistorySize = moveHistory.size();
    stateHistory.push_back(snap);

    const bool castling = isCastlingMove(inCol, inRow, fiCol, fiRow);
    const bool didEP    = enPassant(inCol, inRow, fiCol, fiRow);

    if (capturedPiece == 'R') {
        if (to == calculateIndex(0,1)) whiteLeftRookMoved  = true;
        if (to == calculateIndex(7,1)) whiteRightRookMoved = true;
    }
    if (capturedPiece == 'r') {
        if (to == calculateIndex(0,8)) blackLeftRookMoved  = true;
        if (to == calculateIndex(7,8)) blackRightRookMoved = true;
    }

    if (!didEP) {
        makeMove(inCol, inRow, fiCol, fiRow);
    }

    if (castling) {
        if (fiCol == 6) makeMove(7, fiRow, 5, fiRow);
        if (fiCol == 2) makeMove(0, fiRow, 3, fiRow);
    }

    char promotionPiece = '\0';
    if (p == 'P' && fiRow == 8) {
        char chosen = (promo ? promo : 'q');
        if (chosen == 'q') promotionPiece = 'Q';
        if (chosen == 'r') promotionPiece = 'R';
        if (chosen == 'b') promotionPiece = 'B';
        if (chosen == 'n') promotionPiece = 'N';
        if (!promotionPiece) promotionPiece = 'Q';
        Board[to] = promotionPiece;
    }

    if (p == 'p' && fiRow == 1) {
        char chosen = (promo ? promo : 'q');
        if (chosen == 'q') promotionPiece = 'q';
        if (chosen == 'r') promotionPiece = 'r';
        if (chosen == 'b') promotionPiece = 'b';
        if (chosen == 'n') promotionPiece = 'n';
        if (!promotionPiece) promotionPiece = 'q';
        Board[to] = promotionPiece;
    }

    if (p == 'K') whiteKingMoved = true;
    if (p == 'R' && inRow == 1 && inCol == 0) whiteLeftRookMoved  = true;
    if (p == 'R' && inRow == 1 && inCol == 7) whiteRightRookMoved = true;

    if (p == 'k') blackKingMoved = true;
    if (p == 'r' && inRow == 8 && inCol == 0) blackLeftRookMoved  = true;
    if (p == 'r' && inRow == 8 && inCol == 7) blackRightRookMoved = true;

    Move m;
    m.from = from;
    m.to   = to;
    m.promotion = promotionPiece;
    m.isCapture = wasCapture || didEP;

    if (isWhiteTurn) {
        m.isCheck = isBlackInCheck();
        m.isMate  = isBlackCheckmated();
    } else {
        m.isCheck = isWhiteInCheck();
        m.isMate  = isWhiteCheckmated();
    }

    moveHistory.push_back(m);

    isWhiteTurn = !isWhiteTurn;
    return true;
}

#endif
