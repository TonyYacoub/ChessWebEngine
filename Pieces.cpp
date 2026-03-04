// Pieces.cpp
#include "Pieces.h"

bool isWhiteTurn = true;

bool whiteKingMoved = false;
bool whiteLeftRookMoved = false;
bool whiteRightRookMoved = false;

bool blackKingMoved = false;
bool blackLeftRookMoved = false;
bool blackRightRookMoved = false;

std::vector<Move> moveHistory;
std::vector<FullState> stateHistory;
