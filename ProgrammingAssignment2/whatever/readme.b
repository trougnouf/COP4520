1. Intro


What is happening? ¯\(°_o)/¯

we may never know.



# the SkipTrie, a probabilistically-balanced data structure
	- wat
The SkipTrie is a probabilistically-balanced version of a y-fast trie consisting of a very shallow skiplist from which randomly chosen elements are inserted into a hash-table based x-fast trie.

- Skip list: https://en.wikipedia.org/wiki/Skip_list

- X-fast trie: https://en.wikipedia.org/wiki/X-fast_trie
	"largely as an introduction to y-fast tries"
- Y-fast trie: https://en.wikipedia.org/wiki/Y-fast_trie
	more complicated than x-fast

The main idea of the SkipTrie is to replace the y-fast trie’s balanced binary trees with a very shallow, truncated skiplist [18] of depth log log u.
// u is the size of the key space
// y-fast trie - balanced binary trees = x-fast trie



# atomic primitives: CAS and DCSS instructions

We probably don't care about this section until ProgrammingAssignment3
(assignment2 is sequential, ass#3 is concurrent)
	
- DCSS: double-wide double-compare-single-swap

param: (X, old_X, new_X, old_Y)
result:	if (X == old_X && Y == old_Y)	X = new_X

pseudocode shown uses DCSS


no DCSS hardware instruction 

does not exist per google. paper mentions DCAS  (double compare-and-swap) once:
	"The list maintains both next and prev pointers in each node, but since we do not use a double-compare-and-swap (DCAS), we cannot update both pointers at the same time."
	
DCAS has its own wikipedia page:
  https://en.wikipedia.org/wiki/Double_compare-and-swap
  "Compares two unrelated memory locations with two expected values, and if they're equal, sets both locations to new values. "
  adds newY. No HW instruction either

- CAS: single-word compare-and-swap
supported in hardware
	<stdatomic.h>

"some or all DCSS instructions are replaced with CAS
(by dropping the second guard), the implementation remains
linearizable and lock-free."

"We believe that our explicit use of DCSS captures some de-
sign patterns that are implicit in, e.g., [8], and other lock-free
data structures built from CAS alone"
[8]: http://www.cse.yorku.ca/~ruppert/papers/lfll.pdf



# Insertion

Each key inserted
into the SkipTrie is first inserted into the skiplist; initially
it rises in the usual manner, starting at the bottom level
and tossing a fair coin to determine at each level whether
to continue on to the next level. If a key rises to the top
of the truncated skiplist (i.e., to height log log u), we insert
it into the x-fast trie



# Deletion

To delete a key, we first delete
it from the skiplist, and if it was a top-level node, we also
remove it from the x-fast trie.



# Data structures:
- concurrent hash table
	split-ordered hashing [19]
- concurrent skiplist
- doubly-linked list
- concurrent x-fast trie
	// "has never been implemented concurrently before"
	
	
	
2. Skiplist's brief overview


