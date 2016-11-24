#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUF 1024

double dwalltime();
void* hiloAvg(void *arg);
void* hiloTemp(void *arg);

int P=2; //Cantidad de hilos

int main(int argc,char*argv[]){

  //Controla los argumentos al programa
  //if (argc != 2){
  //  printf("\nError de argumentos!\nP: cantidad hilos (minimo 1)\nEjecutar de la forma: %s P N\n",argv[0]);
  //  return 0;
  //}else{
  //  P=atoi(argv[1]);
  //  if (P < 1){
  //    printf("\nError de argumentos!\nP: cantidad hilos (minimo 1)\nEjecutar de la forma: %s P N\n",argv[0]);
  //    return 0;
  //  }
  //}

  pthread_t misHilos[P];
  int id,threads_ids[P];
  for(id=0;id<P;id++){
    threads_ids[id] = id;
    //id=0 alta prioridad, lee las temp
    //id=1 baja prioridad, calcula los promedios
    if(id==0) pthread_create(&misHilos[id], NULL, &hiloTemp,(void*)&threads_ids[id]);
    pthread_create(&misHilos[id], NULL, &hiloAvg,(void*)&threads_ids[id]);
  }
  for(id=0;id<P;id++){
    pthread_join(misHilos[id],NULL);
  }

} //FIN MAIN

/*****************************************************************/

void* hiloTemp(void *arg){
  int fd;
  char myfifo[12] = "/tmp/myfifo";
  char buf[MAX_BUF];
  int id = *(int*)arg;//para que el hilo conozca su id
  /* open, read, and display the message from the FIFO */
  fd = open(myfifo, O_RDONLY);

  while (buf[0]!='F'){
    read(fd, buf, MAX_BUF);
    printf("Received: %s\n", buf);
  }
  close(fd);

  pthread_exit(NULL);
} //FIN HILO TEMP

/*****************************************************************/

void* hiloAvg(void *arg){
  int id = *(int*)arg;

  pthread_exit(NULL);
} //FIN HILO AVG

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
