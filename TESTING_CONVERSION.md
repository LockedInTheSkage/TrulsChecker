# FEN to Binary Conversion Testing

This directory contains tools to verify that the C implementation of `board_to_binary()` matches the Python implementation used for training the neural network model.

## Files

- **converters.py**: Python implementation of FEN to binary conversion (provided with the model)
- **demo_converter.py**: Demonstration script showing how the Python converter works
- **test_conversion.py**: Automated test comparing C and Python conversions
- **test_board_to_binary.c**: Simple C program that converts a FEN to binary format

## Usage

### 1. Demonstrate the Python Converter

```bash
python3 demo_converter.py
```

This will show:
- Numerical representation (pieces as numbers)
- Binary plane representation (12 planes for piece types)
- Combined flattened binary (768 bits)
- Batch conversion examples
- Typical usage patterns

### 2. Test C vs Python Conversion

```bash
python3 test_conversion.py
```

This compiles `test_board_to_binary.c` and runs it on several test positions, comparing the output with the Python converter. All tests should pass with "✅ All conversions match!".

### 3. Manual Testing

```bash
# Compile the test program
gcc -o test_board_to_binary test_board_to_binary.c \
    src/templechess/templechess/src/ChessBoard.c \
    src/templechess/templechess/src/BitBoard.c \
    src/templechess/templechess/src/LookupTable.c \
    src/templechess/templechess/src/MoveSet.c \
    -Isrc/templechess/templechess/src -I. -lm

# Test a position
./test_board_to_binary "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
```

## Binary Representation Format

The board is converted to a 768-dimensional binary vector:

- **12 piece planes** × 64 squares = 768 bits
- Plane order: P, N, B, R, Q, K (white), p, n, b, r, q, k (black)
- Each plane is 8×8 = 64 squares
- Square ordering: a8=0, b8=1, ..., h8=7, a7=8, ..., h1=63

### Piece Type Mapping

| templechess Type | Python Index (White) | Python Index (Black) |
|------------------|----------------------|----------------------|
| Pawn (0)         | 0                    | 6                    |
| Knight (2)       | 1                    | 7                    |
| Bishop (3)       | 2                    | 8                    |
| Rook (4)         | 3                    | 9                    |
| Queen (5)        | 4                    | 10                   |
| King (1)         | 5                    | 11                   |

**Important**: The templechess enum order (Pawn, King, Knight, Bishop, Rook, Queen) differs from the Python model's expected order (Pawn, Knight, Bishop, Rook, Queen, King). The `PIECE_INDEX` mapping in `src/NeuralHeuristic.c` handles this conversion.

## Test Positions

The test suite includes:
1. Starting position
2. After 1.e4 (with en passant square)
3. Sicilian Defense opening
4. Middle game position
5. Position without castling rights
6. Endgame position with high halfmove clock

## Troubleshooting

If tests fail:

1. **Check piece ordering**: The most common issue is piece plane ordering. The Python model expects P, N, B, R, Q, K, not P, K, N, B, R, Q.

2. **Check square indexing**: Squares should be numbered 0-63 with a8=0, h1=63.

3. **Verify includes**: Make sure all templechess headers are properly included:
   ```c
   #include "BitBoard.h"
   #include "LookupTable.h"  // Defines Type and Color enums
   #include "ChessBoard.h"
   ```

4. **Check compilation**: Compile with all required source files and include paths.

## Implementation Notes

The C implementation in `src/NeuralHeuristic.c` uses the `board_to_binary()` function which:

1. Initializes a 768-element float array to zeros
2. Iterates through all 64 squares
3. For each piece, determines its type and color
4. Sets the appropriate bit in the correct plane: `output[piece_idx * 64 + square] = 1.0f`

This matches the Python implementation in `converters.py` which creates 12 planes using numpy arrays and the python-chess library.

## Dependencies

Python dependencies (installable via `pip3 install python-chess numpy`):
- python-chess
- numpy

C dependencies:
- templechess v2 library (included in `src/templechess/templechess/`)
- Standard C libraries (stdio, string, math)

## Success Criteria

✅ All 6 test positions convert identically in C and Python
✅ 768 bits output (12 planes × 64 squares)
✅ Correct piece type mapping
✅ Correct square indexing

When all tests pass, the neural network model will receive the exact same input format it was trained on.
