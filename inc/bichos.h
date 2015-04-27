#ifndef _BICHOS_H_
#define _BICHOS_H_

    #define ANIM_MOSCA    3
    #define ANIM_AVISPA   4
    #define ANIM_LIBELULA 5

    #define ANIM_MOSCA_CATCHED    6
    #define ANIM_AVISPA_CATCHED   7
    #define ANIM_LIBELULA_CATCHED 8


    #define BICHOS_NB_DISTINCT       3


    #define MAX_FILAS      6



    typedef struct
    {
       s16 pos_x;
       s16 pos_y;
       u8  timer;
       s8  direction_x;

    } ROW_DEF;


    typedef struct
    {
       u8  id;
       u8  rc_pos;
       u8  rc_pos_catched;
       u32 score1;
       u32 score2;
       u8  kills_you:1;
       u8  speed_inc; // *1 o *2
       SpriteDefinition *sd;
    }
    BICHO_DEF;


    typedef struct
    {
       u8 sprite;
       u8 activo:1;

       fix32 x;
       fix32 y;

       BICHO_DEF *bicho_def;
       ROW_DEF   *pi;
    }
    BICHO_CACHE;



    enum
    {
       MOSCA = 1,
       AVISPA,
       LIBELULA
    };


    const BICHO_DEF bichos_list [ BICHOS_NB_DISTINCT ];




    void BICHOS_init   ( void );
    u8   BICHOS_update ( s16 x, s16 y );



   s16 showshot;

#endif
