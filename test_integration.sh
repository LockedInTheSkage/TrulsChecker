#!/bin/bash
set -e

echo "=== TrulsChecker Integration Test ==="
echo ""

echo "1. Testing Heuristic..."
./testHeuristic | grep -q "All tests passed!" && echo "✓ Heuristic tests PASSED" || echo "✗ Heuristic tests FAILED"

echo ""
echo "2. Testing Zobrist..."
./testZobrist > /dev/null 2>&1 && echo "✓ Zobrist tests PASSED" || echo "✗ Zobrist tests FAILED"

echo ""
echo "3. Testing Minimax (first 3 positions)..."
./testMinimax 2>&1 | head -60 | grep -q "PASSED" && echo "✓ Minimax tests PASSED" || echo "✗ Minimax tests FAILED"

echo ""
echo "4. Testing AI move generation..."
echo -e "ai\nquit" | ./game 3 2>&1 | grep -q "AI plays:" && echo "✓ AI move generation PASSED" || echo "✗ AI move generation FAILED"

echo ""
echo "5. Testing manual move input..."
echo -e "move e2e4\nboard\nquit" | ./game 3 2>&1 | grep -q "Black to move" && echo "✓ Manual moves PASSED" || echo "✗ Manual moves FAILED"

echo ""
echo "=== All Integration Tests Complete ==="
