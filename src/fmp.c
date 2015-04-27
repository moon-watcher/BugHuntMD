#include <genesis.h>
#include "../inc/fmp.h"



void FMP_load ( FMP *fmp, u8 dma )
{
   SYS_disableInts();
	VDP_loadTileData ( fmp->map->tiles, fmp->vram, fmp->map->nbTiles, dma );
	SYS_enableInts();

//   if ( dma ) VDP_waitDMACompletion();
}


void FMP_set_palette ( FMP *fmp )
{
	VDP_setPalette ( fmp->pal, fmp->map->pal );
}


void FMP_draw ( FMP *fmp, s8 x_pos, s8 y_pos, s8 x_inner, s8 y_inner, u8 width, u8 height )
{
   SYS_disableInts();

	u16 x, y;
	const u16 *map = fmp->map->map ;

	for ( y = 0; y < height; y++ )
	{
		u16 aux = ( y + y_inner ) * fmp->map->width + x_inner;

		for ( x = 0; x < width; x++ )
		{
         u16 tile = TILE_ATTR_FULL ( fmp->pal, fmp->prio, 0, 0, map [ aux + x ] + fmp->vram );
			VDP_setTileMapXY ( fmp->plan, tile, x + x_pos, y + y_pos );
		}
	}

	SYS_enableInts();
}
