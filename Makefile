CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = c-wildwater
OBJS = main.o avl_usines.o avl_recherche.o utils.o lecture_csv.o calculs.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c avl_usines.h avl_recherche.h lecture_csv.h utils.h calculs.h
	$(CC) $(CFLAGS) -c main.c

avl_usines.o: avl_usines.c avl_usines.h utils.h
	$(CC) $(CFLAGS) -c avl_usines.c

avl_recherche.o: avl_recherche.c avl_recherche.h utils.h
	$(CC) $(CFLAGS) -c avl_recherche.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

lecture_csv.o: lecture_csv.c lecture_csv.h avl_usines.h avl_recherche.h utils.h
	$(CC) $(CFLAGS) -c lecture_csv.c

calculs.o: calculs.c calculs.h avl_recherche.h
	$(CC) $(CFLAGS) -c calculs.c

clean:
	rm -f $(OBJS) $(TARGET)

cleanfile:
	rm -f *.dat *.log *.png *.tmp *.gp

cleanall: clean cleanfile

.PHONY: all clean cleanfile cleanall
