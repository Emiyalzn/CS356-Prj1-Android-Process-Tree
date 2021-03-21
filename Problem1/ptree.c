//Source file for an Android module 
//which implements a "print process tree" system call.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gfp.h>
#include <linux/string.h>

MODULE_LICENSE("DUAL BSD/GPL");

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

static int (*oldcall)(void);

/* 
 * This function get the information in task_struct and store it in prinfo
 * The members and functions of list_head are defined in list.h (next, prev)
 * The members and functions of task_struct are defined in sched.h (state, uid, comm, pid, parent, children, sibling)
 */
void getInfo(struct task_struct *ts, struct prinfo *pf, int depth){
    //get basic info
    pf->state = ts->state;
    pf->pid = ts->pid;
    pf->uid = ts->cred->uid;
    pf->depth = depth;
    strcpy(pf->comm,ts->comm);

    //the member parent points to the parent task_struct
    pf->parent_pid = (ts->parent) ?ts->parent->pid :0;
    
    //the member children is a list links to child process's sibling member
    if(list_empty(&(ts->children))){ //use list_empty to judge whether a list is empty
        pf->first_child_pid = 0; 
    }
    else{ //use list_entry to find the task_struct that the list_head belongs to
        pf->first_child_pid = list_entry((&ts->children)->next, struct task_struct, sibling)->pid;
    }

    //the member sibling is a list links to sibling process's sibling member
    //sibing will link to parent process's children member when it has no other sibling
    if(list_empty(&(ts->sibling))){
        pf->next_sibling_pid = 0;
    }
    else{
        pid_t fakeParent_pid = list_entry((&ts->sibling)->next, struct task_struct, children)->pid;
        if(fakeParent_pid == pf->parent_pid) pf->next_sibling_pid = 0; //check whether it's the last sibling
        else{
            pf->next_sibling_pid = list_entry((&ts->sibling)->next, struct task_struct, sibling)->pid;
        }
    }
}

/* 
 * This function traverse the process tree and get process info by DFS
 */
void DFS(struct task_struct *ts, struct prinfo *buf, int *nr, int depth)
{
    struct task_struct *tmp;
    struct list_head *pos  = (&ts->children)->next;

    getInfo(ts,&buf[*nr],depth);
    *nr = *nr + 1;

    list_for_each(pos, &(ts->children)){ //use list_for_each to traverse all the children
        tmp = list_entry(pos, struct task_struct, sibling);
        DFS(tmp, buf, nr, depth+1);
    }
}

/* 
 *This function is the new system call for android to print the process tree
 */ 
static int ptree(struct prinfo *buf, int *nr){
    //use kcalloc to allocate kernel array memory
    struct prinfo *ker_buf = kcalloc(MAX_BUFFERSIZE, sizeof(struct prinfo), GFP_KERNEL);
    //use kzalloc to allocate kernel int memory
    int *ker_nr = kzalloc(sizeof(int),GFP_KERNEL);
    
    if(ker_buf == NULL || ker_nr == NULL){
        printk("Memory Allocation Error!\n");
        return -1;
    }

    *ker_nr = 0; 

    //use lock to prevent race when doing DFS
    read_lock(&tasklist_lock);
    DFS(&init_task,ker_buf,ker_nr,0); //init_task is the original process
    read_unlock(&tasklist_lock);

    //use copy_to_user to copy information from kernel to user
    if(copy_to_user(buf, ker_buf, MAX_BUFFERSIZE*sizeof(struct prinfo))){
        printk("Copy Error!\n");
        return -1;
    }
    if(copy_to_user(nr,ker_nr,sizeof(int))){
        printk("Copy Error!\n");
        return -1;
    }

    //use kfree to free the kernel memory
    kfree(ker_buf);
    kfree(ker_nr);

    return 0;
}

static int addsyscall_init(void)
{
	long *syscall = (long*)0xc000d8c4;
	oldcall=(int(*)(void))(syscall[__NR_ptreecall]);
	syscall[__NR_ptreecall]=(unsigned long)ptree;
	printk(KERN_INFO "module load!\n");
	return 0;
}

static void addsyscall_exit(void)
{
    long *syscall = (long*) 0xc000d8c4;
    syscall[__NR_ptreecall] = (unsigned long)oldcall;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
