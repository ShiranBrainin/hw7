CC=gcc									# gcc is the compiler
CCLINK=$(CC) 							# Read variable CC using $(CC)
CFLAGS=-g -Wall -std=c99				# flags for the compiler
OBJS=grades.o 							#all of out object files
EXEC=libgrades.so 						#our shared object
RM=rm -rf *.o libgrades.so 				# make clean command

#linker command, creates the execute file from the objects file
libgrades.so: grades.o					
	$(CCLINK) -shared grades.o -o libgrades.so -llinked-list -L.

grades.o: grades.c grades.h linked-list.h
	$(CCLINK) $(CFLAGS) -c -fpic grades.c

# the command "make clean" will call this rule
clean:
	$(RM)			#reading the variable RM

