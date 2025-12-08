#!/usr/bin/env python3
"""
Neural network evaluation wrapper for TrulsChecker
Loads the PyTorch model and provides evaluation for chess positions
"""

import sys
import json
import torch
import torch.nn as nn
import numpy as np
from pathlib import Path


class ChessEvalNet(nn.Module):
    """Neural network for chess position evaluation"""
    
    def __init__(self, hidden_dims=[512, 256, 128], dropout=0.1):
        super(ChessEvalNet, self).__init__()
        
        # Binary representation: 768 features (64 squares * 12 piece types)
        input_dim = 768
        
        layers = []
        prev_dim = input_dim
        
        for hidden_dim in hidden_dims:
            layers.extend([
                nn.Linear(prev_dim, hidden_dim),
                nn.ReLU(),
                nn.Dropout(dropout)
            ])
            prev_dim = hidden_dim
        
        # Output layer (single value for evaluation)
        layers.append(nn.Linear(prev_dim, 1))
        
        self.network = nn.Sequential(*layers)
    
    def forward(self, x):
        return self.network(x)


class NeuralEvaluator:
    """Wrapper for loading and using the neural network"""
    
    def __init__(self, model_path, config_path, device='cpu'):
        self.device = torch.device(device if torch.cuda.is_available() else 'cpu')
        
        # Load config
        with open(config_path, 'r') as f:
            config = json.load(f)
        
        # Initialize model
        self.model = ChessEvalNet(
            hidden_dims=config['hyperparameters']['hidden_dims'],
            dropout=config['hyperparameters']['dropout']
        )
        
        # Load weights - handle both raw state_dict and checkpoint formats
        checkpoint = torch.load(model_path, map_location=self.device, weights_only=False)
        if isinstance(checkpoint, dict) and 'model_state_dict' in checkpoint:
            # Checkpoint format with training metadata
            self.model.load_state_dict(checkpoint['model_state_dict'])
        else:
            # Raw state_dict format
            self.model.load_state_dict(checkpoint)
        
        self.model.to(self.device)
        self.model.eval()
        
        print(f"Loaded neural network model on {self.device}", file=sys.stderr)
    
    def board_to_tensor(self, board_array):
        """
        Convert board representation to tensor
        
        board_array: 776-dimensional binary array where:
        - First 768 elements: 12 piece type planes × 64 squares
          * 0-63: White pawns
          * 64-127: White knights
          * 128-191: White bishops
          * 192-255: White rooks
          * 256-319: White queens
          * 320-383: White kings
          * 384-447: Black pawns
          * ... (same pattern for black)
        - Last 8 elements: metadata (side to move, castling, en passant, halfmove)
        """
        tensor = torch.tensor(board_array, dtype=torch.float32)
        return tensor.unsqueeze(0).to(self.device)  # Add batch dimension
    
    def evaluate(self, board_array):
        """
        Evaluate a position
        
        Returns: evaluation score (in centipawns)
        """
        with torch.no_grad():
            tensor = self.board_to_tensor(board_array)
            score = self.model(tensor).item()
            # Scale to centipawns (model outputs normalized scores)
            return int(score * 100)


def main():
    """
    Main loop: reads board positions from stdin, outputs evaluations
    Format: Each line is 768 space-separated 0/1 values
    """
    model_dir = Path(__file__).parent.parent / "models"
    model_path = model_dir / "chess_pytorch_quick_1300000.pt"
    config_path = model_dir / "chess_pytorch_quick_1300000_config.json"
    
    if not model_path.exists():
        print(f"Error: Model not found at {model_path}", file=sys.stderr)
        sys.exit(1)
    
    evaluator = NeuralEvaluator(str(model_path), str(config_path))
    
    print("Neural evaluator ready", file=sys.stderr)
    sys.stderr.flush()
    
    # Read positions from stdin
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        
        try:
            # Parse board array
            board_array = [float(x) for x in line.split()]
            
            if len(board_array) != 776:
                print(f"ERROR: Expected 776 values (768 pieces + 8 metadata), got {len(board_array)}")
                continue
            
            # Evaluate
            score = evaluator.evaluate(board_array)
            print(score)
            sys.stdout.flush()
            
        except Exception as e:
            print(f"ERROR: {e}")
            sys.stdout.flush()


if __name__ == "__main__":
    main()
