#include "uthash.h"
#include <stdlib.h>
#include <stdio.h>

#define XSIZE 9
#define MID 4
#define BASE 0

/**struct for each node in x-trie.
    Add back pointer to point to parent node to help predecessor/successor function??**/
typedef struct x_trie_node {
    int key;
    struct x_trie_node *left;
    struct x_trie_node *right;
    struct x_trie_node *back;
    UT_hash_handle hh[XSIZE];
} x_node;

/**initialize xnode trie**/
x_node *initialize_trie(){

    x_node *root = NULL;
    root = (x_node*)malloc(sizeof(x_node));
    root->key = 0;
    root->left = NULL;
    root->right = NULL;
    root->back = NULL;
    return root;

}

/**Find if key value is in the hash table at a specific level in the level search structure (LSS).
Returns null if not found.*/
x_node *find_key(int key, int level, x_node **LSS){

    x_node *tmp = NULL;
    HASH_FIND(hh[level], LSS[level], &key, sizeof(int),tmp);

    if(tmp != NULL){
        printf("key found\n");
    }
    else{
        printf("key not found\n");
    }

    return tmp;

}

/**Find lowest ancestor for a key value**/
void find_ancestor(int key, int level, x_node **LSS){

    x_node *ancestor = NULL;
    x_node *canidate = NULL;

    int min = 0;

    /**loop over possible levels until bottom most possible level is reached**/
    while(level>min){

        /**get prefix of key for a level, then see if the prefix is in the hashtable**/
        int lvl_key = key>>level;
        canidate = find_key(lvl_key, level, LSS);

        /**if prefix is in table, change ancestor to node found with that prefix.
        Check lower levels for longer possible prefixes **/
        if(canidate != NULL){
            ancestor = canidate;
            level = level/2;
        }
        /**prefix not found.  Lowest common ancestor must be higher up in table.  Change the minimum possible level
            to the current level and proceed higher in the trie**/
        else{
            printf("check higher. current level: %d\n", level);
            min = level;
            level = level + (level/2);

            /**If the highest possible level is reached, exit the loop**/
            if(level >= XSIZE){
                level = min;
            }
        }

    }

    /**ancestor now holds lowest common ancestor**/
    if(ancestor != NULL){
        printf("lowest ancestor: %d\n ", ancestor->key);
    }
    /**Not sure what to do if no lowest common ancestor is found**/
    else{
        printf("prefix not in tree\n");
    }

}

/**Insert key value into x-trie structure and then add hash to level search structure if needed.
    Each LSS[level] has a matching hashtable called hh[level]

    TODO: add predecessor/successor check??
**/
void insert_x_trie(int key, int level, x_node **LSS, x_node *leaf){

    /**get bit to determine direction for placement in x-trie (0 = left, 1 = right)**/
    int mask = 1<<level-1;
    int combine = key&mask;
    int direction = combine>>level-1;

    /**find prefix value for key at current level in LSS**/
    int tmpKey = key>>level-1;


    x_node *tmp = NULL;
    //printf("level: %d, direction: %d, tmp key: %d\n", level, direction, tmpKey);

    /**return once bottom level is reached.  Indicating key has been completely inserted.
    Possibly use this check to release hashtable lock????**/
    if(level == 0){
        return;
    }

    else{
        /**if next node is in left direction and a node for that prefix does not exist,
            create the x-node and add it to the LSS hashtable for that level.  Set the current node's
            left pointer to the newly created node.

            TODO: possibly add back pointer that will point newly created node to current node (parent node) in order
            to make predecessor/successor backtracking easier
        **/
        if(direction == 0 && leaf->left==NULL){

            tmp = (x_node*)malloc(sizeof(x_node));
            tmp-> key = tmpKey;
            tmp->left = NULL;
            tmp->right = NULL;
            tmp->back = leaf;
            leaf->left = tmp;
            //printf("add to hash level %d\n", level-1);
            HASH_ADD(hh[level-1],LSS[level-1], key, sizeof(int),tmp);

            insert_x_trie(key, level-1, LSS, tmp);
        }
        /**same as above check but in right direction**/
        else if(direction == 1 && leaf->right==NULL){
           // printf("insert right\n");
            tmp = (x_node*)malloc(sizeof(x_node));
            tmp->key = tmpKey;
            tmp->left = NULL;
            tmp->right = NULL;
            tmp->back = leaf;
            leaf->right = tmp;
            //printf("add to hash level %d\n", level-1);
            HASH_ADD(hh[level-1],LSS[level-1], key, sizeof(int),tmp);
            insert_x_trie(key, level-1, LSS, tmp);
        }
        /**A prefix node already exists in the direction we are taking**/
        else{
            if(direction == 0){
                insert_x_trie(key, level-1, LSS, leaf->left);
            }
            else{
                insert_x_trie(key, level-1, LSS, leaf->right);
            }

        }


    }

}

void delete_x_trie(int key, int level, x_node **LSS, x_node *leaf){

    int mask = 1<<level-1;
    int combine = key & mask;
    int direction = combine>>level-1;

    x_node *tmp = leaf->back;

    /**  If in LSS[0]
    TODO: at root level, connect pred(prev?) and succ before deletion**/
    if(level == 0){

        HASH_DELETE(hh[level], LSS[level], leaf);
        free(leaf);
        delete_x_trie(key, level+1, LSS, tmp);
    }

    else if(level == XSIZE-1){
        return;
    }

    else{

        if(direction == 0){

            leaf->left = NULL;

            /**if prefix has another child, do not delete prefix**/
            if(leaf->right != NULL){

                return;
            }
            else{

                tmp = leaf->back;
                HASH_DELETE(hh[level],LSS[level], leaf);
                free(leaf);
                delete_x_trie(key,level+1, LSS, tmp);
            }
        }
        else{

            leaf->right = NULL;

            if(leaf->left != NULL){
                return;
            }
            else{
                tmp = leaf->back;
                HASH_DELETE(hh[level],LSS[level],leaf);
                free(leaf);
                delete_x_trie(key,level+1,LSS,tmp);
            }
        }
    }

}

int main(int argc,char *argv[]){

    //keys for testing
    int key1 = 5;   //00000101
    int key2 = 50;  //00110010
    int key3 = 4;   //00000100
    int key4 = 231; //11100111

    int i,j;

    x_node *tmp;

    //initialize level search structure
    x_node *LSS[XSIZE];

    //initialize all levels to NULL
    for(i=0;i<XSIZE;i++){

        LSS[i] = NULL;
    }

    //initialize x-trie root node
    x_node *root = NULL;

    root = initialize_trie();
   /* root = (x_node*)malloc(sizeof(x_node));
    root->key = 0;
    root->left = NULL;
    root->right = NULL;
    root->back = NULL;*/
    HASH_ADD(hh[XSIZE-1],LSS[XSIZE-1],key,sizeof(int),root);

    //test inserts
    insert_x_trie(key2, XSIZE-1, LSS, root);
    insert_x_trie(key1,XSIZE-1,LSS,root);
    insert_x_trie(key3,XSIZE-1,LSS,root);
    insert_x_trie(key4,XSIZE-1,LSS,root);

    //test find ancestor
    //find_ancestor(key3,MID,LSS);
   // find_ancestor(key4,MID,LSS);

//test code for printing values in hash table
    x_node *tstroot = NULL;
   for(j=XSIZE-2;j>=0;j--){

        printf("hashes as LSS level %d\n", j);
       for(tstroot=LSS[j]; tstroot != NULL; tstroot=(x_node*)(tstroot->hh[j].next)) {
        printf("key %d, ", tstroot->key);
        }
        printf("\n");

   }
/*
   x_node *tstDelete = NULL;
   tstDelete = find_key(key4,BASE,LSS);
   delete_x_trie(key4,BASE,LSS,tstDelete);

   for(j=XSIZE-2;j>=0;j--){

        printf("hashes as LSS level %d\n", j);
       for(tstroot=LSS[j]; tstroot != NULL; tstroot=(x_node*)(tstroot->hh[j].next)) {
        printf("key %d, ", tstroot->key);
        }
        printf("\n");

   }*/

    system("PAUSE");
    return 0;
}
