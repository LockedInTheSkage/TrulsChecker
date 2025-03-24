# Detect compiler
ifneq ($(shell which gcc 2>/dev/null),)
    CC := gcc
else
    $(error Compiler not found! Please install gcc)
endif

# Targets
.PHONY: all

all: clean game train

testHeuristic:
    $(CC) -o testHeuristic src/testHeuristic.c src/BitBoard.c src/LookupTable.c src/ChessBoard.c src/Branch.c src/Heuristic.c -lm -g

testZobrist:
    $(CC) -o testZobrist src/testZobrist.c src/Zobrist.c src/BitBoard.c src/LookupTable.c src/ChessBoard.c src/Branch.c -lm -g

testDictionary:
    $(CC) -o testDictionary src/testDictionary.c src/Zobrist.c src/Dictionary.c src/BitBoard.c src/LookupTable.c src/ChessBoard.c src/Heuristic.c -lm -g

train:
    $(CC) -o train src/train.c src/Zobrist.c src/BitBoard.c src/LookupTable.c src/ChessBoard.c src/Dictionary.c src/Branch.c src/OpeningBook.c src/Minimax.c src/Heuristic.c src/ChessBoardHelper.c -lm -g

game:
    $(CC) -o game src/game.c src/Zobrist.c src/BitBoard.c src/LookupTable.c src/ChessBoard.c src/Dictionary.c src/Branch.c src/Minimax.c src/Heuristic.c src/ChessBoardHelper.c -lm -g

clean:
    @rm -f game train testDictionary testZobrist testHeuristic *.gcda *.gcno