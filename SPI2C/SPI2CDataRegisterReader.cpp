#include "SPI2C.h"

using namespace SPI2C;

CSPI2CDataRegisterReader::CSPI2CDataRegisterReader( const char * pcszAddressType, uint8_t ui8RegisterAddress )
: CSPI2CDataElement( pcszAddressType )
{
	this->m_ui8RegisterAddress = ui8RegisterAddress;
}

CSPI2CDataRegisterReader::~CSPI2CDataRegisterReader()
{
}
