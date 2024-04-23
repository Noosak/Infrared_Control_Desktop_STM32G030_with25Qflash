#include "HAL_Control_UART.h"
#include "string.h"
#include "25QFlash.h"
#include "stdlib.h"
#include "serialctrl_handler.h"
#include "serialctrl_memopt.h"
#include "serialctrl_ircaddrproc.h"

#include "ircode_emit.h"
#include "ircode_catch.h"

#define DBG_CHECKSUM16 1
typedef void (*pkg_handler_func_pt)(uint8_t* pkg,uint8_t pkg_len);

static void pkg_handler_handshake(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_irindex_write(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_irindex_getotal(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_irindex_read(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_irindex_remove(uint8_t* pkg,uint8_t pkg_len);

static void pkg_handler_ircode_write(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_ircode_read(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_ircode_curient(uint8_t* pkg,uint8_t pkg_len);
//static void pkg_handler_ircode_remove(uint8_t* pkg,uint8_t pkg_len);

static void pkg_handler_ircode_usage(uint8_t* pkg,uint8_t pkg_len);

static void pkg_handler_catch_enable(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_catch_send2serial(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_catch_send2IR(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_catch_directSend(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_catch_write2flash(uint8_t* pkg,uint8_t pkg_len);

static void pkg_handler_irindex_setredy_stat(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_irindex_getallindex(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_irindex_rmallindex(uint8_t* pkg,uint8_t pkg_len);
static void pkg_handler_chip_cleanAll(uint8_t *pkg,uint8_t pkg_len);
	
static uint8_t pkg_head[] = {0x1D,0x63,0xC0};
static uint8_t pkg_tail[] = {0x2D,0x31,0x53};

static uint8_t garg_pkg_write_devnum = 0 ;
static uint8_t garg_pkg_write_pcknum = 0 ;
static uint8_t garg_pkg_write_pcktotal = 0 ;

static pkg_handler_func_pt pkg_handle_buffer[] = {
	pkg_handler_handshake,
	pkg_handler_irindex_write,
	pkg_handler_irindex_getotal,
	pkg_handler_irindex_read,
	pkg_handler_irindex_remove,
	pkg_handler_ircode_write,
	pkg_handler_ircode_read,
	pkg_handler_ircode_usage,
	pkg_handler_catch_enable,
	pkg_handler_catch_send2serial,
	pkg_handler_ircode_curient,
	pkg_handler_catch_write2flash,
	NULL,
	pkg_handler_irindex_setredy_stat,
	pkg_handler_catch_send2IR,
	pkg_handler_catch_directSend,
	pkg_handler_irindex_getallindex,
	pkg_handler_irindex_rmallindex,
	pkg_handler_chip_cleanAll,
};

void serial_inpkg(uint8_t cmd,uint8_t* data,uint8_t len)
{
	uint8_t* tmp_pkg_pt = 0 ;
	uint16_t sun16 = 0;
	uint16_t trsun16 = 0;
	if (len != 0 && data == NULL )
		return ;
	
	tmp_pkg_pt = (uint8_t*)malloc(10 + len);
	if (tmp_pkg_pt == NULL)
		return ;
	
	memcpy(tmp_pkg_pt,pkg_head,3);
	tmp_pkg_pt[3] = cmd;
	tmp_pkg_pt[4] = len;
	for (uint8_t i = 0; i < len ; i++)
		sun16 += *(data + i);
	sun16 += cmd;
	sun16 += len;
	trsun16 = ((sun16 & 0x00FF) <<8) + ((sun16 & 0xFF00) >>8);
	memcpy(tmp_pkg_pt + 5,data,len);
	memcpy(tmp_pkg_pt + 5 +len,&trsun16,2);
	memcpy(tmp_pkg_pt+ 7 + len,pkg_tail,3);
	hal_control_uart2_send(tmp_pkg_pt,10 + len);
	free(tmp_pkg_pt);
}

uint8_t* serial_depkg(uint8_t* pkgbuff,uint16_t pkg_len,uint8_t* cmd,uint8_t* data_len)
{
	uint8_t* tmp_data = NULL;
	if (pkgbuff == NULL || pkg_len < 10 || cmd == NULL)
		return NULL;
	
	if (pkgbuff[4] != pkg_len - 10)
		return NULL;
	tmp_data = (uint8_t*) malloc(pkg_len - 10);
	if (tmp_data == NULL)
		return NULL;
	
	*cmd = pkgbuff[3];
	
	*data_len = pkg_len - 10;
	memcpy(tmp_data,pkgbuff+5,pkgbuff[4]);
	return tmp_data;
}

void serial_msg_handler(uint8_t *msg,uint16_t* len){
	static uint16_t getlen = 0;
	static uint8_t*  pkg_data = NULL;
	uint8_t pkg_cmd = 0;
	uint8_t  pkg_len = 0;
	uint16_t sun16 = 0;
	uint16_t trsun16 = 0;
	pkg_data = NULL;
	getlen =  *len;
	if (msg == NULL || getlen == 0 || getlen > 300)
		return ;
	
	
	if (getlen < 10){
		goto CLEAN_PKG;
	}
	/**Check pkg head code & pkg tail code**/
	if (memcmp(msg, pkg_head, 3) != 0 || memcmp(msg + getlen - 3, pkg_tail, 3) != 0)
		goto CLEAN_PKG;
	
	/**check pkg sum16**/
#if DBG_CHECKSUM16
	
	for (uint16_t i = 0; i < (getlen - 8); i++)
		sun16 += msg[i+3];
	trsun16 = ((sun16 & 0x00FF) <<8) + ((sun16 & 0xFF00) >>8);
	if (memcmp(msg + getlen - 5, &trsun16, 2) != 0)
		goto CLEAN_PKG;
#endif
	/**pkg checked**/
	pkg_data = serial_depkg(msg,getlen,&pkg_cmd,&pkg_len);
	if (pkg_data == NULL || pkg_cmd >sizeof(pkg_handle_buffer))
		goto CLEAN_PKG;
	
	if (pkg_handle_buffer[pkg_cmd] == NULL)
		goto CLEAN_PKG;
	
	pkg_handle_buffer[pkg_cmd]((uint8_t*)pkg_data,pkg_len);
	free(pkg_data);
	pkg_data = NULL;
	flash_release_buf();
/**clean msg**/
	CLEAN_PKG:
	memset(msg,0,getlen);
	*len = 0;
	flash_release_buf_w();
}


void pkg_handler_handshake(uint8_t* pkg,uint8_t pkg_len){
	uint8_t tmpbuf[2] = {0};
	uint8_t tmp_jedec_size = F_GET_MEMSIZE(falsh_get_jedec());
	if(tmp_jedec_size >= 0x15 && tmp_jedec_size <= 0x20) {
		tmpbuf[0] = 0xCC;
		tmpbuf[1] = tmp_jedec_size;
	}
	else
	{
		tmpbuf[0] = 0x00;
		tmpbuf[1] = 0x00;
	}
	serial_inpkg(0x00,tmpbuf,2);
}

void pkg_handler_irindex_write(uint8_t* pkg,uint8_t pkg_len){
	PKG_INDEX write_index;
	memset(&write_index,0xFF,sizeof(write_index));
	union pkg_index_if read_buf;
	memset(&read_buf,0xFF,sizeof(read_buf));
	uint8_t tmpbuf[1] = {0};
	uint16_t tmp_offsetsum = 1;
	uint32_t tmp_writeaddr = 0;
	uint64_t tmp_mask = memopt_index_get_index_mask();
	if (pkg == NULL || pkg_len < 5 || pkg_len - 5 != pkg[4]){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x01,tmpbuf,1);
		return ;
	}
	
	if(((tmp_mask >> pkg[0]) & 1UL)==0)
	{
		if (memopt_index_remove(pkg[0]) == 0xCC)
			tmpbuf[0] = 0xCC;
		else{
			tmpbuf[0] = 0xE1;
			serial_inpkg(0x01,tmpbuf,1);
			return ;
		}
	}
	mmc_ircode_addr_flash();
	
	write_index.device_num = pkg[0];
	write_index.device_type = pkg[1];
	write_index.device_ircode_len = (uint16_t)((uint16_t)(pkg[2]<<8) + pkg[3]);
	write_index.device_offset_total = pkg[4];
	for (uint8_t i=0; i < pkg[4];i++){
		if (pkg[5+i] != 0)
			tmp_offsetsum *= pkg[5+i];
		else {
			tmpbuf[0] = 0xE2;
			serial_inpkg(0x01,tmpbuf,1);
			return ;
		}
	}
	
	write_index.ircode_total = (((write_index.device_ircode_len * tmp_offsetsum)/4096)+((((write_index.device_ircode_len * tmp_offsetsum)%4096)!=0)?1:0))*4096;
	memcpy(write_index.offset,pkg+5,pkg[4]);
	
	tmp_writeaddr = mmc_ircode_addr_get(write_index.ircode_total);
	if (tmp_writeaddr == 0)
	{
		tmpbuf[0] = 0xE3;
		serial_inpkg(0x01,tmpbuf,1);
		return ;
	}
	
	write_index.ircode_addr = tmp_writeaddr;
	if (memopt_index_write(&write_index) != 0 )
	{
		tmpbuf[0] = 0xE4;
		serial_inpkg(0x01,tmpbuf,1);
		mmc_ircode_addr_flash();
	}else{
		memopt_index_read(*pkg,&(read_buf.g_pkg_index));
		if (memcmp(&(read_buf.g_pkg_index), &write_index, sizeof(PKG_INDEX)) == 0){
			tmpbuf[0] = 0xCC;
			serial_inpkg(0x01,tmpbuf,1);
		}else 
		{
			tmpbuf[0] = 0xE5;
			serial_inpkg(0x01,tmpbuf,1);
		}
	}
}

void pkg_handler_irindex_getotal(uint8_t* pkg,uint8_t pkg_len)
{
	uint64_t tmp_index_total = 0;
	uint8_t tmp_empty_sum = 0;
	uint8_t tmpbuf[MEMOTP_INDEX_MAX + 1] = {0};
	if (pkg_len != 0 ){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x02,tmpbuf,1);
		return ;
	}
	tmp_index_total = memopt_index_get_index_mask();

	for (uint8_t i = 0; i <MEMOTP_INDEX_MAX ; i++)
	{
		if (((tmp_index_total >>i) & 1UL) == 0)
			tmpbuf[0] = i;
	}
	
	for (uint8_t i = 0; i < tmpbuf[0] ; i++)
	{
		if (((tmp_index_total >>i) & 1UL) != 0){
			tmpbuf[1 + tmp_empty_sum] = i;
			tmp_empty_sum ++;
		}
	}
	
	if (tmp_empty_sum != 0)
		serial_inpkg(0x02,tmpbuf,1+tmp_empty_sum);
	else
		serial_inpkg(0x02,tmpbuf,1);
	
}
void pkg_handler_irindex_read(uint8_t *pkg,uint8_t pkg_len){
	uint8_t tmpbuf[sizeof(PKG_INDEX)] = {0};
	union pkg_index_if read_buf;
	memset(tmpbuf,0,sizeof(PKG_INDEX));
	
	if (pkg == NULL || pkg_len != 1 ){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x03,tmpbuf,1);
		return ;
	}
	if (memopt_index_read(*pkg,&(read_buf.g_pkg_index)) != 0)
	{
		tmpbuf[0] = 0xE1;
		serial_inpkg(0x03,tmpbuf,1);
		return ;
	}
	tmpbuf[0] = read_buf.g_pkg_index.device_num;
	tmpbuf[1] = read_buf.g_pkg_index.device_type;
	tmpbuf[2] = read_buf.g_pkg_index.device_ircode_stat;
	
	tmpbuf[3] = (uint8_t)(read_buf.g_pkg_index.device_ircode_len >> 8);
	tmpbuf[4] = (uint8_t)(read_buf.g_pkg_index.device_ircode_len & 0x00FF);
	tmpbuf[5] = read_buf.g_pkg_index.device_offset_total;
	if (tmpbuf[5] > INDEX_MAX_OFFSER)
		tmpbuf[5] = 0;
	for (uint8_t i = 0; i < tmpbuf[5]; i++)
	{
		tmpbuf[6+i] = read_buf.g_pkg_index.offset[i];
	}
	serial_inpkg(0x03,tmpbuf,tmpbuf[5] + 6);
}

void pkg_handler_irindex_remove(uint8_t* pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[1] = {0};
	uint8_t cncode[2] = {0x44,0x4C}; 
	if (pkg == NULL || (memcmp((pkg+1), cncode,2) != 0)){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x04,tmpbuf,1);
		return ;
	}
	
	tmpbuf[0] = memopt_index_remove(pkg[0]);
	mmc_ircode_addr_flash();
	serial_inpkg(0x04,tmpbuf,1);
}

void pkg_handler_irindex_setredy_stat(uint8_t* pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[1] = {0};
	union pkg_index_if read_buf;
	if (pkg == NULL || pkg_len != 2){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x0D,tmpbuf,1);
		return ;
	}
	
	if (memopt_index_read(*pkg,&(read_buf.g_pkg_index)) == 0xEE)
	{
		tmpbuf[0] = 0xE1;
		serial_inpkg(0x0D,tmpbuf,1);
		return ;
	}
	
	if (pkg[1] == 0xCC)
	{
		read_buf.g_pkg_index.device_ircode_stat = 0xCC;
		memopt_index_write(&(read_buf.g_pkg_index));
		tmpbuf[0] = 0xCC;
		serial_inpkg(0x0D,tmpbuf,1);
	}
	else
	{
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x0D,tmpbuf,1);
	}
}

void pkg_handler_ircode_write(uint8_t* pkg,uint8_t pkg_len){
	
	uint64_t tmp_mask = 0;
	
	uint8_t* readflash_pt = NULL;
	uint8_t tmpbuf[4] = {0};
	uint8_t tmpcmpbuf[8] = {0};
	uint8_t tmpcmpbuf2[8] = {0};
	uint32_t write_addr = 0;
	uint32_t offset_sum = 0 ;
	uint16_t offset_cnt[INDEX_MAX_OFFSER];
	static uint8_t maxRevLen = 0;
	union pkg_index_if read_buf;
	if (pkg == NULL){
		memcpy(tmpbuf,pkg,3);
		tmpbuf[3] = 0xE0;
		serial_inpkg(0x05,tmpbuf,4);
		return ;
	}
	for(uint8_t i = 0; i < INDEX_MAX_OFFSER; i++)
		offset_cnt[i] = 1;
//	memset(offset_cnt, (uint16_t)1, sizeof(offset_cnt));
	memset(tmpcmpbuf, 0xFF, 8);
	memopt_index_read(pkg[0],&(read_buf.g_pkg_index));
	
	tmp_mask = memopt_index_get_index_mask();
	
	if(((tmp_mask >> pkg[0]) & 1UL)!=0)
	{
		memcpy(tmpbuf,pkg,3);
		tmpbuf[3] = 0xE1;
		serial_inpkg(0x05,tmpbuf,4);
		return ;
	}
	
	if (read_buf.g_pkg_index.device_num != pkg[0])
	{
		memcpy(tmpbuf,pkg,3);
		tmpbuf[3] = 0xE2;
		serial_inpkg(0x05,tmpbuf,4);
		return ;
	}
	
	if (read_buf.g_pkg_index.ircode_addr == 0xFFFFFFFF)
	{
		memcpy(tmpbuf,pkg,3);
		tmpbuf[3] = 0xE3;
		memcpy(tmpbuf + 4 ,pkg + 4,read_buf.g_pkg_index.device_offset_total);
		serial_inpkg(0x05,tmpbuf,4 + read_buf.g_pkg_index.device_offset_total);
		return ;
	}
	
	for(uint8_t i = 0 ; i < read_buf.g_pkg_index.device_offset_total -1 ; i ++ )
	{
		if (pkg[4 + i] >= read_buf.g_pkg_index.offset[i])
		{
			memcpy(tmpbuf,pkg,3);
			tmpbuf[3] = 0xE4;
			memcpy(tmpbuf + 4 ,pkg + 4,read_buf.g_pkg_index.device_offset_total);
			serial_inpkg(0x05,tmpbuf,4 + read_buf.g_pkg_index.device_offset_total);
			return ;
		}
	};
	
	if (pkg[1] < pkg[2] - 1)
	{
		maxRevLen = pkg[3];
	}
	
	
	
	garg_pkg_write_devnum = pkg[0];
	garg_pkg_write_pcknum = pkg[1];
	garg_pkg_write_pcktotal = pkg[2];
	
	if (pkg[1] == pkg[2] - 1 )
	{
		garg_pkg_write_devnum = 0 ;
		garg_pkg_write_pcknum = 0 ; 
		garg_pkg_write_pcktotal = 0 ;
	}
	
	for (uint8_t i = 0; i < read_buf.g_pkg_index.device_offset_total - 1; i ++){
		for(uint8_t j = i + 1; j< read_buf.g_pkg_index.device_offset_total; j++)
		{
				offset_cnt[i] *= read_buf.g_pkg_index.offset[j];
		}
		offset_sum += (pkg[4+i]*offset_cnt[i]);
	}
	
	offset_sum +=pkg[4+read_buf.g_pkg_index.device_offset_total - 1];
	
	write_addr = read_buf.g_pkg_index.ircode_addr + offset_sum * read_buf.g_pkg_index.device_ircode_len + pkg[1] * maxRevLen;
	
	readflash_pt = flash_fread(write_addr, 8);
	memcpy(tmpcmpbuf2,readflash_pt,8);
	if (memcmp(tmpcmpbuf,readflash_pt,8) != 0)
	{
		memcpy(tmpbuf,pkg,3);
		tmpbuf[3] = 0xE5;
		serial_inpkg(0x05,tmpbuf,4);
		return ;
	}
	flash_write(write_addr,(pkg + 4 +read_buf.g_pkg_index.device_offset_total),pkg[3]);
	memcpy(tmpbuf,pkg,3);
	tmpbuf[3] = 0xCC;
	memcpy(tmpbuf + 4 ,pkg + 4,read_buf.g_pkg_index.device_offset_total);
	serial_inpkg(0x05,tmpbuf,4 + read_buf.g_pkg_index.device_offset_total);
}

void pkg_handler_ircode_read(uint8_t* pkg,uint8_t pkg_len){
	uint8_t tmpbuf[0x64] = {0};
	uint32_t read_addr = 0;
	uint32_t offset_sum = 0 ;
	uint16_t offset_cnt[INDEX_MAX_OFFSER];
	
	uint8_t max_send_len = 0;
	uint8_t pack_total = 0;
	uint8_t send_len = 0;
	uint8_t index_offset_total = 0;
	uint8_t* readflash_pt = NULL;
	union pkg_index_if read_buf;
	if (pkg == NULL){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x06,tmpbuf,1);
		return ;
	}
	memset(&read_buf,0xFF,sizeof(union pkg_index_if));
	for(uint8_t i = 0; i < INDEX_MAX_OFFSER; i++)
		offset_cnt[i] = 1;
	memopt_index_read(pkg[0],&(read_buf.g_pkg_index));
	if (read_buf.g_pkg_index.device_num != pkg[0])
	{
		tmpbuf[0] = 0xE1;
		serial_inpkg(0x06,tmpbuf,1);
		return ;
	}
	
	if (read_buf.g_pkg_index.ircode_addr == 0xFFFFFFFF)
	{
		tmpbuf[0] = 0xE2;
		serial_inpkg(0x06,tmpbuf,1);
		return ;
	}
	
	if ((pkg[1] == pkg[2]) && (pkg[1] != 0 || pkg[2] != 0))
	{
		tmpbuf[0] = 0xE3;
		serial_inpkg(0x06,tmpbuf,1);
		return ;
	}	
	
	for (uint8_t i = 0; i < read_buf.g_pkg_index.device_offset_total - 1; i ++){
		for(uint8_t j = i + 1; j< read_buf.g_pkg_index.device_offset_total; j++)
		{
				offset_cnt[i] *= read_buf.g_pkg_index.offset[j];
		}
		offset_sum += (pkg[4+i]*offset_cnt[i]);
	}
		
	offset_sum += pkg[3+read_buf.g_pkg_index.device_offset_total - 1];
	index_offset_total = read_buf.g_pkg_index.device_offset_total;
//	max_send_len = UINT8_MAX - read_buf.g_pkg_index.device_offset_total - 3;	
		max_send_len = 0x64;	
	read_addr = read_buf.g_pkg_index.ircode_addr + offset_sum * read_buf.g_pkg_index.device_ircode_len + pkg[1] * max_send_len;
	
	if (max_send_len > read_buf.g_pkg_index.device_ircode_len){
		send_len = read_buf.g_pkg_index.device_ircode_len;
	}else{
		if (max_send_len > read_buf.g_pkg_index.device_ircode_len - pkg[1] * max_send_len)
			send_len = read_buf.g_pkg_index.device_ircode_len - pkg[1] * max_send_len;
		else 
			send_len = max_send_len;
	}
	
	pack_total = read_buf.g_pkg_index.device_ircode_len / max_send_len + 1;
	
	readflash_pt = flash_fread(read_addr, send_len);
	memcpy(tmpbuf,pkg,2);
	tmpbuf[2] = pack_total;
	tmpbuf[3] = send_len;
	
	memcpy(tmpbuf + 4,pkg + 3,index_offset_total);
	
	memcpy(tmpbuf + 4 + index_offset_total ,readflash_pt,send_len);
	serial_inpkg(0x06,tmpbuf,(uint8_t)(index_offset_total + send_len + 4 ));
}

void pkg_handler_ircode_usage(uint8_t* pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[5] = {0};
	uint64_t tmp_index_total = 0;
	uint16_t usage_block = 0;
	uint32_t usage_size = 0;
	union pkg_index_if read_buf;
	if (pkg == NULL || pkg_len != 1){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x07,tmpbuf,1);
		return ;
	}
	
	tmp_index_total = memopt_index_get_index_mask();

	for (uint8_t i = 0; i <MEMOTP_INDEX_MAX ; i++)
	{
		if (((tmp_index_total >>i) & 1UL) == 0)
		{
			memopt_index_read(i ,&(read_buf.g_pkg_index));
			if (read_buf.g_pkg_index.device_num != 0xFF)
				usage_size += read_buf.g_pkg_index.ircode_total;
		}
	}
	usage_block = usage_size / 4096;
	
	if (*pkg == 0x02)
	{
		tmpbuf[0] = 0x02 ;
		tmpbuf[1] = (uint8_t)((usage_size & 0xFF000000) >> (8*3)) ; 
		tmpbuf[2] = (uint8_t)((usage_size & 0x00FF0000)  >> (8*2)) ;
		tmpbuf[3] = (uint8_t)((usage_size & 0x0000FF00)  >> (8*1)) ; 
		tmpbuf[4] = (uint8_t)((usage_size & 0x000000FF)  >> (8*0)) ;
		serial_inpkg(0x07,tmpbuf,5);	
	}
	else{
		tmpbuf[0] = 0x01 ;
		tmpbuf[1] = (uint8_t)((usage_block & 0xFF00) >> 8) ; 
		tmpbuf[2] = (uint8_t)((usage_block & 0x00FF)) ;
		serial_inpkg(0x07,tmpbuf,3);	
	}
}

void pkg_handler_ircode_curient(uint8_t* pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[3] = {0};
	if (pkg == NULL || pkg_len != 0){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x0A,tmpbuf,1);
		return ;
	}
	
	tmpbuf[0] = garg_pkg_write_devnum ;
	tmpbuf[1] = garg_pkg_write_pcknum ; 
	tmpbuf[2] = garg_pkg_write_pcktotal ;
	serial_inpkg(0x0A,tmpbuf,3);	
}

void pkg_handler_catch_enable(uint8_t* pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[2] = {0};
	if (pkg == NULL || pkg_len != 1){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x08,tmpbuf,1);
		return ;
	}
	if (pkg[0] == 0x00){
		ircode_catch_enable(0);
		tmpbuf[0] = pkg[0];
		tmpbuf[1] = 0xCC;
		serial_inpkg(0x08,tmpbuf,2);	
	}
	else{
		if(ircode_catch_enable(1) == 0xCC)
		{
			tmpbuf[0] = pkg[0];
			tmpbuf[1] = 0xCC;
			serial_inpkg(0x08,tmpbuf,2);	
		}else{
			tmpbuf[0] = pkg[0];
			tmpbuf[1] = 0xE1;
			serial_inpkg(0x08,tmpbuf,2);	
		}
	}	
}


void pkg_handler_catch_send2IR(uint8_t* pkg,uint8_t pkg_len)
{
	uint8_t ret = 0;
		uint8_t tmpbuf[4] = {0};
		uint16_t tmp_lenTotal = 0;
		uint16_t tmp_inArrayNum = 0;
		if (pkg == NULL || pkg_len < 4){
			tmpbuf[0] = pkg[0];
			tmpbuf[1] = 0xE0;
			tmpbuf[2] = 0x00;
			tmpbuf[3] = 0x00;
			serial_inpkg(0x0E,tmpbuf,4);
			return ;
		}
		
		tmp_lenTotal = (uint16_t)((uint16_t)pkg[1] << 8) + (uint16_t)pkg[2];
		ret = ircode_emit_inarray(pkg[0],pkg+4,tmp_lenTotal,pkg[3]);
		tmp_inArrayNum = ircode_ircode_emit_getInArrayNum();
		
		tmpbuf[0] = pkg[0];
		tmpbuf[1] = ret;
		tmpbuf[2] = (tmp_inArrayNum & 0xFF00) >> 8;
		tmpbuf[3] = (tmp_inArrayNum & 0x00FF);
		serial_inpkg(0x0E,tmpbuf,4);
}

void pkg_handler_catch_directSend(uint8_t* pkg,uint8_t pkg_len)
{
		uint8_t tmpbuf[1] = {0};
		if (pkg == NULL || pkg_len != 0){
			tmpbuf[0] = 0xE0;
			serial_inpkg(0x0F,tmpbuf,1);
			return ;
		}
			tmpbuf[0] = ircode_emit_directSend();
			serial_inpkg(0x0F,tmpbuf,1);	
}

void pkg_handler_catch_send2serial(uint8_t* pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[4] = {0};
	if (pkg == NULL || pkg_len != 3 || pkg[0] != ircode_send_pkgnum || pkg[1] != ircode_send_pkgtotal){
		tmpbuf[0] = pkg[0];
		tmpbuf[1] = pkg[1];
		tmpbuf[2] = pkg[2];
		tmpbuf[3] = 0xE0;
		serial_inpkg(0x09,tmpbuf,4);
		return ;
	}
	
	switch(pkg[2]){
		case 0xCC:
		{
			if (ircode_send_pkgnum < ircode_send_pkgtotal -1)
			{
				ircode_send_pkgnum ++;
				ircode_catch_send();
			}else
				if(ircode_send_pkgnum ==  ircode_send_pkgtotal -1){
					goto TRANSMATE_END;
				}else
				{
					tmpbuf[0] = pkg[0];
					tmpbuf[1] = pkg[1];
					tmpbuf[2] = pkg[2];
					tmpbuf[3] = 0xE1;
					serial_inpkg(0x09,tmpbuf,4);	
					break;
				}
			
		}break;
		case 0x01:
		{
			ircode_catch_send();
		}break;
		case 0x02:
		{
TRANSMATE_END:
			ircode_catch_free_data();
			ircode_catch_stat = 0;
			ircode_catch_outime_cnt = 0;
			ircode_catch_data_len = 0;
			ircode_send_pkgnum = 0;
			ircode_send_pkgtotal = 0;
			
			tmpbuf[0] = pkg[0];
			tmpbuf[1] = pkg[1];
			tmpbuf[2] = pkg[2];
			tmpbuf[3] = 0xCC;
			serial_inpkg(0x09,tmpbuf,4);
		}break;
	}
}

void pkg_handler_catch_write2flash(uint8_t *pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[1] = {0};
	uint32_t write_addr = 0;
	uint32_t offset_sum = 0 ;
	uint16_t offset_cnt[INDEX_MAX_OFFSER];
	uint8_t* readflash_pt = NULL;
	uint8_t tmpcmpbuf[8] = {0};
	uint8_t tmpcmpbuf2[8] = {0};
	memset(tmpcmpbuf, 0xFF, 8);
	union pkg_index_if read_buf;
	if (pkg == NULL || pkg_len < 1){
		tmpbuf[0] = 0xE0;
		serial_inpkg(0x0B,tmpbuf,1);
		return ;
	}
	
	memopt_index_read(*pkg,&(read_buf.g_pkg_index));
	if (read_buf.g_pkg_index.device_num != *pkg)
	{
		tmpbuf[0] = 0xE1;
		serial_inpkg(0x0B,tmpbuf,1);
		return ;
	}
	
	if (read_buf.g_pkg_index.ircode_addr == 0xFFFFFFFF)
	{
		tmpbuf[0] = 0xE2;
		serial_inpkg(0x0B,tmpbuf,1);
		return ;
	}
	
	if (read_buf.g_pkg_index.device_offset_total != pkg_len - 1 )
	{
		tmpbuf[0] = 0xE3;
		serial_inpkg(0x0B,tmpbuf,1);
		return ;
	}
	
	if (read_buf.g_pkg_index.device_ircode_len != ircode_catch_data_len)
	{
		tmpbuf[0] = 0xE4;
		serial_inpkg(0x0B,tmpbuf,1);
		return ;
	}
	
	if (ircode_catch_get_p() == NULL)
	{
		tmpbuf[0] = 0xE5;
		serial_inpkg(0x0B,tmpbuf,1);
		return ;
	}
	for(uint8_t i = 0; i < INDEX_MAX_OFFSER; i++)
		offset_cnt[i] = 1;
	
	for (uint8_t i = 0; i < read_buf.g_pkg_index.device_offset_total - 1; i ++){
		for(uint8_t j = i + 1; j< read_buf.g_pkg_index.device_offset_total; j++)
		{
			offset_cnt[i] *= read_buf.g_pkg_index.offset[j];
		}
		offset_sum += (pkg[1+i]*offset_cnt[i]);
	}
	
	offset_sum +=pkg[1+read_buf.g_pkg_index.device_offset_total - 1];
	
	write_addr = read_buf.g_pkg_index.ircode_addr + offset_sum * read_buf.g_pkg_index.device_ircode_len;
	
	readflash_pt = flash_fread(write_addr, 8);
	memcpy(tmpcmpbuf2,readflash_pt,8);
	if (memcmp(tmpcmpbuf,readflash_pt,8) != 0)
	{
		tmpbuf[0] = 0xE6;
		serial_inpkg(0x0B,tmpbuf,1);
		return ;
	}
	flash_write(write_addr,(uint8_t*)ircode_catch_get_p(),sizeof(uint16_t)*ircode_catch_data_len);
	ircode_catch_free_data();
	tmpbuf[0] = 0xCC;
	serial_inpkg(0x0B,tmpbuf,1);
} 

void pkg_handler_irindex_getallindex(uint8_t *pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[sizeof(PKG_INDEX)] = {0};
	union pkg_index_if read_buf;
	memset(tmpbuf,0,sizeof(PKG_INDEX));
	
	if (pkg == NULL || pkg_len != 1 ){
		tmpbuf[0] = 0x00;
		serial_inpkg(0x10,tmpbuf,1);
		return ;
	}
	memopt_index_read(*pkg,&(read_buf.g_pkg_index));
	tmpbuf[0] = read_buf.g_pkg_index.device_num;
	tmpbuf[1] = read_buf.g_pkg_index.device_type;
	tmpbuf[2] = read_buf.g_pkg_index.device_ircode_stat;
	
	tmpbuf[3] = (uint8_t)(read_buf.g_pkg_index.device_ircode_len >> 8);
	tmpbuf[4] = (uint8_t)(read_buf.g_pkg_index.device_ircode_len & 0x00FF);
	
	tmpbuf[5] = (uint8_t)(read_buf.g_pkg_index.ircode_addr >> 8*3);
	tmpbuf[6] = (uint8_t)(read_buf.g_pkg_index.ircode_addr >> 8*2);
	tmpbuf[7] = (uint8_t)(read_buf.g_pkg_index.ircode_addr >> 8*1);
	tmpbuf[8] = (uint8_t)(read_buf.g_pkg_index.ircode_addr >> 8*0);
	
	tmpbuf[9] = (uint8_t)(read_buf.g_pkg_index.ircode_total >> 8*3);
	tmpbuf[10] = (uint8_t)(read_buf.g_pkg_index.ircode_total >> 8*2);
	tmpbuf[11] = (uint8_t)(read_buf.g_pkg_index.ircode_total >> 8*1);
	tmpbuf[12] = (uint8_t)(read_buf.g_pkg_index.ircode_total >> 8*0);
	
	tmpbuf[13] = read_buf.g_pkg_index.device_offset_total;
	if (tmpbuf[13] > INDEX_MAX_OFFSER)
		tmpbuf[13] = 0;
	for (uint8_t i = 0; i < tmpbuf[13]; i++)
	{
		tmpbuf[14+i] = read_buf.g_pkg_index.offset[i];
	}
	serial_inpkg(0x10,tmpbuf,tmpbuf[13] + 14);
	
}

void pkg_handler_irindex_rmallindex(uint8_t *pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[1] = {0};
	uint8_t cncode[2] = {0x11,0x11}; 
	if (pkg == NULL || (memcmp((pkg), cncode,2) != 0)){
		tmpbuf[0] = 0xEE;
		serial_inpkg(0x11,tmpbuf,1);
		return ;
	}
	memopt_index_mask_remove();
	
	tmpbuf[0] = 0xCC;
	serial_inpkg(0x11,tmpbuf,1);
}

void pkg_handler_chip_cleanAll(uint8_t *pkg,uint8_t pkg_len)
{
	uint8_t tmpbuf[1] = {0};
	uint8_t cncode[2] = {0xCD,0xCA}; 
	uint8_t memen = 0xff;
	if (pkg == NULL || (memcmp((pkg), cncode,2) != 0)){
		tmpbuf[0] = 0x00;
		serial_inpkg(0x12,tmpbuf,1);
		return ;
	}
		memen = 0xcc;
		flash_chip_erase();
		flash_busy_stat_outime_checker_spt(1000);
		flash_write(MEMOPT_INDEX_ADDR_START+sizeof(union MEMOPT_INDEX_SYMBOL),&memen,1);
	
	tmpbuf[0] = 0xCC;
	serial_inpkg(0x12,tmpbuf,1);
}


