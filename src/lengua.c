#include <genesis.h>

#include "../res/rescomp.h"
#include "../inc/lengua.h"
#include "../inc/spritedispatcher.h"
#include "../inc/sfxpsg.h"
#include "../inc/lightgun.h"
#include "../inc/game.h"
#include "../inc/sfxpsg.h"
#include "../res/musicvgm.h"


#define DRAGON_IZQ -1
#define DRAGON_DER +1


u8 nshot;
u8 cursor;
u8 dragon;
u8 lengua0;
u8 lengua1;
u8 lengua2;
u8 lengua3;
u8 lengua4;
s8 dragon_dir;
u8 flip_offset;




static void _reset_bongo_anim ()
{
   bongo.counter = 0;
   SPR_setFrame ( &sprlist [ dragon ], 0 );
}



static void _bongo_update_dir ( s16 x )
{
   if ( (x+128) != sprlist[cursor].x) // HEMOS MOVIDO AL MUÑECO ??
   {
      s8 dir = ( (x+128) > sprlist[cursor].x ) ? DRAGON_DER : DRAGON_IZQ; //EN QUE DIRECCION LO HEMOS MOVIDO??

      if( dragon_dir != dir ) // ES LA MISMA DIRECCION QUE YA TENIA??
      {
         dragon_dir = dir;
         s8    Hflip = (dragon_dir == DRAGON_DER) ? TRUE : FALSE;
         flip_offset = (dragon_dir == DRAGON_DER) ? 10 : 0;

         SPR_setAttribut ( &sprlist[dragon],  TILE_ATTR ( PAL3, FALSE, FALSE, Hflip ) );
      }
   }
}


static void _bongo_update_pos ( s16 x )
{
   bongo.x = x - 3 - flip_offset;
   bongo.y += 4;

   if ( bongo.y > MAX_BONGO_Y )
   {
      bongo.y =  MAX_BONGO_Y;
   }
}


static void _bongo_update_anim ( u8 anim )
{
   if ( anim != 0 )
   {
      bongo.counter = 0;
      return;
   }

   ++bongo.counter;

   if
   (
      bongo.counter == 200 || // cierra
      bongo.counter == 210 || // abre
      bongo.counter == 220 || // cierra
      bongo.counter == 230 || // abre
      bongo.counter == 330 || // cierra
      bongo.counter == 340    // abre
   )
   {
      SPR_nextFrame ( &sprlist [ dragon ] );
   }

   if ( bongo.counter == 360 )
   {
      bongo.counter = 0;
   }
}


void LENGUA_init ( void )
{
   bongo.y = MAX_BONGO_Y;
   bongo.counter = 0;


   dragon_dir  = DRAGON_IZQ;
   flip_offset = 0;

   nshot   = sd_newEx ( 0,            SD_DOWN );
   cursor  = sd_newEx ( 0,            SD_DOWN );
   dragon  = sd_newEx ( 0,            SD_DOWN );
   lengua0 = sd_newEx ( MAX_SPRITE-1, SD_UP   );
   lengua1 = sd_newEx ( MAX_SPRITE-1, SD_UP   );
   lengua2 = sd_newEx ( MAX_SPRITE-1, SD_UP   );
   lengua3 = sd_newEx ( MAX_SPRITE-1, SD_UP   );
   lengua4 = sd_newEx ( MAX_SPRITE-1, SD_UP   );

   u16 attr = TILE_ATTR ( PAL3, FALSE, FALSE, FALSE ) ;

   SPR_initSprite ( &sprlist [ nshot   ], (SpriteDefinition*) &shotDef,   320, 224, TILE_ATTR ( PAL3, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( &sprlist [ cursor  ], (SpriteDefinition*) &cursorDef, 320, 224, attr );
   SPR_initSprite ( &sprlist [ dragon  ], (SpriteDefinition*) &crewDef,   320, 224, attr );
   SPR_initSprite ( &sprlist [ lengua0 ], (SpriteDefinition*) &lenguaDef, 320, 224, attr );
   SPR_initSprite ( &sprlist [ lengua1 ], (SpriteDefinition*) &lenguaDef, 320, 224, attr );
   SPR_initSprite ( &sprlist [ lengua2 ], (SpriteDefinition*) &lenguaDef, 320, 224, attr );
   SPR_initSprite ( &sprlist [ lengua3 ], (SpriteDefinition*) &lenguaDef, 320, 224, attr );
   SPR_initSprite ( &sprlist [ lengua4 ], (SpriteDefinition*) &lenguaDef, 320, 224, attr );

   SPR_setVRAMTileIndex ( &sprlist [ cursor  ], 0x04B0 );
   SPR_setVRAMTileIndex ( &sprlist [ dragon  ], 0x04B1 );
   SPR_setVRAMTileIndex ( &sprlist [ lengua0 ], 0x04BA );
   SPR_setVRAMTileIndex ( &sprlist [ lengua1 ], 0x04BA );
   SPR_setVRAMTileIndex ( &sprlist [ lengua2 ], 0x04BA );
   SPR_setVRAMTileIndex ( &sprlist [ lengua3 ], 0x04BA );
   SPR_setVRAMTileIndex ( &sprlist [ lengua4 ], 0x04BA );
   SPR_setVRAMTileIndex ( &sprlist [ lengua4 ], 0x04BA );
   SPR_setVRAMTileIndex ( &sprlist [ nshot   ], 0x04BE );

   sprlist[nshot].animation->loop = -1 ; // no loop

   LENGUA_update ( ( VDP_getScreenWidth( ) / 2 ), LENGUA_HIDE_LIMIT  );
}



void LENGUA_dragon_has_been_shot ( )
{
   if ( SHOOT_FLAG == 3  &&  bongo.y == MAX_BONGO_Y  &&  LightgunKillsIt ( bongo.x-4, bongo.y-4 ) )
   {
      SND_playSfx_VGM ( ay_ninjajar, sizeof(ay_ninjajar) );

      bongo.y -= 10;
      _reset_bongo_anim ();

      if ( dragon_dir == DRAGON_DER ) sprlist[cursor].x += 1 ;
      if ( dragon_dir == DRAGON_IZQ ) sprlist[cursor].x -= 1 ;
   }
}



void LENGUA_update ( s16 x, s16 y )
{
   u8 anim = ( y < LENGUA_HIDE_LIMIT );

   SPR_setAnim ( &sprlist[dragon],  anim );
   SPR_setNeverVisible ( &sprlist[cursor], !anim );

   _bongo_update_anim ( anim );
   _bongo_update_dir ( x );
   _bongo_update_pos ( x );

   SPR_setPosition ( &sprlist[cursor],  x, y );
   SPR_setPosition ( &sprlist[dragon],  bongo.x, bongo.y );
   SPR_setPosition ( &sprlist[lengua0], x, y +   8 );
   SPR_setPosition ( &sprlist[lengua1], x, y +  40 );
   SPR_setPosition ( &sprlist[lengua2], x, y +  72 );
   SPR_setPosition ( &sprlist[lengua3], x, y + 104 );
   SPR_setPosition ( &sprlist[lengua4], x, y + 136 );
}


void lengua_hide ( void )
{
   SPR_setNeverVisible ( &sprlist[nshot],   TRUE );
   SPR_setNeverVisible ( &sprlist[cursor],  TRUE );
   SPR_setNeverVisible ( &sprlist[lengua0], TRUE );
   SPR_setNeverVisible ( &sprlist[lengua1], TRUE );
   SPR_setNeverVisible ( &sprlist[lengua2], TRUE );
   SPR_setNeverVisible ( &sprlist[lengua3], TRUE );
   SPR_setNeverVisible ( &sprlist[lengua4], TRUE );
}


void lengua_dragon_set_pos ( s16 x )
{
   bongo.x = x;
   SPR_setPosition ( &sprlist[dragon], bongo.x, bongo.y );
}


void lengua_dragon_muere ( )
{
   u8 i = 120;
   SPR_setAnim ( &sprlist[dragon], 2 ); // dead

   sfxpsg_play ( SFXPSG_DEATH );

   while ( i-- )
   {
      if ( i % 7 == 0 )
      {
         SPR_nextFrame ( &sprlist[dragon] );
         SPR_update ( sprlist, sd_cnt() );
      }

      VDP_waitVSync();
   }
}
