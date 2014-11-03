#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "lib.h"
#include "pagefile.h"


#define BYTES_4 1
#define BYTES_8 2
#define BYTES_32 8
#define BYTES_64 16
#define MAX_FILE 62
#define B_4 4
#define B_8 8 
#define B_32 32
#define B_36 36
#define B_40 40
#define B_64 64






typedef struct dentry
{
	uint8_t file_name[32];
	uint32_t file_type;
	uint32_t inode_number;
	uint8_t reserved[24];
} dentry_t;

typedef struct fs
{
	uint32_t num_dir_entries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t * dentry_begin;
	uint32_t * data_start;
}fs_t; 

typedef struct fops
{
	int32_t (*fops_open)(const uint8_t* filename);
	int32_t (*fops_read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*fops_write)(int32_t fd, const void* buf, int32_t nbytes);
	int32_t (*fops_close)(int32_t fd);
} fops_t;

typedef struct file_struct
{
	fops_t* fops_ptr;
	uint32_t* inode_ptr;
	uint32_t fpos;
	uint32_t flags;
} file_struct_t;

fs_t filesystem;
uint32_t * BOOT_BLOCK_PTR;
dentry_t dentry[8];
file_struct_t file_struct[8];


extern void init_fs();
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern int32_t filesystem_open(const uint8_t* filename);
extern int32_t filesystem_close(int32_t fd);
extern int32_t filesystem_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t filesystem_write(int32_t fd, const void* buf, int32_t nbytes);


#endif /* _FILESYSTEM_H */
