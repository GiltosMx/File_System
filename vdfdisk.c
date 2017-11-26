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

	for (size_t i = 0; i < 446; i++) {
		mbr.bootstrap_code[i] = 0;
	}

	mbr.signature = 0x55AA;

	mbr.partition[0].drive_status = 128 & 0xFF;
	mbr.partition[0].chs_begin[0]=0; // Superficie
	mbr.partition[0].chs_begin[1]=2 & 0x1F;	// Los 5 bits menos significativos son el sector
	mbr.partition[0].chs_begin[2]=0; // Cilindro

	mbr.partition[0].partition_type = 0x08;
	mbr.partition[0].chs_end[0]=8; // Superficie
	mbr.partition[0].chs_end[1]=27 & 0x1F;	// Los 5 bits menos significativos son el sector
	mbr.partition[0].chs_end[2]=199 & 0xFF;	// Cilindro
	mbr.partition[0].chs_end[1]=mbr.partition[0].chs_end[1] |
								(199 & 0x300) >> 2;

	mbr.partition[0].lba = 2 & 0xFFFFFFFF;
	mbr.partition[0].secs_partition = 43199 & 0xFFFFFFFF;

	printf("chs_end de la primera partición %X %X %X\n",mbr.partition[0].chs_end[0],mbr.partition[0].chs_end[1],mbr.partition[0].chs_end[2]);

	for (size_t i = 1; i < 4; i++) {
		mbr.partition[i].drive_status = 0;
		mbr.partition[i].chs_begin[0]=0; // Superficie
		mbr.partition[i].chs_begin[1]=0;	// Los 5 bits menos significativos son el sector
		mbr.partition[i].chs_begin[2]=0; // Cilindro

		mbr.partition[i].partition_type = 0;
		mbr.partition[i].chs_end[0]=0; // Superficie
		mbr.partition[i].chs_end[1]=0;	// Los 5 bits menos significativos son el sector
		mbr.partition[i].chs_end[2]=0;	// Cilindro
		mbr.partition[i].chs_end[1]=0;

		mbr.partition[i].lba = 0 & 0xFFFFFFFF;
		mbr.partition[i].secs_partition = 0 & 0xFFFFFFFF;
	}

	//Usar vdwritesector para escribir el MBR en el sector fisico 1 del disco
	vdwritesector(0, 0, 0, 1, 1, (char*) &mbr);

}
