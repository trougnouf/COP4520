#include "xtrie.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>

//#define XSIZE 16
//#define BASE 0
//#define RIGHT 1

/**initialize xnode trie**/
x_node *initialize_trie(){
	
	//initialize the root of the x-trie.  
    x_node *root = NULL;
    root = (x_node*)malloc(sizeof(x_node));
    root->key = BASE;							//set key = 0, ** change to 1??
    root->pointers[BASE] = NULL;				// set left/predecessor pointer to null
	root->pointers[RIGHT] = NULL;				//set right/successor pointer to null
    root->top = NULL;	
    return root;

}

/**Initialize the hash table for each level of the skip trie. LSS = Level Search Structure
	TODO: change constant 10 to defined max key size
**/
level_hash *hash_init(x_node *root, slNode *head){
	
	int i;
	x_node *tmp = NULL;
	level_hash *lss;
	lss = malloc(sizeof(level_hash));

	for(i=XSIZE; i>=0; i--){
		
		lss->lvl[i] = (x_node**)malloc((1<<(XSIZE-i))*sizeof(x_node*));
	}

	if(!atomic_compare_exchange_strong(&lss->lvl[XSIZE][BASE], &tmp, root)){
		//printf("ERROR: hashing root failed\n");
	}

	insert_x_trie(BASE, lss, head);
	insert_x_trie(MAXKEY, lss, (slNode*)head->next[BASE]);
	root->pointers[BASE] = lss->lvl[BASE][BASE];
	root->pointers[RIGHT] = lss->lvl[BASE][MAXKEY];

	return lss;
}

/**Find if key value is in the hash table at a specific level in the level search structure (LSS).
Returns null if not found.
*/
x_node *find_key(uint32_t key, int level, level_hash *lss){

    x_node *tmp = lss->lvl[level][key];

    return tmp;

}

/**Attempt to insert into hashtable atomically
**/
x_node *hash_insert_leaf(uint32_t key, level_hash *lss, slNode *topLevelN){
	
	x_node *tmp = NULL;
	if(atomic_compare_exchange_strong(&lss->lvl[BASE][key], &tmp, createLeaf(key, topLevelN,lss))){

		tmp = lss->lvl[BASE][key];		
	}
	else
		tmp = NULL;

	return tmp;
}

/**create leaf node to be added atomically to hashtable when comparison is done
	TODO: search for predecessor/successor using top level node
**/
x_node *createLeaf(uint32_t key, slNode *topLevelN, level_hash *lss){

	x_node *leaf = (x_node*)malloc(sizeof(x_node));
	leaf->key = key;
	leaf->top = topLevelN;
	leaf->pointers[BASE]==NULL;
	leaf->pointers[RIGHT] = NULL;
	if(topLevelN->previous!=NULL){

		leaf->pointers[BASE] = find_key(topLevelN->previous->key, BASE, lss); //find key associated with bottom level top level skip list node set as previous for topLevelNode

		if(leaf->pointers[BASE]!=NULL){  		
			leaf->pointers[RIGHT] == leaf->pointers[BASE]->pointers[RIGHT]; //swap atomically
			leaf->pointers[BASE]->pointers[RIGHT] = leaf;
			
			if(leaf->pointers[RIGHT] != NULL){							//
				leaf->pointers[RIGHT]->pointers[BASE] = leaf;
			}
		}
		
	}

	//TODO:need to implement topLevelN->next to set right pointer of leaf
	
	
	return leaf;
}

x_node *createPrefix(uint32_t key, x_node *leaf, int direction){ 			//creates a prefix node to point to top level skip list nodes
	
	x_node *preNode = (x_node*)malloc(sizeof(x_node));
	preNode->key = key;
	preNode->pointers[direction] = leaf;				// set pointer in direction of key for newly created prefix
	preNode->pointers[!direction] = NULL;		
	preNode->top = NULL;

	return preNode;
}

/**compare the node in the hashtable to the current prefix node.  If they are the same remove the key from the hashtable**/
void hash_delete(uint32_t key, int level, level_hash *lss, x_node *prefixN){
	
	if(atomic_compare_exchange_strong(&lss->lvl[level][key], &prefixN, NULL)){
		//printf("prefix %u on level %d has been deleted\n", key, level);
	}
	
	else{
		//printf("key not found %u\n", key);
	}
}

/**Finds lowest ancestor for a key value, which is the node in the xtrie that has the longest common prefix as our search key.  
	If the longest common ancestor has the same key as our search node, the ancestor node is returned.  Otherwise, the child
	node in the opposite direction.
**/
x_node *find_ancestor(uint32_t key, level_hash *lss){

    x_node *ancestor = NULL;
    x_node *canidate = NULL;

    int min = BASE;
	int max = XSIZE;
	int size = XSIZE/2;
	int direction = BASE;

    /**loop over possible levels until bottom most possible level is reached**/
    while(max>=min){
		
		uint32_t prefix = key>>(XSIZE-size); //get the prefix for the current search window

		canidate = find_key(prefix, XSIZE-size, lss); //see if the  prefix is in the hashtable

		if(canidate != NULL){ //if the key is found in the hashtable
			
			ancestor = canidate; //set our newly found node to the new ancestor node
			direction = prefix&RIGHT; //get the direction our next prefix would be in
			min = size + RIGHT; //decrease size of current search window by 1
		}
		else{
			max = size - RIGHT; //increase size of current search window by 1
		}

		size = (max+min)/2; //reduce size of search window
    }

	if(ancestor->key == key){
		return ancestor; //lowest ancestor has same key as our search key, so return this node
	}
	else{
		
		x_node *lowestA = ancestor->pointers[!direction]; //get the child node on the opposite branch of our ancestor node.  This will be either the successor or predecessor of our search key
		return lowestA;
	}

}

/**
	Finds the predecessor of our search key based on the value returned by the lowest ancestor function.  
	Since value could actually be successor or a marked key, traverse to find true predecessor
	TODO : add traversal over ??back?? pointer if predecessor is marked for deletion
**/
x_node *find_xFastTriePred(uint32_t key, level_hash *lss){

	x_node *tmp = find_ancestor(key,lss); //find node with the longest common prefix in the x-trie 
	
	while(tmp->key > key){ //if the node returned is a successor instead of a predecessor
		
		tmp = tmp->pointers[BASE]; //traverse predecessor pointers until a lower key is found
	}
}

/**Insert key value into x-trie structure and then add hash to level search structure if needed.
    Each LSS[level] has a matching hashtable called hh[level]

	TODO: 
	-Use compare and swap for setting predecessor node?  Paper uses DCSS
**/
void insert_x_trie(uint32_t key, level_hash *lss , slNode *topNode){

	x_node *leaf = hash_insert_leaf(key, lss, topNode); //create hash node, insert into hashtable if key is not yet added
	x_node *tmp;

	if(leaf==NULL){ //if the key is already in the hashtable, return without inserting
		return;
	}
	
	//tmp value used to store most recently added/checked node to hashtable 
	tmp = leaf;
	uint32_t tmpKey = key;


	int keySize;
	//loop over prefixes of key for insertion
	for(keySize = XSIZE - RIGHT; keySize>= BASE; keySize--){
		
		uint32_t prefix = tmpKey>>RIGHT; //the key to the node one level above current node
		int direction = tmpKey&RIGHT; //direction of current node from prefix node

		x_node *preNode = find_key(prefix,XSIZE-keySize, lss); //check to find it prefix node is already in hashtable
		
		if(preNode == NULL){ //if the prefix hasn't been added to the hashtable yet

			preNode = NULL; //make sure its not the same null as that of the hashtable??
			if(atomic_compare_exchange_strong(&lss->lvl[XSIZE-keySize][prefix], &preNode, createPrefix(prefix, leaf, direction))){
				
				return;			// returns if the node was created and succesfully inserted into the hashtable.
			}

		}

		if(preNode->pointers[RIGHT]==NULL && preNode->pointers[BASE] == NULL && keySize!=XSIZE){ //if a prefixes nodes are both null and it has no children and is not the root, try to safely remove it from the table

				if(atomic_compare_exchange_strong(&lss->lvl[XSIZE-keySize][prefix], &preNode, NULL)){ //if the node's key was succesfully removed from the hashtable
					free(preNode); 
					keySize++; //Redo this level again
					break;
				}
		}

		if(direction == BASE && (preNode->pointers[BASE] == NULL || preNode->pointers[BASE]->key < leaf->key)){ //if the key is in the prefix node's left tree, and the tree is either null, or has a smaller value than leaf
			
			preNode->pointers[BASE] == leaf; //set left pointer to leaf
		}
		
		//reverse of above
		else if(direction == RIGHT && (preNode->pointers[RIGHT] == NULL || preNode->pointers[RIGHT]->key < leaf->key)){
			
			preNode->pointers[RIGHT] = leaf; //set succcessor to our leaf node
		}
		

		//Now that we have assured our prefix node and leaf node point into our doubley linked list properly 
		//set value of our prefix node's pointer in direction of most recent prefix to the prefix on the level below (this will be our leaf for the first iteration, and prefix nodes afterward)
		//preNode->pointers[direction] = tmp; 
		//tmpKey = prefix; 
		//tmp = preNode;	//change tmpkey and pointer to the prefix node

		
	}

}

/**delete a key from the x-trie
	TODO: 
	SIDE THOUGHT: should there be a check to make sure the key values associated with head and tail of skip list aren't deleted?
**/
void delete_x_trie(uint32_t key, level_hash *lss){


	
	x_node *leaf = find_key(key, BASE, lss);

	if(leaf == NULL){
		return;
	}

	hash_delete(key, BASE, lss, leaf);//remove key from bottom level of hash table.  Leaf node still exists in memory
	
	//adjust leaf's predecessor and successor to point to eachother
	x_node *pred = leaf->pointers[BASE];
	if(atomic_compare_exchange_strong(&pred->pointers[RIGHT], &leaf, leaf->pointers[RIGHT])){
		pred->pointers[RIGHT]->pointers[BASE] = pred;
	}
	else{
		//printf("predecessor does not point to leaf %u??\n", leaf->key);
		//leaf->pointers[BASE] = fixPrev(leaf);
		//pred = leaf->pointers[BASE];
		//if(atomic_compare_exchange_strong(&pred->pointers[RIGHT], &leaf)){
			//pred->pointers[RIGHT]->pointers[BASE] = pred;
			//printf("second try successs\n");
		}
		//else{
			//printf("second try fail\n");
		//}
	

	//leaf->pointers[BASE] = NULL;
	//leaf->pointers[RIGHT] = NULL;
	
	x_node *tmp = leaf;
	uint32_t tmpKey = key;
	int keySize;
	
	//loop through our key's prefixes starting from the top down (root to doubley linked list)
	for(keySize = BASE; keySize<(XSIZE-RIGHT); keySize++){
		
		//get prefix for key at current level as well as the direction our leaf node would be in as its child
		uint32_t prefix = tmpKey>>(XSIZE-keySize);
		int direction = (tmpKey>>(XSIZE-keySize+RIGHT)&RIGHT);
		
		//make sure prefix is still in hashtable
		x_node *preNode = find_key(prefix, XSIZE-keySize, lss);
		
		//if our leaf node is a direct descendent of the prefix node, set it's pointer to leaf to null
		if(preNode->pointers[direction] == leaf){

			preNode->pointers[direction] = NULL; /**TODO: Change to atomic**/
		}
		
		//if our prefix node has no descendants, remove if from the hashtable completely and free the node
		if(preNode->pointers[direction] == NULL && preNode->pointers[!direction] ==NULL){
			
			hash_delete(prefix, XSIZE-keySize, lss, preNode);
			//free(preNode);
		}
	}

	
	//after our leaf node has been successfully removed from all levels of the x-trie, free the leaf node
	//free(leaf);
}
