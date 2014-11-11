#
# IOS Project 2 - 8.4.2014
# Makefile
# Author: Lukáš Rendvanský, FIT
# E-mail: xrendv00@stud.fit.vutbr.cz
#	

# Names of output executable files

NAME=rivercrossing

# Names of object files

OBJFILES=$(NAME).o error.o strtonum.o

# Names of header files

HEADERFILES=error.h rivercrossing.h strtonum.h

# Compiler

CC=gcc

# Compiler flags

CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -lrt -g -W -O2 -lm 
# Start rule

all: $(NAME)

# Universal rule to generate obejct files	

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Other rules to generate object files with universal rule

error.o: error.c $(HEADERFILES)
eratosthenes.o: rivercrossing.c $(HEADERFILES)
strtonum.o: strtonum.c $(HEADERFILES)

# Linking required object files to runnable binary files

$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

# Removing all object and binary files

rmall: pack clean
	rm -f *.c *.h

clean:
	rm -f *~ *.o
	rm -f $(NAME)

pack:
	zip xrendv00.zip *.c *.h Makefile

# End of Makefile
