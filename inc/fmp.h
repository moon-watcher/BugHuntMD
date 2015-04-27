#ifndef _FMP_H_
#define _FMP_H_

#include <genres.h>



typedef struct
{
	u8  pal;
	u16 vram;
	u8  prio;
	u16 plan;
	const genresMap_t *map;
}
FMP;


void FMP_load        ( FMP *fmp, u8 dma );
void FMP_set_palette ( FMP *fmp );
void FMP_draw        ( FMP *fmp, s8 x_pos, s8 y_pos, s8 x_inner, s8 y_inner, u8 width, u8 height );


#endif


//#ifndef _FMP_H_
//#define _FMP_H_
//
//#include "../inc/genres.h"
//
//
//typedef struct
//{
//	u8  pal;
//	u16 vram;
//	u8  prio;
//	u16 plan;
//	const genresMap_t *map;
//}
//FMP;
//
//
////FMP  *FMP_load       ( u16 pal, u16 vram, u16 prio, u16 plan, u16 loadpal, u16 usedma );
//FMP  *FMP_load       ( genresMap_t *map, u16 pal, u16 vram, u16 prio, u16 plan, u16 loadpal, u16 usedma );
//void  FMP_setPalette ( FMP *fmp );
//void  FMP_draw       ( FMP *fmp, s8 x_pos, s8 y_pos, s8 x_inner, s8 y_inner, u8 width, u8 height );
//
//
//#endif
