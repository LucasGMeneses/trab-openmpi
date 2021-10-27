all:
	mpicc main.c -o app -ansi

host: all
	mpirun --hostfile host app test3.txt

