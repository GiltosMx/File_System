#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"

#define LINESIZE 16
#define SECSIZE 512

int main(int argc,char *argv[])
{
	int drive;
	int ncyl,nhead,nsec;
	int fd;
	unsigned char buffer[SECSIZE];
	int offset;
	int i,j,r;
	unsigned char c;

	if(argc==5)
	{	
		drive=atoi(argv[1]);
		ncyl=atoi(argv[2]);
		nhead=atoi(argv[3]);
		nsec=atoi(argv[4]);
		if(drive<0 || drive> 3 || ncyl>CYLINDERS || nhead > HEADS || nsec > SECTORS || ncyl<0 || nhead<0 || nsec<1)
		{
			fprintf(stderr,"Posición invalida\n");
			exit(1);
		}
		printf("Desplegando de disco%d.vd Cil=%d, Sup=%d, Sec=%d\n",drive,ncyl,nhead,nsec);
		
	}
	else	
	{
		fprintf(stderr,"Error en los argumentos\n");
		exit(1);
	}

	if(vdreadsector(drive,nhead,ncyl,nsec,1,buffer)==-1)
	{
		fprintf(stderr,"Error al abrir disco virtual\n");
		exit(1);
	}

	for(i=0;i<SECSIZE/LINESIZE;i++)
	{
		printf("\n %3X -->",i*LINESIZE);
		for(j=0;j<LINESIZE;j++)
		{
			c=buffer[i*LINESIZE+j];
			printf("%2X ",c);
		}
		printf("  |  ");
		for(j=0;j<LINESIZE;j++)
		{
			c=buffer[i*LINESIZE+j]%256;
			if(c>0x1F && c<127)
				printf("%c",c);
			else
				printf(".");
		}
	}
	printf("\n");
}

