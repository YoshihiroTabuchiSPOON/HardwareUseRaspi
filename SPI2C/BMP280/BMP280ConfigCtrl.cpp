#include "BMP280.h"

using namespace SPI2C;

CSPI2CBMP280ConfigCtrl::CSPI2CBMP280ConfigCtrl(uint8_t ui8Address)
: CSPI2CDeviceAccessBase(ui8Address )
{
	//Setting Default is Handheld device dynamic
	this->m_ctrlOsrsT = OsrsT_x1;
	this->m_ctrlOsrsP = OsrsP_x4;
	this->m_ctrlMode = ModeNormal;
	this->m_configTSb = Tsb_0_5;
	this->m_configFilter = Filter_x16;
}

CSPI2CBMP280ConfigCtrl::~CSPI2CBMP280ConfigCtrl()
{
}

void CSPI2CBMP280ConfigCtrl::SetSender()
{
	CSPI2CDataElement * pConfigCtrl = new CSPI2CDataRegisterWriter( "ConfigCtrl", 0xf4 );
	pConfigCtrl->ResetData( NULL, 2 );
	pConfigCtrl->SetData( 0, (uint8_t)(this->m_ctrlOsrsT << 5 |
										    this->m_ctrlOsrsP << 2 |
											this->m_ctrlMode ) );
	pConfigCtrl->SetData( 1, (uint8_t)(this->m_configTSb << 5 |
											this->m_configFilter << 2 ) );

	this->m_listDataProcess.AddElement( pConfigCtrl );
}
