#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main(){
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int num = 21;

    int r = num % world_size;
    int q = num/world_size;
    int size = world_rank/(world_size-1);
    int size1 = (r*size) + q;

    //double* bp = (double*)malloc(sizeof(double)*(r*(world_rank/(world_size-1))+q));
    double *bp = (double*)malloc(sizeof(double)*size1); //sub array
    double *a = (double*)malloc(sizeof(double)*num); //main array
    
    printf("rank is %d, size = %d\n", world_rank, size1);
    MPI_Barrier(MPI_COMM_WORLD);

    // for(int i = 0; i<size1; i++){
    //     bp[i] = i*2;
    // }

    for(int i = 0; i<num; i++){
        a[i] = i*2;
    }

    int count[world_size];
    int disp[world_size]; disp[0] = 0;

    for(int i = 0; i<world_size-1; i++){
        count[i] = q;
    }
    count[world_size - 1] = q + r;

    for(int i = 1; i<world_size; i++){
        disp[i] = disp[i-1] + count[i-1];
    }

    // int count[4] = {2, 2, 2, 4};
    //int disp[4] = {0, 2, 4, 6};
    MPI_Scatterv(a, count, disp, MPI_DOUBLE, bp, size1, MPI_DOUBLE, 0, MPI_COMM_WORLD); //scattering a to bp

    for(int i = 0; i<size1; i++){
        bp[i] = bp[i] + 2;
    }

    for(int i = 0; i<world_size; i++){
        if(world_rank==i){
            for(int i =0; i<size1; i++){
            printf("rank is %d, element = %f\n", world_rank, bp[i]);

            }
        }
        MPI_Barrier(MPI_COMM_WORLD);


    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(bp, size1, MPI_DOUBLE, a, count, disp, MPI_DOUBLE, 0, MPI_COMM_WORLD); //gathering from bp to a

    if(world_rank==0){
        for(int i =0; i<num; i++){
            printf("rank is %d, element = %f\n", world_rank, a[i]);

            }
    }


    MPI_Finalize();
}