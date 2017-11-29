#ifndef _BITMAPFUNCS_H_
#define _BITMAPFUNCS_H_

#include "logical_to_physical.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

struct DATE {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};

void checkSecboot(struct SECBOOTPART* sbp);
unsigned char checkiNodesMap(struct SECBOOTPART* sbp,
                             unsigned char inodesmap[512]);
unsigned char checkBlocksMap(struct SECBOOTPART* sbp,
                             unsigned char blocksmap[]);
int isinodefree(int inode);
int nextfreeinode();
int assigninode(int inode);
int unassignnode(int inode);
int isblockfree(int block);
int nextfreeblock();
int assignblock(int block);
int unassignblock(int block);
unsigned int datetoint(struct DATE date);
int inttodate(struct DATE *date,unsigned int val);
unsigned int currdatetimetoint();

#endif
