#include <genesis.h>


#include "../res/rescomp.h"

#include "../inc/bichos.h"
#include "../inc/captured.h"
#include "../inc/game.h"
#include "../inc/spritedispatcher.h"
#include "../inc/lengua.h"
#include "../inc/hud.h"
#include "../inc/sfxpsg.h"




#define MAX_BICHOS_CAPTURED 12


u8 first_captured_idx = 0;  // INDICE QUE INDICA EN CASO DE QUE LA LENGUA ESTÉ LLENA QUE BICHO DEBE DESAPARECER. (SIEMPRE EL PRIMERO QUE SE CAPTURÓ)


BICHO_CACHE bicho_captured [ MAX_BICHOS_CAPTURED ];

u16 lista_repetidos[3];


void _reset ( u8 i )
{
   bicho_captured[i].sprite    = 0;
   bicho_captured[i].activo    = FALSE;
   bicho_captured[i].x         = 0;
   bicho_captured[i].y         = 0;
   bicho_captured[i].bicho_def = NULL;
   bicho_captured[i].pi        = NULL;
}


u8 _find_inactivo ()
{
   u8 i;

   for ( i=0; i<MAX_BICHOS_CAPTURED; i++ )
   {
      if ( bicho_captured[i].activo == FALSE )
      {
         return i;
      }
   }

    u8 id = first_captured_idx++ % MAX_BICHOS_CAPTURED;

   captured_delete ( id  );
   return id ;
}


void _remove_duplicates ( BICHO_CACHE *b1, u8 desde )
{
   u8 i;

   for ( i=desde; i<MAX_BICHOS_CAPTURED; i++ )
   {
      BICHO_CACHE *b2 = (BICHO_CACHE*) &bicho_captured [ i ];

      if ( !b2->activo )
      {
         continue;
      }

      if (
         b1->y               == b2->y                &&
         b1->bicho_def->id   == b2->bicho_def->id    &&
         b1->pi->direction_x == b2->pi->direction_x
      )
      {
         ++lista_repetidos [ b1->bicho_def->id - 1 ];

         captured_delete(i);
      }
   }
}


void captured_delete ( u8 i )
{
   sd_delete ( bicho_captured[i].sprite );
   _reset ( i );
}


void captured_delete_all ( )
{
   u8 i;

   first_captured_idx = 0;

   for ( i=0; i<MAX_BICHOS_CAPTURED; i++ )
   {
      if ( bicho_captured[i].activo )
      {
         captured_delete ( i );
      }
   }
}


void captured_init ( )
{
   u8 i;

   first_captured_idx = 0;

   lista_repetidos[0] = 1;
   lista_repetidos[1] = 1;
   lista_repetidos[2] = 1;

   for ( i=0; i<MAX_BICHOS_CAPTURED; i++ )
   {
      _reset ( i );
   }
}


void captured_new ( BICHO_CACHE *bicho )
{
   BICHO_CACHE *b = (BICHO_CACHE*) &bicho_captured [ _find_inactivo() ];

   b->sprite    = sd_newEx ( MAX_SPRITE-1, SD_UP   );
   b->activo    = TRUE;
   b->bicho_def = bicho->bicho_def;
   b->pi        = bicho->pi;
   b->y         = bicho->y;
   b->x         = bicho->x;

   SPR_initSprite ( &sprlist [ b->sprite ], (SpriteDefinition*) b->bicho_def->sd, b->x, b->y, TILE_ATTR ( PAL3, TRUE, 0, (b->pi->direction_x<0) ) );
   SPR_setAnim ( &sprlist [ b->sprite ], b->bicho_def->rc_pos_catched );
   SPR_setVRAMTileIndex ( &sprlist [ b->sprite ], 0x0490 + ( b->bicho_def->id - 1 ) * 9 ) ;
}


void captured_update ( s16 x, s16 y )
{
   u8 i;

   for ( i=0; i<MAX_BICHOS_CAPTURED; i++ )
   {
      BICHO_CACHE *b = (BICHO_CACHE*) &bicho_captured [ i ];

      if ( !b->activo )
      {
         continue;
      }

      _remove_duplicates ( b, i+1 );

      if ( intToFix32(y) > b->y )
      {
         b->y = intToFix32(y);
      }

      if ( fix32ToRoundedInt(b->y) > 150 ) // se lo come
      {
          sfxpsg_play ( SFXPSG_SWALLOW );

         HUD_score_add ( b, lista_repetidos [ b->bicho_def->id - 1 ] );
         lista_repetidos [ b->bicho_def->id - 1 ] = 1;
         captured_delete ( i );

         continue;
      }

      b->x = intToFix32 ( x - 12+4 );

      SPR_setPosition ( &sprlist [ b->sprite ], fix32ToInt(b->x), fix32ToInt(b->y) );
   }
}
