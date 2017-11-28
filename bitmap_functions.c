#include "logical_to_physical.h"

void checkSecboot(struct SECBOOTPART* sbp) {
	int secboot_en_memoria = vdreadseclog(0,(char*) sbp);

	if(!secboot_en_memoria) {
		// Si no está en memoria, cárgalo
		printf("NO ESTA EN MEMORIA EL SECBOOT\n");
	}
}

unsigned char checkiNodesMap(struct SECBOOTPART* sbp, char inodesmap[512]){
	unsigned char mapa_bits_nodos_i = sbp->sec_inicpart + sbp->sec_res;
	// Ese mapa está en memoria
	if(!vdreadseclog(mapa_bits_nodos_i,inodesmap)) {
	    printf("NO ESTA EN MEMORIA EL MAPA DE BITS NODOS I\n");
	}
	return mapa_bits_nodos_i;
}

//Usando el mapa de bits, determinar si un nodo i, está libre u ocupado.
int isinodefree(int inode) {
	int offset=inode/8;
	int shift=inode%8;
	char inodesmap[512];
	struct SECBOOTPART sbp;

	// Checar si el sector de boot de la partición está en memoria
	checkSecboot(&sbp);

	//Usamos la información del sector de boot de la partición para
	//determinar en que sector inicia el
	//mapa de bits de nodos i
	checkiNodesMap(&sbp, inodesmap);

	if(inodesmap[offset] & (1<<shift)) {
		return(0); // El nodo i ya está ocupado
	} else {
		return(1); // El nodo i está libre
	}
}

// Buscar en el mapa de bits, el primer nodo i que esté libre, es decir, que su bit está en 0, me regresa ese dato
int nextfreeinode() {
	int i = 0;
	unsigned char j = 0;
	char inodesmap[512];
    struct SECBOOTPART sbp;

	// Checar si el sector de boot de la partición está en memoria
	checkSecboot(&sbp);
	// Usamos la información del sector de boot de la partición para
	// determinar en que sector inicia el mapa de bits de nodos i
	checkiNodesMap(&sbp, inodesmap);


	printf("sbp.sec_mapa_bits_area_nodos_i: %d\n", sbp.sec_mapa_bits_area_nodos_i);

	// Recorrer byte por byte mientras sea 0xFF sigo recorriendo
	while(inodesmap[i]==0xFF && i<3)
		i++;

	if(i<3) {
		// Recorrer los bits del byte, para encontrar el bit que está en cero
		while(inodesmap[i] & (1<<j) && j<8)
			j++;

		return(i*8+j); // Regresar el bit del mapa encontrado en cero
	} else // Todos los bits del mapa de nodos i están en 1
		return(-1); // -1 significa que no hay nodos i disponibles
}

// Poner en 1 el bit que corresponde al numero de inodo indicado
int assigninode(int inode) {
	int offset=inode/8;
	int shift=inode%8;
	struct SECBOOTPART sbp;
	char inodesmap[512];
	unsigned char mapa_bits_nodos_i;

	// Checar si el sector de boot de la particion esta en memoria
	checkSecboot(&sbp);

	// Usamos la información del sector de boot de la partición para determinar
	// en que sector inicia el mapa de bits de nodos i
	mapa_bits_nodos_i = checkiNodesMap(&sbp, inodesmap);

	inodesmap[offset] |= (1<<shift); // Poner en 1 el bit indicado
	vdwriteseclog(mapa_bits_nodos_i, inodesmap);

	return 1;
}

int unassignnode(int inode){
	int offset=inode/8;
	int shift=inode%8;
	struct SECBOOTPART sbp;
	char inodesmap[512];
	unsigned char mapa_bits_nodos_i;

	// Checar si el sector de boot de la particion esta en memoria
	checkSecboot(&sbp);

	// Usamos la información del sector de boot de la partición para determinar
	// en que sector inicia el mapa de bits de nodos i
	mapa_bits_nodos_i = checkiNodesMap(&sbp, inodesmap);

	// Poner en cero el bit que corresponde al inodo indicado
	inodesmap[offset] &= (char) ~(1<<shift);
	vdwriteseclog(mapa_bits_nodos_i, inodesmap);

	return 1;
}

int main(int argc, char const *argv[]) {
	int inode = nextfreeinode();
	printf("Next free inode: %d\n", inode);

	assigninode(inode);

	if(isinodefree(inode))
		printf("Node is free\n");

	unassignnode(inode);

	if(isinodefree(inode))
		printf("Node is free\n");

	return 0;
}
