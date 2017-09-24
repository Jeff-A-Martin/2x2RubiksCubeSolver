all: solver

solver: solver.o cube.o state_table.o queue.o
	gcc -g solver.o cube.o state_table.o queue.o -o solver

solver.o: solver.c cube.h state_table.h
	gcc -g -c solver.c

cube.o: cube.c cube.h
	gcc -g -c cube.c

state_table.o: state_table.c state_table.h queue.h cube.h
	gcc -g -c state_table.c

queue.o: queue.c queue.h
	gcc -g -c queue.c

clean: 
	rm -f solver
	rm -f *.o
