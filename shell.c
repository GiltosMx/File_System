#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXLEN 80
#define BUFFERSIZE 512

void locateend(char *cmd);
int executecmd(char *cmd);

int main()
{
	char linea[MAXLEN];
	int result=1;
	while(result)
	{
		printf("vshell > ");
		fflush(stdout);
		read(0,linea,80);
		locateend(linea);
		result=executecmd(linea);
	} 
}

void locateend(char *linea)
{
	// Localiza el fin de la cadena para poner el fin
	int i=0;
	while(i<MAXLEN && linea[i]!='\n')
		i++;
	linea[i]='\0';
}

int executecmd(char *linea)
{
	char *cmd;
	char *arg1;
	char *arg2;
	char *search=" ";

	// Separa el comando y los dos posibles argumentos
	cmd=strtok(linea," ");
	arg1=strtok(NULL," ");
	arg2=strtok(NULL," ");

	// comando "exit"
	if(strcmp(cmd,"exit")==0)
		return(0);

	// comando "copy"
	if(strcmp(cmd,"copy")==0)
	{
		if(arg1==NULL && arg2==NULL)
		{
			fprintf(stderr,"Error en los argumentos\n");
			return(1);
		}
		if(!isinvd(arg1) && !isinvd(arg2))
			copyuu(arg1,arg2);

		else if(!isinvd(arg1) && isinvd(arg2))
			copyuv(arg1,&arg2[5]);

		else if(isinvd(arg1) && !isinvd(arg2))
			copyvu(&arg1[5],arg2);

		else if(isinvd(arg1) && isinvd(arg2))
			copyvv(&arg1[5],&arg2[5]);
			
		
	}

	// comando "type"
	if(strcmp(cmd,"type")==0)
	{
		if(isinvd(arg1))
			catv(&arg1[5]);
		else
			catu(arg1);
	}

	// comando dir
	if(strcmp(cmd,"dir")==0)
	{
		if(arg1==NULL)
			diru(arg1);
		else if(!isinvd(arg1))
			dirv(&arg1[5]);
	}
}

/* Regresa verdadero si el nombre del archivo no comienza con // y por lo 
   tanto es un archivo que está en el disco virtual */

int isinvd(char *arg)
{
	if(strncmp(arg,"/vfs/",5)!=0)
		return(0);
	else
		return(1);
}


/* Copia un archivo del sistema de archivos de UNIX a un archivo destino
   en el mismo sistema de archivos de UNIX */

int copyuu(char *arg1,char *arg2)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=open(arg1,0);
	dfile=creat(arg2,0640);
	do {
		ncars=read(sfile,buffer,BUFFERSIZE);
		write(dfile,buffer,ncars);
	} while(ncars==BUFFERSIZE);
	close(sfile);
	close(dfile);
	return(1);	
}



/* Copia un archivo del sistema de archivos de UNIX a un archivo destino
   en el el disco virtual */

int copyuv(char *arg1,char *arg2)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=open(arg1,0);
	dfile=vdcreat(arg2,0640);
	do {
		ncars=read(sfile,buffer,BUFFERSIZE);
		vdwrite(dfile,buffer,ncars);
	} while(ncars==BUFFERSIZE);
	close(sfile);
	vdclose(dfile);
	return(1);	
}


/* Copia un archivo del disco virtual a un archivo destino
   en el sistema de archivos de UNIX */

int copyvu(char *arg1,char *arg2)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=vdopen(arg1,0);
	dfile=creat(arg2,0640);
	do {
		ncars=vdread(sfile,buffer,BUFFERSIZE);
		write(dfile,buffer,ncars);
	} while(ncars==BUFFERSIZE);
	vdclose(sfile);
	close(dfile);
	return(1);	
}



/* Copia un archivo del disco virtual a un archivo destino
   en el mismo disco virtual */

int copyvv(char *arg1,char *arg2)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=vdopen(arg1,0);
	dfile=vdcreat(arg2,0640);
	do {
		ncars=vdread(sfile,buffer,BUFFERSIZE);
		vdwrite(dfile,buffer,ncars);
	} while(ncars==BUFFERSIZE);
	vdclose(sfile);
	vdclose(dfile);
	return(1);		
}


/* Despliega un archivo del disco virtual a pantalla */

int catv(char *arg1)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=vdopen(arg1,0);
	do {
		ncars=vdread(sfile,buffer,BUFFERSIZE);
		write(1,buffer,ncars);  // Escribe en el archivo de salida estandard
	} while(ncars==BUFFERSIZE);
	vdclose(sfile);
	return(1);		
}


/* Despliega un archivo del sistema de archivos de UNIX a pantalla */

int catu(char *arg1)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=open(arg1,0);
	do {
		ncars=read(sfile,buffer,BUFFERSIZE);
		write(1,buffer,ncars);  // Escribe en el archivo de salida estandard
	} while(ncars==BUFFERSIZE);
	close(sfile);
	return(1);		
}


/* Muestra el directorio en el sistema de archivosd de UNIX */

int diru(char *arg1)
{
	DIR *dd;	
	struct dirent *entry;

	if(arg1[0]=='\0')
		strcpy(arg1,".");

	printf("Directorio %s\n",arg1);

	dd=opendir(arg1);
	if(dd==NULL)
	{
		fprintf(stderr,"Error al abrir directorio\n");
		return(-1);
	}
	
	while((entry=readdir(dd))!=NULL)
		printf("%s\n",entry->d_name);

	closedir(dd);	
}

/* Muestra el directorio en el sistema de archivos en el disco virtual */

int dirv(char *dir)
{
	VDDIR *dd;	
	struct vddirent *entry;

	printf("Directorio del disco virtual\n");

	dd=vdopendir(".");
	if(dd==NULL)
	{
		fprintf(stderr,"Error al abrir directorio\n");
		return(-1);
	}
	
	while((entry=vdreaddir(dd))!=NULL)
		printf("%s\n",entry->d_name);

	vdclosedir(dd);	
}

