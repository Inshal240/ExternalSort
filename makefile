OBJS = externalsort.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

externalsort : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o externalsort

externalsort.o : externalsort.cpp
	$(CC) $(CFLAGS) externalsort.cpp 

clean:
	\rm *.o *~ externalsort