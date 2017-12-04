#include "file_management.h"

struct OPENFILES openfiles_table[16];
struct INODE inode_table[MAX_INODES];
struct SECBOOTPART sbp;
unsigned char inodesmap[512];

int openfiles_table_loaded = 0;
int inode_table_loaded = 0;
int sbp_loaded = 0;

void checkOpenFilesTable() {
    if (!openfiles_table_loaded) {
        for (size_t i = 3; i < 16; i++) {
            openfiles_table[i].inuse = 0;
            openfiles_table[i].currbloqueenmemoria = -1;
        }
        openfiles_table_loaded = 1;
    }
}

void freeInodeBlocks(int currinode) {
    checkOpenFilesTable();
    int i = 0;
    unsigned int fileSize = inode_table[currinode].size;
    printf("fileSize: %u\n", fileSize);
    // Libera los bloques directos
    while (fileSize > 0) {
        printf("Block to free: %d\n", inode_table[currinode].direct_blocks[i++]);
        unassignblock(inode_table[currinode].direct_blocks[i++]);
        if (fileSize >= 1024)
            fileSize -= 1024;
        else
            fileSize = 0;
    }
}

unsigned short *postoptr(int fd, int pos) {
    int currinode;
    unsigned short* currptr;
    unsigned short indirect1;

    currinode = openfiles_table[fd].inode;
    if(!sbp_loaded) {
        checkSecboot(&sbp);
        sbp_loaded = 1;
    }
    if(!inode_table_loaded) {
        checkiNodesTable(&sbp, inode_table);
        inode_table_loaded = 1;
    }

    // Si esta en los primeros 10 k
    if (pos / 1024 < 10) {
        // Esta entre los 10 apuntadores directos
        currptr = &inode_table[currinode].direct_blocks[pos / 1024];
    } else if (pos / 1024 < 522) {
        // Si el indirecto esta vacio, asignar un bloque
        if (inode_table[currinode].indirect == 0) {
            // El primer bloque disponible
            indirect1 = nextfreeblock();
            assignblock(indirect1);
            inode_table[currinode].indirect = indirect1;
        }
        currptr = &openfiles_table[fd].buffindirect[pos / 1024 - 10];
    } else
        return NULL;

    return currptr;
}

unsigned short* currpostoptr(int fd) {
    unsigned short* currptr;
    currptr = postoptr(fd, openfiles_table[fd].currpos);

    return currptr;
}

int vdcreat(char* filename, unsigned short perms) {
    int numinode = 0, i = 0;

    // Ver si ya existe el archivo
    numinode = searchinode(filename);

    // Si no existe
    if(numinode == -1) {
        // Buscar un nodo i disponible en el mapa de bits
        numinode = nextfreeinode();
        // Si ya no hay nodos disponibles
        if(numinode == -1) {
            return (-1);
        }
    } else {
        freeInodeBlocks(numinode);
        unassigninode(numinode);
    }

    // Escribir el archivo en el nodo encontrado
    setinode(numinode, filename, perms, 1, 1);

    // Poner el archivo en la tabla de archivos abiertos
    // Establecer el archivo como abierto
    checkOpenFilesTable();

    // Buscar si hay lugar en la tabla de archivos abiertos
    i = 3;
    while (openfiles_table[i].inuse && i < 16)
        i++;

    if (i >= 16) // Llegamos al final y no hay lugar
        return (-1);

    openfiles_table[i].inuse = 1; // Poner el archivo en uso
    openfiles_table[i].inode = numinode; // Nodo del archivo abierto
    openfiles_table[i].currpos = 0; // Siempre es 0 al incio
    openfiles_table[i].openMode = -1;

    return i;
}

int vdunlink(char * filename) {
    int numinode = 0, i = 0;

    numinode = searchinode(filename);
    if (numinode == -1)
        return (-1);

    freeInodeBlocks(numinode);

    return unassigninode(numinode);
}

int vdseek(int fd, int offset, int whence) {
    unsigned short oldblock = 0, newblock = 0;

    checkOpenFilesTable();
    if (openfiles_table[fd].inuse == 0)
        return (-1);

    oldblock = *currpostoptr(fd);

    // Si es a partir del inicio
    if (whence == 0) {
        // Si el offset esta antes del inicio o despues del final, error
        if (offset < 0 || offset > inode_table[openfiles_table[fd].inode].size)
            return (-1);

        openfiles_table[fd].currpos = offset;
    } else if (whence == 1) { // Es a partir de la posicion actual
        // Validar que no se quiera mover antes del inicio del archivo
        // o despues del final
        if (openfiles_table[fd].currpos + offset >
            inode_table[openfiles_table[fd].inode].size ||
            openfiles_table[fd].currpos + offset < 0) {
            return (-1);
        }
        openfiles_table[fd].currpos += offset;

    } else if (whence == 2) { // A partir del final
        if (offset > inode_table[openfiles_table[fd].inode].size ||
            openfiles_table[fd].currpos - offset < 0) {
            return (-1);
        }
        openfiles_table[fd].currpos = inode_table[openfiles_table[fd].inode].size - offset;
    } else
        return (-1);

    // Verificar si la nueva posicion del apuntador es un bloque diferente al
    // que estabamos. Si es asi, hay que cargar ese bloque a memoria
    newblock = *currpostoptr(fd);

    // Despues de mover el apuntador, me cambie a otro bloque?
    if (newblock != oldblock) {
        // Escribir el bloque viejo
        writeblock(oldblock, openfiles_table[fd].buffer);
        // Leer el bloque nuevo
        readblock(newblock, openfiles_table[fd].buffer);
        // Indicar el nuevo bloque como bloque actual
        // en la tabla de archivos abiertos
        openfiles_table[fd].currbloqueenmemoria = newblock;
    }
    // Regresar la posicion actual del archivo
    return openfiles_table[fd].currpos;
}

int vdwrite(int fd, char *buffer, int bytes) {
    int currblock, currinode, cont = 0, sector, result;
    unsigned short* currptr;

    checkSecboot(&sbp);
    unsigned char inicio_nodos_i = checkiNodesMap(&sbp, inodesmap);
    checkOpenFilesTable();

    //Si no esta abierto regresa error
    if (openfiles_table[fd].inuse == 0)
        return (-1);

    currinode = openfiles_table[fd].inode;

    // Copiar byte por byte del buffer que recibe como
    // argumento al buffer del archivo
    while (cont < bytes) {
        // Obtener la direccion de donde esta el bloque
        // correspondiente a la posicion actual
        currptr = currpostoptr(fd);
        if (currptr == NULL)
            return (-1);

        // Cual es el bloque en el que escribiriamos
        currblock = *currptr;

        // Si el bloque esta en blanco, dale uno
        if (currblock == 0) {
            currblock = nextfreeblock();
            // El bloque encontrado ponerlo en donde apunta
            // el apuntador al bloque actual
            *currptr = currblock;
            // Asignarlo en el mapa de bits
            assignblock(currblock);

            // Escribir el sector de la tabla de nodos i en el disco
            sector = currinode / 8;
            result = vdwriteseclog(inicio_nodos_i + sector,
                                   (char *) &inode_table[sector * 8]);
        }
        // Si el bloque de la posicion actual no esta en memoria
        // Lee el bloque al buffer del archivo
        if (openfiles_table[fd].currbloqueenmemoria != currblock) {
            // Leer el bloque actual hacia el buffer que
            // esta en la tabla de archivos abiertos
            readblock(currblock, openfiles_table[fd].buffer);
            // Actualizar en la taba de archivos abiertos
            // el bloque actual
            openfiles_table[fd].currbloqueenmemoria = currblock;
        }

        // Copia el caracter al buffer
        openfiles_table[fd].buffer[openfiles_table[fd].currpos % (512*2)]
         = buffer[cont];

         // Incrementa posicion actual del cursor
         openfiles_table[fd].currpos++;

         // Si la posicion es mayor que el cambio, modifica el tamaño
         if (openfiles_table[fd].currpos > inode_table[currinode].size) {
            inode_table[openfiles_table[fd].inode].size
            = openfiles_table[fd].currpos;
        }

        // Incrementa el contador
        cont++;

        // Si se llena el buffer, escribirlo
        if (openfiles_table[fd].currpos % (512*2) == 0)
            writeblock(currblock, openfiles_table[fd].buffer);
    }
    return cont; // Regresa la cantidad de bytes escritos
}

int vdread(int fd, char *buffer, int bytes) {
    int currblock, currinode, cont = 0, sector, result;
    unsigned short* currptr;

    checkSecboot(&sbp);
    unsigned char inicio_nodos_i = checkiNodesMap(&sbp, inodesmap);
    checkOpenFilesTable();

    //Si no esta abierto regresa error
    if (openfiles_table[fd].inuse == 0)
        return (-1);

    currinode = openfiles_table[fd].inode;

    printf("inode for the file: %d\n", currinode);

    // Copiar byte por byte del buffer del archivo, al buffer
    // que recibe como argumento
    while (cont < bytes) {
        // Obtener la direccion de donde esta el bloque
        // correspondiente a la posicion actual
        currptr = currpostoptr(fd);
        printf("currptr for the block: %p\n", currptr);

        if (currptr == NULL)
            return (-1);

        // Cual es el bloque del que vamos a leer
        currblock = *currptr;

        printf("block for the file: %d\n", currblock);

        // Si el bloque es 0, no tiene datos
        if (currblock != 0) {
            // Si el bloque de la posicion actual no esta en memoria
            // Lee el bloque al buffer del archivo
            if (openfiles_table[fd].currbloqueenmemoria != currblock) {
                // Leer el bloque actual hacia el buffer que
                // esta en la tabla de archivos abiertos
                readblock(currblock, openfiles_table[fd].buffer);
                printf("Buffer read from the block: %s\n", openfiles_table[fd].buffer);
                // Actualizar en la taba de archivos abiertos
                // el bloque actual
                openfiles_table[fd].currbloqueenmemoria = currblock;
            }
        } else {
            return (-1);
        }

        // Copia el caracter al buffer
        buffer[cont] =
        openfiles_table[fd].buffer[openfiles_table[fd].currpos % (512*2)];

        // Incrementa la posicion actual del cursor
        openfiles_table[fd].currpos++;

        // Incrementa el contador
        cont++;

        // Si la posicion es mayor que el tamaño del archivo
        if (openfiles_table[fd].currpos > inode_table[currinode].size)
            // Regresamos los bytes leidos, para evitar accesar
            // a posiciones no validas
            return cont;
    }

    return cont; // Regresa la cantidad de bytes leidos
}

int vdclose(int fd) {
    int currblock = 0, sector = 0;
    unsigned short* currptr;

    checkSecboot(&sbp);
    unsigned char inicio_nodos_i = checkiNodesMap(&sbp, inodesmap);
    checkOpenFilesTable();

    // Si el archivo no esta abierto
    if (!openfiles_table[fd].inuse) {
        return (-1);
    }

    int currinode = openfiles_table[fd].inode;

    currptr = currpostoptr(fd);
    currblock = *currptr;

    // Hay algo en el buffer que escribir
    if (openfiles_table[fd].currpos % (512*2) != 0) {
        writeblock(currblock, openfiles_table[fd].buffer);
    }

    // Escribir el sector de la tabla de nodos i en el disco
    sector = currinode / 8;
    vdwriteseclog(inicio_nodos_i + sector,
                           (char *) &inode_table[sector * 8]);

    openfiles_table[fd].inuse = 0;

    return 1;
}

int vdopen(char *filename, unsigned short mode) {
    int numinode = 0, i = 0;

    // Ver si ya existe el archivo
    numinode = searchinode(filename);

    // Si no existe
    if(numinode == -1)
        return (-1);

    checkOpenFilesTable();

    // Buscar si hay lugar en la tabla de archivos abiertos
    i = 3;
    while (openfiles_table[i].inuse && i < 16)
        i++;

    if (i >= 16) // Llegamos al final y no hay lugar
        return (-1);

    openfiles_table[i].inuse = 1; // Poner el archivo en uso
    openfiles_table[i].inode = numinode; // Nodo del archivo abierto
    openfiles_table[i].currpos = 0; // Siempre es 0 al incio
    openfiles_table[i].openMode = mode;

    return i; // Es el fd del archivo
}
