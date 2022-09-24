all:chef saladmaker

chef:chef.o 
	gcc -g3 -o chef chef.o -lpthread

chef.o:chef.c
	gcc -g3 -c chef.c -lpthread

saladmaker:saladmaker.o 
	gcc -g3 -o saladmaker saladmaker.o -lpthread
	
saladmaker.o:saladmaker.c
	gcc -g3 -c saladmaker.c -lpthread





clean:
	rm -f chef.o saladmaker.o log.txt chef saladmaker