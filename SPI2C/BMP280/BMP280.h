#ifndef _BMP280_H_
#define _BMP280_H_

#include "../SPI2C.h"

namespace SPI2C
{
class CSPI2CBMP280ConfigCtrl : public CSPI2CDeviceAccessBase
{
public:
	enum ECtrlOsrsT
	{
		OsrsT_Skipped = 0x00,
		OsrsT_x1 = 0x01,
		OsrsT_x2 = 0x02,
		OsrsT_x4 = 0x03,
		OsrsT_x8 = 0x04,
		OsrsT_x16 = 0x05
	};
	enum ECtrlOsrsP
	{
		OsrsP_Skipped = 0x00,
		OsrsP_x1 = 0x01,
		OsrsP_x2 = 0x02,
		OsrsP_x4 = 0x03,
		OsrsP_x8 = 0x04,
		OsrsP_x16 = 0x05
	};
	enum ECtrlMode
	{
		ModeSleep = 0x00,
		ModeForced = 0x01,
		ModeNormal = 0x03,
	};

	enum EConfigTSb
	{
		Tsb_0_5 = 0x00,
		Tsb_62_5 = 0x01,
		Tsb_125 = 0x02,
		Tsb_250 = 0x03,
		Tsb_500 = 0x04,
		Tsb_1000 = 0x05,
		Tsb_2000 = 0x06,
		Tsb_4000 = 0x07,
	};
	enum EConfigFilter
	{
		Filter_None = 0x00,
		Filter_x1 = 0x01,
		Filter_x2 = 0x02,
		Filter_x4 = 0x03,
		Filter_x8 = 0x04,
		Filter_x16 = 0x05
	};

	CSPI2CBMP280ConfigCtrl(uint8_t ui8Address);
	virtual ~CSPI2CBMP280ConfigCtrl();

	inline void SetCtrlOsrsT( ECtrlOsrsT ctrlOsrsT )
	{
		this->m_ctrlOsrsT = ctrlOsrsT;
	}
	inline void SetCtrlOsrsP( ECtrlOsrsP ctrlOsrsP )
	{
		this->m_ctrlOsrsP = ctrlOsrsP;
	}
	inline void SetCtrlMode( ECtrlMode ctrlMode )
	{
		this->m_ctrlMode = ctrlMode;
	}
	inline void SetConfigTsb( EConfigTSb configTSb )
	{
		this->m_configTSb = configTSb;
	}
	inline void SetConfigFilter( EConfigFilter configFilter )
	{
		this->m_configFilter = configFilter;
	}

	void SetSender();

protected:
	ECtrlOsrsT m_ctrlOsrsT;
	ECtrlOsrsP m_ctrlOsrsP;
	ECtrlMode  m_ctrlMode;
	EConfigTSb m_configTSb;
	EConfigFilter m_configFilter;
};

class CSPI2CBMP280Calib : public CSPI2CDeviceAccessBase
{
public:
	CSPI2CBMP280Calib(uint8_t ui8Address);
	virtual ~CSPI2CBMP280Calib();
	virtual bool AfterProcess();

	inline void SetTempPressInstance( class CSPI2CBMP280TempPress * pTempPress )
	{
		this->m_pTempPress = pTempPress;
	}

protected:
	void InitReceiveData();
	CSPI2CDataElement * m_pCalibData;
	class CSPI2CBMP280TempPress * m_pTempPress;
};

class CSPI2CBMP280TempPress : public CSPI2CDeviceAccessBase
{
public:
	CSPI2CBMP280TempPress(uint8_t ui8Address);
	virtual ~CSPI2CBMP280TempPress();

	inline void SetDigT1( uint16_t ui16DigT1 )
	{
		this->m_ui16DigT1 = ui16DigT1;
	}
	inline void SetDigT2( int16_t i16DigT2 )
	{
		this->m_i16DigT2 = i16DigT2;
	}
	inline void SetDigT3( int16_t i16DigT3 )
	{
		this->m_i16DigT3 = i16DigT3;
	}
	inline void SetDigP1( uint16_t ui16DigP1 )
	{
		this->m_ui16DigP1 = ui16DigP1;
	}
	inline void SetDigP2( int16_t i16DigP2 )
	{
		this->m_i16DigP2 = i16DigP2;
	}
	inline void SetDigP3( int16_t i16DigP3 )
	{
		this->m_i16DigP3 = i16DigP3;
	}
	inline void SetDigP4( int16_t i16DigP4 )
	{
		this->m_i16DigP4 = i16DigP4;
	}
	inline void SetDigP5( int16_t i16DigP5 )
	{
		this->m_i16DigP5 = i16DigP5;
	}
	inline void SetDigP6( int16_t i16DigP6 )
	{
		this->m_i16DigP6 = i16DigP6;
	}
	inline void SetDigP7( int16_t i16DigP7 )
	{
		this->m_i16DigP7 = i16DigP7;
	}
	inline void SetDigP8( int16_t i16DigP8 )
	{
		this->m_i16DigP8 = i16DigP8;
	}
	inline void SetDigP9( int16_t i16DigP9 )
	{
		this->m_i16DigP9 = i16DigP9;
	}

	virtual bool AfterProcess();
	void CalcTemprature( int32_t i32AbcT, double & dblTFine );
	void CalcPressure( int32_t i32AbcP, double dblTFine );

	inline double GetTemperature()
	{
		return this->m_dblTemperature;
	}
	inline double GetPressure()
	{
		return this->m_dblPressure;
	}

protected:
	uint16_t m_ui16DigT1;
	int16_t m_i16DigT2;
	int16_t m_i16DigT3;

	uint16_t m_ui16DigP1;
	int16_t m_i16DigP2;
	int16_t m_i16DigP3;
	int16_t m_i16DigP4;
	int16_t m_i16DigP5;
	int16_t m_i16DigP6;
	int16_t m_i16DigP7;
	int16_t m_i16DigP8;
	int16_t m_i16DigP9;

	void InitReceiveData();

	CSPI2CDataElement * m_pTempPress;

	double m_dblTemperature;
	double m_dblPressure;

};
}

#endif
