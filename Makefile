CC = gcc
CFLAGS = -g
OBJFILES = main.o cli/cli.o assembler/optable.o assembler/symtable.o assembler/pass1.o assembler/pass2.o sicengine/sicengine.o cli/debug.o


default: sic-cli

sic-cli: $(OBJFILES) 
	$(CC) $(CFLAGS) -o sic-cli $(OBJFILES)

main.o: main.c main.h
	$(CC) $(CFLAGS) -c main.h main.c

cli.o: cli.c cli.h
	$(CC) $(CFLAGS) -c cli/cli.h cli/cli.c

optable.o: optable.c assemble.h
	$(CC) $(CFLAGS) -c assembler/assemble.h assembler/optable.c

symtable.o: symtable.c assemble.h
	$(CC) $(CFLAGS) -c assembler/assemble.h assembler/symtable.c 

pass1.o: pass1.c assemble.h
	$(CC) $(CFLAGS) -c assembler/assemble.h assembler/pass1.c

pass2.o: pass2.c assemble.h
	$(CC) $(CFLAGS) -c assembler/assemble.h assembler/pass2.c

sicengine.o: sicengine.c sicengine.h
	$(CC) $(CFLAGS) -c sicengine/sicengine.h sicengine/sicengine.c

debug.o: debug.c debug.h
	$(CC) $(CFLAGS) -c cli/debug.h cli/debug.c


# Commands for removing extra files
clean: 
	@echo 
	@echo Removing backup files 
	$(RM) *~ cli/*~ assembler/*~
	@echo

asmclean:
	@echo
	@echo Removing all files created by SIC Assembler in current directory
	$(RM) *.obj *.lst *.int
	@echo
