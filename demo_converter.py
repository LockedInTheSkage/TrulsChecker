#!/usr/bin/env python3
"""
Demonstration of the FENConverter usage
"""

import numpy as np
from converters import FENConverter


def demo_numerical():
    """Demonstrate numerical conversion (pieces as numbers)"""
    print("=" * 80)
    print("DEMO 1: Numerical Conversion (pieces as numbers)")
    print("=" * 80)
    
    converter = FENConverter()
    
    # Starting position
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    print(f"\nFEN: {fen}")
    
    board = converter.fen_to_numerical(fen)
    print("\nNumerical representation (8x8 array):")
    print("Positive = White, Negative = Black, 0 = Empty")
    print("1/-1=Pawn, 2/-2=Knight, 3/-3=Bishop, 4/-4=Rook, 5/-5=Queen, 6/-6=King")
    print()
    
    # Print with rank labels
    for rank in range(8, 0, -1):
        row = 8 - rank
        print(f"{rank} ", end="")
        for col in range(8):
            val = board[row, col]
            if val == 0:
                print(" . ", end="")
            else:
                print(f"{val:+3}", end="")
        print()
    print("   a  b  c  d  e  f  g  h")
    
    print(f"\nShape: {board.shape}")
    print(f"Data type: {board.dtype}")


def demo_binary():
    """Demonstrate binary plane conversion"""
    print("\n\n" + "=" * 80)
    print("DEMO 2: Binary Plane Conversion (12 planes)")
    print("=" * 80)
    
    converter = FENConverter()
    
    # Position after 1.e4
    fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
    print(f"\nFEN: {fen}")
    
    piece_planes, metadata = converter.fen_to_binary(fen)
    
    print(f"\nPiece planes shape: {piece_planes.shape}")
    print(f"Metadata shape: {metadata.shape}")
    
    # Show which planes have pieces
    piece_names = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
    print("\nPiece counts per plane:")
    for i, name in enumerate(piece_names):
        count = int(np.sum(piece_planes[i]))
        if count > 0:
            print(f"  Plane {i:2d} ({name}): {count} pieces")
    
    # Show metadata
    print("\nMetadata:")
    print(f"  [0] Side to move: {metadata[0]:.1f} ({'White' if metadata[0] else 'Black'})")
    print(f"  [1] White O-O:    {metadata[1]:.1f}")
    print(f"  [2] White O-O-O:  {metadata[2]:.1f}")
    print(f"  [3] Black O-O:    {metadata[3]:.1f}")
    print(f"  [4] Black O-O-O:  {metadata[4]:.1f}")
    print(f"  [5] EP file:      {metadata[5]:.1f}")
    print(f"  [6] EP rank:      {metadata[6]:.1f}")
    print(f"  [7] Halfmove:     {metadata[7]:.3f}")
    
    # Visualize the white pawn plane
    print("\nWhite Pawn plane (plane 0):")
    for rank in range(8, 0, -1):
        row = 8 - rank
        print(f"{rank} ", end="")
        for col in range(8):
            print("P" if piece_planes[0, row, col] else ".", end=" ")
        print()
    print("  a b c d e f g h")


def demo_combined():
    """Demonstrate combined flattened representation"""
    print("\n\n" + "=" * 80)
    print("DEMO 3: Combined Flattened Binary (768 bits)")
    print("=" * 80)
    
    converter = FENConverter()
    
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    print(f"\nFEN: {fen}")
    
    combined = converter.fen_to_combined_binary(fen)
    
    print(f"\nCombined shape: {combined.shape}")
    print(f"Data type: {combined.dtype}")
    print(f"\nFirst 768 values are piece planes (12 planes × 64 squares)")
    print(f"Last 8 values are metadata")
    
    # Count ones
    piece_bits = combined[:768]
    ones = int(np.sum(piece_bits))
    print(f"\nTotal pieces on board: {ones} (bits set to 1)")
    
    # Show breakdown by plane
    print("\nBits set per plane:")
    for plane in range(12):
        start = plane * 64
        end = start + 64
        count = int(np.sum(combined[start:end]))
        piece_names = ['P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k']
        print(f"  Plane {plane:2d} ({piece_names[plane]}): indices {start:3d}-{end-1:3d} = {count} bits")
    
    print(f"\nMetadata: indices 768-775")
    print(f"  {combined[768:]}")


def demo_batch():
    """Demonstrate batch conversion"""
    print("\n\n" + "=" * 80)
    print("DEMO 4: Batch Conversion (multiple positions)")
    print("=" * 80)
    
    converter = FENConverter()
    
    # Multiple positions
    fens = [
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2",
    ]
    
    print(f"\nConverting {len(fens)} positions...")
    
    # Numerical batch
    numerical_batch = converter.batch_convert_numerical(fens)
    print(f"\nNumerical batch shape: {numerical_batch.shape}")
    print(f"  {len(fens)} positions × 8 rows × 8 cols")
    
    # Binary batch
    piece_planes_batch, metadata_batch = converter.batch_convert_binary(fens)
    print(f"\nBinary batch shapes:")
    print(f"  Piece planes: {piece_planes_batch.shape}")
    print(f"  Metadata: {metadata_batch.shape}")
    
    # Combined batch
    combined_batch = converter.batch_convert_combined_binary(fens)
    print(f"\nCombined batch shape: {combined_batch.shape}")
    print(f"  {len(fens)} positions × 776 values (768 pieces + 8 metadata)")
    
    # Show side to move for each position
    print("\nSide to move for each position:")
    for i, fen in enumerate(fens):
        side = "White" if metadata_batch[i, 0] else "Black"
        print(f"  Position {i}: {side}")


def demo_usage_example():
    """Show typical usage pattern"""
    print("\n\n" + "=" * 80)
    print("DEMO 5: Typical Usage Pattern")
    print("=" * 80)
    
    print("\n# Initialize converter")
    print("converter = FENConverter()")
    
    print("\n# Convert a single position")
    print("fen = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'")
    print("board_array = converter.fen_to_numerical(fen)  # For visualization")
    print("piece_planes, metadata = converter.fen_to_binary(fen)  # For neural networks")
    print("combined = converter.fen_to_combined_binary(fen)  # Flattened (768 bits)")
    
    print("\n# Convert multiple positions")
    print("fens = [fen1, fen2, fen3, ...]")
    print("batch = converter.batch_convert_combined_binary(fens)")
    print("# Now batch has shape (N, 776) ready for neural network")
    
    print("\n# The combined binary format is what's used by the chess model:")
    print("# - First 768 values: 12 planes × 64 squares")
    print("#   - Planes 0-5: White pieces (P, N, B, R, Q, K)")
    print("#   - Planes 6-11: Black pieces (p, n, b, r, q, k)")
    print("# - Last 8 values: metadata (turn, castling, en passant, halfmove)")
    
    # Actually run it
    converter = FENConverter()
    fen = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
    combined = converter.fen_to_combined_binary(fen)
    print(f"\n✅ Result: numpy array with shape {combined.shape}, dtype {combined.dtype}")


def main():
    """Run all demos"""
    demo_numerical()
    demo_binary()
    demo_combined()
    demo_batch()
    demo_usage_example()
    
    print("\n\n" + "=" * 80)
    print("All demonstrations complete!")
    print("=" * 80)


if __name__ == '__main__':
    main()
