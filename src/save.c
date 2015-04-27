#include <genesis.h>
#include <sram.h>

#include "../inc/save.h"



const SAVE _default =
{
   SAVE_INITIALIZED,
   {
      { "KANEDA   ", 125000 },
      { "CHILLY   ", 105000 },
      { "BUB      ",  89000 },
      { "BILLY LEE",  65000 },
      { "FALETE   ",  48000 },
      { "NOLDDOR  ",  35000 },
      { "BONGO    ",  15000 }
   }
};




void save_init ( )
{
   const SAVE save = save_read ( );

   if ( save.initialized != SAVE_INITIALIZED )
   {
      save_reset();
   }
}


void save_reset ( )
{
   save_save ( _default );
}


SAVE save_read ( )
{
   u8  i, j;
   u32 pos = 0;

   SAVE save;

   SRAM_enableRO();

   save.initialized = SRAM_readByte ( pos );
   pos += sizeof ( u8 ) ;

   for ( i=0; i<SAVE_MAX_SCORE; i++ )
   {
      for ( j=0; j<SAVE_MAX_NAME; j++ )
      {
         save.score[i].name[j] = SRAM_readByte ( pos );
         pos += sizeof ( u8 ) ;
      }

      save.score[i].value = SRAM_readLong ( pos );
      pos += sizeof ( u32 ) ;
   }

   SRAM_disable();

   return save;
}


void save_save ( SAVE save )
{
   u8  i, j;
   u32 pos = 0;

   SRAM_enable();

   SRAM_writeByte ( pos, save.initialized );
   pos += sizeof ( u8 ) ;

   for ( i=0; i<SAVE_MAX_SCORE; i++ )
   {
      for ( j=0; j<SAVE_MAX_NAME; j++ )
      {
         SRAM_writeByte ( pos, save.score[i].name[j] );
         pos += sizeof ( u8 ) ;
      }

      SRAM_writeLong ( pos, save.score[i].value );
      pos += sizeof ( u32 ) ;
   }

   SRAM_disable();
}


s8 save_highscore_pos ( u32 score )
{
   const SAVE save = save_read();

   u8 i;

   for ( i=0; i<SAVE_MAX_SCORE; i++ )
   {
      if ( score > save.score[i].value )
      {
         return i;
      }
   }

   return SAVE_NO_HS;
}



void save_highscore_save ( u8 pos, u32 score, u8 *texto )
{
   SAVE save = save_read();

   u8 i = 0;
   u8 j = 0;

   for ( i=SAVE_MAX_SCORE-1; i > pos; i-- )
   {
      for ( j=0; j<SAVE_MAX_NAME; j++ )
      {
         save.score[i].name[j] =  save.score[i-1].name[j];
      }

      save.score[i].value = save.score[i-1].value;
   }

   memset ( save.score[pos].name, 0, sizeof (u8) * SAVE_MAX_NAME );

   strcpy ( save.score[pos].name, texto );
   save.score[pos].value = score;

   save_save ( save );
}

