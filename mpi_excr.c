// This code balances load and calculates sin(x) for every element in the array a.
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MASTER 0

int main (int argc, char *argv[]) 
{
	
	int i;
	int n = 10;
	double *a; //data stored in a
	double *b; // array used to receive data from process 0 to process 1
	int total_proc;	 // total nuber of processes	
	int rank;        // rank of each process
	int n_items; //count of elements received
	int n_per_proc;//elements per process
	int num; //final length of array after balancing
	double *bp; //subarray

	// Initialization of MPI environment
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &total_proc);//total number of processes 
	
	MPI_Comm_rank (MPI_COMM_WORLD,&rank);// rank of each process
	
	// To know the number of elements in the array, the data is sent from process 0 to process 1, then
	//MPI_Probe and MPI_Get_count is used to count the incoming message from process 0, the count is stored in variable n_items.
	//The count stored in n_items will be used to divide the work equally among all ranks.
	if(rank==0){
	a = (double *) malloc(sizeof(double)*n);// allocating memory to store generated data

	for(i=0;i<n;i++){
		a[i] = i;//storing angles in a 
		}	

	MPI_Send(a, n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);//sending data to process 1
	
	}
	
	else if(rank==1){
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);//probing to check incoming data

		
		MPI_Get_count(&status, MPI_DOUBLE, &n_items);//counting the number of elements in the message and storing in n_times
		printf("items = %d\n", n_items);

		n_per_proc = n_items/total_proc; // chunks per process

		//checking to see if the work can be divided equally
			if(n_items%total_proc != 0)
				{
						n_per_proc+=1;
						num = n_per_proc*total_proc;//total number of elements in an array needed to balance
						
						
				}
			else
				num = n_items; // if balanced, then leave it as it is
				
					
			b = (double *) malloc(sizeof(double)*num);
			MPI_Recv(b, n_items, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);// receiving data from process 1
			for(int i = n_items; i<num;i++){
				b[i]=0;// adding extra elements if needed to divide the work equally
			}
	}
	
	MPI_Bcast(&num, 1, MPI_INT,1,MPI_COMM_WORLD);// broadcasting num
	MPI_Bcast(&n_per_proc, 1, MPI_INT,1,MPI_COMM_WORLD);// broadcasting n_per_proc

	bp = (double *) malloc(sizeof(double)*n_per_proc);// allocating memory for subarray
    MPI_Scatter(b, n_per_proc, MPI_DOUBLE, bp, n_per_proc, MPI_DOUBLE, 1, MPI_COMM_WORLD); // scattering data in equal chunks, n_per_proc was calculated above.

    for(i=0;i<n_per_proc;i++){
        bp[i] = sin(bp[i]); // calculating sin(x) for each element and storing in parts
    }

    MPI_Gather(bp, n_per_proc, MPI_DOUBLE, b, n_per_proc, MPI_DOUBLE, 1, MPI_COMM_WORLD); //gathering.
	
	if(rank==1){
		MPI_Bcast(b, n_per_proc, MPI_DOUBLE,1,MPI_COMM_WORLD);
		//printf("num=%d\n", num);
		//printf("n=%d\n", n_per_proc);
		for(int i = 0; i<num;i++){
			printf("b=%f\n",b[i]);
		}   
         }
	MPI_Finalize();
	//Terminate MPI Environment
	return 0;
}