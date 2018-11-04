// Author: Josh Morris
// Course: 4330
// Professor: Dr. Pettey
// Lab 2
//
// Pthread program that performs gaussian elmination for up to 10 uknowns.
// Threads are created once and each one handles a column of the augmented 
// matrix created by the user's supplied equations. Main handles the 
// calculations of the divisor and multipliers for each 
//
// 		input:	Number of equations
// 						A array
//						B array
//
//		output: X values


#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define MAXCOL 11
#define MAXROW 10

int numEqs; // num rows
int numCols;
int finThreads; // counter to keep track of threads done calculating
int unknown; // variable to keep track of how many variables to solve for
double divisor;
double multiplier[MAXROW];
double matrix[MAXROW][MAXCOL]; // matrix container
pthread_mutex_t mv = PTHREAD_MUTEX_INITIALIZER; //mutex variable
pthread_cond_t maincv = PTHREAD_COND_INITIALIZER; //main control variable
pthread_cond_t threadcv = PTHREAD_COND_INITIALIZER; //thread control variable

void * processCol(void * colNum);
void printMatrix();

int main() {
	int col, row; //loop variables
	int id[MAXCOL]; // keep track of column thread IDs
	pthread_t thread[MAXCOL]; //thread id array

	// get number of equations from user
	printf("Input the number of items");
	scanf("%i", &numEqs);
	numCols = numEqs + 1;

	printf("Input the A array:\n");
	// read in the A array to the matrix
	for (row = 0; row < numEqs; ++row) {
		for (col = 0; col < numEqs; ++col) {
			scanf("%lf", &matrix[row][col]);
		}
	}

	printf("Input the B array:\n");
	// read B array into numeqs col of matrix
	for (row = 0; row < numEqs; ++row) {
		scanf("%lf", &matrix[row][numEqs]);
	}

	// create threads
	finThreads = 0;
	for (col = 0; col < numCols; ++col) {
		id[col] = col;
		pthread_create(&thread[col],NULL,processCol, (void*)&id[col]);
	}

	// for all unknown values
	for (unknown = 0; unknown < numEqs; ++unknown) {
		pthread_mutex_lock(&mv);
			//printMatrix();
			// make sure all threads are ready
			while (finThreads < numCols) {
				while(pthread_cond_wait(&maincv, &mv) != 0);
			}
			finThreads = 0;
		pthread_mutex_unlock(&mv);

		//determine divisor
		divisor = matrix[unknown][unknown];

		//determine multiplier
		for (row = 0; row < numEqs; ++row) {
			multiplier[row] = matrix[row][unknown];
		}

		// set off threads
		pthread_mutex_lock(&mv);
		pthread_cond_broadcast(&threadcv);
		pthread_mutex_unlock(&mv);
	}

	//wait for all threads to finish
	for(col = 0; col < numCols; ++col) {
		pthread_join(thread[col],NULL);
		//printf("thread %d joined\n", col);
	}

	//output x values
	printf("the X values are: \n");
	for(row = 0; row < numEqs; ++row) {
		printf("%lf \n", matrix[row][numEqs]);
	}

} //end main

void * processCol(void * colNum) {
	int me = *(int *) colNum; // this process's column
	int myRow; // local row count variable
	int myUnknown = 0;

	for (myUnknown = 0; myUnknown < numEqs; ++myUnknown) {
		pthread_mutex_lock(&mv);
		++finThreads;
		if (finThreads == numCols) {
			pthread_cond_broadcast(&maincv);
			pthread_cond_wait(&threadcv, &mv);
		} else {
			pthread_cond_wait(&threadcv, &mv);
		}
		//printf("thread %d myunknown: %d unknown: %d \n", me, myUnknown, unknown);
		pthread_mutex_unlock(&mv);

		matrix[myUnknown][me] /= divisor;

		// for all rows in col
		for (myRow = 0; myRow < numEqs; ++myRow){
			// if not pivot row element multiply pivot row element by multiplier of 
			//cur row and subtract from cur row element
			if(myRow != myUnknown) {
				matrix[myRow][me] -= multiplier[myRow] * matrix[myUnknown][me]; 
			}
		}
	}
	return;
}

void printMatrix(){
	int myRow, myCol;

	for (myRow = 0; myRow < numEqs; ++myRow) {
		for (myCol = 0; myCol < numCols; ++myCol) {
			printf("%lf \t", matrix[myRow][myCol]);
		}

		printf("\n");
	}
}