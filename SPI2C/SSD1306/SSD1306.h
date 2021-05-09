#ifndef _SSD1306_H_
#define _SSD1306_H_

#include "../SPI2C.h"

namespace SPI2C
{
class CSSD1306CommandOnly : public CSPI2CDeviceAccessBase
{
public:
	enum ECommand
	{
		CommandDisplayOn = 0xaf,
		CommandDisplayOff = 0xae,
	};
	CSSD1306CommandOnly(ECommand command);
	virtual ~CSSD1306CommandOnly();

protected:
};

class CSSD1306Contrast : public CSPI2CDeviceAccessBase
{
public:
	CSSD1306Contrast(uint8_t ui8Contrast);
	virtual ~CSSD1306Contrast();

protected:
};

class CSSD1306HorzModeSet : public CSPI2CDeviceAccessBase
{
public:
	CSSD1306HorzModeSet( uint8_t ui8ColumnStart, uint8_t ui8ColumnEnd
						  , uint8_t ui8PageStart, uint8_t ui8PageEnd );
	~CSSD1306HorzModeSet();
};

class CSSD1306OutputVRAM : public CSPI2CDeviceAccessBase
{
public:
	CSSD1306OutputVRAM();
	virtual ~CSSD1306OutputVRAM();

	void SetVRAMData( const uint8_t * pui8Data, uint8_t ui8DataLength );

protected:
};

class CSSD1306FillOrClear : public CSPI2CDeviceAccessBase
{
public:
	CSSD1306FillOrClear(bool bFill);
	virtual ~CSSD1306FillOrClear();

protected:
};

}

#endif

