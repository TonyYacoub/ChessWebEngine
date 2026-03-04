// Bot.cpp
#include "Bot.h"
#include "Board.h"
#include "Pieces.h"
#include "Engine.h"
#include <climits>

using namespace std;

static const int PAWN_PST[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};
static const int KNIGHT_PST[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};
static const int BISHOP_PST[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
static const int ROOK_PST[64] = {
     0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};
static const int QUEEN_PST[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
static const int KING_PST[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};
static const int KING_ENDGAME_PST[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

inline int mirror(int i) {
    return (7 - (i / 8)) * 8 + (i % 8);
}


int distToEdge(int i) {
    int r = i / 8, f = i % 8;
    return std::min({r, 7-r, f, 7-f});
}

int manhattanDist(int a, int b) {
    return abs(a/8 - b/8) + abs(a%8 - b%8);
}

bool clearBetween(int a, int b) {
    int step = (b > a) ? ((b-a == b%8-a%8) ? 1 : 8)
                       : ((a-b == a%8-b%8) ? -1 : -8);
    for (int s = a+step; s != b; s += step)
        if (Board[s] != '.') return false;
    return true;
}

int squareColor(int i) { return (i/8 + i%8) % 2; }


int evaluate() {

    int phase = 0;
    for (int i = 0; i < 64; i++) {
        switch (Board[i]) {
            case 'Q': case 'q': phase += 4; break;
            case 'R': case 'r': phase += 2; break;
            case 'B': case 'b': phase += 1; break;
            case 'N': case 'n': phase += 1; break;
        }
    }
    phase = std::min(phase, 24);

    int whitePawnsOnFile[8] = {};
    int blackPawnsOnFile[8] = {};
    for (int i = 0; i < 64; i++) {
        if (Board[i] == 'P') whitePawnsOnFile[i % 8]++;
        if (Board[i] == 'p') blackPawnsOnFile[i % 8]++;
    }

    int whiteKingPos = -1, blackKingPos = -1;
    for (int i = 0; i < 64; i++) {
        if (Board[i] == 'K') whiteKingPos = i;
        if (Board[i] == 'k') blackKingPos = i;
    }

    bool whitePassedSquare[64] = {};
    bool blackPassedSquare[64] = {};
    for (int i = 0; i < 64; i++) {
        int file = i % 8, rank = i / 8;
        if (Board[i] == 'P') {
            bool passed = true;
            for (int r = rank+1; r < 8 && passed; r++) {
                if (file > 0 && Board[r*8+file-1] == 'p') passed = false;
                if (             Board[r*8+file  ] == 'p') passed = false;
                if (file < 7 && Board[r*8+file+1] == 'p') passed = false;
            }
            if (passed) whitePassedSquare[i] = true;
        }
        if (Board[i] == 'p') {
            bool passed = true;
            for (int r = rank-1; r >= 0 && passed; r--) {
                if (file > 0 && Board[r*8+file-1] == 'P') passed = false;
                if (             Board[r*8+file  ] == 'P') passed = false;
                if (file < 7 && Board[r*8+file+1] == 'P') passed = false;
            }
            if (passed) blackPassedSquare[i] = true;
        }
    }

    int score = 0;

    score += (isWhiteTurn ? 10 : -10);

    int whiteMob = 0, blackMob = 0;
    for (int i = 0; i < 64; i++) {
        char c = Board[i];
        if      (c == 'Q') whiteMob += 9;
        else if (c == 'R') whiteMob += 5;
        else if (c == 'B' || c == 'N') whiteMob += 3;
        else if (c == 'P') whiteMob += 1;
        else if (c == 'q') blackMob += 9;
        else if (c == 'r') blackMob += 5;
        else if (c == 'b' || c == 'n') blackMob += 3;
        else if (c == 'p') blackMob += 1;
    }
    score += (whiteMob - blackMob) * 2;

    int whiteBishops = 0, blackBishops = 0;

    for (int i = 0; i < 64; i++) {
        char c = Board[i];
        if (c == '.') continue;

        int file = i % 8;
        int rank = i / 8;

        switch (c) {

            case 'P': {
                score += 100 + PAWN_PST[i];
                if (whitePawnsOnFile[file] > 1) score -= 20;
                bool isolated = (file == 0 || whitePawnsOnFile[file-1] == 0)
                             && (file == 7 || whitePawnsOnFile[file+1] == 0);
                if (isolated) score -= 15;
                if (whitePassedSquare[i]) score += 20 + rank * 5;
                break;
            }

            case 'p': {
                score -= 100 + PAWN_PST[mirror(i)];
                if (blackPawnsOnFile[file] > 1) score += 20;
                bool isolated = (file == 0 || blackPawnsOnFile[file-1] == 0)
                             && (file == 7 || blackPawnsOnFile[file+1] == 0);
                if (isolated) score += 15;
                if (blackPassedSquare[i]) score -= 20 + (7-rank) * 5;
                break;
            }

            case 'R': {
                score += 500 + ROOK_PST[i];
                if (whitePawnsOnFile[file] == 0)
                    score += blackPawnsOnFile[file] == 0 ? 20 : 10;
                if (rank == 6) score += 30;
                for (int r = 0; r < rank; r++)
                    if (Board[r*8+file] == 'P' && whitePassedSquare[r*8+file])
                        score += 20;
                for (int j = i+1; j < 64; j++) {
                    if (Board[j] == 'R') {
                        bool sameRank = (rank == j/8);
                        bool sameFile = (file == j%8);
                        if ((sameRank || sameFile) && clearBetween(i, j))
                            score += 15;
                    }
                }
                break;
            }

            case 'r': {
                score -= 500 + ROOK_PST[mirror(i)];
                if (blackPawnsOnFile[file] == 0)
                    score -= whitePawnsOnFile[file] == 0 ? 20 : 10;
                if (rank == 1) score -= 30;
                for (int r = 7; r > rank; r--)
                    if (Board[r*8+file] == 'p' && blackPassedSquare[r*8+file])
                        score -= 20;
                for (int j = i+1; j < 64; j++) {
                    if (Board[j] == 'r') {
                        bool sameRank = (rank == j/8);
                        bool sameFile = (file == j%8);
                        if ((sameRank || sameFile) && clearBetween(i, j))
                            score -= 15;
                    }
                }
                break;
            }

            case 'B': {
                score += 300 + BISHOP_PST[i];
                whiteBishops++;
                int col = squareColor(i);
                int badPawns = 0;
                for (int j = 0; j < 64; j++)
                    if (Board[j] == 'P' && squareColor(j) == col) badPawns++;
                score -= badPawns * 4;
                break;
            }

            case 'b': {
                score -= 300 + BISHOP_PST[mirror(i)];
                blackBishops++;
                int col = squareColor(i);
                int badPawns = 0;
                for (int j = 0; j < 64; j++)
                    if (Board[j] == 'p' && squareColor(j) == col) badPawns++;
                score += badPawns * 4;
                break;
            }

            case 'N': {
                score += 300 + KNIGHT_PST[i];
                bool outpost = true;
                for (int r = rank; r < 8 && outpost; r++) {
                    if (file > 0 && Board[r*8+file-1] == 'p') outpost = false;
                    if (file < 7 && Board[r*8+file+1] == 'p') outpost = false;
                }
                bool supported = (rank >= 4)
                    && ((file > 0 && Board[(rank-1)*8+file-1] == 'P')
                     || (file < 7 && Board[(rank-1)*8+file+1] == 'P'));
                if (outpost && supported) score += 25;
                break;
            }

            case 'n': {
                score -= 300 + KNIGHT_PST[mirror(i)];
                bool outpost = true;
                for (int r = rank; r >= 0 && outpost; r--) {
                    if (file > 0 && Board[r*8+file-1] == 'P') outpost = false;
                    if (file < 7 && Board[r*8+file+1] == 'P') outpost = false;
                }
                bool supported = (rank <= 3)
                    && ((file > 0 && Board[(rank+1)*8+file-1] == 'p')
                     || (file < 7 && Board[(rank+1)*8+file+1] == 'p'));
                if (outpost && supported) score -= 25;
                break;
            }

            case 'Q': score += 900 + QUEEN_PST[i];         break;
            case 'q': score -= 900 + QUEEN_PST[mirror(i)]; break;

            case 'K': {
                int mg = KING_PST[i];
                int eg = KING_ENDGAME_PST[i];
                score += (mg * phase + eg * (24-phase)) / 24;
                if (phase > 10) {
                    int shields[3] = { i+7, i+8, i+9 };
                    for (int s : shields)
                        if (s < 64 && abs(s%8 - file) <= 1 && Board[s] != 'P')
                            score -= 20;
                }
                if (phase < 8 && score > 150 && blackKingPos >= 0) {
                    score += (4 - distToEdge(blackKingPos)) * 15;
                    score += (14 - manhattanDist(whiteKingPos, blackKingPos)) * 5;
                }
                break;
            }

            case 'k': {
                int mg = KING_PST[mirror(i)];
                int eg = KING_ENDGAME_PST[mirror(i)];
                score -= (mg * phase + eg * (24-phase)) / 24;
                if (phase > 10) {
                    int shields[3] = { i-9, i-8, i-7 };
                    for (int s : shields)
                        if (s >= 0 && abs(s%8 - file) <= 1 && Board[s] != 'p')
                            score += 20;
                }
                if (phase < 8 && score < -150 && whiteKingPos >= 0) {
                    score -= (4 - distToEdge(whiteKingPos)) * 15;
                    score -= (14 - manhattanDist(blackKingPos, whiteKingPos)) * 5;
                }
                break;
            }
        }
    }

    if (whiteBishops >= 2) score += 30;
    if (blackBishops >= 2) score -= 30;

    return score;
}


inline int pieceVal(char c) {
    switch (c) {
        case 'P': case 'p': return 1;
        case 'N': case 'n': return 3;
        case 'B': case 'b': return 3;
        case 'R': case 'r': return 5;
        case 'Q': case 'q': return 9;
        default:             return 0;
    }
}

inline int scoreMoveOrdering(const string& move) {
    int from = calculateIndex(move[0]-'a', move[1]-'0');
    int to   = calculateIndex(move[2]-'a', move[3]-'0');
    int s = 0;
    if (Board[to] != '.')
        s = 10 * pieceVal(Board[to]) - pieceVal(Board[from]);
    if (move.size() == 5 && move[4] == 'q') s += 90;
    return s;
}


vector<string> legalMovesAllUCI() {
    vector<string> all;
    for (int i = 0; i < 64; i++) {
        char p = Board[i];
        if (p == '.') continue;
        if (isWhiteTurn) { if (p < 'A' || p > 'Z') continue; }
        else             { if (p < 'a' || p > 'z') continue; }

        int col, row;
        indexToColRow(i, col, row);

        string fromSq;
        fromSq += char('a' + col);
        fromSq += char('0' + row);

        vector<string> moves = legalMovesFrom(fromSq);
        all.insert(all.end(), moves.begin(), moves.end());
    }
    return all;
}


int quiescence(int alpha, int beta) {
    int stand_pat = evaluate();
    if (!isWhiteTurn) stand_pat = -stand_pat;

    if (stand_pat >= beta) return beta;
    if (stand_pat > alpha) alpha = stand_pat;

    vector<string> moves = legalMovesAllUCI();

    // Order captures by MVV-LVA
    sort(moves.begin(), moves.end(), [](const string& a, const string& b){
        return scoreMoveOrdering(a) > scoreMoveOrdering(b);
    });

    for (const string& move : moves) {
        int to = calculateIndex(move[2]-'a', move[3]-'0');
        if (Board[to] == '.') continue; // skip non-captures

        if (!tryMoveUCI(move)) continue;
        int score = -quiescence(-beta, -alpha);
        undoLastMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}


int alphabeta(int depth, int alpha, int beta);

int staticLeaf() {
    int score = evaluate();
    if (!isWhiteTurn) score = -score;
    return score;
}

int alphabetaNoQS(int depth, int alpha, int beta) {
    if (depth == 0) return staticLeaf();

    vector<string> moves = legalMovesAllUCI();
    if (moves.empty()) {
        bool inCheck = isWhiteTurn ? isWhiteInCheck() : isBlackInCheck();
        return inCheck ? (INT_MIN + 1 + depth) : 0;
    }

    sort(moves.begin(), moves.end(), [](const string& a, const string& b){
        return scoreMoveOrdering(a) > scoreMoveOrdering(b);
    });

    for (const string& move : moves) {
        if (!tryMoveUCI(move)) continue;
        int score = -alphabetaNoQS(depth - 1, -beta, -alpha);
        undoLastMove();
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

static unsigned int g_rand_state = 12345;
static unsigned int lcg_rand() {
    g_rand_state = g_rand_state * 1664525u + 1013904223u;
    return g_rand_state;
}

string bestMoveUCI(int depth) {
    vector<string> moves = legalMovesAllUCI();
    if (moves.empty()) return "";

    if (depth == 1) {
        if ((lcg_rand() % 100) < 40) {
            return moves[lcg_rand() % moves.size()];
        }
        string bestMove = "";
        int bestScore = -9999999;
        for (const string& move : moves) {
            if (!tryMoveUCI(move)) continue;
            int score = -staticLeaf();
            undoLastMove();
            if (bestMove.empty() || score > bestScore) {
                bestScore = score;
                bestMove  = move;
            }
        }
        return bestMove;
    }

    if (depth == 2) {
        if ((lcg_rand() % 100) < 20) {
            return moves[lcg_rand() % moves.size()];
        }
        sort(moves.begin(), moves.end(), [](const string& a, const string& b){
            return scoreMoveOrdering(a) > scoreMoveOrdering(b);
        });
        string bestMove = "";
        int bestScore = -9999999;
        for (const string& move : moves) {
            if (!tryMoveUCI(move)) continue;
            int score = -alphabeta(depth - 1, -9999999, 9999999);
            undoLastMove();
            if (bestMove.empty() || score > bestScore) {
                bestScore = score;
                bestMove  = move;
            }
        }
        return bestMove;
    }

    sort(moves.begin(), moves.end(), [](const string& a, const string& b){
        return scoreMoveOrdering(a) > scoreMoveOrdering(b);
    });

    string bestMove = "";
    int bestScore = -9999999;

    for (const string& move : moves) {
        if (!tryMoveUCI(move)) continue;
        int score = -alphabeta(depth - 1, -9999999, 9999999);
        undoLastMove();
        if (bestMove.empty() || score > bestScore) {
            bestScore = score;
            bestMove  = move;
        }
    }
    return bestMove;
}

int alphabeta(int depth, int alpha, int beta) {
=    if (depth == 0) return quiescence(alpha, beta);

    vector<string> moves = legalMovesAllUCI();
    if (moves.empty()) {
        bool inCheck = isWhiteTurn ? isWhiteInCheck() : isBlackInCheck();
        return inCheck ? (INT_MIN + 1 + depth) : 0;
    }

    sort(moves.begin(), moves.end(), [](const string& a, const string& b){
        return scoreMoveOrdering(a) > scoreMoveOrdering(b);
    });

    for (const string& move : moves) {
        if (!tryMoveUCI(move)) continue;

        int score = -alphabeta(depth - 1, -beta, -alpha);
        undoLastMove();

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}
