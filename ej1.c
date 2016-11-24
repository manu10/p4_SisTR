#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>

double dwalltime();
void* calcularHilo(void *arg);

const struct timespec time_millis = {0, 10000000L};
int P; //Cantidad de hilos
int N;
double *hilo_time, *hilo_prev_time, *hilo_sum;

int main(int argc,char*argv[]){

  //Controla los argumentos al programa
  if (argc != 3){
    printf("\nError de argumentos!\nP: cantidad hilos (minimo 1)\nN: cantidad segundos (minimo 1)\nEjecutar de la forma: %s P N\n",argv[0]);
    return 0;
  }else{
    P=atoi(argv[1]);
    N=atoi(argv[2]);
    if (P < 1 || N < 1){
      printf("\nError de argumentos!\nP: cantidad hilos (minimo 1)\nN: cantidad segundos (minimo 1)\nEjecutar de la forma: %s P N\n",argv[0]);
      return 0;
    }
  }

  //Declaracion de arreglos para almacenar tiempos
  hilo_time = (double*)malloc(P*sizeof(double));
  hilo_prev_time = (double*)malloc(P*sizeof(double));
  hilo_sum = (double*)malloc(P*sizeof(double));

  pthread_t misHilos[P];
  int id,threads_ids[P];
  for(id=0;id<P;id++){
    threads_ids[id] = id;
    pthread_create(&misHilos[id], NULL, &calcularHilo,(void*)&threads_ids[id]);
  }
  for(id=0;id<P;id++){
    pthread_join(misHilos[id],NULL);
  }

} //FIN MAIN

/*****************************************************************/

void* calcularHilo(void *arg){
  int h=0,i,id = *(int*)arg;

  for (h = 0; h < N; h++) {

    hilo_time[id] = dwalltime();
    for (i = 0; i < 100; i++) {
      if (i%10==0 && i!=0) {
        hilo_prev_time[id]=hilo_time[id];
        hilo_time[id] = dwalltime();
        hilo_sum[id]+=hilo_time[id]-hilo_prev_time[id]-100;
      }
      nanosleep(&time_millis, NULL);
    }
    hilo_prev_time[id]=hilo_time[id];
    hilo_time[id] = dwalltime();
    hilo_sum[id]+=hilo_time[id]-hilo_prev_time[id]-100;

  }

  printf("Hilo %d, tiempo de latencia en milisegundos %f \n", id,hilo_sum[id]/(10*N));

  if (h) {
    printf("El hilo %d finalizo\n", id);
  }

  pthread_exit(NULL);
} //FIN HILO

/*****************************************************************/

#include <sys/time.h>

double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec*1000 + tv.tv_usec/1000.0;
  return sec;
}
