#include "vdisk.h"
#include <stdio.h>
#include <string.h>

#define SEC_X_TRACK 27
#define SFIP 2
#define HIP 0
#define CIP 0
#define HEADS 8
#define CYLINDERS 200
#define UNIDAD 0
#define SLAD 11

struct SECBOOTPART {
	char jump[4];						
	char nombre_particion[8];
	unsigned short sec_inicpart;		
	unsigned char sec_res;		
	unsigned char sec_mapa_bits_area_nodos_i;
	unsigned char sec_mapa_bits_bloques;	
	unsigned short sec_tabla_nodos_i;
	unsigned int sec_log_particion;		
	unsigned char sec_x_bloque;			
	unsigned char heads;				
	unsigned char cyls;				
	unsigned char secfis;				
	char bootcode[484];
};

int secfis,cilindro,superficie;

void log_to_phys(int nseclog) {
	secfis = ((nseclog + SFIP - 1) % SEC_X_TRACK) +1;
	superficie = ((nseclog + SFIP - 1) / SEC_X_TRACK + HIP) % HEADS;
	cilindro = ((nseclog + SFIP - 1) + (HIP * SEC_X_TRACK)) / (SEC_X_TRACK * HEADS);
}

int vdreadseclog(int nseclog, char *buffer) {
	log_to_phys(nseclog);
	return vdreadsector(UNIDAD,superficie,cilindro,secfis,1,buffer);
}

int vdwriteseclog(int nseclog, char *buffer) {
	log_to_phys(nseclog);
	return vdwritesector(UNIDAD,superficie,cilindro,secfis,1,buffer);
}

int readblock(int nbloque,char *buffer) {
	int i, returnValue = 0;
	struct SECBOOTPART sbp;
	returnValue = vdreadsector(0, 0, 0, 2, 1, (char*) &sbp);

	for(i=0;i<sbp.sec_x_bloque;i++) {
		returnValue = vdreadseclog((SLAD + ((nbloque-1)*2))+i,buffer);
		buffer+=512;
	}
	return returnValue;
}

int writeblock(int nbloque,char *buffer) {
	int i, returnValue = 0;
	struct SECBOOTPART sbp;
	returnValue = vdreadsector(0, 0, 0, 2, 1, (char*) &sbp);

	for(i=0;i<sbp.sec_x_bloque;i++) {
		returnValue = vdwriteseclog((SLAD + ((nbloque-1)*2))+i,buffer);
		buffer+=512;
	}
}

int main(){

	char buffer[512];
	strcpy(buffer,"LEEME Y ESCRIBEME ESTA");
	writeblock(1,buffer);

	char buffer2[512];
	readblock(1,buffer2);

	printf("buffer : %s \n",buffer2);
	return 0;
}