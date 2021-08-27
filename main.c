#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 100

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

// Sequential code to be parallelized
int main(int argc, char *argv[]) 
{
    int num_pairs;
    int * pairs;
    pairs = getInput(&num_pairs, pairs);
    printf("------ IN MAIN ------\n");
    for (int i = 0; i < num_pairs * 2; i++)
        printf("Num %d is: %d\n", i + 1, pairs[i]);


}




