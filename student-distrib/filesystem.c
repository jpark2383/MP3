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
	uint32_t ret_val, remaining;
	int num_block = offset/KB_4;
	int block_index = offset % KB_4;
	uint32_t *inode_ptr = BOOT_BLOCK_PTR + (inode+1)*KB_4;
	uint32_t data_length = *inode_ptr;
	uint32_t block_number = *(inode_ptr + ((num_block +1) *B_4));
	uint8_t * block_ptr = (uint8_t *)(filesystem.data_start + ((block_number)*KB_4));
	uint8_t * temp_buf = buf;
	
	// clears the buffer
	memcpy(buf, 0, length);
	
	//checks if inode index is valid
	if(inode < 0 || inode >= (filesystem.num_inodes - 1))
	{
		return -1;
	}
	//checks if offset is valid
	if (offset >= data_length)
	{
		return -1;
	}
	
	// This may or may not be right
	if((offset+length) > data_length)
	{
		length = (data_length - offset);
		ret_val = length;
		remaining = length;
	}
	else
	{
		ret_val = length;
		remaining = ret_val;
	}
		
	//check if what's left is less than a block
	if((block_index + remaining) <= KB_4)
	{
		memcpy(temp_buf,block_ptr,remaining);
		remaining = 0;
	}
	else
	{
		memcpy(temp_buf, block_ptr, (KB_4 - block_index));
		remaining -= KB_4 - block_index;
		temp_buf += KB_4 - block_index;
		num_block++;
	}
	
	while(remaining > 0)
	{
		// check if what's remaining is greater than 1 block
		if(remaining >= KB_4)
		{
			memcpy(temp_buf, block_ptr, KB_4);
			remaining -= KB_4 - block_index;
			temp_buf += KB_4 - block_index;
			num_block++;
		}
		if(remaining < KB_4)
		{
			memcpy(temp_buf, block_ptr, remaining);
			remaining = 0;
		}
	}
	
	return ret_val;
	
}

