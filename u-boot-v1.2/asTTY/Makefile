CC = gcc
CFLAGS = -O2 
VERSION = -1.2
PROG=astty

$(PROG):$(PROG).c
	$(CC) $(CFLAGS) -o $@ $<
	mv $(PROG) $(PROG)$(VERSION)

clean:
	rm -f $(PROG) $(PROG).o
	
