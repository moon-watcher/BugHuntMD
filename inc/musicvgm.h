
enum
{
   MUSICVGM_MAINTHEME = 0,
   MUSICVGM_SEGA,

   MUSICVGM_MAX
};




typedef struct
{
   u8  title[28];
   u8 *data;
}
MUSICVGM;



void      musicvgm_play       ( u8 nb );
MUSICVGM *musicvgm_get_track  ( u8 track );
u8        musicvgm_nb_tracks  ( );
void      musicvgm_stop ( );
