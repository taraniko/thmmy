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


int N;        // data array size
int *a;       // data array to be sorted
int n;		  // number of threads	 


int nThreads = 1;             
int sThreads = 0;			  // DEBUG
int threshold = 1<<15;        // TODO

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

void Psort( void );
void * PrecBitonicSort( void * arg );
void * PbitonicMerge( void * arg );
void PimpBitonicSort( void );
void cilkBitonicSort(void);


void cilksort(void);
void cilkrecBitonicSort(int lo, int cnt, int dir);
void cilkbitonicMerge(int lo, int cnt, int dir);


/** compare for qsort **/
int desc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 > *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}
int asc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 < *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}

/** the main program **/ 
int main( int argc, char **argv ) {

    if (argc != 3 || atoi( argv[ 2 ] ) > 256 ) {
        printf( "Usage: %s q t\n  where n=2^q is problem size (power of two), and t is the number of threads, <=256, to use.\n", argv[ 0 ] );
        exit( 1 );
    }

    N = 1 << atoi( argv[ 1 ] );
    n = 1 << atoi( argv[ 2 ] );

    const char* wow;
    char n_threads_char = (char) n;
    const char* char_pointer;
    char_pointer = &n_threads_char;


    char str[5];
    sprintf(str, "%d", n);
    __cilkrts_set_param("nworkers", str);


    pthread_mutex_init(&n_mutex, NULL);
    
 
    a = (int *) malloc( N * sizeof( int ) );
    init();
    gettimeofday( &startwtime, NULL );
    qsort( a, N, sizeof( int ), asc );
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    printf( "Qsort time = %f\n", seq_time );
//    test();
   
    //init();
    //gettimeofday( &startwtime, NULL );
    //sort();
    //gettimeofday( &endwtime, NULL );
    //seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    //printf( "Bitonic serial recursive wall clock time = %f\n", seq_time );
  //  test();
     init();
    gettimeofday( &startwtime, NULL );
    Psort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    printf( "Bitonic pthreads with %i threads time = %f\n", 1 << atoi( argv[ 2 ] ), seq_time );
    test();
    //init();
    //gettimeofday( &startwtime, NULL );
    //impBitonicSort();
    //gettimeofday( &endwtime, NULL );
    //seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    //printf( "Bitonic serial imperative wall clock time = %f\n", seq_time );
    //test();
    init();
    gettimeofday( &startwtime, NULL );
    PimpBitonicSort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    printf( "Bitonic omp with %i threads time = %f\n", 1 << atoi( argv[ 2 ] ),  seq_time );
    test();

    init();
    gettimeofday( &startwtime, NULL );
    cilkBitonicSort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    printf( "Bitonic cilk with %i threads time = %f\n", 1 << atoi( argv[ 2 ] ),  seq_time );
    test();

    init();
    gettimeofday( &startwtime, NULL );
    cilksort();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    printf( "Bitonic cilk recursive with %i threads time = %f\n", 1 << atoi( argv[ 2 ] ),  seq_time );
    test();
    
    printf("Spawned: %d\n",sThreads);
    printf("Cilk used %d threads\n",__cilkrts_get_nworkers());



}

/** -------------- SUB-PROCEDURES  ----------------- **/ 




/** procedure test() : verify sort results **/
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
    a[i] = rand() % N; // (N - i);
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


typedef struct{
    int lo, cnt, dir;
} sarg;

/** Procedure bitonicMerge
 *  Same as serial, but uses pthreads.
 **/
void * PbitonicMerge( void * arg ){
    int lo = ( ( sarg * ) arg ) -> lo;
    int cnt = ( ( sarg * ) arg ) -> cnt;
    int dir = ( ( sarg * ) arg ) -> dir;
    if( cnt > 1 ){
        int k = cnt / 2;
        int i;
        for( i = lo; i < lo + k; ++i ){
            compare( i, i + k, dir );
        }
        int t1 = 0, t2 = 0;
        sarg arg1, arg2;
        pthread_t thread1, thread2;
        arg1.lo = lo;
        arg1.cnt = k;
        arg1.dir = dir;
        
        arg2.lo = lo + k;
        arg2.cnt = k;
        arg2.dir = dir;
      
        
        if (nThreads<n && cnt>threshold){  
          pthread_create( &thread1, NULL, PbitonicMerge, &arg1 );
          pthread_mutex_lock(&n_mutex);
          nThreads++;
          sThreads++;
          pthread_mutex_unlock(&n_mutex);
          t1 = 1;
        }
        else
        
          //bitonicMerge(lo, k, dir);
          PbitonicMerge(&arg1);

      	/*
        if (nThreads<n && cnt>threshold){  
          pthread_create( &thread2, NULL, PbitonicMerge, &arg2 );
          pthread_mutex_lock(&n_mutex);
          nThreads++;
          sThreads++;
          pthread_mutex_unlock(&n_mutex);
          t2 = 1;
        }
        else
        */
          //bitonicMerge(lo+k, k, dir);
          PbitonicMerge(&arg2);

        if (t1){
              pthread_join( thread1, NULL );
              pthread_mutex_lock(&n_mutex);
              nThreads--;
              pthread_mutex_unlock(&n_mutex);
            }
            if (t2){
              pthread_join( thread2, NULL );
              pthread_mutex_lock(&n_mutex);
              nThreads--;
              pthread_mutex_unlock(&n_mutex);
            }
        
    }
    return 0;
}

/** function PrecBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 

    Uses pthreads
 **/

void * PrecBitonicSort( void * arg ){
    int lo = ( ( sarg * ) arg ) -> lo;
    int cnt = ( ( sarg * ) arg ) -> cnt;
    int dir = ( ( sarg * ) arg ) -> dir;

    if ( cnt > 1 ) {
        int k = cnt / 2;
        //if( abs(k - lo) < (1<< 10) ) {
        //	printf("M %d \n",abs(k-lo));
        //	printf("M %d \n",sThreads);
        //    qsort( a + lo, k, sizeof( int ), asc );
        //    qsort( a + ( lo + k ) , k, sizeof( int ), desc );
        //}
        //else{

            int t1 = 0, t2 = 0;
            sarg arg1;
            pthread_t thread1;
            arg1.lo = lo;
            arg1.cnt = k;
            arg1.dir = ASCENDING;
            
            if (nThreads<n){  
              pthread_create( &thread1, NULL, PrecBitonicSort, &arg1 );
              pthread_mutex_lock(&n_mutex);
              nThreads++;
              sThreads++;
              pthread_mutex_unlock(&n_mutex);
              t1 = 1;
            }
            else
            
              //recBitonicSort(lo, k, ASCENDING);
            	//PrecBitonicSort(&arg1);
            	qsort(a+lo,k,sizeof(int),asc);
            
            sarg arg2;
            pthread_t thread2;
            arg2.lo = lo + k;
            arg2.cnt = k;
            arg2.dir = DESCENDING;
           /* 
            if (nThreads<n){  
              pthread_create( &thread2, NULL, PrecBitonicSort, &arg2 );
              pthread_mutex_lock(&n_mutex);
              nThreads++;
              sThreads++;
              pthread_mutex_unlock(&n_mutex);
              t2 = 1;
            }
            else
            */
              //recBitonicSort(lo+k, k, DESCENDING);
              PrecBitonicSort(&arg2);

            if (t1){
              pthread_join( thread1, NULL );
              pthread_mutex_lock(&n_mutex);
              nThreads--;
              pthread_mutex_unlock(&n_mutex);
            }
            if (t2){
              pthread_join( thread2, NULL );
              pthread_mutex_lock(&n_mutex);
              nThreads--;
              pthread_mutex_unlock(&n_mutex);
            }
        //}
            sarg arg3;
            arg3.lo = lo;
            arg3.cnt = cnt;
            arg3.dir = dir;
            

            PbitonicMerge( &arg3 );
        
    }
    return 0;
}

/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void Psort() {
    sarg arg;
    arg.lo = 0;
    arg.cnt = N;
    arg.dir = ASCENDING;
    
    PrecBitonicSort( &arg );
}



/*
  imperative version of bitonic sort
*/
void PimpBitonicSort() {
    omp_set_num_threads( n );//"n" is the number of threads - arg[2]; 
    int i,j,k=0,term;
    //term = (int) log2( N );
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

/*
  imperative version of bitonic sort
*/
void cilkBitonicSort() { 
    int i,j,k=0,term;
    for (k = 2; k <= N; k *= 2 ) {
        for (j=k>>1; j>0; j=j>>1) {
                cilk_for (i=0; i<N; i++) {
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

/** Procedure cilkbitonicMerge() 
   It recursively sorts a bitonic sequence in ascending order, 
   if dir = ASCENDING, and in descending order otherwise. 
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted. 
 **/
void cilkbitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    cilk_spawn cilkbitonicMerge(lo, k, dir);
    cilkbitonicMerge(lo+k, k, dir);
  }
}



/** function cilkrecBitonicSort() 
    first produces a bitonic sequence by recursively sorting 
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
 **/
void cilkrecBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
  	int k=cnt/2;
  	if(cnt<threshold){
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
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void cilksort() {
  cilkrecBitonicSort(0, N, ASCENDING);
}