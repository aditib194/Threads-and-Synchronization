My implementation starts with include statements and global variables. It then has the phonecall function following the
requirements on the third page. It then has the timer thread function. In the main method, I initialized semaphores and
create 100 phne call threads. I have a check to manage user input correctly. I create and join the timer thread. I end 
by destroying the semaphores and using the exit system call.

My program runs as expected and gives the output required.
a. I use binary semaphorey to protect updating id. I then use it with a while loop to check if a connection can be
made. I then update connected again using a binary semaphore.
b. In line 42 and 49, a counting semaphor is used for operator availability
c. Initalized in lines 75 and 76 and destroyed in lines 109 and 110
d. My phonecall thread function meets the requirements on the last page as evident by my comments
e. My timer function called thread which blocks the main thread until the conference time
f. I loop through 100 times to mark  the phonecall thread function as detached. I then create the timer thread in line 
100 that joins the main thread and the timer thread. In the phonecall fucntions, I mark the thread as detached.
g. My global variable next_id is updated in a protected critical region of code. I increase the value and assign it 
to the local variable id.
h. I update connected when line is not busy and before the call is over.
j. I use the static modifier for global variables and not for local variables as it was unecessary.
k. I have a check to see if argc equals 1 or atoi returns 0, in which case I assign seconds the default value 3.
n. I do error handling by checking if the pthread fucntions return 0.