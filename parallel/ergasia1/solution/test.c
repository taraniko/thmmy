/*
 bitonic.c 

 This file contains two different implementations of the bitonic sort
        recursive  version :  rec
        imperative version :  impBitonicSort() 
 

 The bitonic sort is also known as Batcher Sort. 
 For a reference of the algorithm, see the article titled 
 Sorting networks and their applications by K. E. Batcher in 1968 


 The following codes take references to the codes avaiable at 

 http://www.cag.lcs.mit.edu/streamit/results/bitonic/code/c/bitonic.c

 http://www.tools-of-computing.com/tc/CS/Sorts/bitonic_sort.htm

 http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/bitonicen.htm 
 */

/* 
------- ---------------------- 
   Nikos Pitsianis, Duke CS 
   Taras Nikos, Auth Student
-----------------------------

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#include <omp.h>
#include <cilk/cilk.h>


struct timeval startwtime, endwtime;
double seq_time;

pthread_mutex_t n_mutex;

typedef struct{
    int lo, cnt, dir;
} targ;

int N;        // data array size
int *a;       // data array to be sorted
int n;		    // number of threads	 


int nThreads = 1;          // number of active threads
int threshold;             // quicksort is used below this threshold

const int ASCENDING  = 1;
const int DESCENDING = 0;

void init(void);
void print(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);

void sort( void );
void impBitonicSort( void );
void recBitonicSort( int lo, int cnt, int dir );
void bitonicMerge( int lo, int cnt, int dir );

void pThreadSort( void );
void * pThreadBitonicSort( void * arg );
void * pThreadBitonicMerge( void * arg );

void openmpBitonicSort( void );

void cilkBitonicSort(void);


void cilksort(void);
void cilkrecBitonicSort(int lo, int cnt, int dir);
void cilkbitonicMerge(int lo, int cnt, int dir);

void filewrite(int q, int p, double t1, double t2, double t3, double t4, double t5,double t6);

/* quicksort compare functions */
int asc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 < *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}

int desc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 > *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}


/* program flow */ 
int main( int argc, char **argv ) {

    if (argc != 3 || atoi( argv[ 2 ] ) > 256 ) {
        printf( "Usage: %s q p\n  where N=2^q is problem size (power of two), and 2^p is the number of threads, <=256.\n", argv[ 0 ] );
        exit( 1 );
    }

    N = 1 << atoi( argv[ 1 ] ); 
    n = 1 << atoi( argv[ 2 ] );

    threshold = N/n;

    double t1,t2,t3,t4,t5,t6;


    // set max number of cilk threads
    char str[5];
    sprintf(str, "%d", n);
    __cilkrts_set_param("nworkers", str);

    // initialize pthread mutex
    pthread_mutex_init(&n_mutex, NULL);
    
    // allocate memory for a array
    a = (int *) malloc( N * sizeof( int ) );
    
    // serial quicksort
    init();
    gettimeofday( &startwtime, NULL );
    qsort( a, N, sizeof( int ), asc );
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    t1 = seq_time;
    printf( "Qsort time = %f\n", seq_time );
    test();
   
    // bitonic serial recursive
    init();
    gettimeofday( &startwtime, NULL );
    sort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    t2 = seq_time;
    printf( "Bitonic serial recursive time = %f\n", seq_time );
    test();

    // bitonic pthreads recursive
    init();
    gettimeofday( &startwtime, NULL );
    pThreadSort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    t3 = seq_time;
    printf( "Bitonic pthreads with %i threads time = %f\n", 1 << atoi( argv[ 2 ] ), seq_time );
    test();

    // bitonic serial imperative
    init();
    gettimeofday( &startwtime, NULL );
    impBitonicSort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    t4 = seq_time;
    printf( "Bitonic serial imperative time = %f\n", seq_time );
    test();
    
    // bitonic openmp imperative 
    init();
    gettimeofday( &startwtime, NULL );
    openmpBitonicSort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    t5 = seq_time;
    printf( "Bitonic omp with %i threads time = %f\n", 1 << atoi( argv[ 2 ] ),  seq_time );
    test();

    // bitonic cilk recursive
    init();
    gettimeofday( &startwtime, NULL );
    cilksort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    t6 = seq_time;
    printf( "Bitonic cilk recursive with %i threads time = %f\n", 1 << atoi( argv[ 2 ] ),  seq_time );
    test();

    filewrite(atoi(argv[1]),atoi(argv[2]),t1,t2,t3,t4,t5,t6);

}

/* procedure filewrite()
   writes the results to a file */
void filewrite(int q,int p,double t1, double t2, double t3, double t4, double t5,double t6) {
  FILE *pFile;
  pFile=fopen("results.csv", "a");
  if(pFile==NULL) {
    perror("Error opening file.");
    exit(1);
  }
  fprintf(pFile,"%d,%d,%f,%f,%f,%f,%f,%f\n",q,p,t1,t2,t3,t4,t5,t6 );
  fclose(pFile);
  printf("Successfully stored results!\n");
}





/* procedure test() : verify sort results */
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; 
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
  }
  printf("\n");
}


/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}



/** procedure compare() 
   The parameter dir indicates the sorting direction, ASCENDING 
   or DESCENDING; if (a[i] > a[j]) agrees with the direction, 
   then a[i] and a[j] are interchanged.
**/
inline void compare(int i, int j, int dir) {
  if (dir==(a[i]>a[j])) 
    exchange(i,j);
}


/** Procedure bitonicMerge() 
   It recursively sorts a bitonic sequence in ascending order, 
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted. 
 **/
void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}



/** function recBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void recBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    recBitonicSort(lo, k, ASCENDING);
    recBitonicSort(lo+k, k, DESCENDING);
    bitonicMerge(lo, cnt, dir);
  }
}


/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
  recBitonicSort(0, N, ASCENDING);
}



/*
  imperative version of bitonic sort
*/
void impBitonicSort() {

  int i,j,k;
  
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      for (i=0; i<N; i++) {
  int ij=i^j;
  if ((ij)>i) {
    if ((i&k)==0 && a[i] > a[ij]) 
        exchange(i,ij);
    if ((i&k)!=0 && a[i] < a[ij])
        exchange(i,ij);
  }
      }
    }
  }
}


/** Function pThreadBitonicMerge
 *  If active threads < max threads the function spawns a new thread to split the work
 **/
void * pThreadBitonicMerge( void * arg ){
    int lo = ((targ*)arg) -> lo;
    int cnt = ((targ*)arg) -> cnt;
    int dir = ((targ*)arg) -> dir;
    if( cnt > 1 ){
        int k = cnt / 2;
        int i;
        for( i = lo; i < lo + k; ++i ){
            compare( i, i + k, dir );
        }
        int t1 = 0; // has spawned thread? default = 0
        targ arg1,arg2;

        pthread_t thread1;
        
        arg1.lo = lo;
        arg1.cnt = k;
        arg1.dir = dir;
        
        arg2.lo = lo + k;
        arg2.cnt = k;
        arg2.dir = dir;
      
        if (nThreads<n){  
          pthread_create( &thread1, NULL, pThreadBitonicMerge, &arg1 );
          pthread_mutex_lock(&n_mutex);
          nThreads++;
          pthread_mutex_unlock(&n_mutex);
          t1 = 1; // a thread has been spawned
        }
        else
          pThreadBitonicMerge(&arg1);

        
        pThreadBitonicMerge(&arg2);

        if (t1){
          pthread_join( thread1, NULL );
          pthread_mutex_lock(&n_mutex);
          nThreads--;
          pthread_mutex_unlock(&n_mutex);
        }

    }
    return 0;
}

/** function pThreadBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
    Spawns a new thread if active threads are less than max thread number
    
 **/

void * pThreadBitonicSort( void * arg ){
    int lo = ((targ*)arg) -> lo;
    int cnt = ((targ*)arg) -> cnt;
    int dir = ((targ*)arg) -> dir;

    if ( cnt > 1 ) {
        int k = cnt / 2;
        if( cnt<=threshold ) {  // if array size is less or equal to N/n
            qsort( a + lo, k, sizeof( int ), asc );  // use quicksort 
            qsort( a + ( lo + k ) , k, sizeof( int ), desc );  
        }
        else{

            int t1 = 0;
            
            targ arg1, arg2;

            pthread_t thread1;
            
            arg1.lo = lo;
            arg1.cnt = k;
            arg1.dir = ASCENDING;

            arg2.lo = lo + k;
            arg2.cnt = k;
            arg2.dir = DESCENDING;
            
            if (nThreads<n){  
              pthread_create( &thread1, NULL, pThreadBitonicSort, &arg1 );
              pthread_mutex_lock(&n_mutex);
              nThreads++;
              pthread_mutex_unlock(&n_mutex);
              t1 = 1;
            }
            else
            	pThreadBitonicSort(&arg1);
            
            
            pThreadBitonicSort(&arg2);

            if (t1){
              pthread_join( thread1, NULL );
              pthread_mutex_lock(&n_mutex);
              nThreads--;
              pthread_mutex_unlock(&n_mutex);
            }
        }

        targ arg3;
        arg3.lo = lo;
        arg3.cnt = cnt;
        arg3.dir = dir;
            
        pThreadBitonicMerge( &arg3 );
        
    }
    return 0;
}

/** function pThreadSort() 
   Calls pThreadBitonicSort to sort the array in ASCENDING order
 **/
void pThreadSort() {
    targ arg;
    arg.lo = 0;
    arg.cnt = N;
    arg.dir = ASCENDING;
    
    pThreadBitonicSort( &arg );
}



/*
  imperative version of bitonic sort using omenmp

*/
void openmpBitonicSort() {
    omp_set_num_threads( n ); 
    int i,j,k=0;
    for (k = 2; k <= N; k *= 2 ) {
        for (j=k>>1; j>0; j=j>>1) {
            #pragma omp parallel for
            for (i=0; i<N; i++) {
                int ij=i^j;
                if ((ij)>i) {
                    if ((i&k)==0 && a[i] > a[ij]) {
                        exchange(i,ij);
                    }
                    if ((i&k)!=0 && a[i] < a[ij]){
                        exchange(i,ij);
                    }
                }
            }
        }
    }
} 


/*new cilk recursive */

/** function cilkbitonicMerge() 
    Uses cilk threads 
 **/
void cilkbitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    cilk_spawn cilkbitonicMerge(lo, k, dir);
    cilkbitonicMerge(lo+k, k, dir);
    cilk_sync;
  }
}



/** function cilkrecBitonicSort() 
    uses cilk threads. If array size <= threshold, qsort is used. 
 **/
void cilkrecBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
  	int k=cnt/2;
  	if(cnt<=threshold){
    	cilk_spawn cilkrecBitonicSort(lo, k, ASCENDING);
    	cilkrecBitonicSort(lo+k, k, DESCENDING);
    	cilk_sync;
	}
	else{
		cilk_spawn qsort(a+lo,k,sizeof(int),asc);
		qsort(a+(lo+k),k,sizeof(int),desc);
		cilk_sync;
	}
    cilkbitonicMerge(lo, cnt, dir);
  }
}


/** function cilksort() 
   Calls of recBitonicSort to sort the array in ASCENDING order
 **/
void cilksort() {
  cilkrecBitonicSort(0, N, ASCENDING);
}
