#include <stdio.h>

// Datos sobre la geometría del disco
#define CYLINDERS 200
#define HEADS 8
#define SECTORS 27

// Debe medir 512 bytes
struct SECBOOTPART {
	char jump[4];			
	char nombre_particion[8];
	unsigned short sec_inicpart;		// 1 sector 
	unsigned char sec_res;		// 1 sector reservado
	unsigned char sec_mapa_bits_area_nodos_i;// 1 sector
	unsigned char sec_mapa_bits_bloques;	// 6 sectores
	unsigned short sec_tabla_nodos_i;	// 3 sectores
	unsigned int sec_log_particion;		// 43199 sectores
	unsigned char sec_x_bloque;			// 2 sectores por bloque
	unsigned char heads;				// 8 superficies				
	unsigned char cyls;				// 200 cilindros
	unsigned char secfis;				// 27 sectores por track
	char restante[484];	// Código de arranque
};

int main()
{
	int part_formatear=0;
	struct SECBOOTPART sbp;
	int unidad = 0;
	int sfip,sip,cip;
	
	// Obtener de la tabla de particiones los valores de:
	// 	sfip = Sector físico inicial de la partición
	//	sip = Superficie inicial de la partición
	//	cip = Cilindro inicial de la partición
	
	sbp.sec_inicpart=2;
	sbp.sec_res=1;
	sbp.sec_mapa_bits_area_nodos_i=1;
	sbp.sec_mapa_bits_bloques=6;
	sbp.sec_tabla_nodos_i=3
	sbp.sec_log_particion=43100;
	sbp.sec_x_bloque=2;
	sbp.heads=8;
	sbp.cyls=200;
	sbp.secfis=27;
	
	// Escribir el contenido de la estructura sbp en el sector físico inicial de la 
	// partición
	vdwritesec(unidad,cip,sip,sfip,1,(char *) &sbp);
}