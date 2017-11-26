#define HEADS 8
#define SECTORS 27
#define CYLINDERS 200 

int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);