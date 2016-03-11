#include "xtrie.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//#define XSIZE 16
//#define BASE 0
//#define RIGHT 1

/**initialize xnode trie**/
x_node *initialize_trie(){

    x_node *root = NULL;
    root = (x_node*)malloc(sizeof(x_node));
    root->key = BASE;
    root->pointers[BASE] = NULL;
	root->pointers[RIGHT] = NULL;
    root->top = NULL;
    return root;

}

void initialize_hash(x_node **LSS, x_node *root){

	int i;
	for(i=BASE; i<=XSIZE; i++){
		LSS[i] = NULL;
	}

	HASH_ADD(hh[XSIZE], LSS[XSIZE], key, sizeof(uint32_t), root);
}

/**Find if key value is in the hash table at a specific level in the level search structure (LSS).
Returns null if not found.*/
x_node *find_key(uint32_t key, int level, x_node **LSS){

    x_node *tmp = NULL;
    HASH_FIND(hh[level], LSS[level], &key, sizeof(uint32_t),tmp);

    return tmp;

}

/**Find lowest ancestor for a key value**/
x_node *find_ancestor(uint32_t key, x_node **LSS){

    x_node *ancestor = NULL;
    x_node *canidate = NULL;

    int min = BASE;
	int max = XSIZE;
	int size = XSIZE/2;
	int direction = BASE;

    /**loop over possible levels until bottom most possible level is reached**/
    while(max>=min){
		
		uint32_t prefix = key>>(XSIZE-size);

		canidate = find_key(prefix, XSIZE-size, LSS);

		if(canidate != NULL){
			
			ancestor = canidate;
			direction = prefix&1;
			min = size + RIGHT;
		}
		else{
			max = size - RIGHT;
		}

		size = (max+min)/2;
    }

	if(ancestor->key == key){
		return ancestor;
	}
	else{
		
		x_node *predecessor = ancestor->pointers[!direction];
		return predecessor;
	}

}

/**
	find lowest ancestor of key.  Since value could actually be successor or a marked key, traverse to find true predecessor
	TODO : add traversal over ??back?? pointer if predecessor is marked for deletion
**/
x_node *find_xFastTriePred(uint32_t key, x_node **LSS){

	x_node *tmp = find_ancestor(key,LSS);
	
	while(tmp->key > key){
		
		tmp = tmp->pointers[BASE];
	}
}

/**Insert key value into x-trie structure and then add hash to level search structure if needed.
    Each LSS[level] has a matching hashtable called hh[level]

	TODO: have function check for predecessor before adding extra nodes?
**/
void insert_x_trie(uint32_t key, x_node **LSS, slNode *topNode){

	x_node *leaf;
	x_node *tmp;

    //check if key is already in hashtable
    tmp = find_key(key, BASE, LSS);

	if(tmp!=NULL){
		return;
	}

	leaf = (x_node*)malloc(sizeof(x_node));
	leaf->key = key;
	leaf->pointers[BASE] = NULL;
	leaf->pointers[RIGHT] = NULL;
	leaf->top = NULL; //set equal to skiplist node
	tmp = leaf;
	uint32_t tmpKey = key;

	HASH_ADD(hh[BASE], LSS[BASE], key, sizeof(uint32_t), leaf);

	int keySize;
	for(keySize = XSIZE - RIGHT; keySize>= BASE; keySize--){
		
		uint32_t prefix = tmpKey>>RIGHT;
		int direction = tmpKey&RIGHT;

		x_node *preNode = find_key(prefix, XSIZE-keySize, LSS);
		
		if(preNode == NULL){
			
			preNode = (x_node*)malloc(sizeof(x_node));
			preNode->key = prefix;
			preNode->pointers[direction] = leaf;
			preNode->pointers[!direction] = leaf;
			preNode->top = NULL;
			HASH_ADD(hh[XSIZE-keySize], LSS[XSIZE-keySize], key, sizeof(uint32_t), preNode);
		}

		if(preNode->pointers[direction]!=NULL && preNode->pointers[direction]!=leaf && leaf->pointers[!direction] == NULL){
			
			leaf->pointers[!direction] = preNode->pointers[direction];
			leaf->pointers[direction] = leaf->pointers[!direction]->pointers[direction];
			if(leaf->pointers[direction]!= NULL){
				
				leaf->pointers[direction]->pointers[!direction] = leaf;
			}
		}

		if(preNode->pointers[BASE] == NULL || preNode->pointers[BASE]->key > leaf->key){
			
			preNode->pointers[BASE] == leaf;
		}

		if(preNode->pointers[RIGHT] == NULL || preNode->pointers[RIGHT]->key < leaf->key){
			
			preNode->pointers[RIGHT] = leaf;
		}

		preNode->pointers[direction] = tmp;
		tmpKey = prefix;
		tmp = preNode;

		
	}

}

void delete_x_trie(uint32_t key, x_node **LSS, x_node *leaf){
	

	HASH_DELETE(hh[BASE], LSS[BASE], leaf);

	x_node *pred = leaf->pointers[BASE];
	x_node *succ = leaf->pointers[RIGHT];
	pred->pointers[RIGHT] = succ;
	succ->pointers[BASE] = pred;

	//leaf->pointers[BASE] = NULL;
	//leaf->pointers[RIGHT] = NULL;
	
	x_node *tmp = leaf;
	uint32_t tmpKey = key;
	int keySize;

	for(keySize = BASE; keySize<=(XSIZE-RIGHT); keySize++){

		uint32_t prefix = tmpKey>>(XSIZE-keySize);
		int direction = (tmpKey>>(XSIZE-keySize+RIGHT)&RIGHT);

		x_node *preNode = find_key(prefix, XSIZE-keySize, LSS);

		if(preNode->pointers[direction] == leaf){

			preNode->pointers[direction] = NULL;
		}
		
		if(preNode->pointers[direction] == NULL && preNode->pointers[!direction] ==NULL){
			
			HASH_DELETE(hh[XSIZE-keySize], LSS[XSIZE-keySize], preNode);
			//free(preNode);
		}
	}

	//free(leaf);
}

/**

This code goes in main function to initialize hashtable, level search structure, and x-trie

    //initialize level search structure
    x_node *LSS[XSIZE];

    //initialize all levels to NULL
    for(i=0;i<XSIZE;i++){

        LSS[i] = NULL;
    }

    //initialize x-trie root node
    x_node *root = NULL;

    root = initialize_trie();

    HASH_ADD(hh[XSIZE-1],LSS[XSIZE-1],key,sizeof(int),root);
**/
