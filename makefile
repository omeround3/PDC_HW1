build:
	mpicc -o exec static_gcd.c -lm

clean:
	rm -f *.o

run:
	mpiexec -n 4 ./exec