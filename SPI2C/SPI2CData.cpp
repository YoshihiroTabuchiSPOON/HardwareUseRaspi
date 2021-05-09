#include <string.h>
#include "SPI2C.h"

using namespace SPI2C;

CSPI2CData::CSPI2CData()
{
	this->m_pFirstElement = this->m_pLastElement = NULL;
}

CSPI2CData::~CSPI2CData()
{
	this->ReleaseAllElement();
}

void CSPI2CData::ReleaseAllElement()
{
	while( this->m_pFirstElement )
	{
		CSPI2CDataElement * pDelete = this->m_pFirstElement;
		this->m_pFirstElement = this->m_pFirstElement->m_pNextElement;
		delete pDelete;
	}
	this->m_pLastElement = NULL;
}

void CSPI2CData::AddElement( CSPI2CDataElement * pDataElement )
{
	if( this->m_pFirstElement )
	{
		this->m_pLastElement->m_pNextElement = pDataElement;
		this->m_pLastElement = pDataElement;
	}
	else
	{
		this->m_pFirstElement = this->m_pLastElement = pDataElement;
	}
}



//////////////////////////////////////////////////////////////////////////////////

CSPI2CDataElement::CSPI2CDataElement( const char * pcszAddressType )
{
	this->m_pNextElement = NULL;
	this->m_pui8Data = NULL;
	this->m_ui8DataLength = 0;
	this->m_ui8RegisterAddress = 0xff;

	size_t sizeLength = strlen( pcszAddressType );
	this->m_pszAddressType = new char[ sizeLength + 1 ];
	strcpy( this->m_pszAddressType, pcszAddressType );
}

CSPI2CDataElement::~CSPI2CDataElement()
{
	this->ReleaseData();

	delete[] this->m_pszAddressType;
}

void CSPI2CDataElement::ReleaseData()
{
	if( this->m_pui8Data )
	{
		delete[] this->m_pui8Data;
		this->m_pui8Data = NULL;
		this->m_ui8DataLength = 0;
	}
}

void CSPI2CDataElement::ResetData( const uint8_t * pcui8Data, uint8_t ui8DataLength )
{
	this->ReleaseData();

	this->m_ui8DataLength = ui8DataLength;
	this->m_pui8Data = new uint8_t[ this->m_ui8DataLength ];

	if( pcui8Data )
	{
		memcpy( this->m_pui8Data, pcui8Data, this->m_ui8DataLength );
	}
	else
	{
		memset( this->m_pui8Data, 0, this->m_ui8DataLength );
	}
}

void CSPI2CDataElement::SetData( uint8_t ui8DataOffset, const uint8_t * pcui8Data, uint8_t ui8DataLength )
{
	if( ui8DataOffset + ui8DataLength <= this->m_ui8DataLength )
	{
		memcpy( this->m_pui8Data + ui8DataOffset, pcui8Data, ui8DataLength );
	}
}

void CSPI2CDataElement::SetData( uint8_t ui8DataOffset, uint8_t ui8DataByte )
{
	if( ui8DataOffset < this->m_ui8DataLength )
	{
		*(this->m_pui8Data + ui8DataOffset) = ui8DataByte;
	}
}

uint8_t * CSPI2CDataElement::GetData( uint8_t & ui8DataLength )
{
	ui8DataLength = this->m_ui8DataLength;
	return this->m_pui8Data;
}
