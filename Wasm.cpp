// Wasm.cpp
#include <string>
#include <vector>
#include <array>
#include <cctype>

#include "Engine.h"
#include "Pieces.h"
#include "Bot.h"

#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#else
  #define EMSCRIPTEN_KEEPALIVE
#endif

static std::string g_board;
static std::string g_moves;
static std::string g_history;
static std::string g_best;

static inline std::string indexToSquare(int idx) {
  int col = idx % 8;
  int row = 8 - (idx / 8);
  char file = char('a' + col);
  char rank = char('0' + row);
  return std::string() + file + rank;
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
void new_game() {
  newGame();
}

EMSCRIPTEN_KEEPALIVE
int try_move(const char* uci) {
  if (!uci) return 0;
  const bool ok = tryMoveUCI(std::string(uci));
  return ok ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
const char* board64_cstr() {
  g_board = board64();
  return g_board.c_str();
}

EMSCRIPTEN_KEEPALIVE
int side_to_move() {
  return (sideToMove() == 'w') ? 0 : 1;
}

EMSCRIPTEN_KEEPALIVE
int in_check(int side) {
  const char s = (side == 0) ? 'w' : 'b';
  return isInCheck(s) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int is_checkmate() {
  return isCheckmate() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int is_stalemate() {
  return isStalemate() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int is_threefold_repetition() {
  return isThreefoldRepetition() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int is_insufficient_material() {
  return isInsufficientMaterial() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int get_eval_score() {
  return evaluate();
}

EMSCRIPTEN_KEEPALIVE
const char* legal_moves_from_cstr(const char* square) {
  g_moves.clear();
  if (!square) return g_moves.c_str();

  std::vector<std::string> moves = legalMovesFrom(std::string(square));
  for (size_t i = 0; i < moves.size(); i++) {
    if (i) g_moves.push_back(' ');
    g_moves += moves[i];
  }
  return g_moves.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* legal_moves_all_cstr() {
  g_moves.clear();

  std::vector<std::string> moves = legalMovesAllUCI();
  for (size_t i = 0; i < moves.size(); i++) {
    if (i) g_moves.push_back(' ');
    g_moves += moves[i];
  }

  return g_moves.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* move_history_cstr() {
  g_history.clear();

  const std::vector<Move>& hist = getMoveHistory();
  for (size_t i = 0; i < hist.size(); i++) {
    if (i) g_history.push_back(' ');

    const Move& m = hist[i];

    std::string from = indexToSquare(m.from);
    std::string to   = indexToSquare(m.to);

    g_history += from;
    g_history += to;

    if (m.promotion) {
      g_history.push_back(char(std::tolower((unsigned char)m.promotion)));
    }
  }

  return g_history.c_str();
}

EMSCRIPTEN_KEEPALIVE
int undo_move() {
  return undoLastMove() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
const char* best_move_cstr(int depth) {
  g_best = bestMoveUCI(depth);

  if (g_best.empty()) {
    std::vector<std::string> moves = legalMovesAllUCI();
    if (!moves.empty()) {
      g_best = moves[0];
    }
  }

  return g_best.c_str();
}

} 
