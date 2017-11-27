// Buscar en el mapa de bits, el primer nodo i que esté libre, es decir, que su bit está en 0, me regresa ese dato
int nextfreeinode() {
	int i,j;
	int result;
	// Checar si el sector de boot de la partición está en memoria
	if(!secboot_en_memoria) {
		// Si no está en memoria, cárgalo
		result=vdreadseclog(1,(char *) &secboot);
		secboot_en_memoria=1;
	}

	mapa_bits_nodos_i= secboot.sec_inicpart +secboot.sec_res; 	
    //Usamos la información del sector de boot de la partición para determinar en que sector inicia el mapa de bits de nodos i 
					
	// Ese mapa está en memoria
	if(!inodesmap_en_memoria) {
		// Si no está en memoria, hay que leerlo del disco
		result=vdreadseclog(mapa_bits_nodos_i,inodesmap);
		inodesmap_en_memoria=1;
	}

	// Recorrer byte por byte mientras sea 0xFF sigo recorriendo
	i=0;
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