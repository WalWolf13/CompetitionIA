CC = gcc
SRC= $(notdir $(wildcard src/*.c))
FLAGS_E = -lm -fsanitize=address -O3
FLAGS_C = -fsanitize=address -O3

OBJ= $(SRC:.c=.o)

%.o : src/%.c
	@echo Compilation de $<
	@$(CC) -c $< -Wall $(FLAGS_C) -o bin/$*.o

all : $(OBJ)
	@echo Edition des liens...
	@$(CC) $(addprefix bin/, $^) $(FLAGS_E) -o exe/serveur
	@echo Terminé !

clean :
	@rm bin/*
	@rm exe/*
	@echo "Fichier(s) supprimé(s) !"

start :
	@./exe/serveur carte0.m
