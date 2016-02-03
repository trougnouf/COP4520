1.
Data structures:
	chopstick contains the location of the chopstick where c0 is on
	the left of p0 and so on, and the usedBy variable which contains
	either an integer describing the current user or UINT8_MAX when
	unused.
	
	philosopherData is passed to the philosopher threads. It contains
	the philosopher's location, the isThinking boolean, and a pointer
	to the two nearest chopsticks.
	
Main function:
	The main function creates NUMTHREADS chopsticks and philosophers, 
	then waits for the 'n' signal to terminate.

philosopher threads:
	The thread checks if its philosopher is thinking, in which case it waits
	for hunger and switches the isThinking variable.
	
	If the philosopher is not thinking, the thread checks if the philosopher
	holds both chopsticks. If that is the case, the thread waits for the 
	philosopher to eat, goes back to thinking and frees the chopsticks.
	
	Otherwise the thread checks if either of the chopsticks are free and 
	grabs any available one.
	
	No two philosopher can hold the same chopstick, because a philosopher
	will only grab a free (UINT8_MAX) chopstick. Even if two
	philosophers were to grab a chopstick at the exact same time, the thread 
	goes back through the for loop and checks that both chopsticks are being
	held before eating.


2.
In version 2. of the program, a philosopher will only grab the chopsticks when
both of them are available. This prevents a deadlock situation where
all philosophers are stuck with one chopstick waiting for another.

3.
In order to ensure that no philosopher ever starve, a philosopher shall not use
a chopstick two times in a row. As a result, both of the philosopher's neighbors
will have to eat before the philosopher can eat again. In order to survive, a
philosopher will always stop eating when physically full and get hungry before 
dying of starvation, therefore the neighbor will always get a chance to eat
before starving to death. 
This can be implemented with a lastUser variable in the chopstick struct, it is
set equal to usedBy when a philosopher eats but never gets reset to UINT8_MAX.
The thread checks that the lastUser variable is not itself when it verifies that
the chopsticks are unused, and it sets the lastUser variable to itself along
with usedBy when succesfully grabbing a pair of chopsticks.

4.
Because the number of philosopher is set with the NUMTHREADS preprocessor macro,
this program will run with any number of philosophers less than 255. This is
demonstrated by defining NUMTHREADS to 23. The program will run with 255
philosophers, but the 255th philosopher is instantly killed since a philosopher 
thread is set to commit suicide when its location is equal to UINT8_MAX (255). 
Running the program with a number higher than 255 produces undefined behavior
because the location is stored in an 8-bit unsigned integer. One such behavior
is the crash of the terminal it was running into along with the (gedit) text 
editor it was built into and the loss of any unsaved data.
