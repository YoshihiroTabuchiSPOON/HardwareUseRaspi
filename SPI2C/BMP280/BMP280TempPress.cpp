#include <stdio.h>
#include "BMP280.h"
#include "../../SPCommon/SPCommon.h"

using namespace SPCommon;
using namespace SPI2C;

CSPI2CBMP280TempPress::CSPI2CBMP280TempPress(uint8_t ui8Address)
{
	this->m_ui16DigT1 = this->m_ui16DigP1 = 0;
	this->m_i16DigT2 = this->m_i16DigT3 =
	this->m_i16DigP2 = this->m_i16DigP3 = this->m_i16DigP4 =
	this->m_i16DigP5 = this->m_i16DigP6 = this->m_i16DigP7 =
	this->m_i16DigP8 = this->m_i16DigP9 = 0;

	this->m_dblTemperature = -999.0;
	this->m_dblPressure = -9999.0;

	this->InitReceiveData();

	this->m_ui8DeviceAddress = ui8Address;
	this->m_bLoopAccess = true;
}

CSPI2CBMP280TempPress::~CSPI2CBMP280TempPress()
{
}

void CSPI2CBMP280TempPress::InitReceiveData()
{
	this->m_pTempPress = new CSPI2CDataRegisterReader( "TempPress", 0xf7 );
	this->m_pTempPress->ResetData( NULL, 6 );
	this->m_listDataProcess.AddElement( this->m_pTempPress );
}

bool CSPI2CBMP280TempPress::AfterProcess()
{
	uint8_t ui8DataLength;
	const uint8_t * pcui8Data = this->m_pTempPress->GetData( ui8DataLength );

	int32_t i32AbcP = ToInt32( 0x00
					, pcui8Data[0], pcui8Data[1], pcui8Data[2] ) >> 4;

	int32_t i32AbcT = ToInt32( 0x00
					, pcui8Data[3], pcui8Data[4], pcui8Data[5] ) >> 4;

	double dblTFine = 0.0;

	this->CalcTemprature( i32AbcT, dblTFine );
	this->CalcPressure( i32AbcP, dblTFine );

	return true;
}

//温度の計算
void CSPI2CBMP280TempPress::CalcTemprature( int32_t i32AbcT, double & dblTFine )
{
	double dblVar1 = ((double)i32AbcT / 16384.0 - (double)this->m_ui16DigT1 / 1024.0 )
						* (double)this->m_i16DigT2 ;
	double dblVar2 = (  ((double)i32AbcT / 131072.0 - (double)m_ui16DigT1 / 8192)
						* ((double)i32AbcT / 131072.0 - (double)this->m_ui16DigT1 / 8192.0)
						) * (double)this->m_i16DigT3;

	dblTFine = dblVar1 + dblVar2;

	this->m_dblTemperature = (dblVar1 + dblVar2) / 5120.0;
}

//気圧の計算
void CSPI2CBMP280TempPress::CalcPressure( int32_t i32AbcP, double dblTFine )
{
	double dblVar1 = dblTFine/2.0 - 64000.0;
	double dblVar2 = dblVar1 * dblVar1 * (double)this->m_i16DigP6 / 32768.0;
	dblVar2 = dblVar2 + dblVar1 * (double)this->m_i16DigP5 * 2.0;
	dblVar2 = (dblVar2 / 4.0) + (double)this->m_i16DigP4 * 65536.0;
	dblVar1 = ((double)this->m_i16DigP3 * dblVar1 * dblVar1 / 524288.0 + (double)this->m_i16DigP2 * dblVar1) / 524288.0;;
	dblVar1 = (1.0 + dblVar1 / 32768.0) * (double)this->m_ui16DigP1;
	double p = 1048576.0 - (double)i32AbcP;
	p = (p - dblVar2 / 4096.0) * 6250.0 / dblVar1;
	dblVar1 = (double)this->m_i16DigP9 * p * p / 2147483648.0;
	dblVar2 = p * (double)this->m_i16DigP8 / 32768.0;

	this->m_dblPressure = p + (dblVar1 + dblVar2 + (double)this->m_i16DigP7) / 16.0;
}
