#include <genesis.h>

#include "../inc/hotswap.h"
#include "../inc/lightgun.h"


static u16 supports[2];



u8 hotswap_port1 ( u8 flag )
{
   return flag; // maybe someday

   u8  ret  = flag;
   u16 port = JOY_getPortType ( PORT_1 );

   if ( port != supports[PORT_1] )
   {
      supports[PORT_1] = port;

      ret = ( JOY_getPortType(PORT_1) != PORT_TYPE_PAD );

      if ( ret )
      {
         JOY_setSupport ( PORT_1, JOY_SUPPORT_MOUSE );
      }
      else
      {
         JOY_setSupport ( PORT_1, JOY_SUPPORT_3BTN );
      }
   }

   return ret;
}


void hotswap_port2 ()
{
   return; // maybe someday

   u16 port = JOY_getPortType ( PORT_2 );

   if ( port != supports[PORT_2 ] )
   {
      supports[PORT_2] = port;

      lightgun.support = JOY_SUPPORT_OFF;

      switch ( port )
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

      JOY_setSupport ( PORT_2, supports[PORT_2] );
   }
}


