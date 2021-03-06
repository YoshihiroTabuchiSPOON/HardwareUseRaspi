#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SPCommon.h"


#ifndef	NULL
#define	NULL	(0)
#endif

void SPCommon::WaitMS( int iWaitMS )
{
	if( iWaitMS <= 0 )
	{
		return;
	}

	timespec timeWait;
	timeWait.tv_sec = (time_t)(iWaitMS / 1000);
	timeWait.tv_nsec = (iWaitMS % 1000) * 1000 * 1000;

	nanosleep( &timeWait, NULL );
}

uint16_t SPCommon::ToUInt16( uint8_t ui8HiByte, uint8_t ui8LoByte )
{
	return (uint16_t)( (ui8HiByte << 8 ) | (ui8LoByte) );
}
int16_t SPCommon::ToInt16( uint8_t ui8HiByte, uint8_t ui8LoByte )
{
	return (int16_t)( (ui8HiByte << 8 ) | (ui8LoByte ) );
}

uint32_t SPCommon::ToUInt32( uint8_t ui81stByte, uint8_t ui82ndByte
							   , uint8_t ui83rdByte, uint8_t ui84thByte )
{
	return (uint32_t)( (ui81stByte << 24) | (ui82ndByte << 16) |
						  (ui83rdByte << 8 ) | (ui84thByte) );
}
int32_t SPCommon::ToInt32( uint8_t ui81stByte, uint8_t ui82ndByte
					, uint8_t ui83rdByte, uint8_t ui84thByte )
{
	return (int32_t)( (ui81stByte << 24) | (ui82ndByte << 16) |
						 (ui83rdByte << 8 ) | (ui84thByte) );
}

uint16_t SPCommon::Get10Or16DigitsValue( const char * pcsz10Pr16DigitsValue )
{
	uint16_t ui16Charcode;

	if( strncmp( pcsz10Pr16DigitsValue, "0x", 2 ) == 0 )
	{
		ui16Charcode = (uint16_t)strtoul( pcsz10Pr16DigitsValue+2, NULL, 16 );
	}
	else
	{
		ui16Charcode = (uint16_t)strtoul( pcsz10Pr16DigitsValue, NULL, 10 );
	}

	return ui16Charcode;
}
