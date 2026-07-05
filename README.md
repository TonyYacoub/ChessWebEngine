# ChessWebEngine

The C++ chess engine behind **[ChessWeb](https://github.com/TonyYacoub/ChessWeb)** —
play it live at [chessweb.tonywessly.workers.dev/play](https://chessweb.tonywessly.workers.dev/play).

Compiled to WebAssembly with Emscripten, so the full engine runs client-side in the
browser. The compiled artifacts (`engine.js` + `engine.wasm`) are consumed by the
ChessWeb React app from `public/wasm/`.

## Structure

| File | Role |
|---|---|
| `Board.cpp/.h` | Internal board representation |
| `Pieces.cpp/.h` | Piece definitions, move generation and validation |
| `Engine.cpp/.h` | Game state, legal-move filtering, FEN/UCI parsing |
| `Bot.cpp/.h` | Chess bot: minimax with alpha-beta pruning and piece-square-table evaluation, depth-based difficulty levels |
| `Move.h` | Move encoding |
| `Wasm.cpp` | Emscripten bindings exposing the engine API to JavaScript |

## Build (WebAssembly)

Requires the [Emscripten SDK](https://emscripten.org/):

```bash
emcc Board.cpp Pieces.cpp Engine.cpp Bot.cpp Wasm.cpp \
  -O2 -o engine.js \
  -s EXPORTED_RUNTIME_METHODS=ccall,cwrap -s MODULARIZE=1
```

Outputs `engine.js` + `engine.wasm` (the prebuilt pair is committed for convenience).
