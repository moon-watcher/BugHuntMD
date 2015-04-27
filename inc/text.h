#ifndef _TEXT_H_
#define _TEXT_H_

   #include <genres.h> /* v1.1 */

   //== PROTOTYPES
   void text_init                  ( struct genresSprites *genres, u16 vram, u16 palette );
   void text_draw                  ( u8 *str, u8 x, u8 y, u16 ms );
   void text_draw_centered         ( u8 *str, u16 ms );
   void text_draw_Hcentered        ( u8 *str, u8 y, u16 ms );
   void textNum_draw_centered      ( s32 n, u16 ms );
   void textNum_draw_Hcentered     ( s32 n, u8 y, u16 ms );
   void text_draw_sprite           ( u8 *str, u16 x, u16 y, u16 ms );
   void text_draw_sprites_centered ( u8 *str, u16 ms );
   void VDP_drawNumberBG           (u16 plan, s32 num, u16 flags, u16 x, u16 y, u16 minsize);

#endif
