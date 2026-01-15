import torch
import json
from pathlib import Path

import onnxruntime as ort

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
    with open(config_path, 'r') as f:
        config = json.load(f)

    model = ChessEvalNet(
        hidden_dims=config['hyperparameters']['hidden_dims'],
        dropout=config['hyperparameters']['dropout']
    )

    checkpoint = torch.load(model_path, map_location='cpu', weights_only=False)
    if isinstance(checkpoint, dict) and 'model_state_dict' in checkpoint:
        model.load_state_dict(checkpoint['model_state_dict'])
    else:
        model.load_state_dict(checkpoint)
    
    model.eval()

    dummy_input = torch.randn(1, 776)
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
    
    session = ort.InferenceSession(output_path)
    
    test_input = torch.zeros(1, 776)
    test_input[0, 0] = 1.0
    test_input[0, 768] = 1.0
    
    ort_inputs = {session.get_inputs()[0].name: test_input.numpy()}
    ort_outputs = session.run(None, ort_inputs)
    
    print(f"Test evaluation: {ort_outputs[0][0][0]:.2f}")
    print("ONNX model works!")


if __name__ == "__main__":
    model_path = Path("models_pytorch/chess_pytorch_quick_1300000.pt")
    config_path = Path("models_pytorch/config.json")
    output_path = Path("models/chess_model.onnx")
    
    if not model_path.exists():
        print(f"Model not found: {model_path}")
        exit(1)
    
    if not config_path.exists():
        print(f"Config not found: {config_path}")
        exit(1)
    
    export_model(model_path, config_path, output_path)
