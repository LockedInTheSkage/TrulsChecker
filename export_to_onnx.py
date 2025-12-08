#!/usr/bin/env python3
"""
Export PyTorch chess model to ONNX format for C/C++ inference
"""

import torch
import json
from pathlib import Path

# Load the model architecture
class ChessEvalNet(torch.nn.Module):
    def __init__(self, input_dim=776, hidden_dims=[512, 256, 128], dropout=0.2):
        super(ChessEvalNet, self).__init__()
        
        layers = []
        prev_dim = input_dim
        
        for hidden_dim in hidden_dims:
            layers.append(torch.nn.Linear(prev_dim, hidden_dim))
            layers.append(torch.nn.ReLU())
            layers.append(torch.nn.Dropout(dropout))
            prev_dim = hidden_dim
        
        layers.append(torch.nn.Linear(prev_dim, 1))
        
        self.network = torch.nn.Sequential(*layers)
    
    def forward(self, x):
        return self.network(x)


def export_model(model_path, config_path, output_path):
    """Export PyTorch model to ONNX format"""
    
    print(f"Loading model from {model_path}...")
    
    # Load config
    with open(config_path, 'r') as f:
        config = json.load(f)
    
    # Create model
    model = ChessEvalNet(
        hidden_dims=config['hyperparameters']['hidden_dims'],
        dropout=config['hyperparameters']['dropout']
    )
    
    # Load weights - handle checkpoint format
    checkpoint = torch.load(model_path, map_location='cpu', weights_only=False)
    if isinstance(checkpoint, dict) and 'model_state_dict' in checkpoint:
        model.load_state_dict(checkpoint['model_state_dict'])
    else:
        model.load_state_dict(checkpoint)
    
    model.eval()
    
    print("Model loaded successfully")
    
    # Create dummy input (776 values: 768 pieces + 8 metadata)
    dummy_input = torch.randn(1, 776)
    
    # Export to ONNX
    # Using opset version 17 for ONNX Runtime 1.16.3 compatibility
    print(f"Exporting to {output_path}...")
    torch.onnx.export(
        model,
        dummy_input,
        output_path,
        export_params=True,
        opset_version=17,
        do_constant_folding=True,
        input_names=['input'],
        output_names=['output'],
        dynamic_axes={
            'input': {0: 'batch_size'},
            'output': {0: 'batch_size'}
        }
    )
    
    print(f"✅ Model exported successfully to {output_path}")
    
    # Test the exported model
    print("\nTesting ONNX model...")
    import onnxruntime as ort
    
    session = ort.InferenceSession(output_path)
    
    # Test with starting position (all zeros except piece positions)
    test_input = torch.zeros(1, 776)
    # Add a few pieces as a sanity check
    test_input[0, 0] = 1.0  # White pawn on a2
    test_input[0, 768] = 1.0  # White to move
    
    ort_inputs = {session.get_inputs()[0].name: test_input.numpy()}
    ort_outputs = session.run(None, ort_inputs)
    
    print(f"Test evaluation: {ort_outputs[0][0][0]:.2f} centipawns")
    print("✅ ONNX model works!")


if __name__ == "__main__":
    model_path = Path("models/chess_pytorch_quick_1300000.pt")
    config_path = Path("models/config.json")
    output_path = Path("models/chess_model.onnx")
    
    if not model_path.exists():
        print(f"❌ Model not found: {model_path}")
        exit(1)
    
    if not config_path.exists():
        print(f"❌ Config not found: {config_path}")
        exit(1)
    
    export_model(model_path, config_path, output_path)
