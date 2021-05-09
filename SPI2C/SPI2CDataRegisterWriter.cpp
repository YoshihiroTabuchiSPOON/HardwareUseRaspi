#include <string.h>
#include "SPI2C.h"

using namespace SPI2C;

CSPI2CDataRegisterWriter::CSPI2CDataRegisterWriter( const char * pcszAddressType, uint8_t ui8RegisterAddress )
: CSPI2CDataElement( pcszAddressType )
{
	this->m_ui8RegisterAddress = ui8RegisterAddress;
}

CSPI2CDataRegisterWriter::~CSPI2CDataRegisterWriter()
{
}

void CSPI2CDataRegisterWriter::ResetData( const uint8_t * pcui8Data, uint8_t ui8DataLength )
{
	this->ReleaseData();

	this->m_ui8DataLength = ui8DataLength + 1;
	this->m_pui8Data = new uint8_t[ this->m_ui8DataLength ];

	if( pcui8Data )
	{
		memcpy( this->m_pui8Data+1, pcui8Data, ui8DataLength );
	}
	else
	{
		memset( this->m_pui8Data+1, 0, ui8DataLength );
	}
	*(this->m_pui8Data) = this->m_ui8RegisterAddress;
}

void CSPI2CDataRegisterWriter::SetData( uint8_t ui8DataOffset, const uint8_t * pcui8Data, uint8_t ui8DataLength )
{
	if( ui8DataOffset + ui8DataLength + 1 < this->m_ui8DataLength )
	{
		memcpy( this->m_pui8Data + ui8DataOffset + 1, pcui8Data, ui8DataLength );
	}
}

void CSPI2CDataRegisterWriter::SetData( uint8_t ui8DataOffset, uint8_t ui8DataByte )
{
	if( ui8DataOffset + 1 < this->m_ui8DataLength )
	{
		*(this->m_pui8Data + ui8DataOffset + 1) = ui8DataByte;
	}
}
