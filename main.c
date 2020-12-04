#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>


#define BUFFER_SIZE 5
#define FINAL_BUFFER_SIZE (BUFFER_SIZE + 1)
//buffer[0] - number of products in a buffer

//TIME IN MILLISECONDS
#define MIN_PRODUCTION_TIME 3000
#define MAX_PRODUCTION_TIME 8000


bool debugMode = true;

//CHEESE
int cheeseBuffer[FINAL_BUFFER_SIZE];
sem_t *cheeseSem;
sem_t *cheeseBufferNotEmpty;
sem_t *cheeseBufferNotFull;
bool cheeseConsumerWaiting = false;
bool cheeseManufacturerWaiting = false;

void *cheeseConsumer(void *param);

void *cheeseManufacturer(void *param);


//PASTRY
int pastryBuffer[FINAL_BUFFER_SIZE];
sem_t *pastrySem;
sem_t *pastryBufferNotEmpty;
sem_t *pastryBufferNotFull;
//Number of waiting pastry consumers
int pastryConsumerWaiting = 0;
bool pastryManufacturerWaiting = false;

void *pastryConsumer(void *param);

void *pastryManufacturer(void *param);

//CABBAGE
int cabbageBuffer[FINAL_BUFFER_SIZE];
sem_t *cabbageSem;
sem_t *cabbageBufferNotEmpty;
sem_t *cabbageBufferNotFull;
bool cabbageConsumerWaiting = false;
bool cabbageManufacturerWaiting = false;

void *cabbageConsumer(void *param);

void *cabbageManufacturer(void *param);

//MEAT
int meatBuffer[FINAL_BUFFER_SIZE];
sem_t *meatSem;
sem_t *meatBufferNotEmpty;
sem_t *meatBufferNotFull;
bool meatConsumerWaiting = false;
bool meatManufacturerWaiting = false;

void *meatConsumer(void *param);

void *meatManufacturer(void *param);


void *dumplingsWithMeatConsumer(void *param);

void *dumplingsWithCheeseConsumer(void *param);

void *dumplingsWithCabbageConsumer(void *param);

useconds_t chooseManufacturingTime();

void testChooseManufacturingTime();


int main() {
    srand(time(NULL));


    sem_unlink("pastry_sem");
    sem_unlink("meat_sem");
    sem_unlink("cheese_sem");
    sem_unlink("cabbage_sem");
    sem_unlink("meat_buffer_not_empty");
    sem_unlink("pastry_buffer_not_empty");
    sem_unlink("cheese_buffer_not_empty");
    sem_unlink("cabbage_buffer_not_empty");
    sem_unlink("meat_buffer_not_full");
    sem_unlink("pastry_buffer_not_full");
    sem_unlink("cheese_buffer_not_full");
    sem_unlink("cabbage_buffer_not_full");


    pastrySem = sem_open("pastry_sem", O_CREAT | O_EXCL, 0644, 1);
    meatSem = sem_open("meat_sem", O_CREAT | O_EXCL, 0644, 1);
    cheeseSem = sem_open("cheese_sem", O_CREAT | O_EXCL, 0644, 1);
    cabbageSem = sem_open("cabbage_sem", O_CREAT | O_EXCL, 0644, 1);
    meatBufferNotEmpty = sem_open("meat_buffer_not_empty", O_CREAT | O_EXCL, 0644, 0);
    cheeseBufferNotEmpty = sem_open("cheese_buffer_not_empty", O_CREAT | O_EXCL, 0644, 0);
    pastryBufferNotEmpty = sem_open("pastry_buffer_not_empty", O_CREAT | O_EXCL, 0644, 0);
    cabbageBufferNotEmpty = sem_open("cabbage_buffer_not_empty", O_CREAT | O_EXCL, 0644, 0);
    meatBufferNotFull = sem_open("meat_buffer_not_full", O_CREAT | O_EXCL, 0644, 0);
    cheeseBufferNotFull = sem_open("cheese_buffer_not_full", O_CREAT | O_EXCL, 0644, 0);
    pastryBufferNotFull = sem_open("pastry_buffer_not_full", O_CREAT | O_EXCL, 0644, 0);
    cabbageBufferNotFull = sem_open("cabbage_buffer_not_full", O_CREAT | O_EXCL, 0644, 0);


    pthread_t threads[7];
    void *retvals[7];


    pthread_create(&threads[0], NULL, pastryManufacturer, NULL);
    pthread_create(&threads[1], NULL, meatManufacturer, NULL);
    pthread_create(&threads[2], NULL, dumplingsWithMeatConsumer, NULL);
    pthread_create(&threads[3], NULL, cheeseManufacturer, NULL);
    pthread_create(&threads[4], NULL, dumplingsWithCheeseConsumer, NULL);
    pthread_create(&threads[5], NULL, cabbageManufacturer, NULL);
    pthread_create(&threads[6], NULL, dumplingsWithCabbageConsumer, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);
    pthread_join(threads[4], NULL);
    pthread_join(threads[5], NULL);
    pthread_join(threads[6], NULL);
    return 0;
}



//------------MANUFACTURERS START-----------

void *meatManufacturer(void *param) {
    while (true) {
        usleep(chooseManufacturingTime());
        sem_wait(meatSem);
        if (meatBuffer[0] == BUFFER_SIZE) {
            printf("Meat buffer is full! Waiting...\n");
            meatManufacturerWaiting = true;
            sem_post(meatSem);
            sem_wait(meatBufferNotFull);
            //Theres place for me:
            sem_wait(meatSem);
            meatManufacturerWaiting = false;
        }

        //Add meat to meat buffer
        if (meatBuffer[0] != BUFFER_SIZE) {
            meatBuffer[0]++;
            meatBuffer[meatBuffer[0]] = 1;
            if(debugMode) {
                printf("Producing meat...\n");
            }

        } else {
            printf("Trying to put into full meat buffer\n");
            exit(-1);
        }

        if (meatConsumerWaiting) {
            sem_post(meatBufferNotEmpty);
        }
        sem_post(meatSem);
    }
}


void *cabbageManufacturer(void *param) {
    while (true) {
        usleep(chooseManufacturingTime());
        sem_wait(cabbageSem);
        if (cabbageBuffer[0] == BUFFER_SIZE) {
            printf("Cabbage buffer is full! Waiting...\n");
            cabbageManufacturerWaiting = true;
            sem_post(cabbageSem);
            sem_wait(cabbageBufferNotFull);
            //Theres place for me:
            sem_wait(cabbageSem);
            cabbageManufacturerWaiting = false;
        }

        //Add cabbage to cabbage buffer
        if (cabbageBuffer[0] != BUFFER_SIZE) {
            cabbageBuffer[0]++;
            cabbageBuffer[cabbageBuffer[0]] = 1;
            if (debugMode) {
                printf("Producing cabbage...\n");
            }

        } else {
            printf("Trying to put into full cabbage buffer\n");
            exit(-1);
        }

        if (cabbageConsumerWaiting) {
            sem_post(cabbageBufferNotEmpty);
        }
        sem_post(cabbageSem);
    }
}


void *cheeseManufacturer(void *param) {
    while (true) {
        usleep(chooseManufacturingTime());
        sem_wait(cheeseSem);
        if (cheeseBuffer[0] == BUFFER_SIZE) {
            printf("Cheese buffer is full! Waiting...\n");
            cheeseManufacturerWaiting = true;
            sem_post(cheeseSem);
            sem_wait(cheeseBufferNotFull);
            //Theres place for me:
            sem_wait(cheeseSem);
            cheeseManufacturerWaiting = false;
        }

        //Add cheese to meat buffer
        if (cheeseBuffer[0] != BUFFER_SIZE) {
            cheeseBuffer[0]++;
            cheeseBuffer[cheeseBuffer[0]] = 1;
            if (debugMode) {
                printf("Producing cheese...\n");
            }
        } else {
            printf("Trying to put into full cheese buffer\n");
            exit(-1);
        }

        if (cheeseConsumerWaiting) {
            sem_post(cheeseBufferNotEmpty);
        }
        sem_post(cheeseSem);
    }
}


void *pastryManufacturer(void *param) {
    while (true) {
        usleep(0.5*chooseManufacturingTime());
        sem_wait(pastrySem);
        if (pastryBuffer[0] == BUFFER_SIZE) {
            printf("Pastry buffer is full! Waiting... \n");
            pastryManufacturerWaiting = true;
            sem_post(pastrySem);
            sem_wait(pastryBufferNotFull);
            //Theres place for me:
            sem_wait(pastrySem);
            pastryManufacturerWaiting = false;
        }

        //Add pastry to pastry buffer
        if (pastryBuffer[0] != BUFFER_SIZE) {
            pastryBuffer[0]++;
            pastryBuffer[pastryBuffer[0]] = 1;
            if (debugMode) {
                printf("Producing pastry...\n");
            }
        } else {
            printf("Trying to put into full pastry buffer\n");
            exit(-1);
        }

        if (pastryConsumerWaiting) {
            sem_post(pastryBufferNotEmpty);
        }
        sem_post(pastrySem);
    }
}

//------------MANUFACTURERS END-----------


//------------CONSUMERS START-----------

void *dumplingsWithMeatConsumer(void *param) {
    while (true) {
        usleep(chooseManufacturingTime());
        meatConsumer(NULL);
        pastryConsumer(NULL);
        printf("Dumplings with meat produced!\n");
    }
}

void *dumplingsWithCheeseConsumer(void *param) {
    while (true) {
        usleep(chooseManufacturingTime());
        cheeseConsumer(NULL);
        pastryConsumer(NULL);
        printf("Dumplings with cheese produced!\n");
    }
}

void *dumplingsWithCabbageConsumer(void *param) {
    while (true) {
        usleep(chooseManufacturingTime());
        cabbageConsumer(NULL);
        pastryConsumer(NULL);
        printf("Dumplings with cabbage produced!\n");
    }
}

void *meatConsumer(void *param) {
    sem_wait(meatSem);

    if (meatBuffer[0] == 0) {
        printf("Meat buffer is empty! Waiting...\n");
        meatConsumerWaiting = true;
        sem_post(meatSem);
        sem_wait(meatBufferNotEmpty);
        //Theres meat for me:
        sem_wait(meatSem);
        meatConsumerWaiting = false;
    }

    //Consume meat
    if (meatBuffer[0] != 0) {
        meatBuffer[meatBuffer[0]] = 0;
        meatBuffer[0]--;
        if (debugMode) {
            printf("Consuming meat...\n");
        }
    } else {
        printf("Trying to read from an empty meat buffer\n");
        exit(-1);
    }

    if (meatManufacturerWaiting) {
        sem_post(meatBufferNotFull);
    }
    sem_post(meatSem);
}

void *cabbageConsumer(void *param) {
    sem_wait(cabbageSem);

    if (cabbageBuffer[0] == 0) {
        printf("Cabbage buffer is empty! Waiting...\n");
        cabbageConsumerWaiting = true;
        sem_post(cabbageSem);
        sem_wait(cabbageBufferNotEmpty);
        //Theres cabbage for me:
        sem_wait(cabbageSem);
        cabbageConsumerWaiting = false;
    }

    //Consume cabbage
    if (cabbageBuffer[0] != 0) {
        cabbageBuffer[cabbageBuffer[0]] = 0;
        cabbageBuffer[0]--;
        if (debugMode) {
            printf("Consuming cabbage...\n");
        }
    } else {
        printf("Trying to read from an empty cabbage buffer\n");
        exit(-1);
    }

    if (cabbageManufacturerWaiting) {
        sem_post(cabbageBufferNotFull);
    }
    sem_post(cabbageSem);
}


void *cheeseConsumer(void *param) {
    sem_wait(cheeseSem);

    if (cheeseBuffer[0] == 0) {
        printf("Cheese buffer is empty! Waiting...\n");
        cheeseConsumerWaiting = true;
        sem_post(cheeseSem);
        sem_wait(cheeseBufferNotEmpty);
        //Theres cheese for me:
        sem_wait(cheeseSem);
        cheeseConsumerWaiting = false;
    }

    //Consume cheese
    if (cheeseBuffer[0] != 0) {
        cheeseBuffer[cheeseBuffer[0]] = 0;
        cheeseBuffer[0]--;
        if (debugMode) {
            printf("Consuming cheese...\n");
        }
    } else {
        printf("Trying to read from an empty cheese buffer\n");
        exit(-1);
    }

    if (cheeseManufacturerWaiting) {
        sem_post(cheeseBufferNotFull);
    }
    sem_post(cheeseSem);
}


void *pastryConsumer(void *param) {
    sem_wait(pastrySem);
    //Check if pastry buffer is empty now
    //If it is: increment that your waiting and wait for your turn
    if (pastryBuffer[0] == 0) {
        printf("Pastry buffer is empty! Waiting...\n");
        pastryConsumerWaiting++;
        sem_post(pastrySem);
        sem_wait(pastryBufferNotEmpty);
        //Theres pastry for me:
        sem_wait(pastrySem);
        pastryConsumerWaiting--;
    }

    //Consume pastry
    if (pastryBuffer[0] != 0) {
        pastryBuffer[pastryBuffer[0]] = 0;
        pastryBuffer[0]--;
        if (debugMode) {
            printf("Consuming pastry...\n");
        }
    } else {
        printf("Trying to read from an empty pastry buffer");
        exit(-1);
    }

    //Check if pastry manufacturer is waiting
    if (pastryManufacturerWaiting) {
        sem_post(pastryBufferNotFull);
    }

    sem_post(pastrySem);
}



//------------CONSUMERS END-----------


useconds_t chooseManufacturingTime() {
    return ((rand() % (MAX_PRODUCTION_TIME - MIN_PRODUCTION_TIME + 1)) + MIN_PRODUCTION_TIME) * 1000;
}



// --------------------- TEST FUNCTIONS ---------------------

void testChooseManufacturingTime() {
    int i;
    for (i = 0; i < 20; i++) {
        printf("Time: %d\n", chooseManufacturingTime());
    }
    return;
}