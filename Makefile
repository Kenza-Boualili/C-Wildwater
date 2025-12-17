CCC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O3
OBJ = main.o avl_usines.o avl_recherche.o utils.o lecture_csv.o calculs.o
EXEC = c-wildwater

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)

.PHONY: all clean
