#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <memory.h>

#define MONITOR_FIFO "/tmp/MONITOR_FIFO"

// char my_fifo_name [128];
char buf1 [512];

FILE *temps_file;
struct timespec time_nanos = {0, 10000000L};

int main(int argc,char*argv[]){
  int fd_monitor;
  char *temp,*nanos;
  long nano_seconds;
  const char *corte="F";

  //Controla los argumentos al programa
  if (argc != 2){
    printf("\nError de argumentos!\nS: archivo para leer\nEjecutar de la forma: %s S\n",argv[0]);
    return 0;
  }else{
    temps_file = fopen(argv[1], "r");
    if (temps_file==NULL) {
      printf("\nError de argumentos!\nEl archivo no existe\nS: archivo para leer\nEjecutar de la forma: %s S\n",argv[0]);
      return 0;
    }
  }

  // make Sensor fifo
  // if (mkfifo (SENSOR_FIFO, 0664) == -1)
  //     perror ("mkfifo");
  if (mkfifo (MONITOR_FIFO, 0664) == -1)
          perror ("mkfifo");

  while (!feof(temps_file)) {

    // // get data from file
    if (fgets(buf1, sizeof (buf1), temps_file) == NULL)
        break;
    nanos = strtok(buf1, "\t");
    temp = strtok(NULL, "\n");
    printf("temp ANTES de strcat:\"%s\"\n",temp );
    strcat(temp," ");
    printf("temp DESPUES de strcat: \"%s\"\n",temp );

    printf("\nnanos: --%s-- \n",nanos);
    printf("\ntemp: --%s-- \n",temp);

    nano_seconds=(long)atoi(nanos);
    time_nanos.tv_nsec=nano_seconds;
    nanosleep(&time_nanos, NULL);//TODO: PROBAR CREO QUE DEBO CAMBIARLO
    // send message to Monitor
    if ((fd_monitor = open (MONITOR_FIFO, O_WRONLY)) == -1) {
        perror ("open: Monitor fifo");
        break;
    }

    printf("Longitud de temp leida: %ld\n", strlen (temp));
    if (write (fd_monitor, temp, strlen (temp)) != (unsigned)strlen (temp)) {
        perror ("write problem");
         break;
    }

    if (close (fd_monitor) == -1) {
        perror ("close");
        break;
    }

  }
  //Envio caracter F para terminar, pero creo que no es necesario porque se cierra
  // cuando termina de mandar, de todas maneras esto(escritura) se deberia modularizar
  if ((fd_monitor = open (MONITOR_FIFO, O_WRONLY)) == -1) {
      perror ("open: Monitor fifo");

  }

  if (write (fd_monitor, (void *)corte, strlen(corte)) != (unsigned)strlen (corte)) {

      perror ("write COrte");

  }
  printf("Envio caracter de corte:\"%s\"\n",corte);
  if (close (fd_monitor) == -1) {
      perror ("close");
  }


  fclose(temps_file);
  /* remove the FIFO */
  unlink(MONITOR_FIFO);
  // unlink(SENSOR_FIFO);


  return 0;
} //FIN MAIN
