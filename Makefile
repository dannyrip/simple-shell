#
# In order to execute this "Makefile" just type "make"
#

OBJS 	= main.o execute.o parsing.o signal_handlers.o
SOURCE	= main.cpp execute.cpp parsing.cpp signal_handlers.cpp
HEADER  = execute.h parsing.h signal_handlers.h
OUT  	= mysh
CC	= g++
FLAGS   = -g -c -pedantic -ansi  -Wall -std=c++11
# -g option enables debugging mode 
# -c flag generates object code for separate files

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

# create/compile the individual files >>separately<< 
main.o: main.cpp
	$(CC) $(FLAGS) main.cpp

execute.o: execute.cpp
	$(CC) $(FLAGS) execute.cpp

parsing.o: parsing.cpp
	$(CC) $(FLAGS) parsing.cpp

signal_handlers.o: signal_handlers.cpp
	$(CC) $(FLAGS) signal_handlers.cpp

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
