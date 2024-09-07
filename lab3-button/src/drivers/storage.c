#include <string.h>
#include <system.h>
#include "crc.h"

#include "storage.h"

//*****************************************************************************
//
// The start address of the first storage page
//
//*****************************************************************************
#define STORAGE_START               0x8007800

//*****************************************************************************
//
// The storage page size
//
//*****************************************************************************
#define STORAGE_PAGE_SIZE           0x0000400

//*****************************************************************************
//
// Block structure
//
//*****************************************************************************
typedef struct
{
	struct __attribute__((__packed__))
	{
		u32 value;
		u16 param;

	} data;

	u16 crc;

} blk_t;

//*****************************************************************************
//
// Header structure
//
//*****************************************************************************
typedef struct
{
	struct __attribute__((__packed__))
	{
		u32 magic;
		s16 epoch;

	} data;

	u16 crc;

} hdr_t;

//*****************************************************************************
//
// Storage structure
//
//*****************************************************************************
typedef struct
{
	hdr_t hdr;
	blk_t blk[];

} storage_t;

//*****************************************************************************
//
// The array of storages
//
//*****************************************************************************
static storage_t *storages[2] =
{
	(storage_t *)(STORAGE_START + STORAGE_PAGE_SIZE * 0),
	(storage_t *)(STORAGE_START + STORAGE_PAGE_SIZE * 1),
};

//*****************************************************************************
//
// 'Save all' callback
//
//*****************************************************************************
static void (*save_callback)(void);

//*****************************************************************************
//
// Active storage slot
//
//*****************************************************************************
static u08 active_slot;

//*****************************************************************************
//
// Check the block for empty condition
//
//*****************************************************************************
static
u08 is_blk_empty(blk_t *blk)
{
	for(uint16_t i = 0; i < sizeof(blk_t); i++)
	{
		if(*((u08 *)blk + i) != 0xFF)
		{
			return 0;
		}
	}

	return 1;
}

//*****************************************************************************
//
// Read the header of storage
//
//*****************************************************************************
static
s16 header_read(u08 slot)
{
	u08 *pdata;
	u16 size;
	u16 crc;

	//
	// Check header crc
	//
	pdata = (u08 *)&storages[slot]->hdr.data;
	size = sizeof(storages[slot]->hdr.data);
	crc = storages[slot]->hdr.crc;

	if(crc_8_dallas(pdata, size) == crc)
	{
		return storages[slot]->hdr.data.epoch;
	}

	return -1;
}

//*****************************************************************************
//
// Write the header to storage
//
//*****************************************************************************
static
void header_write(u08 slot, s16 epoch)
{
	hdr_t hdr =
	{
		.data =
		{
			.magic = 'S' << 0 | 'T' << 8 | 'R' << 16 | 'G' << 24,
			.epoch = epoch
		},

		.crc = crc_8_dallas((u08 *)&hdr.data, sizeof(hdr.data))
	};

	//
	// Write data
	//
	FLASH_Unlock();
	FLASH_ProgramWord((u32)&storages[slot]->hdr.data.magic, hdr.data.magic);
	FLASH_ProgramHalfWord((u32)&storages[slot]->hdr.data.epoch, hdr.data.epoch);
	FLASH_ProgramHalfWord((u32)&storages[slot]->hdr.crc, hdr.crc);
	FLASH_Lock();
}


//*****************************************************************************
//
// Read param from storage
//
//*****************************************************************************
u32 storage_read(u16 param)
{
	u32 value = 0xFFFFFFFF;
	u16 blk_cnt;
	u08 *pdata;
	u16 size;
	u16 crc;

	//
	// Compute the total number of blocks
	//
	blk_cnt = (STORAGE_PAGE_SIZE - sizeof(hdr_t)) / sizeof(blk_t);

	//
	// Loop through all blocks
	//
	for(u16 i = 0; i < blk_cnt; i++)
	{
		//
		// Check for target param
		//
		if(storages[active_slot]->blk[i].data.param == param)
		{
			//
			// Check block crc
			//
			pdata = (u08 *)&storages[active_slot]->blk[i].data;
			size = sizeof(storages[active_slot]->blk[i].data);
			crc = storages[active_slot]->blk[i].crc;

			if(crc_8_dallas(pdata, size) == crc)
			{
				value = storages[active_slot]->blk[i].data.value;
			}
		}
	}

	return value;
}


//*****************************************************************************
//
// Write param to storage
//
//*****************************************************************************
void storage_write(u16 param, u32 value)
{
	s32 next_epoch;
	u16 blk_cnt;

	blk_t blk =
	{
		.data =
		{
			.value = value,
			.param = param
		},

		.crc = crc_8_dallas((u08 *)&blk.data, sizeof(blk.data))
	};

	//
	// Compute the total number of blocks
	//
	blk_cnt = (STORAGE_PAGE_SIZE - sizeof(hdr_t)) / sizeof(blk_t);

	//
	// Loop through all blocks
	//
	for(u16 i = 0; i < blk_cnt; i++)
	{
		//
		// Check block for empty
		//
		if(is_blk_empty(&storages[active_slot]->blk[i]))
		{
			//
			// Write data
			//
			FLASH_Unlock();
			FLASH_ProgramWord((u32)&storages[active_slot]->blk[i].data.value, blk.data.value);
			FLASH_ProgramHalfWord((u32)&storages[active_slot]->blk[i].data.param, blk.data.param);
			FLASH_ProgramHalfWord((u32)&storages[active_slot]->blk[i].crc, blk.crc);
			FLASH_Lock();

			//
			// Write complete
			//
			return;
		}
	}

	//
	// Calc the next epoch
	//
	next_epoch = header_read(active_slot) + 1;

	//
	// Change the active slot
	//
	active_slot ^= 1;

	//
	// Erase next storage and write data
	//
	FLASH_Unlock();
	FLASH_ErasePage((u32)storages[active_slot]);
	FLASH_ProgramWord((u32)&storages[active_slot]->blk[0].data.value, blk.data.value);
	FLASH_ProgramHalfWord((u32)&storages[active_slot]->blk[0].data.param, blk.data.param);
	FLASH_ProgramHalfWord((u32)&storages[active_slot]->blk[0].crc, blk.crc);
	FLASH_Lock();

	//
	// Call the save callback
	//
	if(save_callback)
	{
		save_callback();
	}

	//
	// Write header
	//
	header_write(active_slot, next_epoch);
}

//*****************************************************************************
//
// Initialize the storage
//
//*****************************************************************************
void storage_init(void (*save_cb)(void))
{
	s16 epoch[2];

	epoch[0] = header_read(0);
	epoch[1] = header_read(1);

	if((epoch[0] == -1) && (epoch[1] == -1))
	{
		active_slot = 0;

		//
		// Erase the first storage
		//
		FLASH_Unlock();
		FLASH_ErasePage((u32)storages[active_slot]);
		FLASH_Lock();

		//
		// Write header
		//
		header_write(active_slot, 0);
	}
	else
	{
		active_slot = epoch[0] < epoch[1];
	}

	save_callback = save_cb;
}
