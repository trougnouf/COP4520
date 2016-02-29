COP 4520 Programming Assignment 2

Title:		The SkipTrie implementation

Authors:	Brianne Nicole and Benoit Brummer


How to run this project:

	gcc -pthread -std=gnu11 handler.c testcase1.c skiplist.c && ./a.out
	
	This program requires a recent version of the GCC compiler (>=4.9)
	because it includes the C11 <stdatomic.h> library.



Original research paper URL:
	http://groups.csail.mit.edu/mag/oshman-shavit-podc13.pdf

Introduction:
	We implemented an elementary version of the SkipTrie, an algorithm
	that implements a search structure similar to the Y-fast trie. The
	Y-fast trie data structure consists of an X-fast trie (on top) linked to a series of balanced binary search trees (on the bottom). The SkipTrie replaces the balanced binary trees with a single skiplist.
	
	
	The bottom leaves of the x-fast trie connect to the top nodes of the skip list iff t


	
Data structures:
		Skip list:
		
		X-fast trie:
		

Implementation:
		Skip list:

		Hash table library

		Scheduler:
		
		
		Y-Fast Trie like combination:


Testing:
	Testing for the skiplist is done by modifying the preprocessor macros
	located in testcase1.h, namely: NUMTHREADS, NUMTASKS, PERCENTINSERT,
	PERCENTFIND, PERCENTREMOVE. In addition, the slLEVELS macro is located
	in skiplist.h.
	
	The results are shown below in a tabulated form. Testing was done on a 
	AMD Phenom II X4 980 processor with only four physical cores. This
	processor is only capable of running four concurrent threads, therefore
	the 8-threads results are irrelevant and will not be shown in the data
	analysis portion of testing.
	
	The number of tasks is consistent in all test cases: 500000 operations 
	times the number of threads.
	
	Insertion:

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


	Find: 

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


	Remove:
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


	20A-60F-20R:
	1	16	20	60	20	0.376779
	1	64	20	60	20	0.499462
	2	16	20	60	20	0.489676
	2	64	20	60	20	0.635469
	4	16	20	60	20	0.864115
	4	64	20	60	20	0.819840
	8	16	20	60	20	3.517270
	8	64	20	60	20	3.071949

	
Data analysis:
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

Raw data:
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
