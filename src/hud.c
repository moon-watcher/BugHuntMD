#include <genesis.h>


#include "../inc/hud.h"
#include "../res/rescomp.h"
#include "../inc/text.h"
#include "../inc/game.h"
#include "../inc/sfxpsg.h"


static u32 baseScore;


void HUD_init ( void )
{
    VDP_loadFont ( &font_hud, TRUE); /* Tipo de letra */
    VDP_waitDMACompletion();

    baseScore = score = 0;
    // lvl se inicializa en _scr_select ( ); de game.c

    HUD_level ( );
    HUD_score ( );
}


void HUD_level ( void )
{
   if ( lvl > TOPE_LVL ) { lvl = TOPE_LVL; }
   VDP_drawNumberBG ( APLAN, lvl, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ), 5, 18, 2 );
}


void HUD_score ( void )
{
   if ( score > TOPE_SCORE ) { score = TOPE_SCORE; }
   else if (score < 0 ) { score = 0; }
   VDP_drawNumberBG ( APLAN, score, TILE_ATTR( PAL1, TRUE, FALSE, FALSE ), 28, 18, 9 );
}


void HUD_score_add ( BICHO_CACHE *b, u8 repeticiones )
{
   u32 aux = ( ( b->bicho_def->score1 * lvl + b->bicho_def->score2 ) * repeticiones );

   score     += aux;
   baseScore += aux;

   if ( baseScore >= MAX_BASESCORE )
   {
      if ( !IS_PRACTICE_MODE )
      {
         lvl += ( baseScore / MAX_BASESCORE );
         game_change_colors ( );
         sfxpsg_play ( SFXPSG_LEVEL );
      }

      baseScore  = ( baseScore % MAX_BASESCORE );


   }

   HUD_score( );
   HUD_level( );
}

void HUD_score_subtract ( BICHO_CACHE *b )
{
   score -= ( b->bicho_def->score1 * lvl + b->bicho_def->score2 );

   HUD_score( );
}
