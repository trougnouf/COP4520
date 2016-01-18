# Problem 1:

* Compilation and Execution *

Execute and run with the following command:
$	gcc -lpthread  problem1.c && ./a.out >> primes.txt

Older versions of GCC default to -std=gnu89. If you are using a version of GCC prior to GCC 5, you must specify -std=gnu11 as follow:
$	gcc -lpthread -std=gnu11 problem1.c && ./a.out >> primes.txt

* Summary *

This program uses the "Sieve of Eratosthenes" algorithm to find all prime numbers. It was first developed as a standard single-thread program.
This algorithm uses an array of booleans (isComposite) as large as the given limit (highest potential prime number). This array takes up 100000001 bytes (100MB) of RAM memory when the limit (MAXNUM) is set to 10^8. This number could be divided by eight using bits (and bitwise operations) if memory usage was an issue, but this would introduce complications as multiple threads attempt to modify the same byte.

The addition of any number of threads (MAXTHREADS) can safely be implemented with no locking mechanism, using the main thread as a controller.

A structure containing the variables "curValue" and "isWaiting" is created for each thread. It is passed from the main thread as a pointer, allowing the main thread to easily modify these values.
Each thread runs an infinity loop that ends when "curValue" is set to zero. Whenever "isWaiting" is set to false, the thread sets the "isComposite" boolean of every multiple of "curValue" to true, then switches the "isWaiting" flag back to true. The main thread looks for waiting threads, gives them a new "curValue" (using the nextPrime function), and resets the "isWaiting" flag to 1.

Benchmarking:
Using MAXNUM 100000001 (10^8, excluding 0)

Using 8 threads:
gcc -lpthread  problem1.c && ./a.out
Execution time: 5.111911s
Found 6260293 primes.
Sum of primes = 2387434306
Top ten maximum primes: 99999787, 99999821, 99999827, 99999839, 99999847, 99999931, 99999941, 99999959, 99999971, 99999989

Result should be 5,761,455 at 10^8



