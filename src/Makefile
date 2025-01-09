# Detect compiler
ifneq ($(shell which gcc 2>/dev/null),)
    CC := gcc
else
    $(error Compiler not found! Please install gcc)
endif

# Targets
.PHONY: all

all: clean game testDictionary

testHeuristic:
	$(CC) -o testHeuristic testHeuristic.c BitBoard.c LookupTable.c ChessBoard.c Branch.c Heuristic.c -lm -g

testZobrist:
	$(CC) -o testZobrist testZobrist.c Zobrist.c BitBoard.c LookupTable.c ChessBoard.c Branch.c -lm -g

testDictionary:
	$(CC) -o testDictionary testDictionary.c Zobrist.c Dictionary.c BitBoard.c LookupTable.c ChessBoard.c Heuristic.c -lm -g

game:
	$(CC) -o game game.c Zobrist.c BitBoard.c LookupTable.c ChessBoard.c Dictionary.c Branch.c Minimax.c Heuristic.c ChessBoardHelper.c -lm -g

clean:
	@rm -f game testDictionary testZobrist testHeuristic *.gcda *.gcno
