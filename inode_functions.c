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
    struct INODE inode_table[24];
    unsigned char sec_tabla_nodos_i;

    // Cargamos el sector de boot
    checkSecboot(&sbp);

    // Cargamos la tabla de nodos i
    sec_tabla_nodos_i = checkiNodesTable(&sbp, inode_table);

    // Copiar el nombre del archivo en el nodo i
    strncpy(inode_table[num].name, filename, 18);
    // Asegurar que el ultimo caracter es la terminacion de linea
    if(strlen(inode_table[num].name) > 17)
        inode_table[num].name[17] = '\0';

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
    for (size_t i = 0; i < 10; i++) {
        inode_table[num].direct_blocks[i] = 0;
    }

    // Apuntadores indirectos en 0
    inode_table[num].indirect = 0;
    inode_table[num].indirect2 = 0;

    // Escribe el sector logico correspondiente al nodo asignado
    vdwriteseclog(sec_tabla_nodos_i + (num/sec_tabla_nodos_i),
                 (char *) &inode_table[num]);

    return num;
}

int main(int argc, char const *argv[]) {

    setinode(0, "PRUEBA_NOMBRE", 1, 2, 3);

    return 0;
}
