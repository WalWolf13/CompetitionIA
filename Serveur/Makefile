CC = gcc
SRC= $(notdir $(wildcard src/*.c))
FLAGS_E = -lm -fsanitize=address
FLAGS_C = -fsanitize=address

OBJ= $(SRC:.c=.o)


all : createFolder edition
	@echo Terminé !

%.o : src/%.c
	@echo Compilation de $<
	@$(CC) -c $< -Wall $(FLAGS_C) -o bin/$*.o

edition : $(OBJ)
	@echo Edition des liens...
	@$(CC) $(addprefix bin/, $^) $(FLAGS_E) -o exe/serveur


createFolder : cleanFolder
	@mkdir bin
	@mkdir exe

cleanFolder : cleanFile
	@rm -f -r bin
	@rm -f -r exe

cleanFile :
	@rm -f bin/*
	@rm -f exe/*

clean : cleanFolder
	@echo "Fichier(s) supprimé(s) !"

start :
	@./exe/serveur carte0.m
