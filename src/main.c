/***
 *    - Feel free to do anything you want* with this code, consider it as PUBLIC DOMAIN -
 *               ... but be careful, this code is dark and full of terrors.
 *
 *                (*) NOT ALLOW TO USE THIS CODE FOR COMMERCIAL PURPOSES
 *
 *    ***** /\\\\\\\\\\\\\____/\\\________/\\\_____/\\\\\\\\\\\\ **********************
 *    ***** \/\\\/////////\\\_\/\\\_______\/\\\___/\\\////////// **********************
 *    ****** \/\\\_______\/\\\_\/\\\_______\/\\\__/\\\____________ ********************
 *    ******* \/\\\\\\\\\\\\\\__\/\\\_______\/\\\_\/\\\____/\\\\\\\ *******************
 *    ******** \/\\\/////////\\\_\/\\\_______\/\\\_\/\\\___\/////\\\ ******************
 *    ********* \/\\\_______\/\\\_\/\\\_______\/\\\_\/\\\_______\/\\\ *****************
 *    ********** \/\\\_______\/\\\_\//\\\______/\\\__\/\\\_______\/\\\ ****************
 *    *********** \/\\\\\\\\\\\\\/___\///\\\\\\\\\/___\//\\\\\\\\\\\\/ ****************
 *    ************ \/////////////_______\/////////______\//////////// *****************
 *    *************                                                  ******************
 *    ***** /\\\________/\\\__/\\\________/\\\__/\\\\\_____/\\\__/\\\\\\\\\\\\\\\ *****
 *    ***** \/\\\_______\/\\\_\/\\\_______\/\\\_\/\\\\\\___\/\\\_\///////\\\///// *****
 *    ****** \/\\\_______\/\\\_\/\\\_______\/\\\_\/\\\/\\\__\/\\\_______\/\\\ *********
 *    ******* \/\\\\\\\\\\\\\\\_\/\\\_______\/\\\_\/\\\//\\\_\/\\\_______\/\\\ ********
 *    ******** \/\\\/////////\\\_\/\\\_______\/\\\_\/\\\\//\\\\/\\\_______\/\\\ *******
 *    ********* \/\\\_______\/\\\_\/\\\_______\/\\\_\/\\\_\//\\\/\\\_______\/\\\ ******
 *    ********** \/\\\_______\/\\\_\//\\\______/\\\__\/\\\__\//\\\\\\_______\/\\\ *****
 *    *********** \/\\\_______\/\\\__\///\\\\\\\\\/___\/\\\___\//\\\\\_______\/\\\ ****
 *    ************ \///________\///_____\/////////_____\///_____\/////________\/// ****
 *
 * Original game by Lazy Brain Games, released for PC, CHECK OUT THIS! [http://gamejolt.com/games/arcade/bug-hunt/4674/]
 *
 * @CODE: 		    =>  Jack Nolddor    | Twitter: @nolddor, Mail to: nolddor@hotmail.com
 * @MONKEY:        =>  Mun             | Twitter: @MoonWatcherMD
 * @MUSIC / SFX: 	 =>  Davidian        | Twitter: @DavidBonus
 * @GRAPHIC / GFX: =>  Lazy Brain Games (Some fixes by Nolddor & MoonWatcherMD)
 * @PROJECT: 		 =>  #BugHunt
 * @START DATE: 	 =>  24-08-2014
 * @LAST UPDATE: 	 =>  31-01-2015
 * @LIB:           =>  SGDK (v1.11) by Stephane Dallongeville
 *
 *
 *
 * --------------------------------------------------------------------------------------------------------------------------------
 *  SPECIAL THANKS TO
 * --------------------------------------------------------------------------------------------------------------------------------
 *
 * @BUG SLAYER & SAVIOUR => KanedaFr             | Twitter: @spritesmind
 * @DEVIDE SUPPORT       => Chilly Willy
 * @COVER ART            => John Bell            | Twitter: @LazyBrainGames
 * @COVER RETOUCH        => Dani Nevado          | Twitter: @DanySnowyman
 * @MEGAMOUSE LOGO       => Felipe Monge Corbala | Twitter: @vakapp
 * @FRENCH TRANSLATION   => KanedaFr             | Twitter: @spritesmind
 * @ITALIAN TRANSLATION  => Darkhorace           | Twitter: @oongamoonga
 *
 *
 *
 * --------------------------------------------------------------------------------------------------------------------------------
 *  HOW TO COMPILE
 * --------------------------------------------------------------------------------------------------------------------------------
 *
 * Sega Genesis/Megadrive version of Bug Hunt was created using SGDK v1.11 and GenRes v1.1.
 *
 *  - Go gendev.spritesmind.net/page-genres.html
 *  - Download GenRes v1.1
 *  - Extract and replace all genres.* files in the zip into SGDK's bin directory
 *  - Compile SGDK
 *  - Compile Bug Hunt
 *  - Tadah!
 *
 *
 *
 * --------------------------------------------------------------------------------------------------------------------------------
 *  CHANGELOG
 * --------------------------------------------------------------------------------------------------------------------------------
 * 31-01-2015 » First release
 * --------------------------------------------------------------------------------------------------------------------------------
 *
 */


#include <genesis.h>
#include <kdebug.h>


#include "../inc/main.h"
#include "../inc/game.h"
#include "../res/rescomp.h"
#include "../inc/save.h"
#include "../inc/input.h"
#include "../inc/lengua.h"
#include "../inc/spritedispatcher.h"
#include "../inc/musicvgm.h"
#include "../inc/sfxpsg.h"
#include "../inc/lightgun.h"
#include "../inc/segalogo.h"

static volatile s16 h_scroll;



static _voidCallback *VIntCallback()
{
   if ( VINT_SCROLL_FLAG )
   {
      VDP_setHorizontalScroll ( PLAN_B, h_scroll-- ); // Updating Horizontal Scroll (CLOUDS)
   }

   if ( VINT_JOY_UPDATE )
   {
      _JOYupdate ( );
      SPR_setPosition ( pointer, _JOYgetPosX(), _JOYgetPosY() );
   }

   if ( IS_PALSYSTEM || ( !IS_PALSYSTEM && ( vtimer % 6 ) ) )
	{
		sfxpsg_frame();
   }

   SPR_update ( sprlist, sd_cnt() );

   return 0;
}



void _show_scr( const Image *img, u32 ms )
{
   SYS_disableInts();
   VDP_setEnable(FALSE);
      VDP_drawImageEx( BPLAN, img, TILE_ATTR_FULL( PAL0, FALSE, FALSE, FALSE, TILE_USERINDEX ), 0, 0, TRUE, FALSE );
   VDP_setEnable(TRUE);
   SYS_enableInts();

   waitMs(ms);
}



static void _init( u16 hard )
{
   if ( hard == 0 ) // 0 is soft reset
   {
      VDP_drawText ( "BUG HUNT", 16, 9 );
      VDP_drawText ( "FOR SEGA MEGADRIVE", 11, 11 );
      VDP_drawText ( "BY THE AFROMONKEYS, 2015", 8, 13 );

      waitMs(3000);


      // That's weird.
      // Resetting in SGDK v1.11 invokes JOY_init() that
      // makes Justifier | Menacer not to be detected if
      // mouse is on PORT_1

      if ( JOY_getPortType(PORT_1) != PORT_TYPE_PAD )
      {
         VDP_drawText ( "PLEASE, REBOOT YOUR SYSTEM", 7, 19 );

         while ( 1 );
      }

      _start_entry(); // even more reset
   }


   sd_reset();
   SYS_assertReset(); // makes gensKmod crash, WTF?!



   VDP_init();
   //JOY_init();  // can cause mouse + justifer issues

   h_scroll = 0;
   VDP_setScrollingMode ( HSCROLL_PLANE, VSCROLL_PLANE );     /* The scroll mode never change during the game */
   VDP_setPlanSize(64,32);

   save_init( );

   SPR_init(0);                                             /* Sprite Engine INIT */
   VDP_setPalette(PAL3, sprpal.data);                       /* Sprite Palette (never change during the game) */

   /* PAD & Mouse (PORT_1) & Lightgun (PORT_2) Support */
   _JOYint ( TRUE );
   LightgunInit ( PORT_2 );

   VINT_SCROLL_FLAG  = FALSE;
   VINT_JOY_UPDATE   = FALSE;

   SYS_setVIntCallback((_voidCallback*) VIntCallback);

   FIRST_TIME_FLAG = TRUE;
}




/* **************************************
 *         GAME STARTS HERE!	        *


 ************************************** */

int main ( int argc, char *argv[] )
{
   _init( argc );

   _show_scr ( &disclaimer, 3500 );
   _segalogo( );

    musicvgm_play ( MUSICVGM_MAINTHEME );

   while(TRUE)
   {
      game_title( );
      game_loop( );
      game_gameover( );
      game_insert_hs ( );
      game_scores ( );
   }

   return 0; /* Never Reached */
}
