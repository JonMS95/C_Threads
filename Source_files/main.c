#include <stdio.h>
#include <pthread.h>    // This library is mandatory when working with threads.
#include <unistd.h>     // Needed in order to call the "sleep" function. 

/*
In another repo, some information about how to create and manage processes was provided.
Processes are created by using "fork" instruction, which splits the current process into two.
Using processes has two benefits:
    ·Concurrence: each of the processes runs at the same time.
    ·Isolation: they do not depend on each other, so if one of them crashes, other processes
    may keep running, regardless of what happens to others.

Unlike processes, threads are not isolated, it's to say, they are instantiated into the same
process, and they also share the same memory space, in spite of using different call stacks.
This makes the communication between threads way easier than it it between processes.
However, this can be both a blessing and a curse.

Note: remember to use the -lpthread flag when compiling whenever threads are used in the program!
*/

// Note that the dummy function below do has a void pointer as input parameter
// and return type. The purpose of this is to fit the function prototype of the pthread_create
// function.
void* myTurn(void* arg)
{
    while(1)
    {
        sleep(1);
        printf("My turn!\r\n");
    }
    return NULL;
}

void yourTurn()
{
    while(1)
    {
        sleep(2);
        printf("Your turn!\r\n");
    }
}

// Let's see now what happens if one of the threads takes much longer than the other.
// Guessing what's about to happen may not be easy at all.
// One of the processes (the one which calls Hello function) will take 10 seconds, whereas
// the one which calls Goodbye will last 5 seconds. After exiting the Goodbye function,
// the main thread (which calls Goodbye) will reach the end of the program, killing
// the other thread instantly. Therefore, thread_join should be used, which is a function
// that waits for the other thread to be finished before jumping to the next line of code
// in the main thread.
void* Hello(void* arg)
{
    for(int i = 0; i < 10; i++)
    {
        sleep(1);
        printf("Hello!\r\n");
    }
    return NULL;
}

void Goodbye()
{
    for(int i = 0; i < 3; i++)
    {
        sleep(2);
        printf("Goodbye!\r\n");
    }
}

// The function will not only have the prototype that is required for it to be called within a thread,
// but it will also make them useful, actually.
void* FunctionInThread(void* arg)
{
    int* pCounter = (int*)arg;
    for(int i = 0; i < 10; i++)
    {
        sleep(1);
        (*pCounter)++;
        printf("Tick\r\n");
    }
    return (void*)pCounter;
}

void FunctionOutOfThread()
{
    for(int i = 0; i < 3; i++)
    {
        sleep(2);
        printf("Tock\r\n");
    }
}

/*
@brief Main function. Program's entry point.
*/
int main(int argc, char** argv)
{
    // myTurn();
    // yourTurn();
    // If the functions are called as it's shown in the couple of lines above, "myTurn" function's execution
    // will last forever. If what we want is to execute myTurn every second, while executing yourTurn every 
    // two seconds at the same time, then using threads becomes mandatory.

    // First, a thread is instantiated.
    pthread_t newThread;

    // // Then, the thread is told to run the "myTurn" function.
    // pthread_create(&newThread, NULL, myTurn, NULL);
    // // Same as when using the for functions, two different entities are running concurrently, although they
    // // are not isolated (it's to say, they are not different processes) in this case.
    // yourTurn();

    // Then, the thread is told to run the "Hello" function.
    pthread_create(&newThread, NULL, Hello, NULL);
    Goodbye();
    // As long as thread_join function is not being used, the main thread (which calls Goodbye) will end sooner
    // than the other, leaving the other thread with only 5 calls out of 10. To avoid this, thread_join should be
    // used, which makes the main thread wait for the other thread's execution to finish before jumping to the
    // next line.
     pthread_join(newThread, NULL);

    printf("Both main and \"newThread\" threads ended!\r\n");

    int counter = 0;
    pthread_t counterThread;
    pthread_create(&counterThread, NULL, FunctionInThread, (void*)(&counter));

    FunctionOutOfThread();

    pthread_join(counterThread, NULL);
    printf("Counter's final value = %d\r\n", counter);

    // Note that the printf call in the line above does actually print the counter's value that was computated
    // by the function that was being called within a thread. This happens due to the fact that unlike processes,
    // threads do share memory.

    return 0;
}
