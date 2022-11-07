CC=g++
CFLAGS=-g -c
OBJ=envvars.o main.o parsecommand.o execute.o
HEADER=envvars.h parsecommand.h execute.h
EXECUTABLENAME=myshell.cs1

.c.o:
	$(CC) $(CFLAGS) $<


$(EXECUTABLENAME): $(OBJ)
	$(CC) $(OBJ) -o $(EXECUTABLENAME)
	
$(OBJ): $(HEADER)

cleanall:
	rm -f $(OBJ) $(EXECUTABLENAME)