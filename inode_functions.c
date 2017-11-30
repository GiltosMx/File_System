#include "inode_functions.h"

unsigned char checkiNodesTable(struct SECBOOTPART* sbp, struct INODE inode_table[]) {
    int i = 0;
    // Sector de inicio de la tabla de nodos i
    unsigned char sec_tabla_nodos_i = sbp->sec_inicpart + sbp->sec_res +
    sbp->sec_mapa_bits_area_nodos_i + sbp->sec_mapa_bits_bloques;

    for (i = 0; i < sbp->sec_tabla_nodos_i; i++) {
        if(!vdreadseclog(sec_tabla_nodos_i + i, (char *) &inode_table[i * 8]))
            printf("LA TABLA DE NODOS I NO ESTA EN MEMORIA\n");
    }
    return sec_tabla_nodos_i;
}

int setinode(int num, char* filename, unsigned short atribs, int uid, int gid){
    int i = 0;
    struct SECBOOTPART sbp;
    struct INODE inode_table[MAX_INODES];
    unsigned char sec_tabla_nodos_i;

    // Cargamos el sector de boot
    checkSecboot(&sbp);

    // Cargamos la tabla de nodos i
    sec_tabla_nodos_i = checkiNodesTable(&sbp, inode_table);

    unsigned char nodeSector = sec_tabla_nodos_i + (num/8);

    // Copiar el nombre del archivo en el nodo i
    strncpy(inode_table[num].name, filename, MAX_FILENAMESIZE);
    // Asegurar que el ultimo caracter es la terminacion de linea
    if(strlen(inode_table[num].name) > MAX_FILENAMESIZE-1)
        inode_table[num].name[MAX_FILENAMESIZE-1] = '\0';

    // Fecha y hora de creacion con hora y fecha actual
    inode_table[num].datetimecreat = currdatetimetoint();
    inode_table[num].datetimemodif = currdatetimetoint();
    inode_table[num].datetimelaacc = 0;

    // User ID, Group ID, atributos (permisos) no importan en la practica
    inode_table[num].uid = uid;
    inode_table[num].gid = gid;
    inode_table[num].perms = atribs;

    // Archivo nuevo, con tamaño inicial 0
    inode_table[num].size = 0;

    // Establecer los apuntadores a bloques directos en 0
    for (i = 0; i < NUM_DIRECT_BLOCKS; i++) {
        inode_table[num].direct_blocks[i] = 0;
    }

    // Apuntadores indirectos en 0
    inode_table[num].indirect = 0;
    inode_table[num].indirect2 = 0;

    struct INODE buffer[8]; // Buffer de 512 bytes
    memset(&buffer, 0, 64 * 8);

    // Llena el buffer con los 8 nodos i del sector
    for (i = 0; i < 8; i++) {
        buffer[i] = inode_table[i + ((nodeSector-8) * 8)];
    }

    // Escribe el sector logico correspondiente al nodo asignado
    vdwriteseclog(nodeSector, (char *) &buffer);

    // Poner en uno el bit que corresponde al inodo en el
    // mapa de bits de nodos i
    assigninode(num);

    return num;
}

int removeinode(int num) {
    int i = 0;
    unsigned short temp[512]; // 1024 bytes
    struct SECBOOTPART sbp;
    struct INODE inode_table[MAX_INODES];
    unsigned char sec_tabla_nodos_i;

    // Cargamos el sector de boot
    checkSecboot(&sbp);

    // Cargamos la tabla de nodos i
    sec_tabla_nodos_i = checkiNodesTable(&sbp, inode_table);

    unsigned char nodeSector = sec_tabla_nodos_i + (num/8);

    // Eliminar el nombre del nodo i
    memset(&inode_table[num].name,0,MAX_FILENAMESIZE);

    // Recorrer los apuntadores directos del nodo
    for (i = 0; i < NUM_DIRECT_BLOCKS; i++) {
        // Si es diferente de cero esta asignado
        if(inode_table[num].direct_blocks[i] != 0) {
            unassignblock(inode_table[num].direct_blocks[i]);
            inode_table[num].direct_blocks[i] = 0;
        }
    }
    // Si el bloque indirecto ya está asignado
    if(inode_table[num].indirect != 0) {
        // Leer el bloque que contiene los apuntadores
        // a memoria
        readblock(inode_table[num].indirect, (char *) temp);

        // Recorrer todos los apuntadores del bloque para
        // desasignarlos
        for (i = 0; i < 512; i++) {
            if(temp[i] != 0)
                unassignblock(temp[i]);
        }
        // Desasignar el bloque que contiene los apuntadores
        unassignblock(inode_table[num].indirect);
        inode_table[num].indirect = 0;
    }

    struct INODE buffer[8]; // Buffer de 512 bytes
    memset(&buffer, 0, 64 * 8);

    // Llena el buffer con los 8 nodos i del sector
    for (i = 0; i < 8; i++) {
        buffer[i] = inode_table[i + ((nodeSector-8) * 8)];
    }

    // Escribe el sector logico correspondiente al nodo asignado
    vdwriteseclog(nodeSector, (char *) &buffer);

    // Poner en cero el bit que corresponde al inodo en
    // el mapa de bits de nodos i
    unassigninode(num);

    return 1;
}

int searchinode(char* filename) {
    int i = 0;
    struct SECBOOTPART sbp;
    struct INODE inode_table[MAX_INODES];
    unsigned char sec_tabla_nodos_i;

    checkSecboot(&sbp);

    sec_tabla_nodos_i = checkiNodesTable(&sbp, inode_table);

    // El nombre del archivo no debe medir mas de 18 bytes
    if(strlen(filename) > MAX_FILENAMESIZE-1)
        filename[MAX_FILENAMESIZE-1] = '\0';

    // Recorrer la tabla de nodos i hasta encontrar el archivo
    while(strcmp(inode_table[i].name, filename) && i < MAX_INODES)
        i++;

    if(i < MAX_INODES)
        return i; // Posicion donde fue encontrado el archivo
    else
        return (-1); // No se encontro el archivo
}
