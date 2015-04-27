#include <genesis.h>
#include "../inc/scroll.h"



#define MAX_PLANES 2

static SCROLL scrolls [ MAX_PLANES ];




static s32 _trunc ( s32 value, s32 max )
{
	return ( value %= max ) + ( value < 0 ? max : 0 );
}


static void _row ( SCROLL *s, s32 pos )
{
	s16 loop = VDP_getScreenWidth() / 8;
   s16 maxy = _trunc ( pos, VDP_getPlanHeight() );

   SYS_disableInts();

	while ( loop-- )
	{
      u16 index = s->fmp->map->map [ pos * s->fmp->map->width + loop ] + s->fmp->vram;
      u16 tile  = TILE_ATTR_FULL ( s->fmp->pal, s->fmp->prio, 0, 0, index );

		VDP_setTileMapXY ( s->fmp->plan, tile, loop, maxy );
	}

	SYS_enableInts();
}


SCROLL *scroll_init ( FMP *fmp )
{
   u16 plan = ( fmp->plan == APLAN ) ? 0 : 1;

	scrolls[plan].x   = 0;
	scrolls[plan].y   = 0;
   scrolls[plan].fmp = fmp;

   return &scrolls[plan];
}


void scroll_move ( SCROLL *s, s8 x, s8 y )
{
   s->y += y;

	s32 row = s->y / 8 ;

   row += ( ( y < 0 ) ? - 1 : VDP_getScreenHeight() / 8 );

	_row ( s, row );

   VDP_setVerticalScroll ( s->fmp->plan == APLAN ? PLAN_A : PLAN_B, s->y );
}
