#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include "semaphore_class.h"   // Use your uploaded file

using namespace std;

/* -------------------------------------------------------
   Lightswitch class (from Downey)
------------------------------------------------------- */
class Lightswitch {
private:
    int counter = 0;
    Semaphore mutex{1};

public:
    void lock(Semaphore &sem) {
        mutex.wait();
        counter++;
        if (counter == 1)
            sem.wait();
        mutex.signal();
    }
    void unlock(Semaphore &sem) {
        mutex.wait();
        counter--;
        if (counter == 0)
            sem.signal();
        mutex.signal();
    }
};

/* -------------------------------------------------------
   GLOBALS for All Problems
------------------------------------------------------- */
const int NUM_READERS = 5;
const int NUM_WRITERS = 5;
const int NUM_PHILOSOPHERS = 5;

pthread_t readers[NUM_READERS];
pthread_t writers[NUM_WRITERS];
pthread_t philosophers[NUM_PHILOSOPHERS];

/* -------------------------------------------------------
   Problem 1: No-Starve Readers-Writers
------------------------------------------------------- */
Semaphore roomEmpty1(1);
Semaphore turnstile1(1);
Lightswitch readSwitch1;

void* reader1(void* arg) {
    long id = (long)arg;

    while (true) {
        turnstile1.wait();
        turnstile1.signal();
        readSwitch1.lock(roomEmpty1);

        cout << "Reader " << id << ": Reading." << endl;
        usleep(100000);

        readSwitch1.unlock(roomEmpty1);
    }
    return NULL;
}

void* writer1(void* arg) {
    long id = (long)arg;

    while (true) {
        turnstile1.wait();
        roomEmpty1.wait();

        cout << "Writer " << id << ": Writing." << endl;
        usleep(120000);

        roomEmpty1.signal();
        turnstile1.signal();
    }
    return NULL;
}

/* -------------------------------------------------------
   Problem 2: Writer-Priority Readers-Writers
------------------------------------------------------- */
Semaphore roomEmpty2(1);
Semaphore turnstile2(1);
Lightswitch readSwitch2;
Lightswitch writeSwitch2;

void* reader2(void* arg) {
    long id = (long)arg;

    while (true) {
        turnstile2.wait();
        turnstile2.signal();

        readSwitch2.lock(roomEmpty2);

        cout << "Reader " << id << ": Reading." << endl;
        usleep(100000);

        readSwitch2.unlock(roomEmpty2);
    }
    return NULL;
}

void* writer2(void* arg) {
    long id = (long)arg;

    while (true) {
        writeSwitch2.lock(turnstile2);
        roomEmpty2.wait();

        cout << "Writer " << id << ": Writing." << endl;
        usleep(140000);

        roomEmpty2.signal();
        writeSwitch2.unlock(turnstile2);
    }
    return NULL;
}

/* -------------------------------------------------------
   Problem 3: Dining Philosophers #1
   (Allow only 4 philosophers at the table)
------------------------------------------------------- */
Semaphore forks3[NUM_PHILOSOPHERS] = {1,1,1,1,1};
Semaphore room3(4);

void* philosopher3(void* arg) {
    long id = (long)arg;

    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    while (true) {
        cout << "Philosopher " << id << ": Thinking." << endl;
        usleep(120000);

        room3.wait();
        forks3[left].wait();
        forks3[right].wait();

        cout << "Philosopher " << id << ": Eating." << endl;
        usleep(150000);

        forks3[left].signal();
        forks3[right].signal();
        room3.signal();
    }
    return NULL;
}

/* -------------------------------------------------------
   Problem 4: Dining Philosophers #2
   (Odd = left first, Even = right first)
------------------------------------------------------- */
Semaphore forks4[NUM_PHILOSOPHERS] = {1,1,1,1,1};

void* philosopher4(void* arg) {
    long id = (long)arg;
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    while (true) {
        cout << "Philosopher " << id << ": Thinking." << endl;
        usleep(120000);

        if (id % 2 == 0) {
            forks4[right].wait();
            forks4[left].wait();
        } else {
            forks4[left].wait();
            forks4[right].wait();
        }

        cout << "Philosopher " << id << ": Eating." << endl;
        usleep(150000);

        forks4[left].signal();
        forks4[right].signal();
    }
    return NULL;
}

/* -------------------------------------------------------
   MAIN DISPATCHER
------------------------------------------------------- */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: ./cse4001_sync <problem#>" << endl;
        return 1;
    }

    int problem = atoi(argv[1]);

    if (problem == 1) {
        cout << "Running Problem 1: No-Starve Readers-Writers" << endl;
        for (long i = 1; i <= NUM_READERS; i++)
            pthread_create(&readers[i-1], NULL, reader1, (void*)i);
        for (long i = 1; i <= NUM_WRITERS; i++)
            pthread_create(&writers[i-1], NULL, writer1, (void*)i);
    }

    else if (problem == 2) {
        cout << "Running Problem 2: Writer-Priority Readers-Writers" << endl;
        for (long i = 1; i <= NUM_READERS; i++)
            pthread_create(&readers[i-1], NULL, reader2, (void*)i);
        for (long i = 1; i <= NUM_WRITERS; i++)
            pthread_create(&writers[i-1], NULL, writer2, (void*)i);
    }

    else if (problem == 3) {
        cout << "Running Problem 3: Dining Philosophers #1" << endl;
        for (long i = 0; i < NUM_PHILOSOPHERS; i++)
            pthread_create(&philosophers[i], NULL, philosopher3, (void*)i);
    }

    else if (problem == 4) {
        cout << "Running Problem 4: Dining Philosophers #2" << endl;
        for (long i = 0; i < NUM_PHILOSOPHERS; i++)
            pthread_create(&philosophers[i], NULL, philosopher4, (void*)i);
    }

    else {
        cout << "Invalid problem number." << endl;
        return 1;
    }

    pthread_exit(NULL);
    return 0;
}
