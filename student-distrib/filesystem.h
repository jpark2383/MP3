#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "lib.h"

#define MAX_FILE 62
#define KB_4 4096
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

fs_t filesystem;
uint32_t * BOOT_BLOCK_PTR;

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
#endif /* _FILESYSTEM_H */
