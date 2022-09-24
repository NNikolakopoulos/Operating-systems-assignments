main:main.o hashImpl.o invImpl.o funImpl.o
	gcc -g3 -o main main.o hashImpl.o invImpl.o funImpl.o

main.o:main.c
	gcc -g3 -c main.c

hashImpl.o:hashImpl.c
	gcc -g3 -c hashImpl.c

invImpl.o:invImpl.c
	gcc -g3 -c invImpl.c

funImpl.o:funImpl.c
	gcc -g3 -c funImpl.c

clean:
	rm -f main.o hashImpl.o funImpl.o invImpl.o