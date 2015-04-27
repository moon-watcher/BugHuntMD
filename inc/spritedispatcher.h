#ifndef _SPRITEDISPATCHER_H_
#define _SPRITEDISPATCHER_H_


    typedef enum
    {
       SD_DOWN =    0,
       SD_UP =      1

    } SDdir;

    //== PROTOTYPES
    void sd_delete  ( u8 i );
    void sd_reset   ( void );
    u8 sd_cnt       ( void );
    u8 sd_new       ( void );
    u8 sd_newEx     ( u8 start, SDdir direction );

    //== GLOBAL DATA
    Sprite sprlist[MAX_SPRITE];


#endif
