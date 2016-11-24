/*
     add_client.c: Client program to take user input comprising
                   of numbers, send request message to add_server
                   and receive server's response

 */
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

#define MONITOR_FIFO "/tmp/fifo_Monitor"
#define SENSOR_FIFO "/tmp/fifo_Sensor"

char my_fifo_name [128];
char buf1 [512], buf2 [1024];

FILE *temps_file;
struct timespec time_nanos = {0, 10000000L};

int main(int argc,char*argv[]){
  int fd_monitor;
  char *temp,*nanos;
  long nano_seconds;

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
  if (mkfifo (SENSOR_FIFO, 0664) == -1)
      perror ("mkfifo");

  while (!feof(temps_file)) {

    // // get user input
    // printf ("Type numbers to be added: ");
    if (fgets(buf1, sizeof (buf1), temps_file) == NULL)
        break;
    nanos = strtok(buf1, "\t");
    temp = strtok(NULL, "\t");

    nano_seconds=(long)atoi(nanos);

    nanosleep(&time_nanos, NULL);
    // send message to Monitor
    if ((fd_monitor = open (MONITOR_FIFO, O_WRONLY)) == -1) {
        perror ("open: Monitor fifo");
        break;
    }

    if (write (fd_monitor, temp, strlen (temp)) != strlen (temp)) {
        perror ("write");
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

  if (write (fd_monitor, "F", sizeof("F")) != strlen ("F")) {
      perror ("write");

  }

  if (close (fd_monitor) == -1) {
      perror ("close");
  }


  fclose(temps_file);
  /* remove the FIFO */
  unlink(MONITOR_FIFO);


  return 0;
} //FIN MAIN
