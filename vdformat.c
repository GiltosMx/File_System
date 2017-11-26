#include <stdio.h>
#include <string.h>
#include "vdisk.h"

#define CYLINDERS 200
#define HEADS 8
#define SECTORS 27

// Esta estructura debe ser un sector y debe medir 512 bytes
struct SECBOOTPART {
	char jump[4];						// Instrucción JMP para transferir el ccontrol a la dirección donde está el código de boot del SO
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

int main()
{
	struct SECBOOTPART sbp;

	memset(&sbp,0,512);
	strcpy(sbp.nombre_particion,"ELEMENTA");
	sbp.sec_inicpart=0;
	sbp.sec_res=1;
	sbp.sec_mapa_bits_area_nodos_i=1;
	sbp.sec_mapa_bits_bloques=6;
	sbp.sec_tabla_nodos_i=3;
	sbp.sec_log_particion=43199;
	sbp.sec_x_bloque=2;
	sbp.heads=HEADS;
	sbp.cyls=CYLINDERS;
	sbp.secfis=SECTORS;

	int unidad=0;
	// Leer la tabla de particiones, Cilindro 0, Superficie 0, SF 1
	// Determino CHS Inicial
	int cilindro_inicial=0;
	int superficie_inicial=0;
	int secfis_inicial=2;

	// Escribir la estructura en el sector lógico 0 de la partición
	vdwritesector(unidad,superficie_inicial,cilindro_inicial,secfis_inicial,1,(char *) &sbp);
}
