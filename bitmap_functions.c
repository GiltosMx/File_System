#include "logical_to_physical.h"

void checkSecboot(struct SECBOOTPART* sbp) {
	int secboot_en_memoria = vdreadseclog(0,(char*) sbp);

	if(!secboot_en_memoria) {
		// Si no está en memoria, cárgalo
		printf("NO ESTA EN MEMORIA EL SECBOOT\n");
	}
}

unsigned char checkiNodesMap(struct SECBOOTPART* sbp, unsigned char inodesmap[512]){
	unsigned char mapa_bits_nodos_i = sbp->sec_inicpart + sbp->sec_res;
	// Ese mapa está en memoria
	if(!vdreadseclog(mapa_bits_nodos_i,inodesmap)) {
	    printf("NO ESTA EN MEMORIA EL MAPA DE BITS NODOS I\n");
	}
	return mapa_bits_nodos_i;
}

unsigned char checkBlocksMap(struct SECBOOTPART* sbp, unsigned char blocksmap[]){
	unsigned char mapa_bits_bloques = sbp->sec_inicpart + sbp->sec_res +
	sbp->sec_mapa_bits_area_nodos_i;
	int i = 0;

	// Cargar todos los sectores que corresponden al mapa de bits
	for (i = 0; i < sbp->sec_mapa_bits_bloques; i++) {
		if(!vdreadseclog(mapa_bits_bloques+i, blocksmap + (i * 512) ) ) {
			printf("NO ESTA EN MEMORIA EL MAPA DE BITS DE BLOQUES\n");
		}
	}
	return mapa_bits_bloques;
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
	unsigned char inodesmap[512];
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

int isblockfree(int block){
	// Numero de byte en el mapa
	int offset = block/8;
	// Numero de bit en el byte
	int shift = block%8;
	struct SECBOOTPART sbp;

	// Determinar si tenemos el sector de boot de la particion
	// en memoria
	checkSecboot(&sbp);

	unsigned char blocksmap[512*sbp.sec_mapa_bits_bloques];

	checkBlocksMap(&sbp, blocksmap);

	if(blocksmap[offset] & (1<<shift))
		// Si el bit esta en 1, regresar 0 (no esta libre)
		return 0;
	else
		// Si el bit esta en 0, regresar 1 (si esta libre)
		return 1;
}

int nextfreeblock(){
	int i, j = 0;
	struct SECBOOTPART sbp;
	unsigned char mapa_bits_bloques = 0;

	checkSecboot(&sbp);
	unsigned char blocksmap[512*sbp.sec_mapa_bits_bloques];

	mapa_bits_bloques = checkBlocksMap(&sbp, blocksmap);

	// Empezar desde el primer byte del mapa de bloques.
	// Si el byte tiene todos los bits en 1, y mientras no
	// lleguemos al final del mapa de bits
	while(blocksmap[i] == 0xFF && i < sbp.sec_mapa_bits_bloques * 512)
	 	i++;

	// Si no llegue al final del mapa de bits, quiere decir que aun
	// hay bloques libres
	if(i < sbp.sec_mapa_bits_bloques * 512) {
		while(blocksmap[i] & (1<<j) && j < 8)
			j++;

		// Retorno del numero de bloque que se encontro disponible
		return (i * 8 + j);
	} else
		// Si ya no hubo bloques libres, regresar -1 = error
		return (-1);
}

int assignblock(int block){
	// Numero de byte en el mapa
	int offset = block/8;
	// Numero de bit en el byte
	int shift = block%8;
	int sector = 0;
	struct SECBOOTPART sbp;
	unsigned char mapa_bits_bloques = 0;

	checkSecboot(&sbp);
	unsigned char blocksmap[512*sbp.sec_mapa_bits_bloques];

	mapa_bits_bloques = checkBlocksMap(&sbp, blocksmap);

	blocksmap[offset] |= (1<<shift);

	// Determinar en que numero de sector esta el bit que
	// modificamos
	sector = (offset/512);
	// Escribir el sector del mapa de bits donde esta el bit
	// que modificamos
	vdwriteseclog(mapa_bits_bloques + sector, blocksmap + (sector*512));

	return 1;
}

int unassignblock(int block){
	// Numero de byte en el mapa
	int offset = block/8;
	// Numero de bit en el byte
	int shift = block%8;
	int sector = 0;
	int mask = 0;
	struct SECBOOTPART sbp;
	unsigned char mapa_bits_bloques = 0;

	checkSecboot(&sbp);
	unsigned char blocksmap[512*sbp.sec_mapa_bits_bloques];

	mapa_bits_bloques = checkBlocksMap(&sbp, blocksmap);

	blocksmap[offset] &= (char) ~(1<<shift);

	// Calcular en que sector esta el bit modificado
	// Escribir el sector en disco
	sector = (offset/512);
	vdwriteseclog(mapa_bits_bloques + sector, blocksmap + (sector * 512));

	return 1;
}

int main(int argc, char const *argv[]) {
	// int inode = nextfreeinode();
	// printf("Next free inode: %d\n", inode);

	// int block = nextfreeblock();
	// printf("Next free block: %d\n", block);


	// assignblock(block);
	unassignblock(72);

	// if(isblockfree(1))
	// 	printf("Block %d is free\n", 1);
	// else
	// 	printf("Block %d is not free\n", 1);



	// int block = nextfreeblock();
	// printf("Next free block: %d\n", block);
	//
	// assignblock(block);
	//
	// if(isblockfree(block))
	// 	printf("Block %d is free\n", block);
	// else
	// 	printf("Block %d is not free\n", block);

	// assigninode(inode);
	//
	// if(isinodefree(inode))
	// 	printf("Node is free\n");
	//
	// unassignnode(inode);
	//
	// if(isinodefree(inode))
	// 	printf("Node is free\n");

	return 0;
}
