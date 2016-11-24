/*Escriba un programa que contenga dos threads que se ejecutan periódicamente
en 1000 oportunidades. Ambos threads se ejecutan 100 veces por segundo y cada
vez que se ejecutan acumulan el tiempo de latencia que pasó desde que se
cumplió una centésima de segundo, hasta que verdaderamente retomó la
ejecución la tarea. En otras palabras, el tiempo que tardó en retomar el thread la
tarea menos la centésima de segundo que debía transcurrir. Una vez que ambos
threads hayan finalizado, imprima el promedio de la latencia acumulada para cada
uno de los dos threads
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 2
int sumaTiempos[2];
float prom[2];

// pthread_mutex_t count_mutex;
 pthread_mutex_t count_mutexs[2];
/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  int tid;
  double stuff;
} thread_data_t;

/* thread function */
void *thr_func(void *arg) {
  struct timeval startTime, finishTime;
  int i=0;
  thread_data_t *data = (thread_data_t *)arg;
  printf("hello from thr_func, thread id: %d\n", data->tid);
  for (i=0;i<1000;i++){
    gettimeofday(&startTime, NULL);
    //  printf("thread %d, pasada %d se va a dormir \n", data->tid,i);
	   usleep(10000);
     gettimeofday(&finishTime, NULL);
    //  pthread_mutex_lock(&count_mutex);
      pthread_mutex_lock(count_mutexs+data->tid);
     sumaTiempos[data->tid]+=(finishTime.tv_sec * 1000000 + finishTime.tv_usec) - (startTime.tv_sec * 1000000 + startTime.tv_usec)-10000;
    //  pthread_mutex_unlock(&count_mutex);
      pthread_mutex_unlock(count_mutexs+data->tid);


    //  printf("thread %d, pasada %d tardo %ld uSeg desde que se desperto hasta que obtuvo el uP\n", data->tid+1,i+1, ((finishTime.tv_sec * 1000000 + finishTime.tv_usec)
   // 		  - (startTime.tv_sec * 1000000 + startTime.tv_usec)-10000));

  }
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
    pthread_t thr[NUM_THREADS];
  thread_data_t thr_data[NUM_THREADS];
  int i,rc;

  sumaTiempos[0]=0;
  sumaTiempos[1]=0;
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
  prom[0]=((float)sumaTiempos[0])/1000;
  prom[1]=((float)sumaTiempos[1])/1000;
  printf("La suma de tiempo del thread 0 fue de %f uSeg y del thread 1 de %f uSeg \n",prom[0],prom[1]);
  return EXIT_SUCCESS;
}
