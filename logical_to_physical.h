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

void log_to_phys(int nseclog);
int vdreadseclog(int nseclog, char *buffer);
int vdwriteseclog(int nseclog, char *buffer);
int readblock(int nbloque,char *buffer);
int writeblock(int nbloque,char *buffer);