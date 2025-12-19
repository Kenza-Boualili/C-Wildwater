CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude
BINDIR = bin
SRCDIR = src
OBJDIR = obj

# Liste des objets avec leur chemin
_OBJS = main.o avl_usines.o avl_recherche.o utils.o lecture_csv.o calculs.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

TARGET = $(BINDIR)/c-wildwater

all: $(BINDIR) $(OBJDIR) $(TARGET)

# Création des répertoires nécessaires
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compilation de l'exécutable dans le dossier bin
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Règle générique pour les fichiers objets depuis src/ vers obj/
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets
clean:
	rm -rf $(OBJDIR)

# Nettoyage des fichiers de sortie dans le dossier output
cleanfile:
	rm -f output/histo/*.dat output/histo/*.png output/histo/*.tmp output/histo/*.gp
	rm -f output/leaks/*.dat output/leaks/*.log
	rm -f output/*.log output/*.tmp
# Nettoyage complet
cleanall: clean cleanfile
	rm -rf $(BINDIR)

.PHONY: all clean cleanfile cleanall