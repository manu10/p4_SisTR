#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

double dwalltime();

struct timespec time_nanos = {0, 10000000L};
FILE *temps_file;

int main(int argc,char*argv[]){
  int fd;
  char *myfifo = "/tmp/myfifo";
  printf("\nDeclaro fifo\n");

  /* create the FIFO (named pipe) */
  mkfifo(myfifo, 0777);
  printf("\nCreo fifo\n");

  fd = open(myfifo, O_WRONLY);
  printf("\nAbro fifo\n");


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

  char line[100],*temp,*nanos;
  long nano_seconds=50000;

  while (!feof(temps_file)) {

    fgets(line, 100, temps_file);
    nanos = strtok(line, "\t");
    temp = strtok(NULL, "\t");

    nano_seconds=(long)atoi(nanos);
    time_nanos.tv_nsec=nano_seconds;
    //nanosleep(&time_nanos, NULL);

    //manda la temperatura a la salida estandar
    fprintf(stdout, "%s",temp);
    write(fd, temp, sizeof(temp));

  }
  write(fd, "F", sizeof("F"));

  fclose(temps_file);
  close(fd);

  /* remove the FIFO */
  unlink(myfifo);


  return 0;
} //FIN MAIN

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
