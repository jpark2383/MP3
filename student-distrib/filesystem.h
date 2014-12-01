#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "lib.h"
#include "pagefile.h"


#define BYTES_4 1
#define BYTES_8 2
#define BYTES_32 8
#define BYTES_64 16
#define BYTES_4K 1024
#define MAX_FILE 62
#define B_1 1
#define B_2 2
#define B_3 3
#define B_4 4
#define B_8 8 
#define B_32 32
#define B_24 24
#define B_16 16
#define B_36 36
#define B_40 40
#define B_64 64
#define S_27 27
#define S_26 26
#define S_25 25
#define MAGIC_NUM_1  0x7f
#define MAGIC_NUM_2  0x45
#define MAGIC_NUM_3  0x4c
#define MAGIC_NUM_4  0x46
#define PROGRAM_IMG 0x08048000
#define ONE_BLOCK_SIZE 4096
//declartion of data entry struct
typedef struct dentry
{
	uint8_t file_name[B_32];
	uint32_t file_type;
	uint32_t inode_number;
	uint8_t reserved[B_24];
} dentry_t;

//declaration of filesystem struct
typedef struct fs
{
	uint32_t num_dir_entries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t * dentry_begin;
	uint32_t * data_start;
}fs_t; 

//declartion of file operations
typedef struct fops
{
	int32_t (*fops_open)(const uint8_t* filename);
	int32_t (*fops_read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*fops_write)(int32_t fd, const void* buf, int32_t nbytes);
	int32_t (*fops_close)(int32_t fd);
} fops_t;

/* file struct used in the PCB */
typedef struct file_struct
{
	fops_t* fops_ptr;		/* file operations jump table */
	uint32_t* inode_ptr;	/* pointer to inode for this file, only for data files */
	uint32_t fpos;			/* file position member, keeps track of where the user is reading from in the file. read should update this */
	uint32_t flags;			/* flags for keeping this fd in-use */
} file_struct_t;

fs_t filesystem;
uint32_t * BOOT_BLOCK_PTR;
dentry_t dentry[B_8];
dentry_t dentry1;
file_struct_t file_struct[B_8];
uint32_t pc;

extern void init_fs();
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern int32_t filesystem_open(const uint8_t* filename);
extern int32_t filesystem_close(int32_t fd);
extern int32_t filesystem_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t filesystem_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t dirread();
extern int32_t dirwrite();
extern int32_t diropen(const uint8_t* filename);
extern int32_t dirclose(int32_t fd);
    
extern int loader(const uint8_t* filename);
#endif /* _FILESYSTEM_H */
