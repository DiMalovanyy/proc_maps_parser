#include "proc_maps.h"
#include <stdio.h>
#include <limits.h>

//static const char regexp = 

proc_maps_properties_bitmap_t permission_flags_to_bitmap(const char* flags) {
	proc_maps_properties_bitmap_t res = 0;
	//printf("Flags %4c\n", flags[0], flags[1], flags[2]);
	if (flags[0] == 'r') res |= PROC_MAPS_READ;
	if (flags[1] == 'w') res |= PROC_MAPS_WRITE;
	if (flags[2] == 'x') res |= PROC_MAPS_EXECUTE;
	if (flags[3] == 'p') res |= PROC_MAPS_PRIVATE;
	return res;
}

int parse_line(const char* line, proc_maps_ent_t* out) {
	unsigned int major, minor;
	char flags[4];
	sscanf(line, "%lx-%lx %4c %lx %hu:%hu %lu %s",
		   (long unsigned*)&out->addr_start,
		   (long unsigned*)&out->addr_end,
			flags, &out->offset, &out->dev_major, &out->dev_minor, &out->inode, out->pathname);
	out->properties = permission_flags_to_bitmap(flags);	
	return 0;
}

proc_maps_file_iterator_t* create_proc_maps_file_iterator(pid_t pid) {
	proc_maps_file_iterator_t* result;
	proc_maps_ent_t* proc_map_entity;
	char proc_maps_file_path[64];
	FILE* proc_maps_file;
	int rc;

	if ((rc = snprintf(proc_maps_file_path, sizeof(proc_maps_file_path), "/proc/%d/maps", pid)) == 0) {
		return NULL;
	}

	if((proc_maps_file = fopen(proc_maps_file_path, "r")) == NULL) {
		return NULL;
	}

	if((result = malloc(sizeof(proc_maps_file_iterator_t))) == NULL) {
		fclose(proc_maps_file);
		return NULL;
	}
	result->proc_maps_file = proc_maps_file;
	result->current_ent = NULL; /* Initialize on first Next */
	return result;
}

int delete_proc_maps_file_interator(proc_maps_file_iterator_t* it) {
	int rc;
	if ((rc = fclose(it->proc_maps_file)) != 0) {
		return rc; /* Leak: Iterator will not be deleted. Should abort execution while this error occured */
	}
	if (it->current_ent) {
		if (it->current_ent->pathname) {
			free(it->current_ent->pathname);
		}
		free(it->current_ent);
	}
	free(it);
	return 0;
}

proc_maps_ent_t* next_proc_maps(proc_maps_file_iterator_t* it) {
	ssize_t bytes_read;
	char* line_buf = NULL;
	size_t line_buf_size = 0;
	if (it->current_ent == NULL) {
		it->current_ent = malloc(sizeof(proc_maps_ent_t));
		it->current_ent->pathname = malloc(sizeof(char)*PATH_MAX);
	}
	
	if ((bytes_read = getline(&line_buf, &line_buf_size, it->proc_maps_file)) <= 0) {
		//End of file 
		if (line_buf) {
			free(line_buf);
		}
		return NULL;
	}
	parse_line(line_buf, it->current_ent);
	free(line_buf);
	return it->current_ent;
}

void print_proc_maps_ent(const proc_maps_ent_t* ent, FILE* out) {
	fprintf(out, "%p-%p", ent->addr_start, ent->addr_end);
	fprintf(out, " %c%c%c%c", 
			(ent->properties & PROC_MAPS_READ) == PROC_MAPS_READ ? 'r' : '-',
			(ent->properties & PROC_MAPS_WRITE) == PROC_MAPS_WRITE ? 'w' : '-',
			(ent->properties & PROC_MAPS_EXECUTE) == PROC_MAPS_EXECUTE ? 'x' : '-',
			(ent->properties & PROC_MAPS_PRIVATE) == PROC_MAPS_PRIVATE ? 'p' : 's');

	fprintf(out, " %lx", ent->offset);
	fprintf(out, " %x:%x", (int)ent->dev_major, (int)ent->dev_minor);
	fprintf(out, " %lu", ent->inode);
	fprintf(out, " %s", ent->pathname);

	fprintf(out, "\n");
	fflush(out);
}
