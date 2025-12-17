# Compilateur et options
CC = gcc
# Optimisation -O3 pour une exécution plus rapide
CFLAGS = -Wall -Wextra -std=c11 -O3
OBJ = main.o avl_usines.o avl_recherche.o utils.o lecture_csv.o calculs.o
EXEC = c-wildwater

# Règle par défaut : compile l'exécutable
all: $(EXEC)

# Compilation de l'exécutable à partir des objets
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

# Règle générique pour les fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f *.o $(EXEC)

# Nettoyage des fichiers de données et images générés
cleanfile:
	rm -f vol_max_*.dat vol_captation_*.dat vol_traitement_*.dat
	rm -f vol_all_*.dat leaks_*.dat *.png *.sorted *.header *.small *.big

# Déclaration des cibles qui ne sont pas des fichiers
.PHONY: all clean cleanfile
