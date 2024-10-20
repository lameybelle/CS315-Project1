all: pseudo-shell
CFLAGS=-g -W -Wall
OBJECTS=command.o string_parser.o main.o
EXTRAS= output.txt

pseudo-shell: main.o command.o string_parser.o
	gcc -g -o $@ $^

main.o: main.c command.h
command.o: command.c command.h
string_parser.o: string_parser.c string_parser.h

clean:
	rm -f $(OBJECTS)
	rm -f $(EXTRAS)
	rmdir test
