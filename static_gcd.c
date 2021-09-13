#include "/usr/include/mpi/mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum ranks
{
	ROOT
};

/* A function to calculate the GCD of 2 given integers recursivly */
int gcd(int a, int b)
{
	if (a == 0)
		return b;
	return gcd(b % a, a);
}

/* 
This functions gets 2 pairs of integers into an array.
The array size is determined dynamically by the value of pairs_num
*/
int *UserInputFunction(int *arr, int pairs_num, char *buffer)
{
	// Declare variables
	char c;
	int i, num1, num2;
	int arraySize = pairs_num * 2;	// The number of pairs * 2 is the needed size for the integers array
	// Initalize array
	arr = (int *)malloc(sizeof(int) * arraySize); 
	// Get input from the user
	for (i = 0; i < arraySize; i += 2)
	{
		fgets(buffer, 100, stdin);
		sscanf(buffer, " %d %d%c", &num1, &num2, &c);
		if (c == ' ')
		{
			printf("illegal input at line %d\n", i);
			exit(1);
		}
		arr[i] = num1;
		arr[i + 1] = num2;
	}
	return arr;
}

/* The Root process is dealing with all the input/output and managing the other process. */
int main(int argc, char **argv)
{
	// Declare variables
	int i, process, my_rank, pairs_num , arraySize;
	char buffer[100];
	double time;
	int *arr, *MPI_results;
	
	// Initalize MPI program
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process);

	// If the process is the Root process. It will get the input from the user.
	if (my_rank == ROOT)
	{	
		// Get the number of integers pairs
		fgets(buffer, 5, stdin);
		sscanf(buffer, "%d", &pairs_num);

		// Get the integers pairs into the array <arr>
		UserInputFunction(arr, pairs_num, buffer);
		MPI_results = (int *)malloc(4 * pairs_num);
	}


	time = MPI_Wtime();	// Variable to measure time of calculation
	int new_size_even = arraySize / process;
	MPI_Bcast(&arraySize, 1, MPI_INT, ROOT, MPI_COMM_WORLD); 
	new_size_even += new_size_even % 2;
	if (my_rank != ROOT)
	{
		arr = (int *)malloc(4 * new_size_even);
		MPI_results = (int *)malloc(4 * (new_size_even / 2));
	}

	MPI_Scatter(arr, new_size_even, MPI_INT, arr, new_size_even, MPI_INT, ROOT, MPI_COMM_WORLD); // sent the array to everybody

	if (my_rank + 1 == process)
	{
		if (new_size_even > arraySize - (new_size_even * (process - 1)))
		{
			for (i = 0; i < arraySize - new_size_even * (process - 1); i += 2)
				MPI_results[i / 2] = gcd(arr[i], arr[i + 1]);
		}
		else
		{
			for (i = 0; i < new_size_even; i += 2)
				MPI_results[i / 2] = gcd(arr[i], arr[i + 1]);
		}
	}
	else
	{
			for (i = 0; i < new_size_even; i += 2)
			MPI_results[i / 2] = gcd(arr[i], arr[i + 1]);

	}
	int new_size_split = new_size_even / 2;
	MPI_Gather(MPI_results, new_size_split, MPI_INT, MPI_results, new_size_split, MPI_INT, ROOT, MPI_COMM_WORLD);

	if (my_rank == ROOT)
	{
		for (i = (new_size_even * process); i < arraySize; i += 2)
			MPI_results[i / 2] = gcd(arr[i], arr[i + 1]);

		printf("\nStatic time: %lf\n", MPI_Wtime() - time);
		
		for (i = 0; i < arraySize; i += 2)
		{
			printf("%d %d \tgcd: %d\n", arr[i], arr[i + 1], MPI_results[i / 2]);
		}
	}

	free(MPI_results);
	free(arr);
	MPI_Finalize();
	return 0;
}
