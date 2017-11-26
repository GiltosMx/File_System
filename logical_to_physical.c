#include "vdisk.h"

int vdreadseclog(int nseclog, char *buffer)
{
	int secfis,cilindro,superficie;
	int unidad=0;
	// Obtener los datos de la partición para saber
	// en que sector físico, cilindro y superficie inicia 
	// la partición
	// Probablemente sea necesario también tener en memoria
	// el sector de boot de la partición para los datos de
	// la geometría de la partición.
	
	//  Calcular secfis,cilindro,superficie a partir de nseclog
	
	vdreadsector(unidad,superficie,cilindro,secfis,1,buffer);
}

int vdwriteseclog(int nseclog, char *buffer)
{
	int secfis,cilindro,superficie;
	int unidad=0;
	// Obtener los datos de la partición para saber
	// en que sector físico, cilindro y superficie inicia 
	// la partición
	// Probablemente sea necesario también tener en memoria
	// el sector de boot de la partición para los datos de
	// la geometría de la partición.
	
	//  Calcular secfis,cilindro,superficie a partir de nseclog
	
	vdwritesector(unidad,superficie,cilindro,secfis,1,buffer);
}

int readblock(int nbloque,char *buffer)
{
	int secs_x_bloque;
	int seclogbloque;
	int slad;
	int i;
	// Necesito traer a memoria el sector de boot de la partición
	// porque ahí viene el dato de cuántos sectores hay en un bloque
	// Calcular el sector lógico donde comienzan el área de datos (slad)
	
	for(i=0;i<secs_x_bloque;i++)
	{
		vdreadseclog(slad+i,buffer);
		buffer+=512;
	}
}

int writeblock(int nbloque,char *buffer)
{
	int secs_x_bloque;
	int seclogbloque;
	int slad;
	int i;
	// Necesito traer a memoria el sector de boot de la partición
	// porque ahí viene el dato de cuántos sectores hay en un bloque
	// Calcular el sector lógico donde comienzan el área de datos (slad)
	
	for(i=0;i<secs_x_bloque;i++)
	{
		vdwriteseclog(slad+i,buffer);
		buffer+=512;
	}
}