from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import subprocess
import chess
import chess.pgn
import uuid

app = FastAPI()

game_states = {}  # Store game states by browser_id


class NewGameResponse(BaseModel):
    fen: str
    browser_id: str

class LegalMovesResponse(BaseModel):
    moves: list

class MakeMoveResponse(BaseModel):
    fen: str
    move: str

class BestMoveRequest(BaseModel):
    fen: str

@app.post("/new-game", response_model=NewGameResponse)
async def new_game():
    browser_id = str(uuid.uuid4())
    board = chess.Board()
    game_states[browser_id] = {"board": board, "moves": []}
    return {"fen": board.fen(), "browser_id": browser_id}

@app.get("/load-game", response_model=MakeMoveResponse)
async def load_game(browser_id: str):
    if browser_id not in game_states:
        raise HTTPException(status_code=404, detail="Game not found")
    board = game_states[browser_id]["board"]
    moves = game_states[browser_id]["moves"]
    return {"fen": board.fen(), "moves": moves}

@app.get("/legal-moves/{position}", response_model=LegalMovesResponse)
async def get_legal_moves(browser_id: str, position: str):
    if browser_id not in game_states:
        raise HTTPException(status_code=404, detail="Game not found")
    board = game_states[browser_id]["board"]
    square = chess.parse_square(position)
    legal_moves = [move.uci() for move in board.legal_moves if move.from_square == square]
    return {"moves": legal_moves}

@app.get("/make-move/{from_pos}/{to_pos}", response_model=MakeMoveResponse)
async def make_move(browser_id: str, from_pos: str, to_pos: str):
    if browser_id not in game_states:
        raise HTTPException(status_code=404, detail="Game not found")

    board = game_states[browser_id]["board"]
    move_uci = f"{from_pos}{to_pos}"
    move = chess.Move.from_uci(move_uci)

    if move not in board.legal_moves:
        raise HTTPException(status_code=400, detail="Illegal move")

    board.push(move)
    game_states[browser_id]["moves"].append(move_uci)

    if board.is_game_over():
        del game_states[browser_id] # Remove game state if game is over.

    return {"fen": board.fen(), "move": move_uci}

@app.post("/best_move")
async def get_best_move(request: BestMoveRequest):
    fen = request.fen
    print(f"Received fen: {fen}")
    try:
        process = subprocess.Popen(
            ["./chess_program", "--api", fen],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        stdout, stderr = process.communicate()

        if process.returncode == 0:
            chess_output = stdout.decode("utf-8").strip()
            return {"best_move": chess_output}
        else:
            chess_error = stderr.decode("utf-8").strip()
            raise HTTPException(status_code=500, detail={"error": chess_error})

    except Exception as e:
        raise HTTPException(status_code=500, detail={"error": str(e)})

@app.post("/train")
async def train_program():
    try:
        process = subprocess.Popen(
            ["./chess_program", "--train"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        stdout, stderr = process.communicate()

        if process.returncode == 0:
            return {"message": "Training completed successfully."}
        else:
            chess_error = stderr.decode("utf-8").strip()
            raise HTTPException(status_code=500, detail={"error": chess_error})

    except Exception as e:
        raise HTTPException(status_code=500, detail={"error": str(e)})