#include <stdio.h>
#include "vdisk.h"

#pragma pack(1)

struct PARTITION {
	unsigned char drive_status;	// 1
	unsigned char chs_begin[3];	// 3
	unsigned char partition_type; // 1
	unsigned char chs_end[3];	// 3
	unsigned int lba;			// 4
	unsigned int secs_partition; // 4
};

struct MBR {
	unsigned char bootstrap_code[446];
	struct PARTITION partition[4];	// 4 * 16 = 64
	unsigned short signature;		// 16 bits = 2 bytes
};

int main() {
	struct MBR mbr;
	printf("%ld\n",sizeof(struct MBR));

	// Crear la primera partición que será desde
	// 	Cilindro= 0, Superficie = 0, Sector Físico = 2
	mbr.partition[0].chs_begin[0]=0; // Superficie
	mbr.partition[0].chs_begin[1]=2 & 0x1F;	// Los 5 bits menos significativos son el sector
	mbr.partition[0].chs_begin[2]=0; // Cilindro

	mbr.partition[0].chs_end[0]=8; // Superficie
	mbr.partition[0].chs_end[1]=27 & 0x1F;	// Los 5 bits menos significativos son el sector
	mbr.partition[0].chs_end[2]=199 & 0xFF;	// Cilindro
	mbr.partition[0].chs_end[1]=mbr.partition[0].chs_end[1] |
								(199 & 0x300) >> 2;

	printf("chs_end de la primera partición %X %X %X\n",mbr.partition[0].chs_end[0],mbr.partition[0].chs_end[1],mbr.partition[0].chs_end[2]);

	//Usar vdwritesector para escribir el MBR en el sector fisico 1 del disco
	vdwritesector(0, 0, 0, 1, 1, (char*) &mbr);

}
