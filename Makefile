all: c-wildwater

utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o

avl_usines.o: avl_usines.c avl_usines.h utils.h structures.h
	gcc -c avl_usines.c -o avl_usines.o

avl_recherche.o: avl_recherche.c avl_recherche.h utils.h structures.h
	gcc -c avl_recherche.c -o avl_recherche.o

lecture_csv.o: lecture_csv.c lecture_csv.h utils.h structures.h avl_usines.h avl_recherche.h
	gcc -c lecture_csv.c -o lecture_csv.o

calculs.o: calculs.c calculs.h structures.h
	gcc -c calculs.c -o calculs.o

main.o: main.c avl_usines.h avl_recherche.h utils.h structures.h lecture_csv.h calculs.h
	gcc -c main.c -o main.o

c-wildwater: main.o avl_usines.o avl_recherche.o utils.o lecture_csv.o calculs.o
	gcc main.o avl_usines.o avl_recherche.o utils.o lecture_csv.o calculs.o -o c-wildwater

clean:
	rm -f *.o c-wildwater

cleanfile:
   rm -f vol_max_*.dat vol_captation_*.dat vol_traitement_*.dat
   	rm -f vol_all_*.dat leaks_*.dat *.png *.sorted *.header
.PHONY: all clean cleanfile
