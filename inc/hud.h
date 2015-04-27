#ifndef _HUD_H_
#define _HUD_H_

    #include "../inc/bichos.h"

    //==> DEFINES
    #define MAX_BASELVL         8
    #define MAX_BASESCORE    5000

    #define TOPE_LVL           50
    #define TOPE_SCORE  999999999

    //==> GLOBAL DATA
    s32 score;
    u16 lvl;

    //==> PROTOTPES
    void HUD_init      ( void );
    void HUD_level     ( void );
    void HUD_score     ( void );
    void HUD_score_add ( BICHO_CACHE *b, u8 repeticiones );
    void HUD_score_subtract ( BICHO_CACHE *b );


#endif
