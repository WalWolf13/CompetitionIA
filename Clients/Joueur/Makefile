CC = gcc
SRC= $(notdir $(wildcard src/*.c))
FLAGS_E = -lm -fsanitize=address
FLAGS_C = -fsanitize=address

OBJ= $(SRC:.c=.o)

%.o : src/%.c
	@echo Compilation de $<
	@$(CC) -c $< -Wall $(FLAGS_C) -o bin/$*.o

all : $(OBJ)
	@echo Edition des liens...
	@$(CC) $(addprefix bin/, $^) $(FLAGS_E) -o exe/ia
	@echo Terminé !

clean :
	@rm bin/*
	@rm exe/*
	@echo "Fichier(s) supprimé(s) !"

start :
	@./exe/ia 127.0.0.1
