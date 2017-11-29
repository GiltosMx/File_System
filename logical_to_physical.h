#ifndef _LOGTOPHYS_H_
#define _LOGTOPHYS_H_

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

// Esta estructura debe ser un sector y debe medir 512 bytes
struct SECBOOTPART {
	char jump[4]; // Instrucción JMP para transferir el ccontrol a la dirección donde está el código de boot del SO
	char nombre_particion[8];
	unsigned short sec_inicpart;		// 0 sectores
	unsigned char sec_res;		// 1 sector reservado
	unsigned char sec_mapa_bits_area_nodos_i;// 1 sector
	unsigned char sec_mapa_bits_bloques;	// 6 sectores
	unsigned short sec_tabla_nodos_i;	// 3 sectores, directorio raíz
	unsigned int sec_log_particion;		// 43199 sectores
	unsigned char sec_x_bloque;			// 2 sectores por bloque
	unsigned char heads;				// 8 superficies
	unsigned char cyls;				// 200 cilindros
	unsigned char secfis;				// 27 sectores por track
	char bootcode[484];	// Código de arranque
};

void log_to_phys(int nseclog);
int vdreadseclog(int nseclog, char *buffer);
int vdwriteseclog(int nseclog, char *buffer);
int readblock(int nbloque,char *buffer);
int writeblock(int nbloque,char *buffer);

#endif
