all:
	mpicc main.c -o app -ansi

run: all
	mpirun app input.txt

host: all
	mpirun --hostfile host app input.txt
