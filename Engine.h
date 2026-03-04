// Engine.h
#pragma once
#include <string>
#include <vector>
#include "Move.h"

void newGame();
bool tryMoveUCI(const std::string& uci);
std::string board64();
char sideToMove();
bool isInCheck(char side);
bool isCheckmate();
bool isStalemate();
bool isInsufficientMaterial();
bool isThreefoldRepetition();
const std::vector<Move>& getMoveHistory();
std::vector<std::string> legalMovesFrom(const std::string& square);
