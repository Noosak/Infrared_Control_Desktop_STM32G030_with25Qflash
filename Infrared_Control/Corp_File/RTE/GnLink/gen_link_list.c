#include "gen_link_list.h"

#define GEN_LINK_MAXSIZE 32


GEN_LINK_ST* gen_link_creat(void)
{
	GEN_LINK_ST* tmp_pt_head = NULL;
	GEN_LINK_ST* tmp_pt_end = NULL;
	tmp_pt_head = (GEN_LINK_ST*)malloc(sizeof(GEN_LINK_ST));
	if (tmp_pt_head == NULL)
		return NULL;
	
	tmp_pt_end = (GEN_LINK_ST*)malloc(sizeof(GEN_LINK_ST));
	if (tmp_pt_end == NULL){
		free(tmp_pt_head);
		return NULL;
	}
	memset(tmp_pt_head,0,sizeof(GEN_LINK_ST));
	memset(tmp_pt_end,0,sizeof(GEN_LINK_ST));
	
	tmp_pt_end->backward = (GEN_LINK_ST*)tmp_pt_head;
	
	tmp_pt_head->forward = (GEN_LINK_ST*)tmp_pt_end;;
	
	return tmp_pt_head;
}

uint8_t gen_link_add(GEN_LINK_ST* obj_gll_fst,void* add_st)
{
	GEN_LINK_ST* tmp_pt_add;
	GEN_LINK_ST* tmp_pt_jump;
	if ( obj_gll_fst == NULL || obj_gll_fst->backward != NULL)
		return 0xEE;
	tmp_pt_add = (GEN_LINK_ST*)malloc(sizeof(GEN_LINK_ST));
	if (tmp_pt_add == NULL)
		return 0xEE;
	
	tmp_pt_jump = obj_gll_fst;
	for (uint8_t i = 0; i < GEN_LINK_MAXSIZE; i++)
	{
		if (tmp_pt_jump->forward != NULL)
			tmp_pt_jump = (GEN_LINK_ST*)tmp_pt_jump->forward;
		else
			break;
	}
	tmp_pt_jump->backward->forward = tmp_pt_add;
	
	tmp_pt_add->backward = tmp_pt_jump->backward;
	tmp_pt_add->forward = tmp_pt_jump;
	
	tmp_pt_add->struct_pt = add_st;

	tmp_pt_jump->backward = tmp_pt_add;
	return 0;
}

uint8_t gen_link_remove(GEN_LINK_ST* obj_gll_fst,void* rm_st)
{
	GEN_LINK_ST* tmp_pt_rm = NULL;
	GEN_LINK_ST* tmp_pt_jump;
	if ( obj_gll_fst == NULL || obj_gll_fst->backward != NULL)
		return 0xEE;
	tmp_pt_jump = obj_gll_fst;
	for (uint8_t i = 0; i < GEN_LINK_MAXSIZE; i++)
	{
		if (tmp_pt_jump->forward != NULL)
		{	
			if (tmp_pt_jump->struct_pt == rm_st && tmp_pt_jump !=obj_gll_fst)
			{
				tmp_pt_rm = tmp_pt_jump;
				break;
			}
			tmp_pt_jump = (GEN_LINK_ST*)tmp_pt_jump->forward;
		}
		else
			break;
	}
	if (tmp_pt_rm != NULL){
		tmp_pt_rm->forward->backward = tmp_pt_rm->backward;
		tmp_pt_rm->backward->forward = tmp_pt_rm->forward;
		free(tmp_pt_rm);
		return 0x00;
	}
	return 0xee;
}

void** gen_link_get(GEN_LINK_ST* obj_gll_fst, uint8_t* array_size)
{
	void** tmp_array_data_pt = NULL; 
	void** array_data_pt = NULL; 
	
	GEN_LINK_ST* tmp_pt_jump;
	if ( obj_gll_fst == NULL || obj_gll_fst->backward != NULL|| array_size == NULL)
		return NULL;
	
	tmp_array_data_pt = (void*)malloc(sizeof(void*)*GEN_LINK_MAXSIZE);
	if (tmp_array_data_pt == NULL)
		return NULL;
	
	(*array_size) = 0;
	tmp_pt_jump = obj_gll_fst->forward;
	for (uint8_t i = 0; i < GEN_LINK_MAXSIZE; i++)
	{
		if (tmp_pt_jump->forward != NULL)
		{	
			tmp_array_data_pt[i] = tmp_pt_jump->struct_pt;
			(*array_size)++;
			tmp_pt_jump = (GEN_LINK_ST*)tmp_pt_jump->forward;
		}
		else{
			break;
		}
	}
	array_data_pt = (void*)malloc(sizeof(void*)*(*array_size));
	if (array_data_pt == NULL){
		free(tmp_array_data_pt);
		return NULL;
	}
	memcpy(array_data_pt,tmp_array_data_pt,sizeof(void*)*(*array_size));
	free(tmp_array_data_pt);
	return array_data_pt;
}

GEN_LINK_ST* gen_link_find_node_pt(GEN_LINK_ST* obj_gll_fst, void* fn_pt)
{
	GEN_LINK_ST* tmp_pt_rm = NULL;
	GEN_LINK_ST* tmp_pt_jump;
	if ( obj_gll_fst == NULL || obj_gll_fst->backward != NULL)
		return NULL;
	tmp_pt_jump = obj_gll_fst;
	for (uint8_t i = 0; i < GEN_LINK_MAXSIZE; i++)
	{
		if (tmp_pt_jump->forward != NULL)
		{	
			if (tmp_pt_jump->struct_pt == fn_pt && tmp_pt_jump !=obj_gll_fst)
			{
				tmp_pt_rm = tmp_pt_jump;
				break;
			}
			tmp_pt_jump = (GEN_LINK_ST*)tmp_pt_jump->forward;
		}
		else
			break;
	}
	return tmp_pt_rm;
}

uint8_t gen_link_delate(GEN_LINK_ST* obj_gll_fst)
{
	GEN_LINK_ST* tmp_pt_jump;
	GEN_LINK_ST* tmp_pt_now;
	if ( obj_gll_fst == NULL || obj_gll_fst->backward != NULL)
		return 0xEE;
	tmp_pt_jump = obj_gll_fst;
	for (uint8_t i = 0; i < GEN_LINK_MAXSIZE; i++)
	{
		if (tmp_pt_jump->forward != NULL)
		{	
			tmp_pt_now = tmp_pt_jump;
			tmp_pt_jump = (GEN_LINK_ST*)tmp_pt_jump->forward;
			free(tmp_pt_now);
		}
		else{
			free(tmp_pt_jump);
			break;
		}
	}
	return 0x00;
}
