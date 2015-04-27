#include <genesis.h>
#include <genres.h>

#include "../inc/text.h"


#define XCENTER  ( ( VDP_getScreenWidth()  >> 3 ) - ( strlen (str) * _width  ) ) >> 1
#define YCENTER  ( ( VDP_getScreenHeight() >> 3 ) - (                _height ) ) >> 1



static struct genresSprites *_genres;
static u16                  _base;
static u16                  _palette;
static u8                   _counter;
static u16                   _positions[96];
static u8                   _width;
static u8                   _height;
static u8                   _tiles;





void text_init ( struct genresSprites *genres, u16 vram, u16 palette )
{
	_genres  = genres;
	_base    = vram;
	_palette = palette;

	_counter = 0;
   _width  = _genres->width  >> 3;
   _height = _genres->height >> 3;
   _tiles  = _width * _height;

	memset( _positions, 0, sizeof _positions );
}

void text_draw_centered ( u8 *str, u16 ms )
{
	text_draw ( str, XCENTER, YCENTER, ms );
}

void text_draw_Hcentered ( u8 *str, u8 y, u16 ms )
{
	text_draw ( str, XCENTER, y, ms );
}

void textNum_draw_Hcentered ( s32 n, u8 y, u16 ms )
{
   char str[12];

   intToStr(n, str, 1);

	text_draw ( str, XCENTER, y, ms );
}

void textNum_draw_centered ( s32 n, u16 ms )
{
    char str[12];

    intToStr(n, str, 1);

    text_draw_centered (str, ms);
}

void text_draw ( u8 *str, u8 x, u8 y, u16 ms )
{
	#define POSITION	_base + _positions[chr] * _tiles + inc


	u8 chr;

	u16 k;
	u16 j;
	u16 i   = 0;
	u16 inc = 0;

   u8 *aux = str;

	while ( (chr = *aux++) )
	{
		inc = 0;
		chr -= ' ';
		if ( chr >= 96 ) chr = 0;

		if ( ! _positions[chr] )
		{
			_positions[chr] = _counter++;
			VDP_loadTileData ( _genres->sprites[chr], POSITION, _tiles, TRUE );
			VDP_waitDMACompletion();
		}
   }

	while ( (chr = *str++) )
	{
		inc = 0;
		chr -= ' ';
		if ( chr >= 96 ) chr = 0;

		for ( j = 0; j < _height; j++ )
		{
			for ( k = 0; k < _width; k++ )
			{
				VDP_setTileMapXY ( BPLAN, TILE_ATTR_FULL ( _palette, TRUE, 0, 0, POSITION ), i*_width + x + j, y + k );
				++inc;
			}
		}

		waitMs ( ms );
		++i;
	}
}


void text_draw_sprite ( u8 *str, u16 x, u16 y, u16 ms )
{
	#define TILE    _base + _positions[chr] * _width * _height
	#define ATTR	 TILE_ATTR_FULL ( _palette, 1, 0, 0, TILE )

	u8 chr, i = 0;


	const u16 size = _genres->size  >> 8;
	const u8 width = _genres->width;

	while ( (chr = *str++) )
	{
		chr -= ' ';
		chr  = ( chr >= 96 ) ? 0 : chr;

		if ( ! _positions[chr] )
		{
			_positions[chr] = _counter++;
			VDP_loadTileData ( _genres->sprites[chr], TILE, size, TRUE );
			VDP_waitVSync();
      }

		VDP_setSprite ( i, x+i*width, y, size, ATTR, i+1 );
		VDP_updateSprites();

		waitMs ( ms );

		++i;
	}
}


void text_draw_sprites_centered ( u8 *str, u16 ms )
{
	const u16 x = ( ( VDP_getScreenWidth()  - ( strlen (str) * _genres->width  ) ) >> 1);
	const u16 y = ( ( VDP_getScreenHeight() - (                _genres->height ) ) >> 1);

	text_draw_sprite ( str, x, y, ms );
}


void VDP_drawNumberBG(u16 plan, s32 num, u16 flags, u16 x, u16 y, u16 minsize) // Borrowed from @pocket_lucho, hehehe.
{
   char str[12];
   intToStr(num, str, minsize);
   VDP_drawTextBG(plan, str, flags, x, y);
}
