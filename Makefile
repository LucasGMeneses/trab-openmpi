all:
	mpicc main.c -o app -ansi

host: all
	mpirun --hostfile host app input.txt

