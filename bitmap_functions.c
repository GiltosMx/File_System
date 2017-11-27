#include "logical_to_physical.h"

// Buscar en el mapa de bits, el primer nodo i que esté libre, es decir, que su bit está en 0, me regresa ese dato
int nextfreeinode() {
	int i,j;
	int result;
	// Checar si el sector de boot de la partición está en memoria
    struct SECBOOTPART sbp;
	int secboot_en_memoria = vdreadseclog(0,(char*) &sbp);

	if(!secboot_en_memoria) {
		// Si no está en memoria, cárgalo
		printf("NO ESTA EN MEMORIA EL SECBOOT, CARGALO PLS");
	}

	int mapa_bits_nodos_i = sbp.sec_inicpart + sbp.sec_res; 	
    // Usamos la información del sector de boot de la partición para determinar en que sector inicia el mapa de bits de nodos i 
					
	// Ese mapa está en memoria
    char inodesmap[512];
	if(!vdreadseclog(1,inodesmap)) {
	    printf("NO ESTA EN MEMORIA EL MAPA DE BITS NODOS I, CARGALO PLS");
	}

	// Recorrer byte por byte mientras sea 0xFF sigo recorriendo
	int i=0;
    unsigned char j;
    
	while(inodesmap[i]==0xFF && i<3)
		i++;

	if(i<3) {
		// Recorrer los bits del byte, para encontrar el bit que está en cero
		j=0;
		while(inodesmap[i] & (1<<j) && j<8)
			j++;

		return(i*8+j); // Regresar el bit del mapa encontrado en cero
	} else // Todos los bits del mapa de nodos i están en 1
		return(-1); // -1 significa que no hay nodos i disponibles	
}