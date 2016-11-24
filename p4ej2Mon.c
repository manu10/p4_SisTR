/*
     add_server.c: A server to add numbers received in message.

 */
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MONITOR_FIFO "/tmp/fifo_Monitor"
#define SENSOR_FIFO "/tmp/fifo_Sensor"

int main (int argc, char **argv)
{
    int fd, bytes_read;
    char buf [4096];
    //Creo el fifo del Monitor
    if ((mkfifo (MONITOR_FIFO, 0664) == -1) && (errno != EEXIST)) {
        perror ("mkfifo");
        exit (1);
    }
    //Intento abrir el fifo
    if ((fd = open (MONITOR_FIFO, O_RDONLY)) == -1)
        perror ("open");


     while (1) {
         // get a message
         memset (buf, '\0', sizeof (buf));
         if ((bytes_read = read (fd, buf, sizeof (buf))) == -1)
             perror ("read");
         if (bytes_read == 0)
             continue;
         if (bytes_read > 0) {
           printf("%s\n", buf);
         }

     }
}
