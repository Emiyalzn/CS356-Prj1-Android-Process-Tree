//Source file to solve Buger Buddies Problem 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define true 1

sem_t sem_empty, sem_full, sem_cashier, sem_customer, mutex;
pthread_t *cook_thread, *cashier_thread, *customer_thread, *killer_thread;
int *cook_rank, *cashier_rank, *customer_rank;
int cook_cnt, cashier_cnt, customer_cnt, rack_cnt;
int customer_num; //a help variable to determine when to terminate the process

void handle_quit(){ 
    // Since Android doesn't implement pthread_cancel, 
    // we need to modify SIGQUIT to prevent killing the whole process
    pthread_exit(NULL);
}

void *killer(){
    int i;
    //cancel running cook and cashier threads
    for(i = 0; i < cook_cnt; ++i){
        pthread_kill(cook_thread[i],SIGQUIT);
    }
    for(i = 0; i < cashier_cnt; ++ i){
        pthread_kill(cashier_thread[i],SIGQUIT);
    }

    return;
}

void *cook(void *argv){
    int rank = *(int *)argv;
    signal(SIGQUIT,handle_quit); // modify default SIGQUIT handeler

    while(true){
        sem_wait(&sem_full);
        //make a burger and put it on the rack
        sleep(1);
        printf("Cook[%d] makes a burger.\n", rank);
        sem_post(&sem_empty);
    }
    return;
}

void *cashier(void *argv){
    int rank = *(int *)argv;
    signal(SIGQUIT,handle_quit);

    while(true){
        sem_wait(&sem_customer);
        //the cashier is waken up by a customer
        printf("Cashier[%d] accepts an order.\n",rank);
        
        sem_wait(&sem_empty);
        //get a burger from the rack
        sleep(1);
        printf("Cashier[%d] takes a burger to a customer.\n",rank);
        
        //finish the job and wait for another customer
        sem_wait(&mutex);
        customer_num--;
        if(customer_num==0){ //call killer if all customers have been served
            pthread_create(killer_thread,NULL,killer,NULL);
        }
        sem_post(&mutex);

        sem_post(&sem_full);
        sem_post(&sem_cashier);
    }
    return;
}

void *customer(void *argv){
    int rank = *(int *)argv;
    //customers come at different times 
    sleep(rand()%customer_cnt);
    printf("Customer[%d] comes.\n",rank);

    sem_wait(&sem_cashier);
    sem_post(&sem_customer);
    return;
}

int main(int argc, char *argv[]){
    if(argc != 5){
        printf("Input Format Error!\n");
        exit(-1);
    }

    //get counts of different objects
    cook_cnt = atoi(argv[1]);
    cashier_cnt = atoi(argv[2]);
    customer_cnt = customer_num = atoi(argv[3]);
    rack_cnt = atoi(argv[4]);

    if(cook_cnt == 0 || cashier_cnt == 0 || customer_cnt == 0 || rack_cnt == 0){
        printf("There should be at least 1 cook, cashier, customer and rack!");
        exit(-1);
    }

    printf("There are %d cooks, %d cashiers, %d customers, %d racks.\n",cook_cnt,cashier_cnt,customer_cnt,rack_cnt);

    //allocate threads and ranks memory
    killer_thread = malloc(sizeof(pthread_t));
    cook_thread = malloc(cook_cnt * sizeof(pthread_t));
    cashier_thread = malloc(cashier_cnt * sizeof(pthread_t));
    customer_thread = malloc(customer_cnt * sizeof(pthread_t));
    cook_rank = malloc(cook_cnt * sizeof(int));
    cashier_rank = malloc(cashier_cnt * sizeof(int));
    customer_rank = malloc(customer_cnt * sizeof(int));

    //initialize semaphores
    sem_init(&sem_empty,0,0);
    sem_init(&sem_full,0,rack_cnt);
    sem_init(&sem_cashier,0,cashier_cnt);
    sem_init(&sem_customer,0,0);
    sem_init(&mutex,0,1);

    printf("Begin running.\n");

    //create threads
    int i;
    for(i = 0; i < cook_cnt; ++i){
        cook_rank[i] = i + 1;
        pthread_create(cook_thread + i,NULL,cook,cook_rank + i);
    }
    for(i = 0; i < cashier_cnt; ++i){
        cashier_rank[i] = i + 1;
        pthread_create(cashier_thread + i,NULL,cashier,cashier_rank + i);
    }
    for(i = 0; i < customer_cnt; ++i){
        customer_rank[i] = i + 1;
        pthread_create(customer_thread + i,NULL,customer,customer_rank + i);
    }

    //wait and retrieve the threads
    pthread_join(*killer_thread,NULL);
    for(i = 0; i < cook_cnt; ++i) pthread_join(cook_thread[i],NULL);
    for(i = 0; i < cashier_cnt; ++i) pthread_join(cashier_thread[i],NULL);
    for(i = 0; i < customer_cnt; ++i) pthread_join(customer_thread[i],NULL);

    //free memory
    free(cook_thread);
    free(cashier_thread);
    free(customer_thread);
    free(killer_thread);
    free(cook_rank);
    free(cashier_rank);
    free(customer_rank);

    sem_destroy(&sem_empty);
    sem_destroy(&sem_full);
    sem_destroy(&sem_cashier);
    sem_destroy(&sem_customer);
    sem_destroy(&mutex);

    printf("Process finished.\n");

    return 0;
}