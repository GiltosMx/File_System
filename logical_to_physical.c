#include "logical_to_physical.h"

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

	if(returnValue == -1)
		return returnValue;

	for(i=0;i<sbp.sec_x_bloque;i++) {
		returnValue = vdwriteseclog((SLAD + ((nbloque-1)*2))+i,buffer);
		buffer+=512;
	}
	return returnValue;
}

// int main(){
//
// 	unsigned char buffer[512*2];
// 	strcpy(buffer,"LEEME Y ESCRIBEME ESTA");
// 	printf("writeblock: %d\n",writeblock(1,buffer));
//
// 	unsigned char buffer2[512*2];
//
// 	printf("readblock: %d\n",readblock(1,buffer2));
//
// 	printf("buffer2 read: %s \n",buffer2);
// 	return 0;
// }
