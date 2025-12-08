# Detect compiler
ifneq ($(shell which gcc 2>/dev/null),)
    CC := gcc
else
    $(error Compiler not found! Please install gcc)
endif

# Compiler flags
CFLAGS := -Wall -g -Isrc -I/usr/local/include
LDFLAGS := -lm -L/usr/local/lib -lonnxruntime

# Templechess v2 source files
TEMPLECHESS_DIR := src/templechess/templechess/src
TEMPLECHESS_SRC := $(TEMPLECHESS_DIR)/BitBoard.c \
                   $(TEMPLECHESS_DIR)/LookupTable.c \
                   $(TEMPLECHESS_DIR)/ChessBoard.c \
                   $(TEMPLECHESS_DIR)/MoveSet.c

# Project source files
CORE_SRC := src/Zobrist.c src/Dictionary.c src/Heuristic.c src/NeuralHeuristic.c

# Targets
.PHONY: all clean

all: game testHeuristic testMinimax testDictionary testZobrist

game: src/game.c src/Minimax.c src/TemplechessAdapter.c $(CORE_SRC) $(TEMPLECHESS_SRC)
	$(CC) $(CFLAGS) -o game $^ $(LDFLAGS)

testHeuristic: src/testHeuristic.c src/Heuristic.c $(TEMPLECHESS_SRC)
	$(CC) $(CFLAGS) -o testHeuristic $^ $(LDFLAGS)

testMinimax: src/testMinimax.c src/Minimax.c src/TemplechessAdapter.c $(CORE_SRC) $(TEMPLECHESS_SRC)
	$(CC) $(CFLAGS) -o testMinimax $^ $(LDFLAGS)

testDictionary: src/testDictionary.c $(CORE_SRC) $(TEMPLECHESS_SRC)
	$(CC) $(CFLAGS) -o testDictionary $^ $(LDFLAGS)

testZobrist: src/testZobrist.c src/Zobrist.c $(TEMPLECHESS_SRC)
	$(CC) $(CFLAGS) -o testZobrist $^ $(LDFLAGS)

clean:
	@rm -f game testHeuristic testMinimax testDictionary testZobrist
	@rm -f *.gcda *.gcno
	@echo "Cleaned build artifacts"