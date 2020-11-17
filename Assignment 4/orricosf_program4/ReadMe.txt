Assignment 4 - ReadMe - Felipe Orrico Scognamiglio (933-327-510)

1) The Functions:
    I) For each of the used functions in my Assignment, not including main, I put a comment on top of  it explaining its functionality.
    II) I didn't bother to comment all the code inside the functions because I believe the code is relatively easy to follow and comprehend.
    III) Please contact me using my OSU email (orricosf@oregonstate.edu) in case there is any part of the code that requires some explanation.
    We can schedule some time on Zoom to address that.
    IV) The main function is in main.c.

2) Compiling the Code:
    I) I added a Makefile to compile the code. All you have to do is run "make" or "make all" to create the line_processor executable.
    II) To clear all the made files run "make clean"
    III) If the Makefile does not work use one of the following:
        - make other (this will compile using the third option on this list)
        - gcc --std=gnu99 -g -lm -pthread linkedlist.c main.c -o line_processor
        - gcc --std=gnu99 -g -lm -lpthread linkedlist.c main.c -o line_processor

3) Running the Program:
    I) To run the executable, run "./line_processor" or redirecting I/O.

4) In case of Corrupt files:
	I post all my assignments on GitHub! https://github.com/felipeorrico/CS344