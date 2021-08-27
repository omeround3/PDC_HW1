#include <stdio.h>
#include <math.h>
#include "/usr/include/mpi/mpi.h"
#include <time.h>
#include <string.h>

#define SIZE 40
#define BUFF_SIZE 100

enum ranks { ROOT };

// Functions Declartions
int * getInput(int * num_pairs, int * pairs);
void masterJob(int count, double answer, int x, int num_procs, MPI_Status status);
void salveJob(int x, double answer, int count, MPI_Status status, int size);

// This function performs GCD on two given numbers
int gcd(int a, int b) 
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

// Sequential code to be parallelized
int main(int argc, char *argv[]) 
{
	/* Declare Variables */
	int * gcd_results;	// results of GCD calculations
	double time; // holds time of calculation
	int my_rank; /* rank of process */
	int num_procs; /* number of processes */
	int chunk_size;
	MPI_Status status; /* return status for receive */

	/* start up MPI */
	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	int errorCode = MPI_ERR_COMM;

	/* Sanity check */
	if (SIZE % (num_procs - 1) != 0 || num_procs == 1) {
		/*Number of salves does not split correctlyor not enough slaves*/
		MPI_Abort(MPI_COMM_WORLD, errorCode);
	}

	int count = SIZE / (num_procs - 1);

	if (my_rank == ROOT)
	{
		int num_pairs;
		int * pairs;
		pairs = getInput(&num_pairs, pairs);
		gcd_results = (int*)malloc(sizeof(int) * num_pairs);
		// masterJob(count, answer, x, num_procs, status);
	}
	// else
		// salveJob(x, answer, count, status, size);

	/* shut down MPI */
	MPI_Finalize();
}

void masterJob(int count, double answer, int x, int num_procs, MPI_Status status) 
{


	double t1 = MPI_Wtime();
	double total = 0;

	//Sending each slave an equal part of the job
	for (int i = 1; i < num_procs; ++i) {
		MPI_Send(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		x += count;
	}
	//Receiving results from each slave
	for (int i = 1; i < num_processes; ++i) {
		MPI_Recv(&answer, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
		total += answer;
	}
	//Printing total result
	printf("answer = %e, time is %f\n", total, MPI_Wtime() - t1);

}

void salveJob(int x, double answer, int count, MPI_Status status, int size) 
{

	//Each slave receives his part of the job
	MPI_Recv(&x, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
	for (int start = x; start < count + x; start++)
		for (int y = 0; y < size; y++)
			answer += heavy(start, y);

	MPI_Send(&answer, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
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