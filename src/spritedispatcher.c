/*
    Borrowed from @MoonwatcherMD - Game: JunkbotsMD (BitBitJam 2014)

    Last Update: JackNolddor 06-Septiembre-2014
    (Corregido?? Bug en sprlist ahora se setea a NULL en las funciones reset y delete)
*/


#include <genesis.h>
#include "../inc/spritedispatcher.h"


static u8 used_sprites [ MAX_SPRITE ] = { [0 ... MAX_SPRITE-1] = FALSE};
static u8 sp_cnt = 0;


static u8 _find ( u8 start, SDdir direction )
{
   s8 i;

   switch ( direction )
   {
      case SD_DOWN:

         for ( i = start; i < MAX_SPRITE; i++ )
         {
            if ( ! used_sprites[(u8) i] )
            {
               return i;
            }
         }

         return MAX_SPRITE-1;


      case SD_UP:

         for ( i = start; i >= 0; i-- )
         {
            if ( ! used_sprites[(u8) i] )
            {
               return i;
            }
         }

         return 0;
   }

   return 0;
}




u8 sd_cnt ( void )
{
    return sp_cnt;
}




u8 sd_newEx ( u8 start, SDdir direction )
{
   u8 i = ( start >= MAX_SPRITE ) ? (MAX_SPRITE-1) : _find( start, direction );

   used_sprites[ i ] = TRUE;
   if( i >= sp_cnt ) { sp_cnt = i + 1; }

   return i;
}




u8 sd_new ( void )
{
    return sd_newEx( 0, SD_DOWN);
}




void sd_delete ( u8 i )
{
   if ( i >= sp_cnt ) return;

   if ( i+1 == sp_cnt ) { sp_cnt = i;}

   memset(&sprlist[i], NULL, sizeof(Sprite));
   used_sprites[i] = FALSE;
}




void sd_reset( void )
{
    sp_cnt = 0;
    memset( used_sprites, FALSE,    sizeof (u8) * MAX_SPRITE );
    memset( sprlist,       NULL, sizeof(Sprite) * MAX_SPRITE );
}
