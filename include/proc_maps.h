#ifndef UTILS_PROC_MAPS_H
#define UTILS_PROC_MAPS_H
#include <stdio.h>
#include <stdlib.h>

#define PROC_MAPS_READ    1  /* 0001 */
#define PROC_MAPS_WRITE   2  /* 0010 */
#define PROC_MAPS_EXECUTE 4  /* 0100 */
#define PROC_MAPS_PRIVATE 8  /* 1000 */  /* Note 0 bit if region shareable */
#define PROC_MAPS_BITMAP_SIZE 4

typedef unsigned char proc_maps_properties_bitmap_t;
typedef struct proc_maps_ent {
	proc_maps_properties_bitmap_t properties;
	void* addr_start;
	void* addr_end;
	size_t offset;
	unsigned short dev_minor;
	unsigned short dev_major;
	unsigned long int inode; /* ext4 use 2^32 inode */
	char* executable_path;
} proc_maps_ent_t;

void print_proc_maps_ent(const proc_maps_ent_t* ent, FILE* out);

typedef struct proc_maps_file_iterator {
	FILE* proc_maps_file;
	proc_maps_ent_t* current_ent;
} proc_maps_file_iterator_t;

proc_maps_file_iterator_t* create_proc_maps_file_iterator(pid_t pid);
int delete_proc_maps_file_interator(proc_maps_file_iterator_t* it);

proc_maps_ent_t* next_proc_maps(proc_maps_file_iterator_t* it);

#endif
