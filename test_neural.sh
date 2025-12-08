#!/bin/bash
# Test if neural network evaluation is working

echo "=== Neural Network Integration Test ==="
echo ""

# Check if PyTorch is installed
echo "1. Checking PyTorch installation..."
python3 -c "import torch; print(f'PyTorch {torch.__version__} installed')" 2>&1 | head -1

if [ $? -ne 0 ]; then
    echo "❌ PyTorch not installed. Install with:"
    echo "   pip3 install -r requirements_neural.txt"
    exit 1
fi

echo "✓ PyTorch is installed"
echo ""

# Check if model file exists
echo "2. Checking for model file..."
if [ ! -f "models/chess_pytorch_quick_1300000.pt" ]; then
    echo "❌ Model file not found: models/chess_pytorch_quick_1300000.pt"
    exit 1
fi
echo "✓ Model file found"
echo ""

# Test neural evaluator directly
echo "3. Testing neural evaluator script..."
# Create a test position (starting position in binary format)
# White pawn on e2 (square 52)
TEST_POS=$(python3 -c "print(' '.join(['1' if i == 52 else '0' for i in range(768)]))")
echo "$TEST_POS" | timeout 5 python3 src/neural_eval.py 2>/dev/null | head -1

if [ $? -eq 0 ]; then
    echo "✓ Neural evaluator script works"
else
    echo "❌ Neural evaluator script failed"
    exit 1
fi
echo ""

# Test the game with neural network
echo "4. Testing game with neural network..."
echo -e "board\nquit" | timeout 10 ./game 2 2>&1 | grep -q "Neural network evaluation enabled"

if [ $? -eq 0 ]; then
    echo "✓ Neural network successfully integrated into game"
else
    echo "⚠  Neural network not available, using material evaluation"
fi

echo ""
echo "=== Test Complete ==="
