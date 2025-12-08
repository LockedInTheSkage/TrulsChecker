# Neural Network Integration - ONNX Approach

## Summary

The PyTorch model has been successfully exported to ONNX format for efficient C integration. This approach avoids Python subprocess complexity and venv issues.

## What Was Done

### 1. Model Export
- ✅ Created `export_to_onnx.py` script
- ✅ Exported `chess_pytorch_quick_1300000.pt` to `models/chess_model.onnx` (12KB)
- ✅ Verified ONNX model works with test inference

### 2. Model Format
- **Input**: 776 floats (768 piece planes + 8 metadata)
- **Output**: 1 float (evaluation in centipawns)
- **ONNX Opset**: 18 (compatible with ONNX Runtime 1.10+)

## Next Steps to Complete Integration

### Install ONNX Runtime C API

```bash
# Ubuntu/Debian
sudo apt-get install libonnxruntime-dev

# Or download from GitHub releases:
# https://github.com/microsoft/onnxruntime/releases
```

### Update C Code

Replace the Python subprocess approach in `src/NeuralHeuristic.c` with ONNX Runtime C API calls:

```c
#include <onnxruntime_c_api.h>

// Load model once at startup
OrtEnv* env;
OrtSession* session;

int neural_init(void) {
    const OrtApi* api = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    
    // Create environment
    api->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "chess", &env);
    
    // Create session
    OrtSessionOptions* session_options;
    api->CreateSessionOptions(&session_options);
    api->CreateSession(env, "models/chess_model.onnx", session_options, &session);
    
    return 0;
}

int neural_evaluate(LookupTable l, ChessBoard *board) {
    // Convert board to 776-float array (already implemented)
    float input[776];
    board_to_binary(board, input);
    
    // Run inference
    float output;
    // ... ONNX Runtime API calls ...
    
    return (int)output;
}
```

## Advantages Over Python Subprocess

1. **No Python dependency** - Pure C/C++ execution
2. **No venv issues** - No environment activation needed
3. **Better performance** - Direct function calls, no IPC overhead
4. **Simpler deployment** - Single binary + ONNX file
5. **Easier debugging** - No subprocess communication to debug

## Current Status

- ✅ PyTorch model trained and available
- ✅ Model exported to ONNX format
- ✅ ONNX model tested and working
- ✅ C code has board_to_binary() function (776-dimensional)
- ⏳ Need to install ONNX Runtime C library
- ⏳ Need to update `src/NeuralHeuristic.c` to use ONNX Runtime API
- ⏳ Need to update `Makefile` to link against ONNX Runtime

## Testing the Game Without Neural Network

The game currently works with material-based evaluation as fallback:

```bash
./game 4
```

This searches to depth 4 using traditional material counting. Once ONNX Runtime is integrated, it will automatically use the neural network evaluation instead.

## Files

- `export_to_onnx.py` - Script to convert PyTorch to ONNX
- `models/chess_model.onnx` - Exported neural network (12KB)
- `models/config.json` - Model configuration
- `src/NeuralHeuristic.c` - Neural network interface (currently uses subprocess, needs ONNX update)
- `src/neural_eval.py` - Python evaluator (can be removed after ONNX integration)

## References

- [ONNX Runtime C API Documentation](https://onnxruntime.ai/docs/api/c/)
- [ONNX Runtime GitHub](https://github.com/microsoft/onnxruntime)
- [Converting PyTorch to ONNX](https://pytorch.org/docs/stable/onnx.html)
