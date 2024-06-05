CC = gcc
CFLAGS = -ansi -Wall -pedantic -g
DEPS = preassembler.h macrotable.h utils.h symboltable.h firstpass.h wordlist.h secondpass.h
OBJ = preassembler.o macrotable.o assembler.o utils.o  symboltable.o firstpass.o wordlist.o secondpass.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

assembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o assembler

clean_am:
	rm -f *.am *.ent *.ext
