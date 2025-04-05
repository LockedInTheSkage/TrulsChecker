
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "BitBoard.h"
#include "LookupTable.h"
#include "ChessBoard.h"
#include "Zobrist.h"
#include "Dictionary.h"


static const char *DICT_FILENAME = "src/data/heuristicDict.dat";

/* init_dictionary: initialize the dictionary */
void init_dictionary(Dictionary *dict)
{   
    if (malloc(sizeof(*dict)) == NULL) {
        printf("Failed to allocate memory for dictionary\n");
        exit(1);
    }

    printf("Allocated memory for dictionary\n");
    for (int i = 0; i < HASHSIZE; i++) {
        dict->hashtab[i] = NULL;
    }
    printf("Initialized hashtab\n");
    if (DICT_FILENAME != NULL) {
        if (load_dictionary(dict)) {
            printf("Failed to load dictionary from file %s\n", DICT_FILENAME);
        }
    }
    printf("Loaded dictionary from file\n");
    dict->zobrist = init_zobrist();
}

/* hash: form hash value for uint64_t key */
unsigned hash(uint64_t key){
    return key % HASHSIZE;
}

/* lookup: look for key in hashtab */
struct nlist *lookup(Dictionary *dict, uint64_t key)
{
    struct nlist *np;
    for (np = dict->hashtab[hash(key)]; np != NULL; np = np->next)
        if (np->key == key)
            return np; /* found */
    return NULL; /* not found */
}

/* put: put (key, score, depth) in hashtab */
struct nlist *put(Dictionary *dict, uint64_t key, int32_t score, uint8_t depth)
{
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(dict, key)) == NULL) { /* not found */
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL)
            return NULL;
        np->key = key;
        hashval = hash(key);
        np->next = dict->hashtab[hashval];
        dict->hashtab[hashval] = np;
    }
    np->score = score;
    np->depth = depth;
    return np;
}

/* install_board: put (board, score, depth) in hashtab */
struct nlist *install_board(Dictionary *dict, ChessBoard *board, int32_t score, uint8_t depth)
{
    uint64_t key = get_zobrist_hash(board, dict->zobrist);
    return put(dict, key, score, depth);
}

/* lookup_board: look for board in hashtab */
struct nlist *lookup_board(Dictionary *dict, ChessBoard *board)
{
    uint64_t key = get_zobrist_hash(board, dict->zobrist);
    return lookup(dict, key);
}

/* save_dictionary: save the current dictionary to a file */
int save_dictionary(Dictionary *dict)
{
    FILE *file = fopen(DICT_FILENAME, "wb");
    if (file == NULL)
        return -1;

    for (int i = 0; i < HASHSIZE; i++) {
        struct nlist *np = dict->hashtab[i];
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
int load_dictionary(Dictionary *dict)
{
    FILE *file = fopen(DICT_FILENAME, "rb");
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
        put(dict, key, score, depth);
    }

    fclose(file);
    return 0;
}

/* free_dictionary: free the dictionary */
void free_dictionary(Dictionary *dict)
{
    for (int i = 0; i < HASHSIZE; i++) {
        struct nlist *np = dict->hashtab[i];
        while (np != NULL) {
            struct nlist *next = np->next;
            free(np);
            np = next;
        }
        dict->hashtab[i] = NULL;
    }
}

/* exit_dictionary: save the current dictionary to a file, then free the dictionary */
void exit_dictionary(Dictionary *dict)
{
    save_dictionary(dict);
    free_dictionary(dict);
}
