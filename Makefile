CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
OBJ = main.o avl_usines.o avl_recherche.o utils.o lecture_csv.o calculs.o
EXEC = c-wildwater

all: $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

clean:
	rm -f *.o $(EXEC)

cleanfile:
	rm -f vol_max_*.dat vol_captation_*.dat vol_traitement_*.dat
	rm -f vol_all_*.dat leaks_*.dat *.png *.sorted *.header *.small *.big

.PHONY: all clean cleanfile

