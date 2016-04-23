#ifndef XTRIE_H
#define XTRIE_H

#include "skiplist.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

#define XSIZE 16
#define BASE 0
#define RIGHT 1

/**struct for each node in x-trie.
    Add back pointer to point to parent node to help predecessor/successor function??**/
typedef struct x_trie_node {
    uint32_t key;
    struct x_trie_node *pointers[2];
    struct slNode_ *top;
	atomic_int ready;
	atomic_int marked;
} x_node;

typedef struct _level_search_sruct{
	
	struct x_trie_node **lvl[XSIZE+RIGHT];

} level_hash;


x_node *initialize_trie();

level_hash *hash_init(x_node *root, slNode *head);

x_node *find_key(uint32_t key, int level, level_hash *lss);

x_node *hash_insert_leaf(uint32_t key, level_hash *lss, slNode *topLevelN);

x_node *createLeaf(uint32_t key, slNode *topLevelN, level_hash *lss);

x_node *createPrefix(uint32_t key, x_node *leaf, int direction);

void hash_delete(uint32_t key, int level, level_hash *lss, x_node *prefixN);

x_node *find_ancestor(uint32_t key, level_hash *lss);

x_node *find_xFastTriePred(uint32_t key, level_hash *lss);

void insert_x_trie(uint32_t key, level_hash *lss , slNode *topNode);

void delete_x_trie(uint32_t key, level_hash *lss);

#endif
