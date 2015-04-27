#ifndef _SFXPSG_H_
#define _SFXPSG_H_


enum
{
   SFXPSG_PRESS_BTN = 0,
   SFXPSG_COUNTDOWN,
   SFXPSG_BLOW_WHISTLE,
   SFXPSG_STUCK,
   SFXPSG_SWALLOW,
   SFXPSG_DEATH,
   SFXPSG_LEVEL,
   SFXPSG_DEAD_SHOT,
   SFXPSG_WRONG_SHOT,
   SFXPSG_SHOT,
   SFXPSG_MAX
};


typedef struct
{
	u8  title[16];
	u8 *data;
	u8  num;
}
PSG;


void   sfxpsg_play      ( u16 psg );
void   sfxpsg_frame     ( void );
u16    sfxpsg_nb_tracks ( );
PSG   *sfxpsg_get_track ( u16 num );


#endif
