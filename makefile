all: logToPhys.o bitmapFunctions dumpseclog inodeFuncs vdformat vdfdisk fileMgmt prueba

logToPhys.o: logical_to_physical.c
	gcc -o logToPhys.o -c logical_to_physical.c

bitmapFunctions: bitmap_functions.c
	gcc -o bitmapFunctions.o -c bitmap_functions.c

dumpseclog: dumpseclog.c vdisk.o
	gcc -o dumpseclog dumpseclog.c vdisk.o

inodeFuncs: inode_functions.c
	gcc -o inodeFuncs.o -c inode_functions.c

vdfdisk: vdfdisk.c vdisk.o
	gcc -o vdfdisk vdfdisk.c vdisk.o

vdformat: vdformat.c vdisk.o logToPhys.o
	gcc -o vdformat vdformat.c vdisk.o logToPhys.o

fileMgmt: file_management.c 
	gcc -o fileMgmt.o -c file_management.c

prueba: prueba.c vdisk.o logToPhys.o inodeFuncs.o bitmapFunctions.o fileMgmt.o
	 gcc -o prueba prueba.c vdisk.o logToPhys.o inodeFuncs.o bitmapFunctions.o fileMgmt.o
