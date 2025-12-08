import numpy as np
import chess


class FENConverter:
    """Convert FEN positions to various numpy tensor representations"""
    
    # Piece to index mapping for numerical representation
    PIECE_TO_NUM = {
        'P': 1, 'N': 2, 'B': 3, 'R': 4, 'Q': 5, 'K': 6,  # White pieces
        'p': -1, 'n': -2, 'b': -3, 'r': -4, 'q': -5, 'k': -6  # Black pieces
    }
    
    # Piece to plane index for binary representation
    PIECE_TO_PLANE = {
        'P': 0, 'N': 1, 'B': 2, 'R': 3, 'Q': 4, 'K': 5,  # White pieces (planes 0-5)
        'p': 6, 'n': 7, 'b': 8, 'r': 9, 'q': 10, 'k': 11  # Black pieces (planes 6-11)
    }
    
    def __init__(self):
        pass
    
    def fen_to_numerical(self, fen: str) -> np.ndarray:
        """Convert FEN to 8x8 numpy array with pieces as numbers
        
        Args:
            fen: FEN string representation of chess position
            
        Returns:
            8x8 numpy array where pieces are represented as:
            - Positive numbers (1-6) for white pieces
            - Negative numbers (-1 to -6) for black pieces
            - 0 for empty squares
        """
        board = chess.Board(fen)
        tensor = np.zeros((8, 8), dtype=np.int8)
        
        for square in chess.SQUARES:
            piece = board.piece_at(square)
            if piece:
                row = 7 - (square // 8)  # Convert from chess square to array row
                col = square % 8
                piece_symbol = piece.symbol()
                tensor[row, col] = self.PIECE_TO_NUM[piece_symbol]
        
        return tensor
    
    def fen_to_binary(self, fen: str) -> tuple:
        """Convert FEN to binary bitmap representation
        
        Args:
            fen: FEN string representation of chess position
            
        Returns:
            Tuple of (piece_planes, metadata):
            - piece_planes: (12, 8, 8) numpy array with one plane per piece type
            - metadata: (8,) numpy array with additional information:
                [0]: side to move (1=white, 0=black)
                [1]: white kingside castling
                [2]: white queenside castling
                [3]: black kingside castling
                [4]: black queenside castling
                [5-6]: en passant file (0-7, or -1 if none)
                [7]: halfmove clock (normalized to 0-1)
        """
        board = chess.Board(fen)
        
        # Create 12 planes for pieces (6 white + 6 black)
        piece_planes = np.zeros((12, 8, 8), dtype=np.float32)
        
        # Fill piece planes
        for square in chess.SQUARES:
            piece = board.piece_at(square)
            if piece:
                row = 7 - (square // 8)
                col = square % 8
                plane_idx = self.PIECE_TO_PLANE[piece.symbol()]
                piece_planes[plane_idx, row, col] = 1.0
        
        # Create metadata array
        metadata = np.zeros(8, dtype=np.float32)
        
        # Side to move
        metadata[0] = 1.0 if board.turn == chess.WHITE else 0.0
        
        # Castling rights
        metadata[1] = 1.0 if board.has_kingside_castling_rights(chess.WHITE) else 0.0
        metadata[2] = 1.0 if board.has_queenside_castling_rights(chess.WHITE) else 0.0
        metadata[3] = 1.0 if board.has_kingside_castling_rights(chess.BLACK) else 0.0
        metadata[4] = 1.0 if board.has_queenside_castling_rights(chess.BLACK) else 0.0
        
        # En passant
        if board.ep_square is not None:
            metadata[5] = float(board.ep_square % 8)  # file
            metadata[6] = float(7 - (board.ep_square // 8))  # rank
        else:
            metadata[5] = -1.0
            metadata[6] = -1.0
        
        # Halfmove clock (normalized)
        metadata[7] = min(board.halfmove_clock / 100.0, 1.0)
        
        return piece_planes, metadata
    
    def fen_to_combined_binary(self, fen: str) -> np.ndarray:
        """Convert FEN to flattened binary representation
        
        Args:
            fen: FEN string representation of chess position
            
        Returns:
            Flattened numpy array combining piece planes and metadata
            Shape: (12*8*8 + 8,) = (776,)
        """
        piece_planes, metadata = self.fen_to_binary(fen)
        combined = np.concatenate([piece_planes.flatten(), metadata])
        return combined
    
    def batch_convert_numerical(self, fens: list) -> np.ndarray:
        """Convert multiple FENs to numerical representation
        
        Args:
            fens: List of FEN strings
            
        Returns:
            numpy array of shape (N, 8, 8)
        """
        return np.array([self.fen_to_numerical(fen) for fen in fens])
    
    def batch_convert_binary(self, fens: list) -> tuple:
        """Convert multiple FENs to binary representation
        
        Args:
            fens: List of FEN strings
            
        Returns:
            Tuple of (piece_planes, metadata):
            - piece_planes: (N, 12, 8, 8) numpy array
            - metadata: (N, 8) numpy array
        """
        results = [self.fen_to_binary(fen) for fen in fens]
        piece_planes = np.array([r[0] for r in results])
        metadata = np.array([r[1] for r in results])
        return piece_planes, metadata
    
    def batch_convert_combined_binary(self, fens: list) -> np.ndarray:
        """Convert multiple FENs to flattened binary representation
        
        Args:
            fens: List of FEN strings
            
        Returns:
            numpy array of shape (N, 776)
        """
        return np.array([self.fen_to_combined_binary(fen) for fen in fens])
