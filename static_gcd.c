#include <stdio.h>
#include <math.h>
#include "/usr/include/mpi/mpi.h"
#include <time.h>
#include <string.h>

#define BUFF_SIZE 100

enum ranks { ROOT };

/* Functions Declartions */

/* This function performs GCD on two given numbers */
int gcd(int a, int b) 
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

int * getInput(int * num_pairs, int * pairs)
{
    FILE * file = fopen("input.txt", "r");
    if (file)
    {
        char buffer[BUFF_SIZE];
        char c;
        int num1, num2;

        fgets(buffer, BUFF_SIZE, file);
        sscanf(buffer, "%d", num_pairs); // save the number of pairs
		int pairsArrSize = 2 * (*num_pairs);
		pairs = (int*)malloc(sizeof(int)*pairsArrSize); // will keep the numbers

		for (int i = 0; i < pairsArrSize; i)
        {
			fgets(buffer, BUFF_SIZE, file);
			sscanf(buffer, " %d %d%c", &num1, &num2, &c);
			if (c != '\n'){
				printf("illegal input at line %d\n", i);
			}
			else {
				pairs[i] = num1;
				pairs[i + 1] = num2;
				i += 2; //incrise only if the input is OK
			}
		}
        for (int i = 0; i < pairsArrSize; i++)
            printf("Num %d is: %d\n", i + 1, pairs[i]);
    }
    else
    {
        printf("ERROR: Can't open file for reading.\n");
    }
    fclose(file); // close file
    return pairs; // return pointer of pairs array
}

void masterJob(int num_procs) 
{
	/* Declare Variables */
	int num_pairs;	/* The number of integers pairs. To be read from the input file */
	int * pairs;	/* Integers pairs which the GCD algorithm uses as input */
	int work_size;	/* The size of the job to needed to be sent to each process */	
	int * gcd_results;	/* results of GCD calculations */
	MPI_Status status; 	/* return status for receive */
	int errorCode = MPI_ERR_COMM;

	/* Get number of integer pairs and the pairs themselves into an array */
	pairs = getInput(&num_pairs, pairs);
	work_size = (num_pairs * 2) / (num_procs - 1);
	// work_size += work_size % 2;
	gcd_results = (int*)malloc(sizeof(int) * num_pairs);

	/* Sanity check */
	if (work_size % (num_procs - 1) != 0 || num_procs == 1) {
		/*Number of salves does not split correctly or not enough slaves*/
		MPI_Abort(MPI_COMM_WORLD, errorCode);
	}
	double startTime = MPI_Wtime();
	// Sending each slave an equal part of the job
	int count_work = 0;
	for (int i = 1; i < num_procs; ++i) {
		MPI_Send(pairs + count_work, work_size, MPI_INT, i, 0, MPI_COMM_WORLD);
		count_work += work_size;
	}
	// Receiving results from each slave
	for (int i = 1; i < num_procs; ++i) {
		MPI_Recv(gcd_results[i - 1], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
	}
	// Printing total result
	printf("The time for calcuation is: %f\n", MPI_Wtime() - startTime);
	int arrSize = sizeof(pairs) / sizeof(pairs[0]);
	for (int i = 0; i < arrSize; i += 2)
		print("%d %d\t gcd: %d\n", pairs[i], pairs[i + 1], gcd_results[i / 2]);

	free(pairs);
	free(gcd_results);
}

void salveJob() 
{
	int * pairs;	/* Integers pairs which the GCD algorithm uses as input */
	int work_size;	/* The size of the job to needed to be sent to each process */	
	int * gcd_results;	/* results of GCD calculations */
	MPI_Status status; 	/* return status for receive */

	// Each slave receives his part of the job
	MPI_Recv(pairs, work_size, MPI_DOUBLE, ROOT, 0, MPI_COMM_WORLD, &status);
	for (int start = pairs; start < pairs + work_size; start++)
		gcd_results[start]

	MPI_Send(gcd_results, 1, MPI_DOUBLE, ROOT, 0, MPI_COMM_WORLD);
}



/* Sequential code to be parallelized */
int main(int argc, char *argv[]) 
{
	/* Declare Variables */
	int my_rank;	/* rank of process */
	int num_procs;	/* number of processes */
	

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if (my_rank == ROOT)
	{	
		masterJob(num_procs);
	}
	// else
		salveJob();

	/* shut down MPI */
	MPI_Finalize();
	return 0;
}

