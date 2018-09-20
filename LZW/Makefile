CC =  gcc
lib = -lX11
exe = lzw
flags = -g

lab3: table.o lzw.o
	$(CC) table.o lzw.o -o $(exe) $(lib) $(flags)

table.o: table.c table.h
	$(CC) -c table.c $(flags)

lzw.o: lab3.c table.h
	$(CC) $(flags) -c lzw.c

clean:
	rm -f *.o $(exe) core

