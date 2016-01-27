# Problem 1:



* Compilation and Execution *

Execute and run with the following command (with GCC optimizations):
$	gcc -lm -lpthread -march=native -O2 problem1.c && ./a.out >> primes.txt
or without GCC optimization:
$	gcc -lm -lpthread  problem1.c && ./a.out >> primes.txt

Older versions of GCC default to -std=gnu89. If you are using a version of GCC
older than GCC 5, you must specify -std=gnu11 as follow:
$	gcc -lm -lpthread -std=gnu11 -march=native -O2 problem1.c && ./a.out >> primes.txt
or
$	gcc -lm -lpthread -std=gnu11 problem1.c && ./a.out >> primes.txt



* Summary *

This program uses the "Sieve of Eratosthenes" algorithm to find all prime
numbers. It was first developed as a standard single-thread program.
This algorithm uses an array of booleans (isComposite) as large as the given
limit (highest potential prime number). This array takes up 100000001 bytes
(100MB) of RAM memory when the limit (MAXNUM) is set to 10^8. This number could
be divided by eight using bits (and bitwise operations) if memory usage was an
issue, but this would introduce complications as multiple threads attempt to
modify the same byte.

The addition of any number of threads (MAXTHREADS) can safely be implemented
with no locking mechanism, using the main thread as a controller.

A structure containing the variables "curValue" and "isWaiting" is created for
each thread. It is passed from the main thread as a pointer, allowing the main
thread to easily modify these values.
Each thread runs an infinity loop that ends when "curValue" is set to zero.
Whenever "isWaiting" is set to false, the thread sets the "isComposite" boolean
of every multiple of "curValue" to true, then switches the "isWaiting" flag back
to true. The main thread looks for waiting threads, gives them a new "curValue"
(using the nextPrime function), and resets the "isWaiting" flag to 1.



* Optimizations *

- Set the main function to call the nextPrime() function as soon as a thread is 
given a prime to loop through. This allows for the next thread to have work
available as soon as it is ready. As a result, the main function acts as a
working thread (this is shown in the performance benchmark, especially the AMD
section).



* Benchmarking *

Benchmarking is done by setting the MAXTHREADS macro to 1 (clean output) or 2 (
includes an individual count of the time taken to count the prime numbers). The
following command is run, and the MAXTHREADS macro is modified once sufficient 
samples have been collected.

The runtime is calculated from the last 5 values obtained. Raw data is
provided. The performance increase is the time obtained with n threads divided
by the time obtained with a single thread.

i7 processor
$	while true; do gcc -lpthread problem1.c && ./a.out; sleep .5; done >> P1-i7-default.benchmark
		[ default			]	[ -march=native -O2 ]
# threads	runtime (s)	perf increase (%)	runtime (s)	perf incr
1		1.89		100%			0.777		100%
2		1.21		156%			0.584		133%
3		0.96		197%			0.545		143%
4		0.86		220%			0.550		141%
5		0.77		245%			0.564		138%
6		0.77		245%			0.559		140%
7		0.77		245%			0.581		134%
8		0.83		228%			0.590		132%
9		0.81		233%			0.634		122%
10		0.87		217%			0.667		116%
16		1.25		152%
32		1.34		141%
64		1.60		118%
128		1.56		121%
254		1.74		109%

These results show a large increase when new threads are introduced. The
performance increase peaks when MAXTHREADS is set to 5 (which means 6 threads
are running because the main thread is active as well). There is not enough data
to determine whether the performance increase stops because of a software design
limitation, or it is caused by the Intel i7 having only four physical cores
running 8 threads. While the performance increase stopped and it started
decreasing once we had more threads running than present on the processor, 
performance never decreased from the original 1-2 thread result (tested with 254
running threads).

These measurements were repeated with the compiler optimization flag set to
-march=native -O2. This reduces the time it takes to count the primes found by a
factor of 10. The results are shown on the right two columns. 
The performance increase is less drastic with the -O2 flag. The worst result
which is obtained on a single (2) thread is equivalent to the best case scenario
obtained without compiler optimizations (5-8 threads). This is a 243%
performance increase with no modification to the source code.

Finally the program was tested on an older 4-cores 4-threads AMD Phenom II X4 980 CPU.

AMD Phenom II X4 980
		[ default			]	[ -march=native -O2 ]
# threads	runtime (s)	perf increase (%)	runtime (s)	perf incr
1		1.31		100			1.22		100
2		1.01		130			0.93		131
3		0.93		141			0.87		140
4		1.06		124			0.94		130
8		1.17		112			1.14		107
254		1.22		107			1.45		84

While the performance per thread increase is not as drastic on the AMD processor,
it is shown clearly. Performance increases until the number of cores is reached
(3 threads + main), then decreases. While completely irrelevant to this
assignment, it is interesting to note that GCC offers very little optimization
on the AMD platform which may otherwise be faster than the i7.


Result should be 5,761,455 at 10^8

Found 5761456 primes. using i=100

