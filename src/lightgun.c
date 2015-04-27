#include <genesis.h>

#include "../inc/lightgun.h"




static u16 _click = 0;
static u16 _port  = 0;



void LightgunInit ( u16 port )
{
   _click = 0;
   _port  = port;

   lightgun.x = -1;
   lightgun.y = -1;
}


void LightgunUpdate ( )
{
   lightgun.x = -1;
   lightgun.y = -1;

   if ( lightgun.support == JOY_SUPPORT_JUSTIFIER_BLUE )
   {
      lightgun.x = ( JOY_readJoypadX ( _port ) - 22 ) * 2;
      lightgun.y =   JOY_readJoypadY ( _port );
   }
   else if ( lightgun.support == JOY_SUPPORT_MENACER )
   {
      lightgun.x = ( JOY_readJoypadX ( _port ) - 22 ) * 2 - 44;
      lightgun.y =   JOY_readJoypadY ( _port );
   }
   else if ( lightgun.support == JOY_SUPPORT_PHASER )
   {
      lightgun.x = ( JOY_readJoypadX ( _port ) - 22 ) * 2 - 22;
      lightgun.y =   JOY_readJoypadY ( _port );
   }


//   //
//   // more accurate position read
//   //
//
//   #define TIMES 1 // 30
//
//   s16
//      i,
//      x , y,
//      xinc = 0, yinc = 0,
//      repeat = 0;
//
//   for ( i = 0; i<TIMES; i++ )
//   {
//      x = JOY_readJoypadX ( _port );
//      y = JOY_readJoypadY ( _port );
//
//      if ( y == -1 )
//      {
//         continue;
//      }
//
//      xinc += x;
//      yinc += y;
//
//      ++repeat ;
//   }
//
//
//   if ( repeat )
//   {
//      lightgun.x = ( xinc / repeat - 22 ) * 2;
//      lightgun.y =   yinc / repeat;
//   }
//   else
//   {
//      lightgun.x = -1;
//      lightgun.y = -1;
//   }
}


u16 LightgunClick ( void )
{
   u16 state   = ( JOY_readJoypad ( _port ) & BUTTON_BTN );
   u16 changed = ( state != _click );
   u16 ret     = ( state && changed );

   _click      = state;

   return ret ;
}


u16 LightgunKillsIt ( u16 x, u16 y )
{
   return
   (
      lightgun.x > x  &&  lightgun.x < ( x + 24 ) &&
      lightgun.y > y  &&  lightgun.y < ( y + 24 )
   );
}
