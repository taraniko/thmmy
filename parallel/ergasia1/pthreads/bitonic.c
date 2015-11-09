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

struct timeval startwtime, endwtime;
double seq_time;

const int MAX_THREAD_NUM = 256;

int N;          // data array size
int *a;         // data array to be sorted
int P;          // number of threads
pthread_t *threads;
pthread_mutex_t array_mutex;
pthread_mutex_t args_mutex;
pthread_mutex_t work_mutex;
pthread_mutex_t finished_work_mutex;
pthread_cond_t finished_work_cv;
pthread_cond_t work_cv;
int exists_work = 0;
int end_sort = 0;


struct arg_struct{   // thread arguments
    int start;
    int middle;
    int dir;

}t_args;

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void threads_init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);
void *worker(void *arguments);


/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s q p\n  where n=2^q is problem size (power of two) and P = 2^p number of threads \n", 
	   argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  a = (int *) malloc(N * sizeof(int));
  P = 1<<atoi(argv[2]);
  if(P>MAX_THREAD_NUM) {
    printf("Number of threads should be smaller than %d \n",MAX_THREAD_NUM);
    exit(1);
  }
  
  init();

  gettimeofday (&startwtime, NULL);
  impBitonicSort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative wall clock time = %f\n", seq_time);

  test();

  init();
  
  threads_init();
  gettimeofday (&startwtime, NULL);
  sort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive wall clock time = %f\n", seq_time);

  test();

   print();
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

/** procedure threads_init() : initialize threads **/
void threads_init() {
  threads = (pthread_t *)malloc(P * sizeof(pthread_t));
  int i;
  
  for (i=0; i<P; i++){
    pthread_create(&threads[i], NULL, worker, &t_args);
  }
  
  pthread_mutex_init(&array_mutex, NULL);
  pthread_mutex_init(&args_mutex, NULL);
  pthread_mutex_init(&work_mutex, NULL);
  pthread_mutex_init(&finished_work_mutex, NULL);
  usleep(10000);
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
    // TODO change args struct to add new lo k dir and activate a thread

    pthread_mutex_lock(&args_mutex);
    t_args.start = lo;
    t_args.middle = k;
    t_args.dir = dir;
    pthread_mutex_lock(&work_mutex);
    pthread_cond_signal(&work_cv);   // exists work!
    pthread_mutex_unlock(&work_mutex);
    pthread_mutex_unlock(&args_mutex);
    printf("D");
    
    //SHOULD WAIT TILL WORKER THREAD FINISH JOB
    pthread_mutex_lock(&finished_work_mutex);
    pthread_cond_wait(&finished_work_cv, &finished_work_mutex);
    pthread_mutex_unlock(&finished_work_mutex);
    //WAIT TILL HERE
    
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
  printf("ARXH\n");
  recBitonicSort(0, N, ASCENDING);
  printf("\nTELOS\n");
  end_sort = 1; // used to get out of threads loop
  // /* Wait for all threads to complete */
  //int i;
  //for (i=0; i<P; i++) {
  //  pthread_join(threads[i], NULL);
  //}
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

/*
  thread worker
*/


void *worker(void *arguments)
{
  struct arg_struct *args = (struct arg_struct *)arguments;
  printf("i");
  while(!end_sort){
    printf("H");
    pthread_mutex_lock(&work_mutex);
    pthread_cond_wait(&work_cv, &work_mutex);  // Waits till there is work to do
    pthread_mutex_unlock(&work_mutex);

    pthread_mutex_lock (&array_mutex);
    // TODO code for comparing data
    int i;
    int lo = args -> start;
    int k = args -> middle;
    int dir = args -> dir;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    // END code for comparing data
    pthread_mutex_unlock (&array_mutex);
    pthread_mutex_lock(&finished_work_mutex);
    pthread_cond_signal(&finished_work_cv); //finished job
    pthread_mutex_unlock(&finished_work_mutex);
  }


    pthread_exit(NULL);
    return NULL;
}
