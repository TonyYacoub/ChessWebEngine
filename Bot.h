// Bot.h

#pragma once
#include <string>
#include <vector>

std::vector<std::string> legalMovesAllUCI();
int evaluate();
std::string bestMoveUCI(int depth);
int alphabeta(int depth, int alpha, int beta);
