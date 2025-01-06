#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>

struct nlist {
    struct nlist *next;
    uint64_t key;
    int32_t score;
    uint8_t depth;
};

#define HASHSIZE 101

unsigned hash(uint64_t key);
struct nlist *lookup(uint64_t key);
struct nlist *install(uint64_t key, int32_t score, uint8_t depth);
struct nlist *install_board(struct ChessBoard *board, int32_t score, uint8_t depth);
struct nlist *lookup_board(struct ChessBoard *board);

#endif /* DICTIONARY_H */