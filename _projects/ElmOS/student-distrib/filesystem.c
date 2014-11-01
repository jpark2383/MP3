#include "filesystem.h"


/* 
 * filesystem_init
 *   DESCRIPTION: Initializes the filesystem driver
 *   INPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets global variables with values that we need in other functions
 */
void init_fs()
{
   filesystem.num_dir_entries = *(BOOT_BLOCK_PTR);
   filesystem.num_inodes = *(BOOT_BLOCK_PTR + B_4);
   filesystem.num_data_blocks = *(BOOT_BLOCK_PTR + B_8);
   filesystem.dentry_begin = (uint8_t *)(BOOT_BLOCK_PTR + B_64);
   filesystem.data_start = BOOT_BLOCK_PTR + (filesystem.num_inodes+1)*KB_4;
}


/* 
 * read_dentry_by_name
 *   DESCRIPTION: searches the directory entries for the filename and sends back the dentry.
 *   INPUTS: filename, dentry to fill	
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: sets the global dentry with the values that we need for read or write
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	int i;
	uint8_t file_num[B_32];
	for(i = 0; i < MAX_FILE; i++)
	{
		memcpy(file_num, (filesystem.dentry_begin + i*B_64), B_32);
		if(strncmp((const int8_t*)fname, (const int8_t*)file_num, B_32) ==0)
		{	
			memcpy(dentry, (filesystem.dentry_begin + i*B_64), B_40);
			return 0;
		}
	}
	return -1;
}

/* 
 * read_dentry_by_index
 *   DESCRIPTION: searches the directory entries for the inode_index and sends back the dentry.
 *   INPUTS: inode_index, dentry to fill	
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: sets the global dentry with the values that we need for read or write
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
	int i;
	uint32_t inode_num;
	for(i = 0; i < MAX_FILE; i++)
	{
		inode_num = *filesystem.dentry_begin + i*B_64 + B_36;
		if(inode_num == index)
		{	
			memcpy(dentry, (filesystem.dentry_begin + i*B_64), B_40);
			return 0;
		}
	}
	return -1;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{

}

