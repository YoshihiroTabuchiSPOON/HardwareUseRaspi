#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "../../SPI2C/BMP280/BMP280.h"

int main()
{
	SPI2C::CSPI2CManager i2cManager("/dev/i2c-1");

	//Control & Configuration
	SPI2C::CSPI2CBMP280ConfigCtrl * pConfigCtrl = new SPI2C::CSPI2CBMP280ConfigCtrl( 0x76 );
	pConfigCtrl->SetCtrlOsrsP(SPI2C::CSPI2CBMP280ConfigCtrl::OsrsP_x16);
	pConfigCtrl->SetSender();
	i2cManager.AddDeviceAccess( pConfigCtrl );

	SPI2C::CSPI2CBMP280TempPress * pTempPress = new SPI2C::CSPI2CBMP280TempPress( 0x76 );
	SPI2C::CSPI2CBMP280Calib * pCalib         = new SPI2C::CSPI2CBMP280Calib( 0x76 );
	pCalib->SetTempPressInstance( pTempPress );

	i2cManager.AddDeviceAccess( pCalib );
	i2cManager.AddDeviceAccess( pTempPress );

	pthread_t threadId;
	pthread_create( &threadId, NULL, SPI2C::CSPI2CManager::ThreadProc, (void*)&i2cManager );

	int iCounter = 0;
	while( 1 )
	{
		sleep( 1 );
		double dblTemp = pTempPress->GetTemperature();
		double dblPress = pTempPress->GetPressure() / 100.0;

		printf( "Temp: %.2fC      Press:%.5fHPa        \r"
				, dblTemp
				, dblPress );fflush(stdout);

		if( iCounter % 60 == 0 )
		{
			time_t timeNow = time(NULL);
			struct tm tmNow;
			localtime_r( &timeNow, &tmNow );

			char szName[64];
			snprintf( szName, 64, "TempPress%04d%02d%02d.csv"
					, tmNow.tm_year+1900, tmNow.tm_mon+1, tmNow.tm_mday );
			FILE * fp = fopen( szName, "a" );
			if( fp )
			{
				fprintf( fp, "%04d/%02d/%02d %02d:%02d:%02d,%.2f,%.4f\n"
						, tmNow.tm_year+1900, tmNow.tm_mon+1, tmNow.tm_mday
						, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec
						, dblTemp, dblPress );
				fclose( fp );
			}

			iCounter = 0;
		}
		iCounter++;
	}
	printf( "Ended\n" );
	pthread_join( threadId, NULL );

	return 9;
}
