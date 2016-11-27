#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void
display_thread_sched_attr(const char *msg)
{
    int policy, s;
    struct sched_param param;

    s = pthread_getschedparam(pthread_self(), &policy, &param);
    if (s != 0)
        handle_error_en(s, "pthread_getschedparam");


    printf("%s:    policy=%s, priority=%d\n",msg,
            (policy == SCHED_FIFO)  ? "SCHED_FIFO" :
            (policy == SCHED_RR)    ? "SCHED_RR" :
            (policy == SCHED_OTHER) ? "SCHED_OTHER" :
            "???",
            param.sched_priority);

}

#define MONITOR_FIFO "/tmp/MONITOR_FIFO"
#define MAX_NUMBERS 500
int fin_prom=0,cant=0,fin=0;
double ult_temp[3];
void * thr_lec_func(void *arg);
void * thr_pro_func(void *arg);

const char *corte="F";
int indexNumbers = 0;
int main (int argc, char **argv)
{

  // pthread_attr_t tattr_lectura,tattr_promedio;
  pthread_t thr_lectura,thr_promedio;
  int rc;
  int s, inheritsched,  policy;
  pthread_attr_t attr_lec,attr_pro;
  pthread_attr_t *attrp_lec,*attrp_pro;
  struct sched_param param_lec,param_pro;
  inheritsched = PTHREAD_EXPLICIT_SCHED; //Para poder configurar una politica diferente a la del padre
  policy=SCHED_RR;

  attrp_lec = NULL;
  s = pthread_attr_init(&attr_lec);
  if (s != 0)
      handle_error_en(s, "pthread_attr_init");
  attrp_lec = &attr_lec;
  s = pthread_attr_setinheritsched(&attr_lec, inheritsched);
  if (s != 0)
      handle_error_en(s, "pthread_attr_setinheritsched");
  param_lec.sched_priority = 50;
  s = pthread_attr_setschedpolicy(&attr_lec, policy);
  if (s != 0)
      handle_error_en(s, "pthread_attr_setschedpolicy");
  s = pthread_attr_setschedparam(&attr_lec, &param_lec);
  if (s != 0){
      printf("linea 86\n");
      handle_error_en(s, "pthread_attr_setschedparam");
  }
  //seteo parametros para el thread promedio (repito codigo al pedo pero ni ganas de acomodarlo XD)
  attrp_pro = NULL;
  s = pthread_attr_init(&attr_pro);
  if (s != 0)
      handle_error_en(s, "pthread_attr_init");
  attrp_pro = &attr_pro;
  s = pthread_attr_setinheritsched(&attr_pro, inheritsched);
  if (s != 0)
      handle_error_en(s, "pthread_attr_setinheritsched");
  param_lec.sched_priority = 10;
  s = pthread_attr_setschedpolicy(&attr_pro, policy);
  if (s != 0)
      handle_error_en(s, "pthread_attr_setschedpolicy");
  s = pthread_attr_setschedparam(&attr_pro, &param_pro);
  if (s != 0)
      handle_error_en(s, "pthread_attr_setschedparam");


  if ((rc = pthread_create(&thr_lectura, attrp_lec, thr_lec_func, (void*)0))) {
    fprintf(stderr, "(LINEA 149)error: pthread_create, rc: %d\n", rc);
    return EXIT_FAILURE;
  }

  if ((rc = pthread_create(&thr_promedio, attrp_pro, thr_pro_func, (void*)1))) {
    fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
    return EXIT_FAILURE;
  }

/* Destroy unneeded thread attributes object */
  s = pthread_attr_destroy(&attr_lec);
  if (s != 0)
      handle_error_en(s, "pthread_attr_destroy");
  s = pthread_attr_destroy(&attr_pro);
  if (s != 0)
      handle_error_en(s, "pthread_attr_destroy");


  /* block until all threads complete */
  pthread_join(thr_lectura, NULL);
  pthread_join(thr_promedio, NULL);


return 0;
}

void * thr_lec_func(void *arg){
      char *ptr;
      double temp;
      int fd, bytes_read;
      char buf[100];
      char *numbers [MAX_NUMBERS];
      display_thread_sched_attr("Scheduler attributes of LECTURA");
      //Creo el fifo del Monitor
      if ((mkfifo (MONITOR_FIFO, 0664) == -1) && (errno != EEXIST)) {
          perror ("mkfifo");
          exit (1);
      }
      //Intento abrir el fifo
      if ((fd = open (MONITOR_FIFO, O_RDONLY)) == -1)
          perror ("open");


       while (fin==0) {
           // get a message
           memset (buf, '\0', sizeof (buf));
           if ((bytes_read = read (fd, buf, sizeof (buf))) == -1)
               perror ("read");
           if (bytes_read == 0)
               continue;
           if (bytes_read > 0) {
            //  printf("buf leido:%s\n",buf );
             numbers[indexNumbers] = strtok (buf, ", \n");
             while (numbers [indexNumbers] != NULL && fin==0){
                // printf("last__number:\"%s\"\n",numbers[indexNumbers] );
                if (strcmp(numbers[indexNumbers],corte)==0){
                  fin=1;
                  printf("\nRecibi caracter de corte!\n");
                }else{
                  temp = strtod (numbers[indexNumbers], &ptr);
                  if (temp>90){
                   printf("\n////////////////ALERTA[%.2f°C]////////////////\n",temp);
                  }else{
                    ult_temp[cant % 3]=temp;
                    if (cant==2){
                      cant=0;
                    }else{
                      cant++;
                    }
                   }
                 }
                 numbers [++indexNumbers] = strtok (NULL, ", \n");
           }
           }

       }
return 0;
}
void * thr_pro_func(void *arg){
  int countdown=5;
  display_thread_sched_attr("Scheduler attributes of PROMEDIO");
  while(countdown>0){
    double promedio=((double)(ult_temp[0]+ult_temp[1]+ult_temp[2]))/3;
    printf("\rPromedio de ultimas 3:%f",promedio);
    fflush(stdout);//Junto con el \r esta linea es para que imprima sobre la misma linea
    usleep(500000);//Se duerme cada medio segundo
    //Cuenta regresiva por si queda algun dato sin ser analizado
    if (fin==1){
      countdown--;
      printf("\ncountdown:%d\n",countdown);
    }
  }
  return 0;
}
