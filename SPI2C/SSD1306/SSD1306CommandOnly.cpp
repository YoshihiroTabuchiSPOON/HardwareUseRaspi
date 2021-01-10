#include <string.h>
#include "SSD1306.h"

using namespace SPI2C;

CSSD1306CommandOnly::CSSD1306CommandOnly( ECommand command )
: CSPI2CDeviceAccessBase( 0x3c )
{
	char szCommand[32];
	switch( command )
	{
	case CommandDisplayOn:	strcpy( szCommand, "DisplayOn" );	break;
	case CommandDisplayOff:	strcpy( szCommand, "DisplayOff");	break;
	}

	CSPI2CDataCommandWriter * pWriter = new CSPI2CDataCommandWriter(szCommand);
	pWriter->ResetData( NULL, 2 );
	pWriter->SetData( 0, 0x00 );
	pWriter->SetData( 1, (uint8_t)command );

	this->m_listDataProcess.AddElement( pWriter );

}

CSSD1306CommandOnly::~CSSD1306CommandOnly()
{
}
