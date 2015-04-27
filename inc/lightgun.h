#ifndef _LIGHTGUN_H_
#define _LIGHTGUN_H_


   typedef struct
   {
      s16 x;
      s16 y;
      u16 support;
   }
   LIGHTGUN ;


   volatile LIGHTGUN lightgun;


   void LightgunInit    ( u16 port );
   void LightgunUpdate  ( );
   u16  LightgunClick   ( void );
   u16  LightgunKillsIt ( u16 x, u16 y );

#endif
