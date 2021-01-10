#include "SSD1306.h"

using namespace SPI2C;

CSSD1306HorzModeSet::CSSD1306HorzModeSet( uint8_t ui8ColumnStart
											  , uint8_t ui8ColumnEnd
											  , uint8_t ui8PageStart
											  , uint8_t ui8PageEnd )
: CSPI2CDeviceAccessBase( 0x3c )
{
	CSPI2CDataCommandWriter * pWriter = new CSPI2CDataCommandWriter( "HorzModeSet" );

	pWriter->ResetData( (const uint8_t *)
							"\x00\x20\x00"
							"\x00\x21\x00\x00"
							"\x00\x22\x00\x00", 11 );

	pWriter->SetData( 5, (uint8_t)ui8ColumnStart );
	pWriter->SetData( 6, (uint8_t)ui8ColumnEnd );
	pWriter->SetData( 9, (uint8_t)ui8PageStart );
	pWriter->SetData(10, (uint8_t)ui8PageEnd );

	this->m_listDataProcess.AddElement( pWriter );
}

CSSD1306HorzModeSet::~CSSD1306HorzModeSet()
{
}
