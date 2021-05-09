#include <stdio.h>
#include "BMP280.h"
#include "../../SPCommon/SPCommon.h"

using namespace SPI2C;
using namespace SPCommon;

CSPI2CBMP280Calib::CSPI2CBMP280Calib(uint8_t ui8Address)
: CSPI2CDeviceAccessBase( ui8Address )
{
	this->m_ui8DeviceAddress = ui8Address;
	this->InitReceiveData();
}

CSPI2CBMP280Calib::~CSPI2CBMP280Calib()
{
}

void CSPI2CBMP280Calib::InitReceiveData()
{
	this->m_pCalibData = new CSPI2CDataRegisterReader("Calib", 0x88);
	this->m_pCalibData->ResetData( NULL, 24 );
	this->m_listDataProcess.AddElement( this->m_pCalibData );
}

bool CSPI2CBMP280Calib::AfterProcess()
{
	if( !this->m_pTempPress )
	{
		return false;
	}

	uint8_t ui8DataLength = 0;
	const uint8_t * pcui8Data = this->m_pCalibData->GetData( ui8DataLength );

	this->m_pTempPress->SetDigT1( ToUInt16( pcui8Data[1], pcui8Data[0] ) );
	this->m_pTempPress->SetDigT2( ToInt16( pcui8Data[3], pcui8Data[2] ) );
	this->m_pTempPress->SetDigT3( ToInt16( pcui8Data[5], pcui8Data[4] ) );

	this->m_pTempPress->SetDigP1( ToUInt16( pcui8Data[7], pcui8Data[6] ) );
	this->m_pTempPress->SetDigP2( ToInt16(  pcui8Data[9], pcui8Data[8] ) );
	this->m_pTempPress->SetDigP3( ToInt16( pcui8Data[11], pcui8Data[10] ) );
	this->m_pTempPress->SetDigP4( ToInt16( pcui8Data[13], pcui8Data[12] ) );
	this->m_pTempPress->SetDigP5( ToInt16( pcui8Data[15], pcui8Data[14] ) );
	this->m_pTempPress->SetDigP6( ToInt16( pcui8Data[17], pcui8Data[16] ) );
	this->m_pTempPress->SetDigP7( ToInt16( pcui8Data[19], pcui8Data[18] ) );
	this->m_pTempPress->SetDigP8( ToInt16( pcui8Data[21], pcui8Data[20] ) );
	this->m_pTempPress->SetDigP9( ToInt16( pcui8Data[23], pcui8Data[22] ) );

	return true;
}
