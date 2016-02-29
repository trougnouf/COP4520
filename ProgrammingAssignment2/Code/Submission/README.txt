\documentclass{COP 4520 programming assignment}
\title{The SkipTrie: Low-Depth Concurrent Search without Rebalancing}
\author{Brianne Canonico and Benoit Brummer}
\date{February 2016}

\begin{document}
\maketitle

\section{How to run this project}

	gcc -pthread -std=gnu11 handler.c testcase1.c skiplist.c && ./a.out

This program requires a recent version of the GCC compiler (>=4.9) because it includes the C11 <stdatomic.h> library.

Different test case macros can be set in testcase1.h. The number of skip list levels can be set in skiplist.h

\section{Introduction}
	We implemented an elementary version of the SkipTrie, an algorithm that implements a search structure similar to the Y-fast trie. The Y-fast trie data structure consists of an X-fast trie (on top) linked to a series of balanced binary search trees (on the bottom). The SkipTrie replaces the balanced binary trees with a single skiplist.
        
\section{Data structures}\label{data structures}

\subsecton( Skip list)
	Structure: The Skiplist data structure consists of a key, an optional data pointer (if the data is not the key), and a double pointer to the next node in each level of the skiplist. There is an arbitrary number of levels in the skip list (although the author suggests using log log u, or 4). The bottom (first) level of the skip list contains every key in the list, and each subsequent level contains half the number of keys present in its predecessor.
	
\subsubsection{Search:}
	A search begins at the top of the top-left of the skip list (root) and continues to the right until either a NULL pointer or a key higher than the desired one is found. At that point, the search moves to the level below, which contains approximately twice as many keys. This process repeats until the desired key is found, or the bottom is reached and the next key is higher than desired. This method allows us to skip through most keys and results in a O(log u) search time, which remains relatively fast regardless of the amount of data stored in the structure as long as an appropriately high enough number of levels is chosen. The author suggest using 4 levels (log log u) but we found that performance is best using at least 16. The path looks like a set of uneven stairs going from the top-left to the bottom-right.
	
\subsubsection{Insert:}
	Inserting a key into the skip list starts out the same way as a search, but the last node for every level is saved before descending. When the bottom is reached and the correct location is found, a new node is created, its next pointer is set to the previous node’s next pointer, and the previous node’s next pointer is set to the new node. A “coin” is tossed to determine whether the node should be inserted into the next level up, and this process is repeated on every level (until the coin returns a negative value) using the list of nodes saved while descending the list. This implementation uses a single random number (rand()) to generate all the necessary coin tosses (up to 32) using each of the single random number’s bits as a coin. The insert function returns a flag if the top-level is reached, in which case the value is to be added into the x-fast trie as well.

\subsubsection{Remove:}
	To remove from the skip list, we perform a search for the undesirable key. Once the key is found, the previous node’s next pointer is set to the undesirable node’s next pointer, and this process is repeated on for every level below. Once the pointers have been appropriately set, the memory is freed using the free() function.
        
\subsection{ X-fast trie:}  
	The x-fast trie structure is composed of a bitwise trie and a series of hashtables called the Level Search Structure (LSS).  The top most nodes of the skiplist are inserted into the trie at the bottom level and their keys are placed in the hash table associated with  LSS[0].  The skiplist nodes are linked to a predecessor and successor, also in the bottom level of the x-fast trie, to form a doubly linked list.  Each level of the trie stores the bit prefix of the skiplist node’s key (with the smallest prefix - length one - being closest to the root, and the longest - the entire key itself - at the base).  The prefixes are stored in the hash table of the LSS at the same level (such that a prefix at height three is in the hashtable of LSS[3]). 

\subsubsection{X-fast trie operations:}
	Insertion into the x-fast trie only occurs when a node inserted into the skip list reaches the top level.  The prefix nodes are only added if a previous key value has yet to insert the prefix into the trie and hash table.  Deletion occurs in response to a deletion of a top level node from the skip list.  During a find operation, the x-fast trie uses a binary search method to sort through the prefixes in the trie.  If the prefix is not found at the current level, then the lowest common ancestor is stored higher in the trie, and the search moves to the top half of the search window.  If the prefix is found, the search moves to the bottom half of the search window.  The searches continue until the longest common ancestor (the node with the longest common prefix with that of our key we use to search).  This type of search is used specifically to find the lowest common ancestor for a key.  Searching to see if a node is already in the trie is done simply by searching for the key in the bottom level hashtable, LSS[0].
    
\subsection{Hash Table:}
	The hash table is implemented using the outside library ‘uthash.h’ in order to hash the prefix nodes to the correct level.  This greatly reduces the time of checking to see if a key is already in the trie, or if its prefix is in the trie, to O(1) time.

\subsection{Scheduler}
	The original scheduler had the program’s main function pass a status ENUM to each thread inside of the shared data structure. This implementation proved to be horrendously inefficient as tests showed that the threads were spending most of their time idling. Scheduling was moved inside of the threads instead. A common test case array is used to store all of the randomly generated test cases (state and key), and each thread pulls a struct from that array as soon as it starts a new cycle. An atomic variable integer (from <stdatomic.h>) is used to ensure that all threads can share the same work load. The atomic_fetch_add function is used to get the atomically get the current array index and increment it by one (tsk = atomic_fetch_add(sharedData->tsknum, 1);)



\section{Testing}
	Testing for the skiplist is done by modifying the preprocessor macros located in testcase1.h, namely: NUMTHREADS, NUMTASKS, PERCENTINSERT, PERCENTFIND, PERCENTREMOVE. In addition, the slLEVELS macro is located in skiplist.h.
    
    The results are shown below in a tabulated form. Testing was done on a AMD Phenom II X4 980 processor with only four physical cores. This processor is only capable of running four concurrent threads, therefore the 8-threads results are irrelevant and will not be shown in the data analysis portion of testing.

    The number of tasks is consistent in all test cases: 500000 operations times the number of threads.
    
\subsection{Insertion}   
	Threads	Levels	%Ins	%Find	%Remove	Time (s)
	-------------------------------------------------
	1	16	80	10	10	0.655403
	1	32	80	10	10	0.718322
	-------------------------------------------------
	2	8	80	10	10	30.255755
	2	16	80	10	10	0.943954
	2	32	80	10	10	0.945865
	-------------------------------------------------
	4	16	80	10	10	1.491076
	4	32	80	10	10	1.456308


\subsection{Find}

	Threads	Levels	%Ins	%Find	%Remove	Time (s)
	-------------------------------------------------
	1	16	10	80	10	0.250450
	1	32	10	80	10	0.303515
	------------------------------------------------
	2	4	10	80	10	68.928917
	2	8	10	80	10	1.930917
	2	16	10	80	10	0.364168
	2	32	10	80	10	0.404995
	------------------------------------------------
	4	16	10	80	10	0.600258
	4	32	10	80	10	0.566595


\subsection{Remove}
	1	16	10	10	80	0.249403
	1	32	10	10	80	0.289373
	------------------------------------------------
	2	4	10	10	80	69.872426
	2	8	10	10	80	1.958623
	2	16	10	10	80	0.365717
	2	32	10	10	80	0.397467
	------------------------------------------------
	4	16	10	10	80	0.537178
	4	32	10	10	80	0.577230


\subsection{20A-60F-20R}
	1	16	20	60	20	0.376779
	1	64	20	60	20	0.499462
	2	16	20	60	20	0.489676
	2	64	20	60	20	0.635469
	4	16	20	60	20	0.864115
	4	64	20	60	20	0.819840
	8	16	20	60	20	3.517270
	8	64	20	60	20	3.071949

	
\section{Data analysis:}
    While the author suggests using log log u skip list levels (where u is
    the key universe, in this case u=2^30 and log log u <= 4), our results
    show that this figure is far from optimal. Some of our tests could
    barely run through completion using four levels, taking longer to
    execute as the skip list size increased because all of the levels were
    quickly being filled up and all functions had to go through virtually
    all nodes instead of skipping most as intended by the skip list
    structure. Using 16 levels proved to be optimal in all cases, with
    minimal performance change as the number of levels increased. The
    performance hit is not significant (+/- 11% at most). We notice that
    multiple threads can benefit from using an increased (sometimes obscene)
    number of levels, while running our program with a low thread number
    (1 or 2)

\section{Raw data:}
    Raw data from the skip list testing. All test cases are done with
    500000*Threads operations. 8-Threads results are irrelevant because
    testing was done on a 4-thread platform.
	
	Threads	Levels	%Ins	%Find	%Remove	Time (s)
	4	16	34	33	33	0.995659
	1	16	34	33	33	0.456966
	2	16	34	33	33	0.668343
	8	16	34	33	33	3.925073
	8	8	34	33	33	163.963169
	4	8	34	33	33	28.030096
	2	8	34	33	33	8.421152
	1	8	34	33	33	3.237801
	1	4	34	33	33	125.281050
	1	32	34	33	33	0.486938
	2	32	34	33	33	0.703730
	4	32	34	33	33	0.917991
	8	32	34	33	33	3.547862
	8	64	34	33	33	3.793403
	8	64	20	60	20	3.071949
	4	64	20	60	20	0.819840
	2	64	20	60	20	0.635469
	1	64	20	60	20	0.499462
	1	16	20	60	20	0.376779
	2	16	20	60	20	0.489676
	4	16	20	60	20	0.864115
	8	16	20	60	20	3.517270
	8	16	80	10	10	4.853561
	4	16	80	10	10	1.491076
	2	16	80	10	10	0.943954
	1	16	80	10	10	0.655403
	1	32	80	10	10	0.718322
	2	32	80	10	10	0.945865
	4	32	80	10	10	1.456308
	8	32	80	10	10	4.552659
	8	32	10	80	10	2.342136
	4	32	10	80	10	0.566595
	2	32	10	80	10	0.404995
	1	32	10	80	10	0.303515
	1	16	10	80	10	0.250450
	2	16	10	80	10	0.364168
	4	16	10	80	10	0.600258
	8	16	10	80	10	2.080276
	8	16	10	10	80	2.240176
	4	16	10	10	80	0.537178
	2	16	10	10	80	0.365717
	1	16	10	10	80	0.249403
	1	32	10	10	80	0.289373
	2	32	10	10	80	0.397467
	4	32	10	10	80	0.577230
	8	32	10	10	80	2.302058

\section{Bibliography}
http://groups.csail.mit.edu/mag/oshman-shavit-podc13.pdf
https://troydhanson.github.io/uthash/

\end{document}



