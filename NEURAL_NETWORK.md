# Neural Network Integration Guide

This guide explains how to use the PyTorch neural network model for chess position evaluation in TrulsChecker.

## Architecture

The neural network integration consists of three layers:

1. **Python Script** (`src/neural_eval.py`)
   - Loads the PyTorch model
   - Converts 768-dimensional binary board representation to evaluation scores
   - Runs as a subprocess, communicating via pipes

2. **C Interface** (`src/NeuralHeuristic.c/h`)
   - Manages the Python subprocess
   - Converts ChessBoard to 768-dimensional binary format
   - Sends positions to Python and receives evaluations
   - Falls back to material evaluation if neural network unavailable

3. **Heuristic Layer** (`src/Heuristic.c/h`)
   - Provides `evaluate_position()` function
   - Automatically uses neural network if available
   - Falls back to material-based evaluation otherwise

## Board Representation

The neural network expects a **768-dimensional binary vector**:
- 12 piece types (6 white + 6 black) × 64 squares = 768 features
- Order: White Pawn, King, Knight, Bishop, Rook, Queen, then Black pieces
- Each piece type gets 64 bits (one per square)

Example for white pawn on e2 (square 52):
```
Position 52 in first 64 elements = 1, all others = 0
```

## Setup

### 1. Install Python Dependencies

```bash
pip3 install -r requirements_neural.txt
```

This installs:
- PyTorch >= 2.0.0
- NumPy >= 1.24.0

### 2. Verify Model Files

Ensure these files exist:
```
models/chess_pytorch_quick_1300000.pt          # Model weights
models/chess_pytorch_quick_1300000_config.json # Model configuration
```

### 3. Build with Neural Network Support

```bash
make clean
make game
```

The build now includes `NeuralHeuristic.c` which provides the Python interface.

## Usage

### Running the Game

```bash
./game [depth]
```

On startup, you'll see one of:
- `Neural network evaluation enabled` - Neural network is active
- `Using material-based evaluation` - Fallback mode (neural network unavailable)

### Testing Neural Network

Run the integration test:
```bash
./test_neural.sh
```

This checks:
1. PyTorch installation
2. Model file existence  
3. Neural evaluator script functionality
4. Game integration

### Direct Testing of Neural Evaluator

Test the Python script directly:
```bash
# Create a test position (starting position)
python3 -c "print(' '.join(['0'] * 768))" | python3 src/neural_eval.py
```

You should see:
```
Neural network model loaded on cpu
Neural evaluator ready
<evaluation score>
```

## How It Works

### 1. Initialization (game startup)

```c
neural_init();  // Forks Python subprocess
```

This:
- Creates pipes for communication
- Forks a child process
- Executes `python3 src/neural_eval.py`
- Waits for "ready" signal

### 2. Position Evaluation

```c
int score = evaluate_position(l, board);
```

This:
- Checks if neural network is available
- If yes: Converts board to 768-bit vector, sends to Python, receives score
- If no: Falls back to material evaluation

### 3. Cleanup (game exit)

```c
neural_shutdown();  // Closes pipes, terminates subprocess
```

## Performance Considerations

- **First evaluation**: ~100-200ms (model loading)
- **Subsequent evaluations**: ~5-20ms per position
- **CPU mode**: Good for testing
- **GPU mode**: Faster, but requires CUDA setup

To use GPU, edit `src/neural_eval.py`:
```python
device = 'cuda'  # Instead of 'cpu'
```

## Troubleshooting

### "Neural network not available"

**Cause**: Python subprocess failed to start

**Solutions**:
1. Check PyTorch installation: `python3 -c "import torch"`
2. Verify model path: `ls models/chess_pytorch_quick_1300000.pt`
3. Test script manually: `python3 src/neural_eval.py`
4. Check stderr output: `./game 2>&1 | grep -i error`

### "Neural evaluator communication failed"

**Cause**: Pipe communication error

**Solutions**:
1. Check if Python process is still running: `ps aux | grep neural_eval`
2. Look for Python errors: Check stderr during game startup
3. Verify input format (768 space-separated values)

### Slow evaluation

**Solutions**:
1. Use GPU if available (edit neural_eval.py)
2. Reduce search depth: `./game 3` instead of `./game 5`
3. Use iterative deepening for time management

### Python not found

**Cause**: `python3` not in PATH

**Solutions**:
1. Install Python 3: `sudo apt install python3`
2. Or modify `NeuralHeuristic.c` to use absolute path

## Model Retraining

To use a different model:

1. Train new model following same architecture
2. Save as `models/new_model.pt` and `models/new_model_config.json`
3. Update `neural_eval.py` to load new model name
4. Rebuild: `make clean && make game`

## Fallback Behavior

The system gracefully degrades:

1. If neural network initialization fails → Material evaluation
2. If evaluation fails during game → Material evaluation + warning
3. If Python crashes → Material evaluation for remainder of game

This ensures the game always works, even without neural network.

## Example Output

```bash
$ ./game 4
=== TrulsChecker Chess AI ===
Using templechess v2 library

Neural network evaluation enabled
Allocated memory for dictionary
Initialized hashtab
Loaded dictionary from file
Search depth: 4

r n b q k b n r 8
p p p p p p p p 7
...

White to move> ai
AI thinking...
AI plays: e2e4
...
```

## Integration with Minimax

The minimax algorithm automatically uses neural evaluation:

```c
// In Minimax.c
if (depth == 0) {
    return evaluate_position(l, board);  // Uses neural network if available
}
```

No changes needed to minimax logic - it's transparent!

## Files Modified

- `src/neural_eval.py` - NEW: Python neural network wrapper
- `src/NeuralHeuristic.c/h` - NEW: C interface to Python
- `src/Heuristic.c/h` - UPDATED: Added `evaluate_position()` function
- `src/Minimax.c` - UPDATED: Uses `evaluate_position()` instead of `evaluate()`
- `src/game.c` - UPDATED: Calls `neural_init()` and `neural_shutdown()`
- `Makefile` - UPDATED: Includes `NeuralHeuristic.c` in builds
- `requirements_neural.txt` - NEW: Python dependencies
- `test_neural.sh` - NEW: Integration test script

## Benefits

✅ **Better evaluation**: Neural network learns complex patterns
✅ **Fallback safe**: Always works, even without PyTorch
✅ **Transparent**: Minimax code unchanged
✅ **Fast**: Subprocess communication is efficient
✅ **Flexible**: Easy to swap models

Enjoy stronger chess play with neural network evaluation! 🧠♟️
