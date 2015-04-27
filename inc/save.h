#define SAVE_MAX_SCORE    7
#define SAVE_MAX_NAME     9+1
#define SAVE_NO_HS       -1

#define SAVE_INITIALIZED  37



typedef struct
{
   u8 initialized;

   struct st_scrore
   {
      u8  name [ SAVE_MAX_NAME ];
      u32 value;
   }
   score [ SAVE_MAX_SCORE ];
}
SAVE;



void save_init  ( );
void save_reset ( );
SAVE save_read  ( );
void save_save  ( SAVE save );

s8   save_highscore_pos  ( u32 score );
void save_highscore_save ( u8 pos, u32 score, u8 *texto );
