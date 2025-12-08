# TrulsChecker - Chess AI

A chess AI using minimax algorithm with alpha-beta pruning, powered by the templechess v2 move generation library.

## Project Structure

```
TrulsChecker/
├── src/
│   ├── templechess/templechess/    # Templechess v2 library (do not modify)
│   │   └── src/
│   │       ├── BitBoard.c/h
│   │       ├── LookupTable.c/h
│   │       ├── ChessBoard.c/h
│   │       └── MoveSet.c/h
│   │
│   ├── Minimax.c/h                  # Minimax with alpha-beta pruning
│   ├── Heuristic.c/h                # Board evaluation function
│   ├── TemplechessAdapter.c/h       # Helper functions for templechess v2
│   │
│   ├── Zobrist.c/h                  # Zobrist hashing for positions
│   ├── Dictionary.c/h               # Transposition table
│   ├── OpeningBook.c/h              # Opening book generation
│   │
│   ├── game.c                       # Interactive chess game
│   ├── testMinimax.c                # Tests for minimax
│   ├── testHeuristic.c              # Tests for evaluation
│   ├── testDictionary.c             # Tests for transposition table
│   └── testZobrist.c                # Tests for zobrist hashing
│
├── Makefile
└── README.md
```

## Architecture

### Core Components

1. **Templechess v2 Library** (`src/templechess/templechess/src/`)
   - Self-contained chess move generation library
   - Provides: BitBoard operations, LookupTable for attacks, ChessBoard representation, MoveSet for legal moves
   - **Do not modify these files** - keep them separate for easy updates

2. **Minimax Engine** (`Minimax.c/h`)
   - Implements minimax algorithm with alpha-beta pruning
   - Uses templechess v2 for move generation
   - Integrates with transposition table for position caching

3. **Heuristic Evaluation** (`Heuristic.c/h`)
   - Board evaluation function
   - Currently: Simple material counting
   - TODO: Add positional evaluation, king safety, pawn structure, mobility

4. **Templechess Adapter** (`TemplechessAdapter.c/h`)
   - Helper functions to work with templechess v2
   - Move parsing and formatting
   - Game state detection (checkmate, stalemate, insufficient material)

5. **Position Management**
   - **Zobrist** (`Zobrist.c/h`): Hashing for board positions
   - **Dictionary** (`Dictionary.c/h`): Transposition table using zobrist hashes
   - **OpeningBook** (`OpeningBook.c/h`): Opening position generation

## Setup

Before building, ensure the data directory exists with the magic numbers file:

```bash
mkdir -p data
cp src/templechess/templechess/src/data/magicNumbers.out data/
```

## Building

```bash
# Build everything
make all

# Build specific targets
make game           # Interactive game
make testHeuristic  # Heuristic tests
make testMinimax    # Minimax tests

# Clean build artifacts
make clean
```

All executables compile successfully with only minor warnings about unused variables.

## Usage

### Play Against the AI

```bash
./game [depth]     # Optional: specify search depth (default: 5)
```

Commands:
- `move e2e4` - Make a move
- `ai` - Let AI move
- `board` - Display board
- `fen` - Show FEN string
- `quit` - Exit

### Run Tests

```bash
./testHeuristic    # Test evaluation function
./testMinimax      # Test search algorithm
./testDictionary   # Test transposition table
./testZobrist      # Test zobrist hashing

# Run all integration tests
bash test_integration.sh
```

All tests pass successfully!

## Updating Templechess

To update to a newer version of templechess:

1. Replace the contents of `src/templechess/templechess/` with the new version
2. Rebuild: `make clean && make all`
3. Run tests to ensure compatibility

The templechess library is kept in its own folder to make updates straightforward.

## Data Files

- `data/magicNumbers.out` - Magic bitboard numbers for move generation (from templechess)
- `src/data/zobrist.dat` - Zobrist hash table (auto-generated on first run)
- `src/data/heuristicDict.dat` - Transposition table (saved positions)

## Future Improvements

### Heuristic
- [ ] Piece-square tables
- [ ] King safety evaluation
- [ ] Pawn structure analysis
- [ ] Mobility scoring
- [ ] Center control
- [ ] Development evaluation

### Search
- [ ] Iterative deepening
- [ ] Quiescence search
- [ ] Move ordering improvements
- [ ] Null move pruning
- [ ] Late move reductions

### Features
- [ ] UCI protocol support
- [ ] Time management
- [ ] Opening book from PGN
- [ ] Endgame tablebases

## License

See LICENSE file for details.
