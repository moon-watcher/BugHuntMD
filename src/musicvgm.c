#include <genesis.h>

#include "../inc/musicvgm.h"
#include "../res/musicvgm.h"



static const MUSICVGM _list [ ] =
{

        { "Main", (u8*) musicvgm_maintheme },
        { "Sega", (u8*) musicvgm_sega },
};




void musicvgm_play ( u8 nb )
{
   musicvgm_stop ( );
   SND_startPlay_VGM ( _list[ nb ].data );
}


MUSICVGM *musicvgm_get_track ( u8 nb )
{
   return ( MUSICVGM* ) & _list [ nb ];
}


u8 musicvgm_nb_tracks()
{
   return MUSICVGM_MAX;
}


void musicvgm_stop ( )
{
   SND_isPlaying_MVS ( );
}
