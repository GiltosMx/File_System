#ifndef _INODEFUNCS_H_
#define _INODEFUNCS_H_

#define MAX_INODES 24
#define MAX_FILENAMESIZE 18
#define NUM_DIRECT_BLOCKS 10

#include "bitmap_functions.h"
#pragma pack(1)

// Mide 64 bytes
struct INODE {
    char name[18];
    unsigned int datetimecreat; // 32 bits
    unsigned int datetimemodif; // 32 bits
    unsigned int datetimelaacc; // 32 bits
    unsigned short uid; // 16 bits
    unsigned short gid; // 16 bits
    unsigned short perms; // 16 bits
    unsigned int size; // 32 bits
    unsigned short direct_blocks[10]; // 10 x 16 bits = 20 bytes
    unsigned short indirect; // 16 bits
    unsigned short indirect2; // 16 bits
};

unsigned char checkiNodesTable(struct SECBOOTPART* sbp, struct INODE inode_table[]);
int setinode(int num, char* filename, unsigned short atribs, int uid, int gid);
int removeinode(int num);
int searchinode(char* filename);

#endif
