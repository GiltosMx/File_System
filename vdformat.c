#include "inode_functions.h"

#define CYLINDERS 200
#define HEADS 8
#define SECTORS 27

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
	vdwritesector(unidad,superficie_inicial,cilindro_inicial,
				  secfis_inicial,1,(char *) &sbp);

	// Arreglo de nodos i, donde solo podremos tener 24. Cada uno mide 64
	// bytes. 512 bytes * 3 sectores / 64 bytes por nodo = 24 nodos i.
	struct INODE inode_table[24];
	memset(&inode_table, 0, 64 * 24);

	// Escribir inode_table en los sectores logicos 8,9 y 10 de la particion
	for (size_t i = 0; i < 3; i++) {
		// Caben 8 nodos i por cada sector logico
		vdwriteseclog(i + 8, (char *) &inode_table[i * 8]);
	}
}
