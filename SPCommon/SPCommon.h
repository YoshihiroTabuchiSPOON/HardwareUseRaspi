#ifndef _SPCOMMON_H_
#define _SPCOMMON_H_

#include <stdint.h>

namespace SPCommon
{
	void WaitMS( int iWaitMS );
	uint16_t ToUInt16( uint8_t ui8HiByte, uint8_t ui8LoByte );
	int16_t ToInt16( uint8_t ui8HiByte, uint8_t ui8LoByte );

	uint32_t ToUInt32( uint8_t ui81stByte, uint8_t ui82ndByte
						, uint8_t ui83rdByte, uint8_t ui84thByte );
	int32_t ToInt32( uint8_t ui81stByte, uint8_t ui82ndByte
						, uint8_t ui83rdByte, uint8_t ui84thByte );
}

#endif
