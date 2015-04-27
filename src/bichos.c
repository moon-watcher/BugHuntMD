#include <genesis.h>


#include "../res/rescomp.h"
#include "../inc/bichos.h"
#include "../inc/captured.h"
#include "../inc/game.h"
#include "../inc/spritedispatcher.h"
#include "../inc/lengua.h"
#include "../inc/hud.h"
#include "../inc/sfxpsg.h"
#include "../inc/input.h"
#include "../inc/lengua.h"
#include "../inc/lightgun.h"
#include "../res/musicvgm.h"


#define MAX_BICHOS_WA       30





static ROW_DEF bicho_filas [ MAX_FILAS ] =
{
   //   x        y      timer   dir_x
   {  -24,    16-8,         0,      +1 }, // "+1" va a la derecha
   {  -24,    64-8,         0,      +1 },
   {  -24,   112-8,         0,      +1 },
   {  320,    40-8,         0,      -1 }, // "-1" va a la izquierda
   {  320,    88-8,         0,      -1 },
   {  320,   136-8,         0,      -1 },
};


static BICHO_CACHE bicho_wa [ MAX_BICHOS_WA ];

const BICHO_DEF bichos_list [ BICHOS_NB_DISTINCT ] =
{
   { MOSCA,    ANIM_MOSCA,    ANIM_MOSCA_CATCHED,     50, 100, FALSE, 1, (SpriteDefinition*) &crewDef },
   { AVISPA,   ANIM_AVISPA,   ANIM_AVISPA_CATCHED,     0,   0,  TRUE, 1, (SpriteDefinition*) &crewDef },
   { LIBELULA, ANIM_LIBELULA, ANIM_LIBELULA_CATCHED, 250, 500, FALSE, 2, (SpriteDefinition*) &crewDef }
};



#define SWAP(x, y) typeof(x) tmp = x; x = y; y = tmp;

/* ORDEN BICHOS */

#define BICHO_MAX_SECUENCIA     12

static const u8 _ORDEN_LIST[ BICHO_MAX_SECUENCIA ] = { MOSCA, MOSCA, AVISPA, MOSCA, MOSCA, AVISPA, MOSCA, MOSCA, AVISPA, MOSCA, MOSCA, LIBELULA };

static u8 orden_bichos [ BICHO_MAX_SECUENCIA ];   // Array que indica el orden de aparición de los bichos
static u8 orden_bichos_idx = 0;


/* TIMER BICHOS */

#define TIMER_MAX_SECUENCIA     12

static const u8 _TIMER_LIST[ TIMER_MAX_SECUENCIA ] = { 240, 220, 200, 210, 190, 180, 170, 160, 150, 140, 130, 120 };

static u8 timer_bichos [ TIMER_MAX_SECUENCIA ];            // Array que indica el tiempo entre bichos de una fila
static u8 timer_bichos_idx = 0;



static u32 counter;

static s16 prev_x;





static void ShuffleVector_u8(u8 *array, u32 n)
{
    u32 i;

    for (i = 0; i < n ; i++)        /* RECORREMOS UNO A UNO EL VECTOR... */
    {
        u32 j = random() % n;       /* ... Y LO INTERCAMBIAMOS CON OTRO ÍNDICE AL AZAR */
        SWAP( array[i], array[j] );
    }
}


static void ORDEN_BICHO_new()
{
    orden_bichos_idx = 0;                                    /* RESETEAMOS EL INDICE */

    memcpy(orden_bichos, _ORDEN_LIST, sizeof(_ORDEN_LIST) ); /* COPIAMOS EN EL ARRAY LA SECUENCIA ORIGINAL */
    ShuffleVector_u8(orden_bichos, BICHO_MAX_SECUENCIA);     /* MEZCLAMOS EL ARRAY */
}


static u8 ORDEN_BICHO_next ( void )
{
   u8 next_bicho = orden_bichos[ orden_bichos_idx ]; /* COGEMOS EL BICHO QUE CORRESPONDA */

   if ( ++orden_bichos_idx >= BICHO_MAX_SECUENCIA )  /* AUMENTAMOS EL ÍNDICE PARA LA PRÓXIMA VEZ */
   {
      ORDEN_BICHO_new();                             /* SI EL INDICE LLEGA AL FINAL, CREAMOS NUEVA LISTA */
   }

   return next_bicho;                                /* DEVOLVEMOS EL BICHO */
}



static void TIMER_BICHO_new()
{
    timer_bichos_idx = 0;                                   /* RESETEAMOS EL INDICE */

    u8 i;
    for (i=0; i < TIMER_MAX_SECUENCIA; i++)                 /* COPIAMOS EN EL ARRAY LA SECUENCIA ORIGINAL */
    {
        timer_bichos[ i ] = _TIMER_LIST[ i ] - (lvl >> 1); /* POR CADA NIVEL EL TIEMPO ENTRE BICHOS ES MENOR */
    }

    ShuffleVector_u8(timer_bichos, TIMER_MAX_SECUENCIA);    /* MEZCLAMOS EL ARRAY */
}

static u8 TIMER_BICHO_next ( void )
{
   u8 next_timer = timer_bichos[ timer_bichos_idx ]; /* COGEMOS EL TIMER QUE CORRESPONDA */

   if ( ++timer_bichos_idx >= TIMER_MAX_SECUENCIA )   /* AUMENTAMOS EL ÍNDICE PARA LA PRÓXIMA VEZ */
   {
      TIMER_BICHO_new();                               /* SI EL INDICE LLEGA AL FINAL, CREAMOS NUEVA LISTA */
   }

   return next_timer;                                  /* DEVOLVEMOS EL BICHO */
}


void _bicho_wa_reset ( u8 i )
{
   bicho_wa[i].sprite    = 0;
   bicho_wa[i].activo    = FALSE;
   bicho_wa[i].x         = 0;
   bicho_wa[i].y         = 0;
   bicho_wa[i].bicho_def = NULL;
   bicho_wa[i].pi        = NULL;
}


void _delete_bicho_wa ( u8 i )
{
   sd_delete ( bicho_wa[i].sprite );
   _bicho_wa_reset ( i );
}


s8 _find_bicho_inactivo ()
{
   u8 i;

   for ( i=0; i<MAX_BICHOS_WA; i++ )
   {
      if ( bicho_wa[i].activo == FALSE )
      {
         return i;
      }
   }

   return -1;
}


void BICHOS_init()
{
   counter  = 0;
   showshot = 0;
   prev_x   = 0;

   ORDEN_BICHO_new();   /* CREAMOS LA LISTA DE ORDEN DE APARICION DE LOS BICHOS */
   TIMER_BICHO_new();  /* CREAMOS LA LISTA DE LOS POSIBLES TIMERS */

   SPR_setNeverVisible ( shot, TRUE );

   u8 i;

   for ( i=0; i < MAX_FILAS ; i++)      /* SETEAMOS LAS ALARMAS PARA CADA FILA */
   {
       bicho_filas[ i ].timer = TIMER_BICHO_next();
   }

   for ( i=0; i < MAX_BICHOS_WA; i++ )     /*  BORRAMOS TODOS LOS BICHOS */
   {
      _bicho_wa_reset ( i );
   }
}


u8 BICHOS_update ( s16 x, s16 y )
{
   u8 i, row;


   for (row = 0; row < MAX_FILAS ; row++)
   {
      if ( !( -- bicho_filas[ row ].timer ) )  // EL TIMER DE LA FILA HA LLEGADO A CERO (?) => SACAR NUEVO BICHO
      {
         bicho_filas[ row ].timer = TIMER_BICHO_next();

         s8 inactivo = _find_bicho_inactivo ();

         if ( inactivo >= 0 )
         {
            BICHO_CACHE *b = (BICHO_CACHE*) &bicho_wa [ (u8) inactivo ];

            b->sprite    = sd_new();
            b->activo    = TRUE;
            b->pi        = &bicho_filas [ row ] ;
            b->x         = intToFix32 ( b->pi->pos_x );
            b->y         = intToFix32 ( b->pi->pos_y );
            b->bicho_def = (BICHO_DEF*) &bichos_list [ ORDEN_BICHO_next() -1 ];

            SPR_initSprite ( &sprlist [ b->sprite ], (SpriteDefinition*) b->bicho_def->sd, fix32ToRoundedInt(b->x), fix32ToRoundedInt(b->y), TILE_ATTR(PAL3,TRUE,0,(b->pi->direction_x<0) ) );
            SPR_setAnim ( &sprlist [ b->sprite ], b->bicho_def->rc_pos );
            SPR_setVRAMTileIndex ( &sprlist [ b->sprite ], 0x0470 + ( b->bicho_def->id - 1 ) * 9 );
         }
      }
   }

   u8 update_mosca    = ( counter % 11 == 0 );
   u8 update_avispa   = ( counter %  7 == 0 );
   u8 update_libelula = ( counter %  5 == 0 );

   ++counter;



   for ( i=0; i<MAX_BICHOS_WA; i++ )
   {
      BICHO_CACHE *b = (BICHO_CACHE*) &bicho_wa[i];

      if ( ! b->activo )
      {
         continue;
      }

      // ( 0.5 + 0.3 * lvl ) * speed [ 1 | 2 ]
      fix32 speed = fix32Mul ( fix32Add ( FIX32(0.5), fix32Mul ( FIX32(0.3), FIX32(lvl) ) ), FIX32(b->bicho_def->speed_inc) ) ;

      b->x = fix32Add ( b->x, fix32Mul ( speed, intToFix32(b->pi->direction_x) ) );

      s16 rounded_x = fix32ToRoundedInt(b->x);
      s16 rounded_y = fix32ToRoundedInt(b->y);

      if ( rounded_x < -24  ||  rounded_x > VDP_getScreenWidth() )
      {
         _delete_bicho_wa ( i );
         continue;
      }
      else
      {
         SPR_setPosition ( &sprlist [ b->sprite ], rounded_x, rounded_y );

         if
         (
            ( update_mosca    == 1  &&  b->bicho_def->id == MOSCA     &&  update_mosca++    ) ||
            ( update_avispa   == 1  &&  b->bicho_def->id == AVISPA    &&  update_avispa++   ) ||
            ( update_libelula == 1  &&  b->bicho_def->id == LIBELULA  &&  update_libelula++ )
         )
         {
            SPR_nextFrame ( &sprlist [ b->sprite ] );
         }
      }


      //
      // Lightgun
      //
      if ( showshot )
      {
         if ( LightgunKillsIt ( rounded_x, rounded_y ) )
         {
            if( b->bicho_def->id != AVISPA )
            {
                 HUD_score_subtract(b);
                 sfxpsg_play ( SFXPSG_WRONG_SHOT );
            }
            else
            {
                sfxpsg_play ( SFXPSG_DEAD_SHOT );
            }

            _delete_bicho_wa ( i );

            continue;
         }
      }


      //
      // Lengua pegajosa
      //
      if ( y < rounded_y + 24 )
      {
         u16 entra = 0;
         s16 pos_x = rounded_x - 4;
         s16 width = rounded_x + 24 - 4;

         if ( !entra  &&  pos_x  < x      &&  x     < width  ) entra = 1; // la condición normal de si el bicho está tocando la lengua
         if ( !entra  &&  prev_x < pos_x  &&  width < x      ) entra = 2; // si la lengua se mueva muy rápido a la derecha
         if ( !entra  &&  x      < pos_x  &&  width < prev_x ) entra = 3; // si la lengua se mueva muy rápido a la izquierda

         if ( entra )
         {
            if ( b->bicho_def->kills_you ) // is it a wasp?
            {
               game_palette_restore();
               SYS_enableInts();

               if ( entra > 1 )
               {
                  lengua_dragon_set_pos ( rounded_x );    // mueve el dragón hasta x. Lo situa bajo la avista asesina para cuando vaya muy muy rapdio
               }

               lengua_hide (  );                        // quita la lengua
               captured_delete_all();                   // quita los bichos de la lengua
               lengua_dragon_muere();                   // animación de morir

               SYS_disableInts();

               return FALSE;                            // y sale
            }

            captured_new ( b );                                                // añade el bicho a la lengua
            _delete_bicho_wa ( i );                                            // borra el bicho de la lista de sprites
            sfxpsg_play ( SFXPSG_STUCK );

            continue;
         }
      }
   }

   LENGUA_dragon_has_been_shot ( );


   prev_x = x;


   return TRUE;
}

