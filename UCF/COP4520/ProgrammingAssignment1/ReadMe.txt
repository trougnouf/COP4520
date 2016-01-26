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
Each thread runs an infinity loop that ends when "curValue" is set to zero. Whenever "isWaiting" is set to false, the thread sets the "isComposite" boolean of every multiple of "curValue" to true, then switches the "isWaiting" flag back to true. The main thread looks for waiting threads, gives them a new "curValue" (using the nextPrime function), and resets the "isWaiting" flag to 1.

Benchmarking:

1 thread : 5.9462s (n=10)	100% speed
2 threads: 4.0886s (n=6)	145%
4 threads: 2.9888s (n=9)	199%
7 threads: 2.8629s (n=10)	208%
8 threads: 3.2755s (n=10)	182%
16 threads: 4.1422s (n=10)	144%

Result should be 5,761,455 at 10^8

Found 5761456 primes. using i=100

