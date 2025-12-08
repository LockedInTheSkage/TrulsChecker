#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "templechess/templechess/src/BitBoard.h"
#include "templechess/templechess/src/LookupTable.h"
#include "templechess/templechess/src/ChessBoard.h"
#include "Zobrist.h"
#include "Dictionary.h"




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

/* Helper function to create a new nlist node */
static nlist *create_node(uint64_t key, int32_t score, uint8_t depth) {
    nlist *node = (nlist *)malloc(sizeof(nlist));
    if (node == NULL) {
        return NULL;
    }
    node->key = key;
    node->score = score;
    node->depth = depth;
    node->left = node->right = NULL;
    return node;
}

/* Helper function to insert a node into a binary search tree */
static nlist *insert_node(nlist *root, uint64_t key, int32_t score, uint8_t depth) {
    // If tree is empty or we've reached a leaf node, create a new node
    if (root == NULL) {
        return create_node(key, score, depth);
    }
    
    // If key already exists, update score and depth
    if (key == root->key) {
        root->score = score;
        root->depth = depth;
        return root;
    }
    
    // Recursively insert into the appropriate subtree
    if (key < root->key) {
        root->left = insert_node(root->left, key, score, depth);
    } else {
        root->right = insert_node(root->right, key, score, depth);
    }
    
    return root;
}

/* Helper function for searching in a binary search tree */
static nlist *search_node(nlist *root, uint64_t key) {
    // Base case: tree is empty or key is found
    if (root == NULL || root->key == key) {
        return root;
    }
    
    // Search in the appropriate subtree
    if (key < root->key) {
        return search_node(root->left, key);
    } else {
        return search_node(root->right, key);
    }
}

/* lookup: look for key in hashtab using BST */
nlist *lookup(Dictionary *dict, uint64_t key)
{
    unsigned hashval = hash(key);
    return search_node(dict->hashtab[hashval], key);
}

/* put: put (key, score, depth) in hashtab using BST */
nlist *put(Dictionary *dict, uint64_t key, int32_t score, uint8_t depth)
{
    unsigned hashval = hash(key);
    dict->hashtab[hashval] = insert_node(dict->hashtab[hashval], key, score, depth);
    return search_node(dict->hashtab[hashval], key);
}

/* install_board: put (board, score, depth) in hashtab */
nlist *install_board(Dictionary *dict, ChessBoard *board, int32_t score, uint8_t depth)
{
    uint64_t key = get_zobrist_hash(board, dict->zobrist);
    return put(dict, key, score, depth);
}

/* lookup_board: look for board in hashtab */
nlist *lookup_board(Dictionary *dict, ChessBoard *board)
{
    uint64_t key = get_zobrist_hash(board, dict->zobrist);
    return lookup(dict, key);
}

/* Helper function to traverse BST in-order and save nodes */
static void save_tree_nodes(nlist *root, FILE *file) {
    if (root != NULL) {
        save_tree_nodes(root->left, file);
        
        fwrite(&root->key, sizeof(root->key), 1, file);
        fwrite(&root->score, sizeof(root->score), 1, file);
        fwrite(&root->depth, sizeof(root->depth), 1, file);
        
        save_tree_nodes(root->right, file);
    }
}

/* save_dictionary: save the current dictionary to a file */
int save_dictionary(Dictionary *dict)
{
    FILE *file = fopen(DICT_FILENAME, "wb");
    if (file == NULL)
        return -1;
    
    for (int i = 0; i < HASHSIZE; i++) {
        if (dict->hashtab[i] != NULL) {
            save_tree_nodes(dict->hashtab[i], file);
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

/* Helper function to free a BST */
static void free_tree(nlist *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

/* free_dictionary: free the dictionary */
void free_dictionary(Dictionary *dict)
{
    for (int i = 0; i < HASHSIZE; i++) {
        if (dict->hashtab[i] != NULL) {
            free_tree(dict->hashtab[i]);
            dict->hashtab[i] = NULL;
        }
    }
}

/* exit_dictionary: save the current dictionary to a file, then free the dictionary */
void exit_dictionary(Dictionary *dict)
{
    save_dictionary(dict);
    free_dictionary(dict);
}
