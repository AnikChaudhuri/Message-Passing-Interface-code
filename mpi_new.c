
// This code balances load and calvulates sin(x) for every element in the array a.
// The data is scattered into array ap of size n_per_poc.
// Computed sin(x) on ap
// Gather the data from ap to array a

#include "mpi.h"      // must have a system with an MPI library
#include <stdio.h>    //printf
#include <stdlib.h>   //malloc
#include <math.h>

#define MASTER 0         //MASTER
#define ARRAY_SIZE 10    //Size of arrays 


int main (int argc, char *argv[]) 
{
    double * a;
	double * ap;
	
	int total_proc;	 // total nuber of processes	
	int rank;        // rank of each process
	int n_per_proc;	// elements per process	
	int n = ARRAY_SIZE;   // number of array elements
	int i;       // loop index
    int num; // actual size of array after balancing 
		
	// Initialization of MPI environment
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &total_proc); //total no. of processes
	
	MPI_Comm_rank (MPI_COMM_WORLD,&rank); //rank of each process

    
   
	
    n_per_proc = n/total_proc; // chunks per process
    //printf("%d\n", n_per_proc);

    //checking to see if the work can be divided equally, 
    //if the work can not be divided equally then extra elements will be added.
    if(n%total_proc != 0)
			{
			    	n_per_proc+=1;
                    num = n_per_proc*total_proc;//total number of elements in an array needed to balance 
			    	
			}
    else
        num = n; // if balanced, then leave it as it is

    if (rank == MASTER)  {
		a = (double *) malloc(sizeof(double)*num);
		
		// initialize arrays a with consecutive integer values
		for(i=0;i<n;i++)
			a[i] = i;

        for(i = n; i<num; i++)//extra elements are set to 0
            a[i] = 0;
		
	}

    ap = (double *) malloc(sizeof(double)*n_per_proc);
    MPI_Scatter(a, n_per_proc, MPI_DOUBLE, ap, n_per_proc, MPI_DOUBLE, MASTER, MPI_COMM_WORLD); // scattering array a from MASTER

    for(i=0;i<n_per_proc;i++){
        ap[i] = sin(ap[i]); // calculating sin(x) for each element and storing in parts
    }

    MPI_Gather(ap, n_per_proc, MPI_DOUBLE, a, n_per_proc, MPI_DOUBLE, MASTER, MPI_COMM_WORLD); //gathering from all ap array and storing into a.
		


        // if(rank==MASTER){
        //     //printf("%d\n", num);
        //     for(int j = 0; j<n;j++){
        //     //printf("%f\n", a[j]);
        //     }
        // }

    if(rank==MASTER){
        for(int j = 0; j<num;j++){
            if(sin(j) == a[j]){
                printf("all good, loop %d\n",j);
            }
        }
    }

    MPI_Finalize();

    
    //printf("%d\n", n_per_proc);
    


}