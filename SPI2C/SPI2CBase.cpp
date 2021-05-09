#include "SPI2C.h"

using namespace SPI2C;

CSPI2CDeviceAccessBase::CSPI2CDeviceAccessBase(uint8_t ui8DeviceAddress)
{
	this->m_pNextAccess = NULL;
	this->m_bLoopAccess = false;
	this->m_ui8DeviceAddress = ui8DeviceAddress;
	this->m_pManager = NULL;
}

CSPI2CDeviceAccessBase::~CSPI2CDeviceAccessBase()
{
}

bool CSPI2CDeviceAccessBase::AfterProcess()
{
	return true;
}
