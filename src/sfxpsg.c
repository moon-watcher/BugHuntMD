/* PSG multichannel sound effects player v1.0 by Shiru, 03.11.07 */

/* gcc version 12.12.07 */

/* Modified by @MoonWatcherMD 20130611. Thanks, Shiru! */


#include <genesis.h>

#include "../inc/sfxpsg.h"
#include "../res/sfxpsg.h"

static const PSG _list [] =
{
    { "Press Btn",      (u8*) sfxpsg_press_btn_data,        0 }, // 0
    { "Countdown",      (u8*) sfxpsg_countdown_data,        0 }, // 1
    { "Blow Whistle",   (u8*) sfxpsg_blow_whistle_data,     0 }, // 2
    { "Stuck",          (u8*) sfxpsg_stuck_data,            0 }, // 3
    { "Swallow",        (u8*) sfxpsg_swallow_data,          0 }, // 4
    { "Death",          (u8*) sfxpsg_death_data,            0 }, // 5
    { "Level",          (u8*) sfxpsg_level_data,            0 }, // 6
    { "DeadShot",       (u8*) sfxpsg_dead_shot_data,        0 }, // 7
    { "WrongShot",      (u8*) sfxpsg_wrong_shot_data,       0 }, // 8
    { "Shot",           (u8*) sfxpsg_shot_data,             0 }, // 9

    { "EOF",            NULL,                               0 } /* DON'T REMOVE */
};

#define PSG_DATA		0xc00011
#define PSG_VCH_MAX	    4



static struct
{
	const u8 *data;

	struct
	{
		struct
		{
			s16 ptr;
			s16 wait;
			s16 time;
			u16 div;
			u8 vol;
		}
		slot[PSG_VCH_MAX];
	}
	chn[4];
}
PSGFX;





///////////////////////////////////////////////////////////////////////////////////////////////////


static void _psgfx_addch ( u16 chn,u16 off );





///////////////////////////////////////////////////////////////////////////////////////////////////


static void _psgfx_addch ( u16 chn, u16 off )
{
	s16 i, j, vchn, ntime, tmax, vcnt;

	if(chn<2)
	{
		tmax=PSG_VCH_MAX;

		for(i=2;i>=0;i--)
		{
			vcnt=0;

			for(j=0;j<PSG_VCH_MAX;j++)
			{
				if(PSGFX.chn[i].slot[j].ptr>=0)
				{
					vcnt++;
				}
			}

			if(vcnt==0)
			{
				chn=i;
				break;
			}
			if(vcnt<tmax)
			{
				tmax=vcnt;
				chn=i;
			}
		}
	}

	vchn=-1;

	for(i=0;i<PSG_VCH_MAX;i++)
	{
		if(PSGFX.chn[chn].slot[i].ptr<0)
		{
			vchn=i;
			break;
		}
	}

	if(vchn<0)
	{
		tmax=-1;

		for(i=0;i<PSG_VCH_MAX;i++)
		{
			ntime=PSGFX.chn[chn].slot[i].time;

			if(ntime>tmax)
			{
				tmax=ntime;
				vchn=i;
			}
		}
	}

	PSGFX.chn[chn].slot[vchn].ptr=off;
	PSGFX.chn[chn].slot[vchn].wait=0;
	PSGFX.chn[chn].slot[vchn].time=0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////




void sfxpsg_play ( u16 psg )
{
	volatile u8 *pb;

	s16 chn, eoff, doff, chcnt;
	u16 i, j;


	pb = (u8*) PSG_DATA;

	*pb = 0x9f;
	*pb = 0xbf;
	*pb = 0xdf;
	*pb = 0xff;

	PSGFX.data = (u8*) _list[psg].data;

	for(i=0;i<4;i++)
	{
		for(j=0;j<PSG_VCH_MAX;j++)
		{
			PSGFX.chn[i].slot[j].ptr=-1;
			PSGFX.chn[i].slot[j].wait=0;
		}
	}


	eoff  = 2 + ( _list[psg].num << 1 );
	doff  = ( PSGFX.data [ eoff ] << 8 ) + PSGFX.data [ eoff + 1 ];
	chcnt = PSGFX.data [ doff++ ];

	for ( i = 0; i < chcnt; i++ )
	{
		eoff=(PSGFX.data[doff++]<<8);
		eoff+=PSGFX.data[doff++];
		chn=PSGFX.data[eoff++];
		_psgfx_addch(chn,eoff);
	}
}


void sfxpsg_frame ( void )
{
	volatile u8 *pb;
	u8 mbyte;
	s16 pchn, vchn, rchn, mvol, nvol;
	u16 div;

	if ( !PSGFX.data )
	{
		return;
	}

	pb=(u8*) PSG_DATA;

	for ( pchn=0; pchn<4; pchn++ )
	{
		for(vchn=0;vchn<PSG_VCH_MAX;vchn++)
		{
			if(PSGFX.chn[pchn].slot[vchn].ptr<0)
			{
				continue;
			}

			PSGFX.chn[pchn].slot[vchn].time++;

			if(PSGFX.chn[pchn].slot[vchn].wait)
			{
				PSGFX.chn[pchn].slot[vchn].wait--;
				continue;
			}

			mbyte=PSGFX.data[PSGFX.chn[pchn].slot[vchn].ptr++];

			switch(mbyte&0xc0)
			{
				case 0x00:/*0=eof 1..31=wait*/
					if(!mbyte) PSGFX.chn[pchn].slot[vchn].ptr=-1; else PSGFX.chn[pchn].slot[vchn].wait=mbyte-1;
					break;
				case 0x40:/*vol only*/
					PSGFX.chn[pchn].slot[vchn].vol=mbyte&0x0f;
					break;
				case 0x80:/*div only*/
					PSGFX.chn[pchn].slot[vchn].div=((u16)mbyte<<8)|PSGFX.data[PSGFX.chn[pchn].slot[vchn].ptr++];
					break;
				case 0xc0:/*vol and div*/
					PSGFX.chn[pchn].slot[vchn].vol=(mbyte>>2)&0x0f;
					PSGFX.chn[pchn].slot[vchn].div=((u16)(mbyte&0x03)<<8)|PSGFX.data[PSGFX.chn[pchn].slot[vchn].ptr++];
					break;
			}
		}

		rchn=-1;
		mvol=16;

		for ( vchn = 0; vchn < PSG_VCH_MAX; vchn++ )
		{
			if ( PSGFX.chn[pchn].slot[vchn].ptr < 0 )
			{
				continue;
			}

			nvol=PSGFX.chn[pchn].slot[vchn].vol;

			if(nvol<mvol)
			{
				mvol=nvol;
				rchn=vchn;
			}
		}

		if(rchn>=0)
		{
			vchn=rchn;
			rchn=pchn<<5;
			*pb=0x80|0x10|rchn|PSGFX.chn[pchn].slot[vchn].vol;
			div=PSGFX.chn[pchn].slot[vchn].div;
			*pb=0x80|rchn|(div&0x0f);
			*pb=div>>4;
		}
	}
}


u16 sfxpsg_nb_tracks ()
{
	u16 i = 0;

	while ( _list[i++].data );

	return i - 2;
}


PSG *sfxpsg_get_track ( u16 num )
{
	return (PSG*) &_list[num];
}
