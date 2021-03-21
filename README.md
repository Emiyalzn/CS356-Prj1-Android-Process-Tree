# OS Project 1 

 The project is composed of 4 subproblems, each corresponding to a folder called "ProblemX" in the current directory.

### Problem 1: Implement a system call to print Android process tree

ptree.c: Source file for an Android module which implements a "print process tree" system call.

Makefile: Android Module Makefile for ptree.c.

Type `"make"` in command line below Problem1's directory, we could get the target module file `ptree.ko`.

### Problem 2: Test the system call, print the process tree

testscript.txt: Program running output.

jni/testPtree.c: Source file to test my ptree system call.

jni/Android.mk: Android Makefile for testPtree.c.

Type `"ndk-build"` in command line below jni's directory, we could get the target executable file `testPtreeARM` in `libs/armeabi` .

### Problem 3: Generate a process running testPtreeARM, and show its relationship with its parent process.

testscript.txt: Program running output.

jni/generateProcess.c: Source file to generate a child process which calls ptree to show its relationship with its parent process.

jni/Android.mk: Android Makefile for generateProcess.c.

 Type `"ndk-build"` in command line below jni's directory, we could get the target executable file `generateProcessARM` in `libs/armeabi` .

### Problem 4: Use multi-threads and semaphores to solve the IPC Burger Buddies Problem.

testscript.txt: Program running output.

jni/BurgerBuddies.c: Source file to solve Buger Buddies Problem.

jni/Android.mk: Android Makefile for BurgerBuddies.c.

Type `"ndk-build"` in command line below jni's directory, we could get the target executable file `BBC` in `libs/armeabi` .
