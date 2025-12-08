#!/usr/bin/env python3
"""
Test script to verify C board_to_binary() matches Python FENConverter.fen_to_combined_binary()
"""

import subprocess
import numpy as np
from converters import FENConverter

def get_c_conversion(fen: str) -> np.ndarray:
    """Get binary conversion from C program
    
    Compiles and runs a small C test program that uses board_to_binary()
    """
    # Compile test program if not already compiled
    import os
    if not os.path.exists('./test_board_to_binary'):
        compile_cmd = [
            'gcc',
            '-o', './test_board_to_binary',
            'test_board_to_binary.c',
            'src/templechess/templechess/src/ChessBoard.c',
            'src/templechess/templechess/src/BitBoard.c',
            'src/templechess/templechess/src/LookupTable.c',
            'src/templechess/templechess/src/MoveSet.c',
            '-Isrc/templechess/templechess/src',
            '-I.',
            '-lm'
        ]
        
        result = subprocess.run(compile_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print("Compilation failed:")
            print(result.stderr)
            return None
    
    # Run with FEN as argument
    result = subprocess.run(['./test_board_to_binary', fen], capture_output=True, text=True)
    if result.returncode != 0:
        print("Execution failed:")
        print(result.stderr)
        return None
    
    # Parse output
    values = [int(x) for x in result.stdout.strip().split(',')]
    return np.array(values, dtype=np.uint8)


def compare_conversions(fen: str, verbose: bool = True):
    """Compare C and Python conversions for a given FEN"""
    
    # Get Python conversion
    converter = FENConverter()
    piece_planes, metadata = converter.fen_to_binary(fen)
    
    # Flatten piece planes (12 planes * 64 squares = 768 values)
    py_binary = (piece_planes.flatten() > 0.5).astype(np.uint8)
    
    # Get C conversion
    c_binary = get_c_conversion(fen)
    
    if c_binary is None:
        print(f"❌ Failed to get C conversion for: {fen}")
        return False
    
    # Compare
    if np.array_equal(py_binary, c_binary):
        if verbose:
            print(f"✅ MATCH for: {fen}")
        return True
    else:
        print(f"❌ MISMATCH for: {fen}")
        
        # Find differences
        diff_indices = np.where(py_binary != c_binary)[0]
        print(f"   Found {len(diff_indices)} differences at indices: {diff_indices[:10]}...")
        
        # Show first few differences
        for i in diff_indices[:5]:
            plane = i // 64
            square = i % 64
            row = square // 8
            col = square % 8
            print(f"   Index {i} (plane {plane}, row {row}, col {col}): Python={py_binary[i]}, C={c_binary[i]}")
        
        return False


def visualize_conversion(fen: str):
    """Visualize the binary conversion for a FEN"""
    
    converter = FENConverter()
    piece_planes, metadata = converter.fen_to_binary(fen)
    
    print(f"\nFEN: {fen}")
    print("\nPiece Planes:")
    
    piece_names = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
    
    for plane_idx, piece_name in enumerate(piece_names):
        plane = piece_planes[plane_idx]
        if np.any(plane):
            print(f"\n{piece_name} (plane {plane_idx}):")
            for row in range(8):
                print("  ", end="")
                for col in range(8):
                    print("1" if plane[row, col] else ".", end=" ")
                print()
    
    print("\nMetadata:")
    print(f"  Side to move: {'White' if metadata[0] else 'Black'}")
    print(f"  White castling: K={'Yes' if metadata[1] else 'No'}, Q={'Yes' if metadata[2] else 'No'}")
    print(f"  Black castling: K={'Yes' if metadata[3] else 'No'}, Q={'Yes' if metadata[4] else 'No'}")
    if metadata[5] >= 0:
        print(f"  En passant: file {int(metadata[5])}, rank {int(metadata[6])}")
    else:
        print(f"  En passant: None")
    print(f"  Halfmove clock: {metadata[7]:.2f} (normalized)")


def main():
    print("=" * 80)
    print("Testing FEN to Binary Conversion: C vs Python")
    print("=" * 80)
    
    # Test positions
    test_fens = [
        # Starting position
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        
        # After e4
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        
        # Sicilian Defense
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2",
        
        # Middle game position
        "r1bqkb1r/pppp1ppp/2n2n2/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4",
        
        # Position without castling rights
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1",
        
        # Endgame position
        "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - - 99 50",
    ]
    
    print("\n1. VISUALIZATION EXAMPLES")
    print("-" * 80)
    
    # Visualize first two positions
    for fen in test_fens[:2]:
        visualize_conversion(fen)
    
    print("\n\n2. CONVERSION COMPARISON TESTS")
    print("-" * 80)
    
    # Test all positions
    results = []
    for fen in test_fens:
        result = compare_conversions(fen)
        results.append(result)
    
    # Summary
    print("\n" + "=" * 80)
    print("SUMMARY")
    print("=" * 80)
    passed = sum(results)
    total = len(results)
    print(f"Tests passed: {passed}/{total}")
    
    if passed == total:
        print("\n✅ All conversions match! The C implementation is correct.")
    else:
        print("\n❌ Some conversions don't match. Check the implementation.")
        print("\nNote: The C implementation in board_to_binary() should:")
        print("  1. Create 12 planes (P,N,B,R,Q,K,p,n,b,r,q,k)")
        print("  2. Each plane is 8x8 = 64 bits")
        print("  3. Total: 12*64 = 768 bits")
        print("  4. Bit ordering: plane by plane, row by row, col by col")
        print("  5. Row 0 = rank 8, Row 7 = rank 1 (board is flipped)")
    
    return passed == total


if __name__ == '__main__':
    import sys
    success = main()
    sys.exit(0 if success else 1)
