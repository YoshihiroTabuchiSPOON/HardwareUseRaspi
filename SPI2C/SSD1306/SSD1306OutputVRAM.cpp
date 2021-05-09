#include "SSD1306.h"

using namespace SPI2C;

CSSD1306OutputVRAM::CSSD1306OutputVRAM()
: CSPI2CDeviceAccessBase( 0x3c )
{

}

CSSD1306OutputVRAM::~CSSD1306OutputVRAM()
{
}

void CSSD1306OutputVRAM::SetVRAMData( const uint8_t * pcui8Data, uint8_t ui8DataLength )
{
	CSPI2CDataCommandWriter * pWriter = new CSPI2CDataCommandWriter("VRAM");

	pWriter->ResetData( NULL, ui8DataLength+1 );
	pWriter->SetData( 0, 0x40 );
	pWriter->SetData( (uint8_t)1, pcui8Data, ui8DataLength );

	this->m_listDataProcess.AddElement( pWriter );
}
