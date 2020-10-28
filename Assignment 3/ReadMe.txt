Assignment 3 - ReadMe - Felipe Orrico Scognamiglio (933-327-510)

1) The Functions:
    I) For each of the used functions in my Assignment, not including main, I put a comment on top of  it explaining its functionality.
    II) I didn't bother to comment all the code inside the functions because I believe the code is relatively easy to follow and comprehend.
    III) Please contact me using my OSU email (orricosf@oregonstate.edu) in case there is any part of the code that requires some explanation.
    We can schedule some time on Zoom to address that.
    IV) The main function is in main.c. All other files are in the following folders: Built-in, Shell, and Utils. Within those folders there are .c and .h files of the different parts of the assignment. Utils contains Handlers, Linked Lists, and other utils functions that I decided to leave there. Shell contains Shell.c and Shell.h and those are the main drivers of the Shell program. Built-in contains CD and Exit commands as well as different funtions to support them.

2) Compiling the Code:
    I) I added a Makefile to compile the code. All you have to do is run "make" or "make all" to create the SmallSh executable. ***Please be sure that you extract everything as it is, meaning, do not mode the files from within the folders somewhere else. Contact me if you have any problems compiling the code.
    II) To clear all the made files run "make clean"
    III) If the Makefile does not work use :
	gcc --std=gnu99 -g ./Utils/linkedlist.c ./Utils/handler.c ./Utils/intList.c ./Utils/utils.c ./Shell/Shell.c ./Built-in/cd.c ./Built-in/exit.c main.c -o smallsh

3) Running the Program:
    I) To run the executable, run "./smallsh" or run the test script.
    II) When the script runs the LS sending the output to junk and then reading junk, the folders Built-in, Shell, and Utils will show up on the output. This was pointed out by Prof. Goins as he wondered what "Built-in" was when I showed him my results for the test script. 

4) In case of Corrupt files:
	I post all my assignments in GitHub! https://github.com/felipeorrico/CS344