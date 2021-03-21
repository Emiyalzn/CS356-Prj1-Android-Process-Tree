//Source file to generate a child process which calls ptree
//to show its relationship with its parent process

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(){
    //fork a child process
    pid_t child_pid = fork();

    if(child_pid < 0){
        printf("Fork Error!\n");
        return -1;
    }
    //let the child process print the process tree
    else if(child_pid == 0){
        pid_t pid = getpid();
        printf("519021911033 Child is %d.\n",pid);
        execl("/data/misc/testPtreeARM","testPtreeARM",NULL);
    }

    else{
        pid_t pid = getpid();
        printf("519021911033 Parent is %d.\n",pid);
    }

    return 0;
}