CC = gcc
SRC= $(notdir $(wildcard src/*.c))

OBJ= $(SRC:.c=.o)

%.o : src/%.c
	@echo Compilation de $<
	@$(CC) -c $< -Wall $(FLAGS_C) -o bin/$*.o

all : $(OBJ)
	@echo Edition des liens...
	@$(CC) $(addprefix bin/, $^) $(FLAGS_E) -o exe/prog
	@echo Terminé !

start :
	@./exe/prog
