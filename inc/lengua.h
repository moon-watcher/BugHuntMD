#ifndef _LENGUA_H_
#define _LENGUA_H_

   #define LENGUA_HIDE_LIMIT 165
   #define MAX_BONGO_Y 169

   //== PROTOTYPES
   void LENGUA_init                 ( );
   void LENGUA_update               ( s16 x, s16 y );
   void lengua_hide                 ( );
   void lengua_dragon_set_pos       ( s16 x );
   void lengua_dragon_muere         ( );
   void LENGUA_dragon_has_been_shot ( );


   struct
   {
      s16 x;
      s16 y;
      s16 counter;
   }
   bongo;

#endif
