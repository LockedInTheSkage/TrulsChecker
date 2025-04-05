#ifndef DICTIONARY_H
#define DICTIONARY_H

#define HASHSIZE 100001

static const char *DICT_FILENAME = "src/data/heuristicDict.dat";

typedef struct nlist {
    struct nlist *left;   // Left child in BST
    struct nlist *right;  // Right child in BST
    uint64_t key;
    int32_t score;
    uint8_t depth;
} nlist;

typedef struct {
    Zobrist_Table *zobrist;
    nlist *hashtab[HASHSIZE];
} Dictionary;

void init_dictionary(Dictionary *dict);
unsigned hash(uint64_t key);
nlist *lookup(Dictionary *dict, uint64_t key);
nlist *put(Dictionary *dict, uint64_t key, int32_t score, uint8_t depth);
nlist *install_board(Dictionary *dict, ChessBoard *board, int32_t score, uint8_t depth);
nlist *lookup_board(Dictionary *dict, ChessBoard *board);
int save_dictionary(Dictionary *dict);
int load_dictionary(Dictionary *dict);
void free_dictionary(Dictionary *dict);
void exit_dictionary(Dictionary *dict);

#endif /* DICTIONARY_H */