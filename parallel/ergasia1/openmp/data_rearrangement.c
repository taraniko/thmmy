#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <omp.h>



#define DIM 3


void data_rearrangement(float *Y, float *X, 
			unsigned int *permutation_vector, 
			int N){
  omp_set_dynamic(0);
  omp_set_num_threads(2);
  #pragma omp parallel for
    for(int i=0; i<N; i++){
      memcpy(&Y[i*DIM], &X[permutation_vector[i]*DIM], DIM*sizeof(float));
    }

}
