#ifndef _JOY_INPUT_H_
#define _JOY_INPUT_H_

   //== PROTOTYPES
   void _JOYint       ( u8 unbounded );
   void _JOYreset     ( void );
   void _JOYsetXY     ( s16 x, s16 y );

   s16  _JOYgetPosX   ( void );
   s16  _JOYgetPosY   ( void );

   void _JOYupdate    ( void );
   u16  _JOYclick     ( void );
   void _JOYwaitClick ( void );
   void _JOYsetGameMode (u8 flag);

#endif
