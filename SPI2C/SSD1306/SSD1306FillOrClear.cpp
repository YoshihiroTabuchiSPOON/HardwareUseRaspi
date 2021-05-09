#include "SSD1306.h"
using namespace SPI2C;

CSSD1306FillOrClear::CSSD1306FillOrClear( bool bFill )
: CSPI2CDeviceAccessBase( 0x3c )
{
	int iDisplayWidth = 128;
	int iDisplayHeight = 64;

	CSPI2CDataCommandWriter * pWriter
	= new CSPI2CDataCommandWriter((bFill)?("FillDisplay"):("ClearDisplay"));

	//Horizontal Addressing Mode 0x00 0x20 0x00
	//Set Column Address         0x00 0x21 0x00 0x7f
	//Set Page Address           0x00 0x22 0x00 0x03

	pWriter->ResetData( (const uint8_t*)
							"\x00\x20\x00"
							"\x00\x21\x00\x00"
							"\x00\x22\x00\x00", 11 );
	pWriter->SetData(  6, (uint8_t)iDisplayWidth-1 );
	pWriter->SetData( 10, (uint8_t)iDisplayHeight / 8-1 );
	this->m_listDataProcess.AddElement( pWriter );

	int iPages = iDisplayHeight / 8;

	for( int iPage=0; iPage<iPages; iPage++ )
	{
		pWriter = new CSPI2CDataCommandWriter((bFill)?("Fill"):("Clear"));

		pWriter->ResetData( NULL, iDisplayWidth+1);

		pWriter->SetData( 0, 0x40 );
		uint8_t ui8Set = (bFill)?(0xff):(0x00);

		for( int i=1; i<iDisplayWidth+1; i++ )
		{
			pWriter->SetData( (uint8_t)i, ui8Set );
		}
		this->m_listDataProcess.AddElement( pWriter );
	}

}

CSSD1306FillOrClear::~CSSD1306FillOrClear()
{
}
