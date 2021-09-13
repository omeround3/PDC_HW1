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
int *UserInputFunction(int *input_numbers, int pairs_num, char *buffer)
{
	// Declare variables
	char c;
	int i, num1, num2;
	int arraySize = pairs_num * 2;	// The number of pairs * 2 is the needed size for the integers array
	// Initalize array
	input_numbers = (int *)malloc(sizeof(int) * arraySize); 
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
		input_numbers[i] = num1;
		input_numbers[i + 1] = num2;
	}
	return input_numbers;
}

/* The Root process is dealing with all the input/output and managing the other process. */
int main(int argc, char **argv)
{
	// Declare variables
	int i, world_rank, pairs_num;
	int process_num;
	char buffer[100];
	double time;
	int *input_numbers, *MPI_results, * procs_nums;
	
	// Initalize MPI program
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_num);

	// If the process is the Root process. It will get the input from the user.
	if (world_rank == ROOT)
	{	
		// Get the number of integers pairs
		fgets(buffer, 5, stdin);
		sscanf(buffer, "%d", &pairs_num);

		// Get the integers pairs into the array <input_numbers>
		UserInputFunction(input_numbers, pairs_num, buffer);
		MPI_results = (int *)malloc(4 * pairs_num);
	}


	time = MPI_Wtime();	// Variable to measure time of calculation
	
	// Calculate array size and make it even
	int arraySize = 2 * pairs_num;
	
	// Broacast the array size variable to all process so they can allocate correctly
	MPI_Bcast(&arraySize, 1, MPI_INT, ROOT, MPI_COMM_WORLD); 

	int per_process_size = arraySize / process_num;
	per_process_size += per_process_size % 2;
	
	// Allocate arrays in the non-root process
	if (world_rank != ROOT)
	{
		procs_nums = (int *)malloc(per_process_size * sizeof(int));
		MPI_results = (int *)malloc((per_process_size / 2) * sizeof(int));
	}

	// Send chuncks of the array to to worker processes
	MPI_Scatter(input_numbers, per_process_size, MPI_INT, procs_nums, per_process_size, MPI_INT, ROOT, MPI_COMM_WORLD); 

	if (world_rank + 1 == process_num)
	{
		if (per_process_size > arraySize - (per_process_size * (process_num - 1)))
		{
			for (i = 0; i < arraySize - per_process_size * (process_num - 1); i += 2)
				MPI_results[i / 2] = gcd(input_numbers[i], input_numbers[i + 1]);
		}
		else
		{
			for (i = 0; i < per_process_size; i += 2)
				MPI_results[i / 2] = gcd(input_numbers[i], input_numbers[i + 1]);
		}
	}
	else
	{
			for (i = 0; i < per_process_size; i += 2)
			MPI_results[i / 2] = gcd(input_numbers[i], input_numbers[i + 1]);

	}
	int new_size_split = per_process_size / 2;
	MPI_Gather(MPI_results, new_size_split, MPI_INT, MPI_results, new_size_split, MPI_INT, ROOT, MPI_COMM_WORLD);

	if (world_rank == ROOT)
	{
		for (i = (per_process_size * process_num); i < arraySize; i += 2)
			MPI_results[i / 2] = gcd(input_numbers[i], input_numbers[i + 1]);

		printf("\nStatic time: %lf\n", MPI_Wtime() - time);
		
		for (i = 0; i < arraySize; i += 2)
		{
			printf("%d %d \tgcd: %d\n", input_numbers[i], input_numbers[i + 1], MPI_results[i / 2]);
		}
	}

	free(MPI_results);
	free(input_numbers);
	MPI_Finalize();
	return 0;
}
