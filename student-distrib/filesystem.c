#include "filesystem.h"
#include "systemcalls.h"
int fd_val;
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
   pc = 0;
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

	uint32_t *inode_ptr , data_length , index , i , block_num;
	//uint8_t * temp;
	uint32_t * block_ptr ;
	uint8_t * block_p8tr;
	uint8_t index_flag;
	uint32_t rval , left;
	uint8_t * bufholder;
	/*clear the buffer first*/
	for(i=0;i<length;i++)
		buf[i]=0;
		
	bufholder=buf;

	inode_ptr = BOOT_BLOCK_PTR + (inode + 1)*BYTES_4K;
	data_length = *inode_ptr;
	/*means it is read*/
	if(offset>=data_length)
		return 0;

	if(inode < 0 || inode >= (filesystem.num_inodes-1) )  
		return -1;

	if((offset+length)>data_length)
	{
		length=(data_length-offset);
		rval=length;
		left=rval;
	}
	else
	{
		rval=length;
		left=rval;
	}
	/*get the block num*/
	i = offset/ONE_BLOCK_SIZE;
	index = offset % ONE_BLOCK_SIZE;
	index_flag=1;
	

	while(left>0)
	{

		block_num=* (inode_ptr + ((i+1)*BYTES_4) );
		block_ptr=filesystem.data_start + (block_num)*BYTES_4K;
		block_p8tr=(uint8_t *)block_ptr;
		/*check to see if it is the first block*/
		if(index_flag)
		{
			index_flag=0;
			block_p8tr=block_p8tr+index;
			if((index+left)<=ONE_BLOCK_SIZE)
			{
				memcpy(buf,block_p8tr,left);
				left=0;
			}	
			else
			{
				memcpy(bufholder,block_p8tr,(ONE_BLOCK_SIZE-index));
				left=left-(ONE_BLOCK_SIZE-index);
				bufholder=bufholder+(ONE_BLOCK_SIZE-index);
				i++;
			}
		}
		/*keep reading the rest of the blocks*/
		else
		{
			if(left>=ONE_BLOCK_SIZE)
			{
				memcpy(bufholder,block_p8tr,ONE_BLOCK_SIZE);
				left=left-(ONE_BLOCK_SIZE-index);
				bufholder=bufholder+(ONE_BLOCK_SIZE-index);
				i++;
			}
			if(left<ONE_BLOCK_SIZE)
			{
				memcpy(bufholder,block_p8tr,left);
				left=0;
			}


		}

	}
	pcblock.file_struct[fd_val].fpos=offset+rval;
	return rval;
	
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
		return read_dentry_by_name(filename, &pcblock.dentry[0]);
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
	fd_val = fd;
	if(pcblock.dentry[fd].file_type == 1)
	{
		return dirclose(fd);
	}
	
	if (fd == 0 || fd == 1)
		return -1;
	pcblock.file_struct[fd].flags = 0;
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
	fd_val = fd;
	if(pcblock.dentry[fd].file_type == 1)
	{
		return dirread(fd, buf);
	}
	return read_data(pcblock.dentry[fd].inode_number, pcblock.file_struct[fd].fpos, buf, nbytes);
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
 *   SIDE EFFECTS: prints the directory to the terminal screen
 */
 
int32_t dirread(int32_t fd, void* buf)
{
	uint32_t temp = 0;
	int8_t* currptr;
		
	if (pcblock.file_struct[fd].fpos >= filesystem.num_dir_entries)
		return 0;
	else{
		while(!temp){
			currptr = (int8_t*)(filesystem.dentry_begin + pcblock.file_struct[fd].fpos*(B_64));
			temp = *(currptr + B_32);
			pcblock.file_struct[fd].fpos++;	
		}
		memcpy(buf,currptr,strlen(currptr));		
		return strlen(currptr);
	}
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
/*
 *   loader
 *   DESCRIPTION: check if the file is executable, if it is load it in programming memory
 *   INPUTS: name of the file
 *   RETURN VALUE: eip if success, -1 if fail 
 *   SIDE EFFECTS: does nothing
*
*/
int loader(const uint8_t* filename)
{
    uint32_t inode, data_length, block_num, eip;
	uint32_t *inode_ptr;
	uint32_t *block_ptr;
	uint8_t buf[B_4];
	if(read_dentry_by_name(filename, &dentry1) == -1)
		return  -1; //File not found
	/*check if the file is executable*/
	inode = dentry1.inode_number;
	inode_ptr = BOOT_BLOCK_PTR + (inode + 1) * BYTES_4K;
	data_length = *(inode_ptr);
	//check if data length is valid
	if(data_length< B_4)
		return -1;
	block_num = *(inode_ptr + BYTES_4);
	block_ptr = filesystem.data_start + block_num * BYTES_4K;
	memcpy(buf, block_ptr, B_4);
	/*executable*/
	if(buf[0] == MAGIC_NUM_1 && buf[B_1] == MAGIC_NUM_2 && buf[B_2] ==MAGIC_NUM_3 && buf[B_3] == MAGIC_NUM_4)
	{
		uint8_t data_buf[data_length];
		read_data(dentry1.inode_number, 0, data_buf, data_length);
		/*get the eip*/
		eip = data_buf[S_27] << B_24 | data_buf[S_26] << B_16 | data_buf[S_25] << B_8  | data_buf[24];
		//printf("in loader pc = %d\n", pc);
		
		// think of pc as # of programs running at the moment..
		// if pc = 0, there are no programs running, so should use task1_pd
			if (pc ==0)
				asm volatile ("mov %0, %%CR3":: "b"(task1_page_directory));
			else if (pc ==1)
				asm volatile ("mov %0, %%CR3":: "b"(task2_page_directory));
			else if (pc ==2)
				asm volatile ("mov %0, %%CR3":: "b"(task3_page_directory));
			else if (pc ==3)
				asm volatile ("mov %0, %%CR3":: "b"(task4_page_directory));
			else if (pc ==4)
				asm volatile ("mov %0, %%CR3":: "b"(task5_page_directory));
			else if (pc ==5)
				asm volatile ("mov %0, %%CR3":: "b"(task6_page_directory));
		//load the program into execution space. 
		memcpy((uint32_t *)PROGRAM_IMG, data_buf, data_length);
		pc = pc+1;
		return eip;
	}
	else
	{
		return -1;
	}
}
