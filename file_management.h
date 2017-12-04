#ifndef _FILEMGMT_H_
#define _FILEMGMT_H_

#include "inode_functions.h"

struct OPENFILES {
    int inuse;// 0 cerrado, 1 abierto
    unsigned short inode;
    int currpos;
    int currbloqueenmemoria;
    char buffer[1024];
    unsigned short buffindirect[512];
    unsigned short openMode; // 1 read, 2 read/write
};

void checkOpenFilesTable();
void freeInodeBlocks(int fd);
// A partir de la posicion actual del apuntador del archivo, regresa la
// direccion de memoria en el nodo i o en el bloque donde esta el
// puntero del archivo
unsigned short* currpostoptr(int fd);

// Esta funcion se usa con las funciones seek, write y read. Con estas
// operaciones el nodo i del archivo debe estar cargado en RAM. Por ejemplo,
// si escribimos informacion en el archivo, se le empiezan a asignar bloques;
// cuales? Los que encontremos disponibles, y esos bloques disponibles hay
// que escribirlos en el nodo i que esta en memoria. Con esta funcion sabemos
// cual es la direccion donde debe ir ese apuntador.
unsigned short *postoptr(int fd,int pos);

int vdcreat(char* filename,unsigned short perms);
int vdopen(char* filename,unsigned short mode);
int vdwrite(int fd, char *buffer, int bytes);
int vdread(int fd, char *buffer, int bytes);
int vdclose(int fd);
int vdopen(char *filename,unsigned short mode);
int vdunlink(char* filename);
int vdseek(int fd, int offset, int whence);
int vdwrite(int fd, char* buffer, int bytes);
int vdread(int fd, char* buffer, int bytes);
int vdclose(int fd);

#endif
