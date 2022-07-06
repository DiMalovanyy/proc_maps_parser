# proc/<pid>/maps parser
Lightweight Stream Oriented /proc/pid/maps parser

## Build
`make` This command will build *libproc_maps_parser.a* and also tests binary *proc_maps_parser_test*

Then you can link static library to your project and include *<proc_maps_parser_path>/include* headers

There is also Ptyhon3 version in *src/python*
**Note:** Python version is not stream oriented. It just parse all file and store it in memory

## Usage

This library is very simple in usage. It will iterate over /proc/<pid>/file and store each entity in *struct proc_maps_ent*
  
**Note:** memory for *proc_maps_ent* will allocates only once, and on each itteretion it just overrides
```
  #define PROC_MAPS_READ    1  /* 0001 */
  #define PROC_MAPS_WRITE   2  /* 0010 */
  #define PROC_MAPS_EXECUTE 4  /* 0100 */
  #define PROC_MAPS_PRIVATE 8  /* 1000 */  /* Note 0 bit if region shareable */

  typedef struct proc_maps_ent {
        proc_maps_properties_bitmap_t properties;
        void* addr_start;
        void* addr_end;
        size_t offset;
        unsigned short dev_minor;
        unsigned short dev_major;
        unsigned long int inode; /* ext4 use 2^32 inode */
        char* pathname;
} proc_maps_ent_t;  
```
### Examples
  
While loop
```
        pid_t pid = <some awesome pid>;
        proc_maps_file_iterator_t* iter = create_proc_maps_file_iterator(pid);
        proc_maps_ent_t* ent;
        while ((ent = next_proc_maps(iter))) {
                /* Do some iteresting stuff with ent */
        }
        delete_proc_maps_file_interator(iter);
```
  
For loop
```
        pid_t pid = <some awesome pid>;
        proc_maps_file_iterator_t* iter = create_proc_maps_file_iterator(pid);

        for(proc_maps_ent_t* ent = next_proc_maps(iter); 
            ent;
            ent = next_proc_maps(iter)) {
               /* Do some iteresting stuff with ent */
        }
        delete_proc_maps_file_interator(iter);
```
  
Discover entitites properties
```
        proc_maps_ent_t* ent = ...;
        if ((ent->properties & PROC_MAPS_READ) && 
            (ent->properties & PROC_MAPS_EXECUTE) &&
            (ent->properties & PROC_MAPS_PRIVATE)) {
            /* We found .text section :) */
        }
```
Python
```
        records = proc_maps_record.parse(<some_awesome_pid>)
        for record in records:
            <do some cool stuff with record>
```
  
  
