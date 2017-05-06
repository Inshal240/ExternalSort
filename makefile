OBJS = externalsort.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

externalsort : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o externalsort

externalsort.o : externalsort.h externalsort.cpp
	$(CC) $(CFLAGS) externalsort.cpp 

gen: gen.cpp
	g++ -g -O2 -Werror gen.cpp -o gen

clean:
	\rm *.o *~ externalsort