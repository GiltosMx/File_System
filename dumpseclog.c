#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"

#define LINESIZE 16
#define SECSIZE 512
#define SEC_X_TRACK 27
#define SFIP 2
#define HIP 0
#define SUP 8

int secfis,cilindro,superficie;

void log_to_phys(int nseclog) {
	secfis = ((nseclog + SFIP - 1) % SEC_X_TRACK) +1;
	superficie = ((nseclog + SFIP - 1) / SEC_X_TRACK + HIP) % SUP;
	cilindro = ((nseclog + SFIP - 1) + (HIP * SEC_X_TRACK)) / (SEC_X_TRACK * SUP);
}

int main(int argc,char *argv[]) {
	int drive;
	int ncyl,nhead,nsec;
	int fd;
	unsigned char buffer[SECSIZE];
	int offset;
	int i,j,r;
	unsigned char c;

	if(argc==2) {
        log_to_phys(atoi(argv[1]));
		drive=0;
		if(drive<0 || drive> 3 || cilindro>CYLINDERS || superficie > HEADS || secfis > SECTORS || cilindro<0 || superficie<0 || secfis<1) {
			fprintf(stderr,"Posición invalida\n");
			exit(1);
		}
		printf("Desplegando de disco%d.vd Cil=%d, Sup=%d, Sec=%d\n",drive,cilindro,superficie,secfis);

	} else {
		fprintf(stderr,"Error en los argumentos\n");
		exit(1);
	}

	if(vdreadsector(drive,superficie,cilindro,secfis,1,buffer)==-1) {
		fprintf(stderr,"Error al abrir disco virtual\n");
		exit(1);
	}

	for(i=0;i<SECSIZE/LINESIZE;i++) {
		printf("\n %3X -->",i*LINESIZE);
		for(j=0;j<LINESIZE;j++) {
			c=buffer[i*LINESIZE+j];
			printf("%2X ",c);
		}
		printf("  |  ");
		for(j=0;j<LINESIZE;j++) {
			c=buffer[i*LINESIZE+j]%256;
			if(c>0x1F && c<127)
				printf("%c",c);
			else
				printf(".");
		}
	}
	printf("\n");
}
