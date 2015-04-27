/* JACK NOLDDOR - 02/09/2014 */

/*
    TODO:

    - CAMBIO EN CALIENTE DEL MOUSE (SUGERIDO POR MOONCHY 02/09/2014-23:00
*/

#include <genesis.h>
#include "../inc/input.h"
#include "../inc/lightgun.h"
#include "../inc/hotswap.h"
#include "../res/musicvgm.h"


#define MEDIUM_SPEED    4
#define HIGH_SPEED      8
#define MASTER_SPEED    10
#define ULTRA_SPEED     12


/* ******************************* */
/*   We don't want to share this   */
/* ******************************* */

 static u8     MOUSE_FLAG = FALSE;
 static u8   BOUNDED_FLAG = FALSE;
 static u8    INGAME_FLAG = FALSE;

 static u16 SCR_W = 0;
 static u16 SCR_H = 0;

 static volatile s16 readedX = 0;
 static volatile s16 readedY = 0;

 static volatile u16 click = 0;

 static volatile Vect2D_s16 joypos;



/* ******************************* */

void _JOYsetGameMode (u8 flag)
{
    INGAME_FLAG = flag;
}


void _JOYint ( u8 bounded )
{
   lightgun.support = JOY_SUPPORT_OFF;

   JOY_setSupport ( PORT_1, JOY_SUPPORT_OFF );
   JOY_setSupport ( PORT_2, JOY_SUPPORT_OFF );


   JOY_setSupport ( PORT_1, JOY_SUPPORT_3BTN );

   switch ( JOY_getPortType ( PORT_2 ) )
   {
      case PORT_TYPE_JUSTIFIER:
         lightgun.support = JOY_SUPPORT_JUSTIFIER_BLUE;
         break;

      case PORT_TYPE_MENACER:
         lightgun.support = JOY_SUPPORT_MENACER;
         break;

      default:
         lightgun.support = JOY_SUPPORT_PHASER;
         break;
   }

   JOY_setSupport ( PORT_2, lightgun.support );


   JOY_setSupport ( PORT_1, JOY_SUPPORT_OFF );

   BOUNDED_FLAG = bounded;
     MOUSE_FLAG = ( JOY_getPortType(PORT_1) != PORT_TYPE_PAD );
    _JOYsetGameMode(FALSE);

   if ( MOUSE_FLAG )
   {
      JOY_setSupport ( PORT_1, JOY_SUPPORT_MOUSE );

      readedX = JOY_readJoypadX(JOY_1);
      readedY = JOY_readJoypadY(JOY_1);
   }
   else
   {
      JOY_setSupport ( PORT_1, JOY_SUPPORT_3BTN );
   }



   SCR_W = VDP_getScreenWidth();
   SCR_H = VDP_getScreenHeight();

   joypos.x = SCR_W >> 1;
   joypos.y = SCR_H >> 1;

   click = 0;
}


void _JOYreset ( void )
{
   _JOYsetXY ( SCR_W >> 1, SCR_H >> 1 );
}


void _JOYsetXY ( s16 x, s16 y )
{
   if ( MOUSE_FLAG )
   {
      readedX = JOY_readJoypadX(JOY_1);
      readedY = JOY_readJoypadY(JOY_1);
   }

   joypos.x = x;
   joypos.y = y;
}


s16 _JOYgetPosX ( void ) { return joypos.x; }
s16 _JOYgetPosY ( void ) { return joypos.y; }


static void _JOYupdateMouse ( void )
{
   s16 readX = JOY_readJoypadX(JOY_1);
   s16 readY = JOY_readJoypadY(JOY_1);

   joypos.x -= readedX - readX;
   joypos.y += readedY - readY;

   readedX = readX;
   readedY = readY;
}



static void _JOYupdatePad ( void )
{
   u8 y_speed = MEDIUM_SPEED;
   u16 state  = JOY_readJoypad ( JOY_1 );

    if (INGAME_FLAG)
    {
             if ( state & BUTTON_A     ) { y_speed = HIGH_SPEED; }
        else if ( state & BUTTON_B     ) { y_speed = MASTER_SPEED; }
        else if ( state & BUTTON_C     ) { y_speed = ULTRA_SPEED; }
    }

        if ( state & BUTTON_RIGHT ) { joypos.x += MEDIUM_SPEED; }
   else if ( state & BUTTON_LEFT  ) { joypos.x -= MEDIUM_SPEED; }

        if ( state & BUTTON_DOWN  ) { joypos.y += y_speed; }
   else if ( state & BUTTON_UP    ) { joypos.y -= y_speed; }
}



void _JOYupdate( void )
{
   MOUSE_FLAG = hotswap_port1 ( MOUSE_FLAG );
   hotswap_port2 ( );

   MOUSE_FLAG ? _JOYupdateMouse() : _JOYupdatePad();

   if ( BOUNDED_FLAG )
   {
           if ( joypos.x < 0 )         { joypos.x = 0; }
      else if ( joypos.x > (SCR_W-8) ) { joypos.x = (SCR_W-8); }

           if ( joypos.y < 1 )         { joypos.y = 1; }
      else if ( joypos.y > (SCR_H-8) ) { joypos.y = (SCR_H-8); }
   }
}



u16 _JOYclick ( void )
{
   u16 state   = ( JOY_readJoypad(JOY_1) & BUTTON_BTN );
   u16 changed = ( state != click );
   u16 ret     = ( state && changed );

   click       = state;

   return ret ;
}



void _JOYwaitClick ( )
{
   while ( !_JOYclick() )
   {
      VDP_waitVSync();
   }
}
