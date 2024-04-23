#include "serialctrl_memopt.h"
#include "serialctrl_ircaddrproc.h"
#include "gen_link_list.h"
#include "25QFlash.h"
#define MMC_IRCODE_START MEMOPT_INDEX_ADDR_END
uint32_t MMC_FLASH_END_ADDR = 0;
typedef struct 
{
	uint32_t addr;
	uint32_t sector_sum;
}MMC_EMPTY_SPACE;


typedef struct 
{
	uint32_t addr;
	uint32_t size;
}INDEX_SPACE;

GEN_LINK_ST* gpt_empty_addr = NULL;

uint8_t mmc_ircode_addr_flash (void)
{
	MMC_EMPTY_SPACE* pt_empty_space = NULL;
	union pkg_index_if read_buf;
	uint64_t tmp_index_total = 0;
	uint32_t mmc_addr_offset = MMC_IRCODE_START;
	
	INDEX_SPACE tmp_index_st[MEMOTP_INDEX_MAX];
	INDEX_SPACE tmp_tf_st;
	uint8_t index_sum = 0;
	
	uint8_t rcv_len = 0;
	void ** rcv_empty_info;
	
	
	if (gpt_empty_addr != NULL){
		
		rcv_empty_info = gen_link_get(gpt_empty_addr,&rcv_len);
		for (uint8_t i = 0; i < rcv_len; i++ ){
			gen_link_remove(gpt_empty_addr,rcv_empty_info[i]);
			free(rcv_empty_info[i]);
		}
	}else{
		gpt_empty_addr = gen_link_creat();
	}
	
	
	pt_empty_space = (MMC_EMPTY_SPACE*)malloc(sizeof(MMC_EMPTY_SPACE));
	if (gpt_empty_addr == NULL)
		return 0xee;
	
	if (pt_empty_space == NULL)
	{
		gen_link_delate(gpt_empty_addr);
		return 0xee;
	}
	MMC_FLASH_END_ADDR = 2 << (F_GET_MEMSIZE( flash_read_jedec() ) -1 );
	

	tmp_index_total = memopt_index_get_index_mask();
	for (uint8_t i = 0; i < MEMOTP_INDEX_MAX; i++){
		if (((tmp_index_total >> i) & 1UL) == 0)
		{
			memopt_index_read(i,&(read_buf.g_pkg_index));
			if (read_buf.g_pkg_index.ircode_addr != 0xFFFFFFFF && read_buf.g_pkg_index.ircode_total != 0)
			{
				tmp_index_st[index_sum].addr = read_buf.g_pkg_index.ircode_addr;
				tmp_index_st[index_sum].size = read_buf.g_pkg_index.ircode_total;
				index_sum++;
			}
		}
	}
	
	if (index_sum == 0){
		pt_empty_space->addr = MMC_IRCODE_START;
		pt_empty_space->sector_sum = (MMC_FLASH_END_ADDR - MMC_IRCODE_START)/FINFO_SECTOR_SIZE;
		gen_link_add(gpt_empty_addr,pt_empty_space);
	}
	else
	{
		if (index_sum != 1){
			for (uint8_t i = 0; i < index_sum; i++)
			 {
				for (uint8_t j = i; j < index_sum - 1; j++)
				{
					if (tmp_index_st[j].addr > tmp_index_st[j+1].addr)
					{
						tmp_tf_st.addr = tmp_index_st[j].addr;
						tmp_tf_st.size = tmp_index_st[j].size;
						
						tmp_index_st[j].addr = tmp_index_st[j+1].addr;
						tmp_index_st[j].size = tmp_index_st[j+1].size;
						
						tmp_index_st[j+1].addr = tmp_tf_st.addr;
						tmp_index_st[j+1].size = tmp_tf_st.size;
					}
				}
			}
		}
		for (uint8_t i = 0; i < index_sum; i++)
		{
			if (mmc_addr_offset < tmp_index_st[i].addr)
			{
				pt_empty_space = (MMC_EMPTY_SPACE*)malloc(sizeof(MMC_EMPTY_SPACE));
				if (gpt_empty_addr == NULL)
					return 0xe1;
				pt_empty_space->addr = mmc_addr_offset;
				pt_empty_space->sector_sum = (tmp_index_st[i].addr - mmc_addr_offset)/4096;
				gen_link_add(gpt_empty_addr,pt_empty_space);
				mmc_addr_offset = tmp_index_st[i].addr + (((tmp_index_st[i].size)/4096)+ (((tmp_index_st[i].size%4096)!=0)?1:0))*4096;
				if(i == index_sum - 1 && mmc_addr_offset < MMC_FLASH_END_ADDR)
				{	
					pt_empty_space = (MMC_EMPTY_SPACE*)malloc(sizeof(MMC_EMPTY_SPACE));
					if (gpt_empty_addr == NULL)
						return 0xe2;
					pt_empty_space->addr = mmc_addr_offset;
					pt_empty_space->sector_sum = (MMC_FLASH_END_ADDR - mmc_addr_offset)/4096;
					gen_link_add(gpt_empty_addr,pt_empty_space);
				}
			}else
			{	
				if (mmc_addr_offset == tmp_index_st[i].addr)
				{
					mmc_addr_offset = tmp_index_st[i].addr + (((tmp_index_st[i].size)/4096)+ (((tmp_index_st[i].size%4096)!=0)?1:0))*4096;
					if (i == index_sum - 1){
						pt_empty_space = (MMC_EMPTY_SPACE*)malloc(sizeof(MMC_EMPTY_SPACE));
						if (gpt_empty_addr == NULL)
							return 0xe3;
						pt_empty_space->addr = mmc_addr_offset;
						pt_empty_space->sector_sum = (MMC_FLASH_END_ADDR - mmc_addr_offset)/4096;
						gen_link_add(gpt_empty_addr,pt_empty_space);
					}
				}
			}
		}
	}
	return 0;
}


uint32_t mmc_ircode_addr_get (uint32_t len)
{
	uint32_t tmp_addr = 0;
	
	uint8_t rcv_len = 0;
	void ** rcv_empty_info;
	rcv_empty_info = gen_link_get(gpt_empty_addr,&rcv_len);
	if (rcv_empty_info == NULL || rcv_len == 0)
		return 0;
	for(uint8_t i = 0; i < rcv_len; i++)
	{
		if (len <(((MMC_EMPTY_SPACE*)rcv_empty_info[i])->sector_sum * 4096))
		{
			if (((((MMC_EMPTY_SPACE*)rcv_empty_info[i])->sector_sum * 4096) - (len/4096 + ((len%4096 != 0)?1:0))*4096) > 4096)
			{
				tmp_addr = ((MMC_EMPTY_SPACE*)rcv_empty_info[i])->addr;
				((MMC_EMPTY_SPACE*)rcv_empty_info[i])->sector_sum -= (len/4096 + ((len%4096 != 0)?1:0));
				((MMC_EMPTY_SPACE*)rcv_empty_info[i])->addr +=(len/4096 + ((len%4096 != 0)?1:0))*4096;
				return tmp_addr;
			}
			else{
				tmp_addr = ((MMC_EMPTY_SPACE*)rcv_empty_info[i])->addr;
				gen_link_remove(gpt_empty_addr,rcv_empty_info[i]);
				free(rcv_empty_info[i]);
				return tmp_addr;
			}
		}
	}
	return 0;
}
