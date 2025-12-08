#include "NeuralHeuristic.h"
#include "Heuristic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <onnxruntime_c_api.h>

// ONNX Runtime globals
static const OrtApi* ort_api = NULL;
static OrtEnv* ort_env = NULL;
static OrtSession* ort_session = NULL;
static OrtMemoryInfo* ort_memory_info = NULL;
static int neural_available = 0;

// Piece type to index mapping for binary representation
// Order must match Python converter: P, N, B, R, Q, K for white (0-5), black (6-11)
// templechess Type enum: Pawn=0, King=1, Knight=2, Bishop=3, Rook=4, Queen=5
static const int PIECE_INDEX[7][2] = {
    {0, 6},   // Pawn: White=0, Black=6
    {5, 11},  // King: White=5, Black=11
    {1, 7},   // Knight: White=1, Black=7
    {2, 8},   // Bishop: White=2, Black=8
    {3, 9},   // Rook: White=3, Black=9
    {4, 10},  // Queen: White=4, Black=10
    {-1, -1}  // Empty
};

/**
 * Convert board to 776-dimensional binary representation
 * Format: 12 piece types * 64 squares + 8 metadata values
 * Metadata: [0]=side_to_move, [1-4]=castling, [5-6]=en_passant, [7]=halfmove
 */
static void board_to_binary(ChessBoard *board, float *output) {
    // Initialize to zeros
    memset(output, 0, 776 * sizeof(float));
    
    // Fill piece planes (first 768 values)
    for (int square = 0; square < BOARD_SIZE; square++) {
        Type type = board->squares[square];
        
        if (type == Empty) continue;
        
        // Determine color
        Color color = (board->colors[White] & ((BitBoard)1 << square)) ? White : Black;
        
        // Get piece index (0-11)
        int piece_idx = PIECE_INDEX[type][color];
        
        if (piece_idx >= 0) {
            // Set bit: piece_idx * 64 + square
            output[piece_idx * 64 + square] = 1.0f;
        }
    }
    
    // Add metadata (indices 768-775)
    // [0]: Side to move (1=white, 0=black)
    output[768] = (board->turn == White) ? 1.0f : 0.0f;
    
    // [1-4]: Castling rights
    output[769] = (board->castling & 0x01) ? 1.0f : 0.0f; // White kingside
    output[770] = (board->castling & 0x02) ? 1.0f : 0.0f; // White queenside
    output[771] = (board->castling & 0x04) ? 1.0f : 0.0f; // Black kingside
    output[772] = (board->castling & 0x08) ? 1.0f : 0.0f; // Black queenside
    
    // [5-6]: En passant square (file and rank, or -1 if none)
    if (board->enPassant != EMPTY_SQUARE) {
        output[773] = (float)(board->enPassant % 8);           // file (0-7)
        output[774] = (float)(7 - (board->enPassant / 8));     // rank (0-7, flipped)
    } else {
        output[773] = -1.0f;
        output[774] = -1.0f;
    }
    
    // [7]: Halfmove clock (normalized to 0-1)
    // Note: ChessBoard v2 doesn't store halfmove clock, so we use 0
    output[775] = 0.0f;
}

int neural_init(void) {
    if (neural_available) {
        return 0; // Already initialized
    }
    
    // Initialize ONNX Runtime API
    ort_api = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (!ort_api) {
        fprintf(stderr, "Failed to get ONNX Runtime API\n");
        return -1;
    }
    
    // Create ONNX Runtime environment
    OrtStatus* status = ort_api->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "TrulsChecker", &ort_env);
    if (status != NULL) {
        const char* msg = ort_api->GetErrorMessage(status);
        fprintf(stderr, "Failed to create ONNX Runtime environment: %s\n", msg);
        ort_api->ReleaseStatus(status);
        return -1;
    }
    
    // Create session options
    OrtSessionOptions* session_options;
    status = ort_api->CreateSessionOptions(&session_options);
    if (status != NULL) {
        const char* msg = ort_api->GetErrorMessage(status);
        fprintf(stderr, "Failed to create session options: %s\n", msg);
        ort_api->ReleaseStatus(status);
        return -1;
    }
    
    // Set number of threads (optional, for performance)
    status = ort_api->SetIntraOpNumThreads(session_options, 1);
    if (status != NULL) {
        ort_api->ReleaseStatus(status);
    }
    
    status = ort_api->SetSessionGraphOptimizationLevel(session_options, ORT_ENABLE_BASIC);
    if (status != NULL) {
        ort_api->ReleaseStatus(status);
    }
    
    // Create session from ONNX model file
    const char* model_path = "models/chess_model.onnx";
    status = ort_api->CreateSession(ort_env, model_path, session_options, &ort_session);
    ort_api->ReleaseSessionOptions(session_options);
    
    if (status != NULL) {
        const char* msg = ort_api->GetErrorMessage(status);
        fprintf(stderr, "Failed to load ONNX model '%s': %s\n", model_path, msg);
        ort_api->ReleaseStatus(status);
        return -1;
    }
    
    // Create memory info for CPU
    status = ort_api->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &ort_memory_info);
    if (status != NULL) {
        const char* msg = ort_api->GetErrorMessage(status);
        fprintf(stderr, "Failed to create memory info: %s\n", msg);
        ort_api->ReleaseStatus(status);
        return -1;
    }
    
    neural_available = 1;
    fprintf(stderr, "✅ Neural network loaded successfully from ONNX model\n");
    return 0;
}

void neural_shutdown(void) {
    if (ort_memory_info) {
        ort_api->ReleaseMemoryInfo(ort_memory_info);
        ort_memory_info = NULL;
    }
    if (ort_session) {
        ort_api->ReleaseSession(ort_session);
        ort_session = NULL;
    }
    if (ort_env) {
        ort_api->ReleaseEnv(ort_env);
        ort_env = NULL;
    }
    neural_available = 0;
}

int neural_is_available(void) {
    return neural_available;
}

int neural_evaluate(LookupTable l, ChessBoard *board) {
    // Fall back to material evaluation if neural network not available
    if (!neural_available) {
        return evaluate(l, board);
    }
    
    // Convert board to binary representation (776 values)
    float binary_board[776];
    board_to_binary(board, binary_board);
    
    // Prepare input tensor
    const int64_t input_shape[] = {1, 776};  // Batch size 1, 776 features
    const char* input_names[] = {"input"};
    const char* output_names[] = {"output"};
    
    // Create input tensor
    OrtValue* input_tensor = NULL;
    OrtStatus* status = ort_api->CreateTensorWithDataAsOrtValue(
        ort_memory_info,
        binary_board,
        776 * sizeof(float),
        input_shape,
        2,
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
        &input_tensor
    );
    
    if (status != NULL) {
        const char* msg = ort_api->GetErrorMessage(status);
        fprintf(stderr, "Failed to create input tensor: %s\n", msg);
        ort_api->ReleaseStatus(status);
        return evaluate(l, board);
    }
    
    // Run inference
    OrtValue* output_tensor = NULL;
    status = ort_api->Run(
        ort_session,
        NULL,  // Run options
        input_names,
        (const OrtValue* const*)&input_tensor,
        1,  // Number of inputs
        output_names,
        1,  // Number of outputs
        &output_tensor
    );
    
    ort_api->ReleaseValue(input_tensor);
    
    if (status != NULL) {
        const char* msg = ort_api->GetErrorMessage(status);
        fprintf(stderr, "Failed to run inference: %s\n", msg);
        ort_api->ReleaseStatus(status);
        return evaluate(l, board);
    }
    
    // Get output data
    float* output_data;
    status = ort_api->GetTensorMutableData(output_tensor, (void**)&output_data);
    
    if (status != NULL) {
        const char* msg = ort_api->GetErrorMessage(status);
        fprintf(stderr, "Failed to get output data: %s\n", msg);
        ort_api->ReleaseStatus(status);
        ort_api->ReleaseValue(output_tensor);
        return evaluate(l, board);
    }
    
    // Convert output to centipawns (multiply by 100)
    int score = (int)(output_data[0] * 100.0f);
    
    ort_api->ReleaseValue(output_tensor);
    
    return score;
}
