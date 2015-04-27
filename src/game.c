#include <genesis.h>
#include <kdebug.h>

#include "../inc/main.h"
#include "../inc/spritedispatcher.h"
#include "../res/rescomp.h"
#include "../inc/input.h"
#include "../inc/hud.h"
#include "../inc/text.h"
#include "../inc/save.h"
#include "../inc/game.h"
#include "../inc/lengua.h"
#include "../inc/bichos.h"
#include "../inc/captured.h"
#include "../inc/fmp.h"
#include "../inc/scroll.h"
#include "../inc/sfxpsg.h"
#include "../inc/main.h"
#include "../inc/lightgun.h"





extern struct FMPMap        foreground;
extern struct genresSprites bigfont;    /* Font para el submit and game over */


static const u16 palGameScrBg [ MAX_BASELVL ] [ 16 ] = /* DATOS DE LAS PALETAS DISPONIBLES DURANTE EL JUEGO */
{
    //                                                  x     x         x         x      x       X
   { 0x0000, 0x00CE, 0x0084, 0x0AE6, 0x00C0, 0x006E, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 },
   { 0x0000, 0x06D6, 0x0E46, 0x0EE0, 0x0E80, 0x04A0, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 },
   { 0x0000, 0x06D6, 0x00AE, 0X006E, 0X004E, 0x0888, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 },
   { 0x0000, 0x00C0, 0x0AE6, 0x0E46, 0x060E, 0X04A0, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 },
   { 0x0000, 0x0EC4, 0x0888, 0x00CE, 0x0E80, 0x0E80, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 },
   { 0x0000, 0x00CE, 0x04A0, 0x06E6, 0x0888, 0x0888, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 },
   { 0x0000, 0x0E80, 0x0E46, 0x0C0E, 0x00CE, 0x00CE, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 },
   { 0x0000, 0x0EE0, 0x0EE0, 0x00AE, 0x0C0E, 0x0E46, 0x0F70, 0x04A0, 0x0000, 0x0F46, 0x0777, 0x0FFF, 0x0000, 0x00AE, 0x004E, 0x0000 }
};

static u16 palette_before_shoot[64];
static const u16 palette_white_all[63] = { [0 ... 62] = 0x0EEE };

/* ****************** */


FMP     map;
SCROLL *scroll ;



static _joyEventCallback *_lightgun_handler ( u16 joy, u16 changed, u16 state )
{
   if ( !showshot  &&  !SHOOT_FLAG  &&  joy == JOY_2  &&  changed & BUTTON_BTN  &&  state & BUTTON_BTN  )
   {
      SHOOT_FLAG = 1;
   }

   return 0;
}


static void _update_lightgun()
{
   if ( showshot && --showshot == 0 )
   {
      SPR_setNeverVisible ( shot, TRUE );
   }
   else if ( ! showshot  &&  SHOOT_FLAG == 1 )
   {
      sfxpsg_play ( 9 );

      game_palette_white();

      showshot   = 16;
      SHOOT_FLAG = 2;
   }
   else if ( SHOOT_FLAG == 2 )
   {
      LightgunUpdate ( );

      SHOOT_FLAG = 3;
   }
   else if ( SHOOT_FLAG == 3 )
   {
      game_palette_restore();

      SPR_setPosition ( shot, lightgun.x - 4, lightgun.y - 4 );
      SPR_setAlwaysVisible ( shot, TRUE );
      SPR_setFrame ( shot, 0 );

      SHOOT_FLAG = 0;
   }
}


static void _init_sprites()
{
   sd_reset();

   shot     = &sprlist [ sd_new() ];
   pointer  = &sprlist [ sd_new() ];
   selector = &sprlist [ sd_new() ];

   SPR_initSprite ( pointer,  (SpriteDefinition*) &cursorDef,   _JOYgetPosX(), _JOYgetPosY(), TILE_ATTR ( PAL3, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( selector, (SpriteDefinition*) &selectorDef,           -50,             0, TILE_ATTR ( PAL3, TRUE, FALSE, FALSE ) ); // 117, 47 para que siempre quede encima del nivel 0 cuando comienza en la pantalla de selección de nivel

   SPR_setVRAMTileIndex ( pointer,  0x04B1 );
   SPR_setVRAMTileIndex ( selector, 0x04B2 );

   SPR_setAnim ( pointer,  1 );
}



static void _scroll ( s8 direction )
{
   u8 i = 0;

   for ( i=0; i<28; i++ )
   {
      scroll_move ( scroll, 0, 8 * direction );
      VDP_waitVSync();
   }
}




static void _scr_select ( )
{
   u8 frame_cnt = 0;
   lvl = 0;

   SPR_setPosition(selector, 113, 41 );

   while ( TRUE )
   {
      s16 x = _JOYgetPosX();
      s16 y = _JOYgetPosY();

      if ( _JOYclick() ) // Comprobamos si se ha pulsado alguna opción de la screen
      {
         if ( y >= 38  && y <= 58 )
         {
                 if ( x >= 112 && x <= 128 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 0; SPR_setPosition(selector, 113, 41); }
            else if ( x >= 136 && x <= 152 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 1; SPR_setPosition(selector, 137, 41); }
            else if ( x >= 160 && x <= 176 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 2; SPR_setPosition(selector, 161, 41); }
            else if ( x >= 184 && x <= 200 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 3; SPR_setPosition(selector, 185, 41); }
         }

         else if ( y >= 70  && y <= 90 )
         {
                 if ( x >= 112 && x <= 128 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 4; SPR_setPosition(selector, 113, 73); }
            else if ( x >= 136 && x <= 152 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 5; SPR_setPosition(selector, 137, 73); }
            else if ( x >= 160 && x <= 176 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 6; SPR_setPosition(selector, 161, 73); }
            else if ( x >= 184 && x <= 200 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); lvl = 7; SPR_setPosition(selector, 185, 73); }
         }

         else if ( x >=   8 && x <= 144  &&  y >= 176 && y <= 200 )
         {
            IS_PRACTICE_MODE = TRUE;
            break;
         }

         else if ( x >= 168 && x <= 304  &&  y >= 176 && y <= 200 )
         {
            IS_PRACTICE_MODE = FALSE;
            break;
         }
      }

      if ( ++frame_cnt & 4 ) // Make selector to blink
      {
         SPR_setAlwaysVisible( selector, TRUE );
      }
      else
      {
         SPR_setNeverVisible ( selector, TRUE );
      }

      VDP_waitVSync();
   }

      sfxpsg_play ( SFXPSG_PRESS_BTN );

   /* Borramos los sprites de la pantalla */
   SPR_clear( );
   sd_reset( );


   VDP_waitVSync();
}



static void _countdown( )
{
   u8 i;
   const Image * countdown[3] = { &countdown_3, &countdown_2, &countdown_1 };

   for ( i = 0; i < 3; i++ )
   {
      SYS_disableInts();
      VDP_setHorizontalScroll ( PLAN_B, VDP_getScreenWidth() - 80 );

      VDP_drawImageEx ( BPLAN, countdown[i], TILE_ATTR_FULL ( PAL0, TRUE, FALSE, FALSE, 0x0330 ), 12, 4, TRUE, TRUE );
      VDP_setHorizontalScroll ( PLAN_B, 0 );
      SYS_enableInts();

      sfxpsg_play ( SFXPSG_COUNTDOWN );

      waitMs(1000);
      VDP_waitVSync( );
   }

   VDP_clearPlan ( BPLAN, FALSE );

    sfxpsg_play ( SFXPSG_BLOW_WHISTLE );
}



static void _creditos ( )
{

   SYS_disableInts();
   VDP_drawImageEx ( APLAN, &creditos, TILE_ATTR_FULL ( PAL1, TRUE, FALSE, FALSE, 0x02F0 ), 0, 0, TRUE, TRUE );
   SYS_enableInts();



   u8 deleting = FALSE;


   u8 yn1  = sd_new();
   u8 yn2  = sd_new();
   u8 yn3  = sd_new();
   u8 yn4  = sd_new();

   u8 del1 = sd_new();
   u8 del2 = sd_new();
   u8 del3 = sd_new();
   u8 del4 = sd_new();


   SPR_initSprite ( &sprlist [ yn1 ], (SpriteDefinition*) &yesnoDef,    0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( &sprlist [ yn2 ], (SpriteDefinition*) &yesnoDef,    0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( &sprlist [ yn3 ], (SpriteDefinition*) &yesnoDef,    0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( &sprlist [ yn4 ], (SpriteDefinition*) &yesnoDef,    0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );

   SPR_initSprite ( &sprlist [ del1 ], (SpriteDefinition*) &deletedDef, 0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( &sprlist [ del2 ], (SpriteDefinition*) &deletedDef, 0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( &sprlist [ del3 ], (SpriteDefinition*) &deletedDef, 0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );
   SPR_initSprite ( &sprlist [ del4 ], (SpriteDefinition*) &deletedDef, 0, -50, TILE_ATTR ( PAL1, TRUE, FALSE, FALSE ) );


   SPR_setAnim ( &sprlist [ yn1 ], 0 );
   SPR_setAnim ( &sprlist [ yn2 ], 1 );
   SPR_setAnim ( &sprlist [ yn3 ], 2 );
   SPR_setAnim ( &sprlist [ yn4 ], 3 );

   SPR_setAnim ( &sprlist [ del1 ], 0 );
   SPR_setAnim ( &sprlist [ del2 ], 1 );
   SPR_setAnim ( &sprlist [ del3 ], 2 );
   SPR_setAnim ( &sprlist [ del4 ], 3 );


   SPR_setVRAMTileIndex ( &sprlist [ yn1 ], 0x0460 );
   SPR_setVRAMTileIndex ( &sprlist [ yn2 ], 0x0470 );
   SPR_setVRAMTileIndex ( &sprlist [ yn3 ], 0x0480 );
   SPR_setVRAMTileIndex ( &sprlist [ yn4 ], 0x0490 );

   SPR_setVRAMTileIndex ( &sprlist [ del1 ], 0x0420 );
   SPR_setVRAMTileIndex ( &sprlist [ del2 ], 0x0430 );
   SPR_setVRAMTileIndex ( &sprlist [ del3 ], 0x0440 );
   SPR_setVRAMTileIndex ( &sprlist [ del4 ], 0x0450 );


   while ( TRUE )
   {
      if ( _JOYclick() )
      {
         s16 x = _JOYgetPosX();
         s16 y = _JOYgetPosY();

         if ( !deleting   )
         {
            if ( x >= 288 && y>= 192 && x <= 304 && y <= 208 ) // info
            {
               sfxpsg_play ( SFXPSG_PRESS_BTN );

               return;
            }

            if ( x >= 8 && y>= 192 && x <= 40 && y <= 208 ) // info
            {
               sfxpsg_play ( SFXPSG_PRESS_BTN );

               deleting = TRUE;

               SPR_setPosition ( &sprlist [ yn1 ], 8 * 11, 8*12 );
               SPR_setPosition ( &sprlist [ yn2 ], 8 * 15, 8*12 );
               SPR_setPosition ( &sprlist [ yn3 ], 8 * 21, 8*12 );
               SPR_setPosition ( &sprlist [ yn4 ], 8 * 25, 8*12 );
            }
         }
         else
         {
            // yes
            if ( x >= 8 * 11  &&  y >= 8 * 12  &&  x <= 8 * 18  &&  y <= 8 * 15 )
            {
               sfxpsg_play ( SFXPSG_LEVEL );

               deleting = FALSE;

               SPR_setPosition ( &sprlist [ yn1 ], 0, -50 );
               SPR_setPosition ( &sprlist [ yn2 ], 0, -50 );
               SPR_setPosition ( &sprlist [ yn3 ], 0, -50 );
               SPR_setPosition ( &sprlist [ yn4 ], 0, -50 );

               save_reset();

               SPR_setPosition ( &sprlist [ del1 ], 8*12, 8*12 );
               SPR_setPosition ( &sprlist [ del2 ], 8*16, 8*12 );
               SPR_setPosition ( &sprlist [ del3 ], 8*20, 8*12 );
               SPR_setPosition ( &sprlist [ del4 ], 8*24, 8*12 );

               _JOYwaitClick();

               SPR_setPosition ( &sprlist [ del1 ], 0, -50 );
               SPR_setPosition ( &sprlist [ del2 ], 0, -50 );
               SPR_setPosition ( &sprlist [ del3 ], 0, -50 );
               SPR_setPosition ( &sprlist [ del4 ], 0, -50 );
            }

            // no
            if ( x >= 8 * 21  &&  y >= 8 * 12  &&  x <= 8 * 28  &&  y <= 8 * 15 )
            {
               sfxpsg_play ( SFXPSG_WRONG_SHOT );

               deleting = FALSE;

               SPR_setPosition ( &sprlist [ yn1 ], 0, -50 );
               SPR_setPosition ( &sprlist [ yn2 ], 0, -50 );
               SPR_setPosition ( &sprlist [ yn3 ], 0, -50 );
               SPR_setPosition ( &sprlist [ yn4 ], 0, -50 );
            }
         }
      }

      VDP_waitVSync();
   }
}



void game_change_colors ( )
{
   SYS_disableInts();
	VDP_setPalette ( PAL1, palGameScrBg [ lvl % MAX_BASELVL ] );
	SYS_enableInts();
}


void game_palette_restore()
{
   if ( SHOOT_FLAG )
   {
      VDP_setPaletteColors ( 0, palette_before_shoot, 64 );
   }
}


void game_palette_white ( )
{
   VDP_getPaletteColors ( 0, (u16*)palette_before_shoot, 64 );
   VDP_setPaletteColors ( 1, (u16*)palette_white_all, 63 );
}


void game_title ( void )
{
    u8 go = FALSE;
   VINT_SCROLL_FLAG = TRUE;
   VINT_JOY_UPDATE  = TRUE;

   if ( FIRST_TIME_FLAG )
   {
      _JOYreset();

      VDP_setEnable(FALSE);
      SYS_disableInts();

      VDP_drawImageEx ( BPLAN, &title_scr_bg, TILE_ATTR_FULL ( PAL0, FALSE, FALSE, FALSE, 16 ), 0, 0, TRUE, TRUE );

      map.map  = (genresMap_t*)&foreground;
      map.pal  = PAL1;
      map.plan = APLAN;
      map.prio = FALSE;
      map.vram = 55;

      FMP_load        ( &map, TRUE );
      FMP_set_palette ( &map );

      while (!go)
      {

         FMP_draw ( &map, 0, 0, 0, 0, 40, 28 );

         VDP_setEnable(TRUE);
         SYS_enableInts();

         scroll = scroll_init ( &map );

         _init_sprites();

         while ( TRUE )
         {
            if ( _JOYclick() )
            {
               s16 x = _JOYgetPosX();
               s16 y = _JOYgetPosY();

               if ( x >= 104 && y>= 136 && x <= 208 && y <= 160 ) // start
               {
                  go = TRUE;
                  break;
               }

               if ( x >= 288 && y>= 192 && x <= 304 && y <= 208 ) // info
               {
                  sfxpsg_play ( SFXPSG_PRESS_BTN );
                 _creditos ( );
                 break; // goto label_1;  // please, somebody kill me for doing this ==> DIEEEEEEE !
               }
            }

            VDP_waitVSync();
         }
         SYS_disableInts();
      }

      SYS_enableInts();

      sfxpsg_play ( SFXPSG_PRESS_BTN );

      FIRST_TIME_FLAG = FALSE;

      _scroll ( +1 );
    }



   _scr_select( );

    game_change_colors ( ); // Seteamos el color, dependiendo del nivel elegido.

   _scroll ( +1 );


   VINT_SCROLL_FLAG  = FALSE;
   VDP_setHorizontalScroll ( PLAN_B, 0 );

   SYS_disableInts();
   VDP_drawImageEx ( APLAN, &sueloselva_scr_bg, TILE_ATTR_FULL ( PAL1, TRUE, FALSE, FALSE, 0x02F0 ), 0, 16, FALSE, TRUE);
   SYS_enableInts();


}



void game_loop ( )
{
    _JOYsetGameMode(TRUE);

   sd_reset();
   SPR_clear();

   LENGUA_init( );
   BICHOS_init( );
   captured_init ( );

   HUD_init( );

   _countdown( );

   _JOYsetXY ( VDP_getScreenWidth() >> 1, LENGUA_HIDE_LIMIT );
   JOY_setEventHandler ( (_joyEventCallback*) &_lightgun_handler );

   VINT_JOY_UPDATE = FALSE;
   SHOOT_FLAG = FALSE;

   u8 continuar = TRUE;

   while ( continuar )
   {
      SYS_disableInts();

      _update_lightgun();
      _JOYupdate();

      s16 x = _JOYgetPosX();
      s16 y = _JOYgetPosY();

                  LENGUA_update   ( x, y );
      continuar = BICHOS_update   ( x, y );
                  captured_update ( x, y );

      SYS_enableInts();

      VDP_waitVSync ( );
   }

   VINT_JOY_UPDATE = TRUE;

   JOY_setEventHandler ( (_joyEventCallback*) NULL );
   _JOYsetGameMode(FALSE);
}



void game_gameover ( void )
{

   _init_sprites();

   SYS_disableInts();
       FMP_draw ( &map, 0, 16, 0, 28*3-4, 40, 4 ); // Overwrite floor with low priority

       VDP_drawImageEx ( BPLAN, &game_over_scr_fg, TILE_ATTR_FULL ( PAL2, TRUE, FALSE, FALSE, 0x02F0 ), 0, 0, TRUE, TRUE );



      // VES TU PUNTUACIÓN
      text_init( &bigfont, 0x0330, PAL2 );
      textNum_draw_Hcentered ( score, 12, 0 );

   SYS_enableInts();

   _JOYwaitClick();
   sfxpsg_play ( SFXPSG_PRESS_BTN );
}


void game_insert_hs ( )
{

   s8 pos = save_highscore_pos ( score );

   if ( IS_PRACTICE_MODE  ||  pos == SAVE_NO_HS )
   {
      return;
   }

   VDP_clearPlan ( BPLAN, FALSE );

   u8 idx = 0;
   s8 previous_len = 0;
   u8 str [ SAVE_MAX_NAME ] = "";

    SYS_disableInts();
        VDP_drawImageEx ( BPLAN, &submit_scr_fg, TILE_ATTR_FULL ( PAL2, TRUE, FALSE, FALSE, 0x02F0 ), 0, 0, TRUE, TRUE );
    SYS_enableInts();


   while( TRUE )
   {
      if ( previous_len != strlen ( str ) )
      {
         SYS_disableInts();
         text_init ( &bigfont, 0x03B0, PAL2 );
         VDP_clearTileMapRect ( BPLAN, 0, 9, VDP_getScreenWidth()/8, 4 );
         text_draw_Hcentered ( str, 9, 0 );

         previous_len = strlen ( str );
         SYS_enableInts();
      }

      _JOYwaitClick();

      s16 x = _JOYgetPosX();
      s16 y = _JOYgetPosY();

      if ( y >= 140 && y <= 156 )      // ROW 1
      {
              if ( x >=  12 && x <=  36 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'A'; }
         else if ( x >=  36 && x <=  60 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'B'; }
         else if ( x >=  60 && x <=  84 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'C'; }
         else if ( x >=  84 && x <= 108 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'D'; }
         else if ( x >= 108 && x <= 132 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'E'; }
         else if ( x >= 132 && x <= 156 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'F'; }
         else if ( x >= 156 && x <= 180 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'G'; }
         else if ( x >= 180 && x <= 204 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'H'; }
         else if ( x >= 204 && x <= 228 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'I'; }
         else if ( x >= 228 && x <= 252 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'J'; }
         else if ( x >= 252 && x <= 276 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'K'; }
         else if ( x >= 276 && x <= 300 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'L'; }
      }

      else if ( y >= 156 && y <= 172 ) // ROW 2
      {
              if ( x >=  12 && x <=  36 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'M'; }
         else if ( x >=  36 && x <=  60 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'N'; }
         else if ( x >=  60 && x <=  84 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'O'; }
         else if ( x >=  84 && x <= 108 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'P'; }
         else if ( x >= 108 && x <= 132 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'Q'; }
         else if ( x >= 132 && x <= 156 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'R'; }
         else if ( x >= 156 && x <= 180 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'S'; }
         else if ( x >= 180 && x <= 204 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'T'; }
         else if ( x >= 204 && x <= 228 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'U'; }
         else if ( x >= 228 && x <= 252 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'V'; }
         else if ( x >= 252 && x <= 276 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'W'; }
         else if ( x >= 276 && x <= 300 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'X'; }
      }

      else if ( y >= 172 && y <= 188 ) // ROW 3
      {
              if ( x >=  12 && x <=  36 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'Y'; }
         else if ( x >=  36 && x <=  60 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = 'Z'; }
         else if ( x >=  60 && x <=  84 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '1'; }
         else if ( x >=  84 && x <= 108 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '2'; }
         else if ( x >= 108 && x <= 132 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '3'; }
         else if ( x >= 132 && x <= 156 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '4'; }
         else if ( x >= 156 && x <= 180 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '5'; }
         else if ( x >= 180 && x <= 204 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '6'; }
         else if ( x >= 204 && x <= 228 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '7'; }
         else if ( x >= 228 && x <= 252 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '8'; }
         else if ( x >= 252 && x <= 276 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '9'; }
         else if ( x >= 276 && x <= 300 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '0'; }
      }

      else if ( y >= 188 && y <= 204 ) // ROW 4
      {
              if ( x >=  12 && x <=  36 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '?'; }
         else if ( x >=  36 && x <=  60 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '!'; }
         else if ( x >=  60 && x <=  84 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '-'; }
         else if ( x >=  84 && x <= 108 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '.'; }
         else if ( x >= 108 && x <= 132 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = ':'; }
         else if ( x >= 132 && x <= 156 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = ','; }
         else if ( x >= 156 && x <= 180 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = ';'; }
         else if ( x >= 180 && x <= 204 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '"'; }
         else if ( x >= 204 && x <= 228 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = '\''; }
         else if ( x >= 228 && x <= 252 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); str[ idx++ ] = ' '; }
         else if ( x >= 252 && x <= 276 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); if(idx) { str[ idx-- ] = ' ';} }
         else if ( x >= 276 && x <= 300 ) { sfxpsg_play ( SFXPSG_PRESS_BTN ); break; }
      }

      /* OVERFLOW CONTROL */
      if ( idx == SAVE_MAX_NAME ) idx = SAVE_MAX_NAME - 1;
      str [ idx ] = '\0';
   }

   save_highscore_save ( pos, score, str );
   VDP_waitVSync();
}


void game_scores ( void )
{
   VDP_waitVSync();
   SYS_disableInts();

    //VDP_clearPlan ( BPLAN, 0 );
   VDP_drawImageEx ( BPLAN, &score_scr_fg, TILE_ATTR_FULL( PAL2, TRUE, FALSE, FALSE, 0x02F0), 0, 0, TRUE, 0 );
   VDP_loadFont ( &font_score, 0 );

   //VDP_waitVSync();

   const SAVE save = save_read();

   u8 i;

   for ( i=0; i<SAVE_MAX_SCORE; i++ )
   {
      u8 number [ 2 ];
      u8 value [ 10 ];

      uintToStr ( i+1, number, 0 );
      uintToStr ( save.score[i].value, value, 0 );

      u8 x = 27;

      if ( save.score[i].value >=     10000 ) --x;
      if ( save.score[i].value >=    100000 ) --x;
      if ( save.score[i].value >=   1000000 ) --x;
      if ( save.score[i].value >=  10000000 ) --x;
      if ( save.score[i].value >= 100000000 ) --x;

      VDP_drawTextBG ( APLAN, number,             TILE_ATTR(PAL2,1,0,0),  9, i*2 );
      VDP_drawTextBG ( APLAN, ".",                TILE_ATTR(PAL2,1,0,0), 10, i*2 );
      VDP_drawTextBG ( APLAN, save.score[i].name, TILE_ATTR(PAL2,1,0,0), 12, i*2 );
      VDP_drawTextBG ( APLAN, value,              TILE_ATTR(PAL2,1,0,0),  x, i*2 );

      if (LIB_DEBUG)
      {
         KDebug_Alert ( save.score[i].name ); // ops, that doesnt work
         waitMs(1000);
      }

   }

   SYS_enableInts();


   _JOYwaitClick();
   sfxpsg_play ( SFXPSG_PRESS_BTN );


    SYS_disableInts();
       FMP_draw ( &map, 0, -1, 0, 28*2+7, 40, 16 ); // repaint jungle to clean up the score lines

       VDP_drawImageEx ( BPLAN, &title_scr_bg, TILE_ATTR_FULL ( PAL0, FALSE, FALSE, FALSE, 16 ), 0, 0, TRUE, TRUE );
   SYS_enableInts();

   VINT_SCROLL_FLAG = TRUE;
   VINT_JOY_UPDATE  = TRUE;

   _scroll ( -1 ); // -1 goes up
}
