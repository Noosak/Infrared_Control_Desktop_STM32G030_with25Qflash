#pragma once
#ifndef __GEN_LINK_LIST_H__
#define __GEN_LINK_LIST_H__ 

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct GEN_LINK_ST
{
	void* struct_pt;
	struct GEN_LINK_ST* forward;
	struct GEN_LINK_ST* backward;
};

typedef struct GEN_LINK_ST GEN_LINK_ST;

extern GEN_LINK_ST* gen_link_creat(void);
extern uint8_t gen_link_add(GEN_LINK_ST* obj_gll_fst,void* add_st);
extern uint8_t gen_link_delate(GEN_LINK_ST* obj_gll_fst);

extern uint8_t gen_link_remove(GEN_LINK_ST* obj_gll_fst,void* rm_st);
extern void** gen_link_get(GEN_LINK_ST* obj_gll_fst, uint8_t* array_size);
extern GEN_LINK_ST* gen_link_find_node_pt(GEN_LINK_ST* obj_gll_fst, void* fn_pt);
#endif
