#include <stdio.h>

struct PRUEBA {
	unsigned char x[5];
	unsigned char y;
};

int main()
{
	printf("Tamaño de prueba = %ld\n",sizeof(struct PRUEBA));
}