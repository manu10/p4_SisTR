#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>



#define NUM_THREADS 1000





/*
clock_gettime(CLOCK_MONOTONIC, &start);

 ...

clock_gettime(CLOCK_MONOTONIC, &finish);

elapsed = (finish.tv_sec - start.tv_sec);
elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

*/




/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  int tid;
  double stuff;
} thread_data_t;

/* thread function */
void *thr_func(void *arg) {
  int i=0;
  thread_data_t *data = (thread_data_t *)arg;

  printf("hello from thr_func, thread id: %d\n", data->tid);
  for (i=0;i<data->tid;i++){
	printf(".");
  }
  printf("thread %d\n", data->tid);

  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  struct timeval startDY, endDY;
  pthread_t thr[NUM_THREADS];
  int i, rc;
  struct timespec start, finish,finishRT,startRT;
  double elapsed,elapsedRT;
  gettimeofday(&startDY, NULL);


  clock_gettime(CLOCK_MONOTONIC, &start);
  clock_gettime(CLOCK_REALTIME, &startRT);
  /* create a thread_data_t argument array */
  thread_data_t thr_data[NUM_THREADS];

  /* create threads */
  for (i = 0; i < NUM_THREADS; ++i) {
    thr_data[i].tid = i;
    if ((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      return EXIT_FAILURE;
    }
  }
  /* block until all threads complete */
  for (i = 0; i < NUM_THREADS; ++i) {
    pthread_join(thr[i], NULL);
  }
  clock_gettime(CLOCK_MONOTONIC, &finish);
  clock_gettime(CLOCK_REALTIME, &finishRT);
  gettimeofday(&endDY, NULL);

  elapsed = (finish.tv_sec - start.tv_sec);
  printf("tiempo MONOTONIC %lf \n", elapsed);

  elapsedRT = (finishRT.tv_sec - startRT.tv_sec);
  printf("Tiempo RT %lf \n", elapsedRT);

 usleep(10000000);

 printf("%ld\n", ((endDY.tv_sec * 1000000 + endDY.tv_usec)
		  - (startDY.tv_sec * 1000000 + startDY.tv_usec)));




//  if (sysconf(_SC_MONOTONIC_CLOCK))
//    printf("_SC_MONOTONIC_CLOCK habilitado");
//  else
//    printf("_SC_MONOTONIC_CLOCK NO habilitado");
  return EXIT_SUCCESS;
}
