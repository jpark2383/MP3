#include "filesystem.h"

/* 
 *   filesystem_init
 *   DESCRIPTION: Initializes the filesystem driver
 *   INPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets global variables with values that we need in other functions
 */
void init_fs()
{
   filesystem.num_dir_entries = *(BOOT_BLOCK_PTR);
   filesystem.num_inodes = *(BOOT_BLOCK_PTR + BYTES_4);
   filesystem.num_data_blocks = *(BOOT_BLOCK_PTR + BYTES_8);
   filesystem.dentry_begin = (uint8_t *)(BOOT_BLOCK_PTR + BYTES_64);
   filesystem.data_start = BOOT_BLOCK_PTR + (filesystem.num_inodes+1)*BYTES_4K;
}


/* 
 *   read_dentry_by_name
 *   DESCRIPTION: searches the directory entries for the filename and sends back the dentry.
 *   INPUTS: filename, dentry to fill	
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: sets the global dentry with the values that we need for read or write
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	int i;
	uint8_t file_name[B_32];
	for(i = 0; i < MAX_FILE; i++)
	{
		memcpy(file_name, (filesystem.dentry_begin + i*B_64), B_32);
		if(strncmp((const int8_t*)fname, (const int8_t*)file_name, B_32) == 0)
		{	
			memcpy(dentry, (filesystem.dentry_begin + i*B_64), B_40);
			return 0;
		}
	}
	return -1;
}

/* 
 *   read_dentry_by_index
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

/* 
 * 	 read_data
 *   DESCRIPTION: reads the datablock specified in the inode in the dentry.
 *   INPUTS: inode#, offset where we left off, buf to write the read data to, length of bytes that we need to read	
 *   RETURN VALUE: number of bytes read successfully. returns 0 if EOF is reached
 *   SIDE EFFECTS: buf is filled with the data that was read
 */
 
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	uint32_t ret_val, remaining;
	int num_block = offset/KB_4;
	int block_index = offset % KB_4;
	uint32_t *inode_ptr = BOOT_BLOCK_PTR + (inode+1)*BYTES_4K;
	uint32_t data_length = *inode_ptr;
	uint32_t block_number = *(inode_ptr + ((num_block +1) *BYTES_4));
	uint8_t * block_ptr = (uint8_t *)(filesystem.data_start + ((block_number)*BYTES_4K));
	uint8_t * temp_buf = buf;
	int i;

	// clears the buffer
	for(i = 0; i < length; i++)
	{
		buf[i] = 0;
	}

	//checks if inode index is valid
	if(inode < 0 || inode >= (filesystem.num_inodes - 1))
	{
		return -1;
	}
	//checks if the entire data is being read
	if (offset >= data_length)
	{
		return 0;
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

/* 
 * filesystem_open
 *   DESCRIPTION: filesytem's open function
 *   INPUTS: filename
 *   RETURN VALUE: 0 if successful, -1 if fail
 *   SIDE EFFECTS: sets up the global dentry to enable read/write
 */
int32_t filesystem_open(const uint8_t* filename)
{
	if((strncmp((const int8_t*)filename, (const int8_t*)"" , 1)) != 0)
		return read_dentry_by_name(filename, &dentry[0]);
	else
		return -1;
}

/* 
 * filesystem_close
 *   DESCRIPTION: filesytem's close function
 *   INPUTS: fd
 *   RETURN VALUE: 0 
 *   SIDE EFFECTS: closes the opened file
 */
int32_t filesystem_close(int32_t fd)
{
	return 0;
}

/* 
 * filesystem_read
 *   DESCRIPTION: filesytem's read function
 *   INPUTS: fd,buffer to write read data to, #of bytes to read	
 *   RETURN VALUE: #bytes is successful if successful, -1 if fail
 *   SIDE EFFECTS: reads the data
 */
int32_t filesystem_read(int32_t fd, void* buf, int32_t nbytes)
{
	if(dentry[fd].file_type == 1)
	{
		return dirread();
	}
	return read_data(dentry[fd].inode_number, 0, buf, nbytes);
}
 /* 
 * filesystem_write	
 *   DESCRIPTION: filesytem's write function
 *   INPUTS: fd,buffer to write data to, #of bytes to read	
 *   RETURN VALUE: -1 as it is a read-only file system
 *   SIDE EFFECTS: does nothing
 */
int32_t filesystem_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}

int32_t diropen(const uint8_t* filename)
{
    return 0;
}

int32_t dirclose(int32_t fd)
{
    return 0;
    
}

/* 
 *   dirread
 *   DESCRIPTION: searches the directory entries for regular files
 *   INPUTS: none	
 *   RETURN VALUE: none
 *   SIDE EFFECTS: prints the dierectory to the terminal screen
 */
 
int32_t dirread()
{
	uint32_t i , temp;
	uint8_t max_string[B_32];
	uint8_t * currptr;
	for(i = 0; i < filesystem.num_dir_entries; i++)
    {
		currptr = filesystem.dentry_begin + (i)*(B_64);
		temp = *(currptr + B_32);
		if(temp)
		{
			memcpy(max_string,currptr,B_32);
			printf("%s\n",max_string);
		}
	}
	return 0;
}

 /* 
 *   filesystem_write	
 *   DESCRIPTION: directory write function
 *   INPUTS: NONE
 *   RETURN VALUE: -1 as it is a read-only file system
 *   SIDE EFFECTS: does nothing
 */
 
int32_t dirwrite()
{
	return -1;
}



int loader(const uint8_t* filename)
{
    uint32_t inode; inode_ptr, data_length, block_num, eip;
	uint32_t *block_ptr;
	uint8_t buf[4];
	if(read_dentry_by_name(filename, &dentry) == -1)
		return  -1; //File not found
	/*check if the file is executable*/
	inode = dentry.inode_number;
	inode_ptr = BOOT_BLOCK_PTR + (inode + 1) * BYTES_4K;
	data_length = *(inode__ptr);
	//check if data length is valid
	if(data_length< 4)
		return -1;
	block_num = *(inode__ptr + BYTES_4);
	block_ptr = filesystem.data_start + block_num * BYTES_4K;
	memcpy(buf, block_ptr, 4);
	/*executable*/
	if(buf[0] == 0x7f && buf[1] == 0x45 && buf[2] ==0x4c && buf[3] == 0x46)
	{
		uint8_t data_buf[data_length];
		read_data(dentry.inode_index, 0, data_buf, data_length);
		eip = data_buf[27] << 24 | data_buf[26] << 16 | data_buf[25] << 8  | data_buf[24];
		asm volatile ("mov %0, %%CR3":: "b"(page_directory_task1));
		memccpy()
		
		return eip;
	}
	else
	{
		return -1;
	}
}


