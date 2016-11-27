#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define CANT 10000

//Esta funcion genera un numero aleatorio tipo float mayor que a y menor que b
float rand_FloatRange(float a, float b)
{
return ((b-a)*((float)rand()/RAND_MAX))+a;
}

long rand_longRange(long a, long b)
{
  return ((long)(b-a)*((float)rand()/RAND_MAX))+a;
}


int isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return 0;
    }
    return 1;
}


int main(int argc, char* argv[])
{
  int i,cant;
FILE *fp= NULL;//puntero del archivo

//Controla los argumentos al programa
if ((argc == 2)&&(isNumber(argv[1]))){
   cant=atoi(argv[1]);
}else{
  cant=CANT;
}

fp = fopen ("Datos.txt", "w+");// Abre el archivo con permiso de escritura y si existe lo descarta su contenido

for (i=cant;i>0;i--)
{
  fprintf(fp, "%ld\t%.2f\n",rand_longRange(5000000, 5000000000),rand_FloatRange(1,91));//temperatura
  // fprintf(fp, "%.2f,",rand_FloatRange(10,99));//humedad
  // fprintf(fp, "%.1f,",rand_FloatRange(1000,1200));//presion atmosferica
  // fprintf(fp, "%.2f\n",rand_FloatRange(10,50));//vel. de viento
  fflush(fp); //Escribe en el archivo los datos que con fprintf se hbaian almacenado en el buffer
  // sleep(1);// Retardo de 1 segundo

}
fclose(fp);//Cierra el archivo
return (0);
}
