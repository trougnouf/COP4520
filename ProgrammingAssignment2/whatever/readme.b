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



# atomic primitives: CAS and DCSS instructions
	
	
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



