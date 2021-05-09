#include "SSD1306.h"

using namespace SPI2C;

CSSD1306Contrast::CSSD1306Contrast( uint8_t ui8Contrast )
:CSPI2CDeviceAccessBase( 0x3c )
{
	CSPI2CDataCommandWriter * pWriter = new CSPI2CDataCommandWriter("Contrast");

	pWriter->ResetData( (const uint8_t*)"\x00\x81\x00", 3 );
	pWriter->SetData( (uint8_t)2, ui8Contrast );

	this->m_listDataProcess.AddElement( pWriter );
}

CSSD1306Contrast::~CSSD1306Contrast()
{
}
