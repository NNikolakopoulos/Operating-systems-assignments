all:primes internal leaf

primes:primes.o root.o functions.o listImpl.o
	gcc -o primes primes.o root.o functions.o listImpl.o -lm
internal:internal.o functions.o listImpl.o
	gcc -o internal internal.o functions.o listImpl.o -lm
leaf:leaf.o functions.o
	gcc -o leaf leaf.o functions.o -lm


functions.o:functions.c
	gcc -c functions.c

primes.o:primes.c
	gcc -c primes.c

root.o:root.c
	gcc -c root.c

internal.o:internal.c
	gcc -c internal.c

leaf.o:leaf.c
	gcc -c leaf.c

listImpl.o:listImpl.c
	gcc -c listImpl.c


clean:
	rm -f primes.o root.o internal.o