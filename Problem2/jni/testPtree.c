//Source file to test my ptree system call

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>

#define __NR_ptreecall 356
#define MAX_BUFFERSIZE 1024

struct prinfo{
    pid_t parent_pid; /* process id of parent, set 0 if it has no parent*/
    pid_t pid; /* process id */
    pid_t first_child_pid; /* pid of youngest child, set 0 if it has no child */
    pid_t next_sibling_pid; /* pid of older sibling, set 0 if it has no sibling*/
    long state; /* current state of process */
    long uid; /* user id of process owner */
    char comm[64]; /* name of program executed */
    int depth; /* help to print the info in depth (\t) */
};

int main(){
    //allocate memory
    int *nr = malloc(sizeof(int));
    struct prinfo *buf = malloc(MAX_BUFFERSIZE*sizeof(struct prinfo));
    if(buf == NULL || nr == NULL){
        printf("Memory Allocation Error!\n");
        return -1;
    }

    //system call
    syscall(__NR_ptreecall,buf,nr);

    //print process information
    printf("There are %d processes.\n", *nr);
    int i, j;
    for(i=0;i<*nr;i++){
        for(j=0;j<buf[i].depth;j++) 
            printf("\t");
        printf("%s,%d,%ld,%d,%d,%d,%d\n",buf[i].comm,buf[i].pid,buf[i].state,buf[i].parent_pid,
        buf[i].first_child_pid,buf[i].next_sibling_pid,buf[i].uid);
    }

    //free memory
    free(buf);
    free(nr);

    return 0;
}