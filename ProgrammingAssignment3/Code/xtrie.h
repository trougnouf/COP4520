#ifndef XTRIE_H
#define XTRIE_H

#include "skiplist.h"
#include "uthash.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define XSIZE 16
#define BASE 0
#define RIGHT 1

/**struct for each node in x-trie.
    Add back pointer to point to parent node to help predecessor/successor function??**/
typedef struct x_trie_node {
    uint32_t key;
    struct x_trie_node *pointers[2];
    struct slNode_ *top;
    UT_hash_handle hh[XSIZE+1];
} x_node;

x_node *initialize_trie();

void initialize_hash(x_node **LSS, x_node *root);

x_node *find_key(uint32_t key, int level, x_node **LSS);

x_node *find_ancestor(uint32_t key, x_node **LSS);

x_node *find_xFastTriePred(uint32_t key, x_node **LSS);

void insert_x_trie(uint32_t key, x_node **LSS, slNode *topNode);

void delete_x_trie(uint32_t key,x_node **LSS, x_node *leaf);

#endif
