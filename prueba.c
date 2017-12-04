#include <stdio.h>
#include "file_management.h"

int main(int argc, char const *argv[]) {
    char* writeBuffer = "CONTENIDO DEL ARCHIVO PARA PROBAR.";
    char readBuffer[1024];
    int fd = vdcreat("ARCHIVO_PRUEBA", 9);

    vdwrite(fd, writeBuffer, strlen(writeBuffer));
    vdclose(fd);

    readblock(2, readBuffer);
    printf("read buffer: %s\n", readBuffer);

    return 0;
}
