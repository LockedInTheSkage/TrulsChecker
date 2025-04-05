import chess
import json

def chess_to_json(fen):
    """
    Converts a FEN string to a JSON object containing piece positions and score.

    Args:
        fen (str): The FEN string representing the chess position.

    Returns:
        str: A JSON string representing the board state.
    """
    board = chess.Board(fen)
    
    piece_map = {
        chess.PAWN: 'P',
        chess.KNIGHT: 'N',
        chess.BISHOP: 'B',
        chess.ROOK: 'R',
        chess.QUEEN: 'Q',
        chess.KING: 'K',
    }
    
    color_map = {
        chess.WHITE: 'W',
        chess.BLACK: 'B',
    }

    squares_map = {
        chess.A8: 0, chess.B8: 1, chess.C8: 2, chess.D8: 3, chess.E8: 4, chess.F8: 5, chess.G8: 6, chess.H8: 7,
        chess.A7: 16, chess.B7: 17, chess.C7: 18, chess.D7: 19, chess.E7: 20, chess.F7: 21, chess.G7: 22, chess.H7: 23,
        chess.A6: 32, chess.B6: 33, chess.C6: 34, chess.D6: 35, chess.E6: 36, chess.F6: 37, chess.G6: 38, chess.H6: 39,
        chess.A5: 48, chess.B5: 49, chess.C5: 50, chess.D5: 51, chess.E5: 52, chess.F5: 53, chess.G5: 54, chess.H5: 55,
        chess.A4: 64, chess.B4: 65, chess.C4: 66, chess.D4: 67, chess.E4: 68, chess.F4: 69, chess.G4: 70, chess.H4: 71,
        chess.A3: 80, chess.B3: 81, chess.C3: 82, chess.D3: 83, chess.E3: 84, chess.F3: 85, chess.G3: 86, chess.H3: 87,
        chess.A2: 96, chess.B2: 97, chess.C2: 98, chess.D2: 99, chess.E2: 100, chess.F2: 101, chess.G2: 102, chess.H2: 103,
        chess.A1: 112, chess.B1: 113, chess.C1: 114, chess.D1: 115, chess.E1: 116, chess.F1: 117, chess.G1: 118, chess.H1: 119,
    }

    result = {}
    
    board_data = {}
    for square in chess.SQUARES:
        piece = board.piece_at(square)
        if piece:
            board_data[chess.SQUARE_NAMES[square]] = {
                'color': color_map[piece.color],
                'piece': piece_map[piece.piece_type],
            }
        else:
            board_data[chess.SQUARE_NAMES[square]] = {
                'color': '',
                'piece': None,
            }
    result['board'] = board_data
    
    score = {
        'W': {'P': 0, 'N': 0, 'B': 0, 'R': 0, 'Q': 0, 'K': 0},
        'B': {'P': 0, 'N': 0, 'B': 0, 'R': 0, 'Q': 0, 'K': 0},
    }
    
    # No score calculation in this version.
    # for square in chess.SQUARES:
    #     piece = board.piece_at(square)
    #     if piece:
    #         score[color_map[piece.color]][piece_map[piece.piece_type]] += 1
    
    result['score'] = score
    
    return json.dumps(result, indent=4)
