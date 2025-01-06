#include "Dictionary.h"
#include "Zobrist.h"
#include "ChessBoard.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct nlist *hashtab[HASHSIZE];

/* hash: form hash value for uint64_t key */
unsigned hash(uint64_t key)
{
    return key % HASHSIZE;
}

/* lookup: look for key in hashtab */
struct nlist *lookup(uint64_t key)
{
    struct nlist *np;
    for (np = hashtab[hash(key)]; np != NULL; np = np->next)
        if (np->key == key)
            return np; /* found */
    return NULL; /* not found */
}

/* install: put (key, score, depth) in hashtab */
struct nlist *install(uint64_t key, int32_t score, uint8_t depth)
{
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(key)) == NULL) { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL)
            return NULL;
        np->key = key;
        hashval = hash(key);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    np->score = score;
    np->depth = depth;
    return np;
}

/* install_board: put (board, score, depth) in hashtab */
struct nlist *install_board(struct ChessBoard *board, int32_t score, uint8_t depth)
{
    uint64_t key = zobrist_hash(board);
    return install(key, score, depth);
}

/* lookup_board: look for board in hashtab */
struct nlist *lookup_board(struct ChessBoard *board)
{
    uint64_t key = zobrist_hash(board);
    return lookup(key);
}

/* save_dictionary: save the current dictionary to a file */
int save_dictionary(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
        return -1;

    for (int i = 0; i < HASHSIZE; i++) {
        struct nlist *np = hashtab[i];
        while (np != NULL) {
            fwrite(&np->key, sizeof(np->key), 1, file);
            fwrite(&np->score, sizeof(np->score), 1, file);
            fwrite(&np->depth, sizeof(np->depth), 1, file);
            np = np->next;
        }
    }

    fclose(file);
    return 0;
}

/* load_dictionary: load the dictionary from a file */
int load_dictionary(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return -1;

    uint64_t key;
    int32_t score;
    uint8_t depth;

    while (fread(&key, sizeof(key), 1, file) == 1) {
        if (fread(&score, sizeof(score), 1, file) != 1 ||
            fread(&depth, sizeof(depth), 1, file) != 1) {
            fclose(file);
            return -1;
        }
        install(key, score, depth);
    }

    fclose(file);
    return 0;
}