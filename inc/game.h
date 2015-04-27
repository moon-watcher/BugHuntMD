#ifndef _GAME_H_
#define _GAME_H_

   //== PROTOTYPES
   void game_title           ( void );
   void game_loop            ( void );
   void game_change_colors   ( void );
   void game_gameover        ( void );
   void game_insert_hs       ( void );
   void game_scores          ( void );
   void game_palette_restore ( void );
   void game_palette_white   ( void );
   //== GLOBAL DATA
   u8 IS_PRACTICE_MODE;
   u8 FIRST_TIME_FLAG;
   u8 SHOOT_FLAG;


   Sprite *pointer;
   Sprite *selector;
   Sprite *shot;


#endif
