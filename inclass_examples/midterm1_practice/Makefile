CC = gcc
CFLAGS = -g -no-pie
LDFLAGS = -no-pie

EXES = countTo10

%.o  : %.S
	$(CC) -c $(CFLAGS) -o $*.o $*.S

all : $(EXES)

countTo10 : countTo10.o
	$(CC) $(LDFLAGS) -o $@ countTo10.o

countToN : countToN.o
	$(CC) $(LDFLAGS) -o $@ countToN.o

countToNInc : countToNInc.o
	$(CC) $(LDFLAGS) -o $@ countToNInc.o

clean :
	rm -f $(EXES) *.o
