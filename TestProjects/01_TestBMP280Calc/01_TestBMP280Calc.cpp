#include <stdio.h>
#include "../../SPI2C/BMP280/BMP280.h"

using namespace SPI2C;

int main()
{
	CSPI2CBMP280TempPress tempPress(0x76);
	tempPress.SetDigT1( 27504 );
	tempPress.SetDigT2( 26435 );
	tempPress.SetDigT3( -1000 );

	tempPress.SetDigP1( 36477 );
	tempPress.SetDigP2( -10685 );
	tempPress.SetDigP3( 3024 );
	tempPress.SetDigP4( 2855 );
	tempPress.SetDigP5( 140 );
	tempPress.SetDigP6( -7 );
	tempPress.SetDigP7( 15500 );
	tempPress.SetDigP8( -14600 );
	tempPress.SetDigP9( 6000 );

	double dblTFine = 0.0;
	tempPress.CalcTemprature( 282804, dblTFine );
	tempPress.CalcPressure( 475520, dblTFine );

	printf( "Temp: %.3f    Press: %.3f\n", tempPress.GetTemperature()
											   , tempPress.GetPressure() );

	return 0;
}
