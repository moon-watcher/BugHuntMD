#ifndef _SCROLL_H_
#define _SCROLL_H_ 1

#include "../inc/fmp.h"


typedef struct
{
	s32 x;
	s32 y;

	FMP *fmp;
}
SCROLL;




SCROLL *scroll_init   ( FMP *fmp );
void    scroll_move   ( SCROLL *s, s8 x, s8 y );


#endif
