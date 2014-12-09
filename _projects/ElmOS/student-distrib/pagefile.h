/* pagefile.h - Defines used to support paging
 * controller
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "lib.h"
#include "x86_desc.h"

#define KB_4 4096 


#define KERNEL_PTR 0x00400183

#define PRESENT 0x1
#define MASK_RW 0x2
#define USER_SUPERVISOR 0x4

#define VIDEO_MEM_LOCATION 184
#define VIDEO_MEM_PAGE_SU 0x000B8005
#define VIDEO_MEM_PAGE_U 0x000B8007
#define MB_8_MAP 0x00800087
#define MB_12_MAP 0x00C00087
#define MB_16_MAP 0x01000087
#define MB_20_MAP 0x01400087
#define MB_24_MAP 0x01800087
#define MB_28_MAP 0x01C00087
#define MB_32_MAP 0x02000087
#define P_32 32
#define CR4_PSE 0x00000010
#define ENABLE_PAGING 0x80000000
#define EMPTY 0
//uint32_t page_directory[NUM_ENTRIES_PD] __attribute__ ((aligned(KB_4))); 
//uint32_t page_table[NUM_ENTRIES_PT] __attribute__((aligned(KB_4))); 

extern void paging_init();


