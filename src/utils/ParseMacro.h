/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _PARSEMACRO_H_
#define _PARSEMACRO_H_

#define GET_UINT32(p)  (unsigned int)(((unsigned char)p[3] << 24) | ((unsigned char)p[2] << 16) | ((unsigned char)p[1] << 8) | ((unsigned char)p[0]))
#define GET_UINT24(p)  (unsigned int)(((unsigned char)p[2] << 24) | ((unsigned char)p[1] << 16) | ((unsigned char)p[0] << 8))
#define GET_UINT16(p)  (short int)(((unsigned char)p[1] << 8) | ((unsigned char)p[0]))
#define GET_UINT8(p)   (unsigned char)((unsigned char)p[0])


#endif //_PARSEMACRO_H_
