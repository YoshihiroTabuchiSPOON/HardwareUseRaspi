#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>

#include "SPI2C.h"
#include "../SPCommon/SPCommon.h"
#include "../SPCommon/OutputLog.h"

//#define	_OUTPUTI2CLOG

using namespace SPI2C;

void * CSPI2CManager::ThreadProc( void * pVoidParameter )
{
	CSPI2CManager * pI2CManager = (CSPI2CManager*)pVoidParameter;
	pI2CManager->CommunicationLoop();

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

CSPI2CManager::CSPI2CManager(const char * pcszI2CName)
{
	this->m_pFirstAccess = this->m_pLastAccess = NULL;
	int iLength = strlen( pcszI2CName );
	this->m_pszI2CName = new char[ iLength ];
	strcpy( this->m_pszI2CName, pcszI2CName );

	this->m_bStopLoop = false;
	this->m_iFD = -1;
	this->m_ui8NowDeviceAddress = 0xff;

	this->m_iWaitMSNoAccessBuffer = 100;

	pthread_mutex_init( &this->m_mutex, NULL );
}

CSPI2CManager::~CSPI2CManager()
{
	if( this->m_iFD >= 0 )
	{
		close( this->m_iFD );
	}

	this->ReleaseAllDevice();
	delete[] this->m_pszI2CName;
	pthread_mutex_destroy( &this->m_mutex );
}

void CSPI2CManager::ReleaseAllDevice()
{
	pthread_mutex_lock( &this->m_mutex );

	while( this->m_pFirstAccess )
	{
		CSPI2CDeviceAccessBase * pDelete = this->m_pFirstAccess;
		this->m_pFirstAccess = this->m_pFirstAccess->m_pNextAccess;
		delete pDelete;
	}
	this->m_pLastAccess = NULL;

	pthread_mutex_unlock( &this->m_mutex );
}

void CSPI2CManager::AddDeviceAccess( CSPI2CDeviceAccessBase * pAccess )
{
	pthread_mutex_lock( &this->m_mutex );

	if( !this->m_bStopLoop )
	{
		if( this->m_pFirstAccess )
		{
			this->m_pLastAccess->m_pNextAccess = pAccess;
			this->m_pLastAccess = pAccess;
		}
		else
		{
			this->m_pFirstAccess = this->m_pLastAccess = pAccess;
		}
		pAccess->m_pManager = this;
	}
	else
	{
		delete pAccess;
	}

	pthread_mutex_unlock( &this->m_mutex );
}

CSPI2CDeviceAccessBase * CSPI2CManager::CheckoutAccess()
{
	pthread_mutex_lock( &this->m_mutex );

	CSPI2CDeviceAccessBase * pCheckout = this->m_pFirstAccess;
	if( pCheckout )
	{
		this->m_pFirstAccess = this->m_pFirstAccess->m_pNextAccess;
		if( !this->m_pFirstAccess )
		{
			this->m_pLastAccess = NULL;
		}

		pCheckout->m_pNextAccess = NULL;
	}

	pthread_mutex_unlock( &this->m_mutex );

	return pCheckout;
}

bool CSPI2CManager::CommunicationLoop()
{
	this->m_iFD = open( this->m_pszI2CName, O_RDWR );
	if( this->m_iFD < 0 )
	{
		return false;
	}
#ifdef _OUTPUTI2CLOG
	{
		char szMessage[256];
		snprintf( szMessage, 256, "[I2C][.]I2C opened( fd= %d )", this->m_iFD );
		SPCommon::OutputLog( szMessage );
	}
#endif
	while( 1 )
	{
		int iWaitAfter = 0;

		CSPI2CDeviceAccessBase * pAccess = this->CheckoutAccess();
		if( pAccess )
		{
			bool bSuccess = true;

			for( CSPI2CDataElement * pDataElement = pAccess->m_listDataProcess.GetFirstElement()
					; pDataElement; pDataElement = pDataElement->GetNextElement() )
			{
				if( pDataElement->GetReadWriteType() == CSPI2CDataElement::TypeRead )
				{
					if( !this->ReadI2C( pAccess->GetDeviceAddress()
										 , pDataElement ) )
					{
						bSuccess = false;
						break;
					}
				}
				else
				{
					if( !WriteI2C( pAccess->GetDeviceAddress()
									,pDataElement ) )
					{
						bSuccess = false;
						break;
					}
				}

				if( pDataElement->GetAfterProcessWaitMS() )
				{
					SPCommon::WaitMS( pDataElement->GetAfterProcessWaitMS() );
				}
			}

			if( !bSuccess )
			{
				break;
			}

			pAccess->AfterProcess();

			if( pAccess->GetLoopAccess() )
			{
				this->AddDeviceAccess( pAccess );
			}
			else
			{
				delete pAccess;
			}
		}
		else if( this->m_bStopLoop )
		{
			break;
		}
		else
		{
			iWaitAfter = this->m_iWaitMSNoAccessBuffer;
		}

		SPCommon::WaitMS( iWaitAfter );
	}

	close( this->m_iFD );
	this->m_iFD = -1;

	return true;
}

bool CSPI2CManager::WriteI2C( uint8_t ui8DeviceAddress, CSPI2CDataElement * pDataElement )
{
	bool bRet = true;

	uint8_t ui8DataLength;
	uint8_t * pui8Data = pDataElement->GetData( ui8DataLength );

	i2c_msg i2cMessage = { ui8DeviceAddress, 0, ui8DataLength, pui8Data };
	i2c_rdwr_ioctl_data ioctlData = { &i2cMessage, 1 };

#ifdef _OUTPUTI2CLOG
	char szLog[1024];
	{
		snprintf( szLog, 1024, "[I2C][W][DEV:%02X][REG:%02X]<=["
				, ui8DeviceAddress, pDataElement->GetRegisterAddress() );

		char szTemp[16];
		for( int i=0; i<(int)ui8DataLength; i++ )
		{
			snprintf( szTemp, 16, "%02X ", pui8Data[i] );
			strcat( szLog, szTemp );
		}

		szLog[ strlen(szLog)-1 ] = ']';
	}
#endif

	if( ioctl( this->m_iFD, I2C_RDWR, &ioctlData ) != 1 )
	{
		bRet = false;

#ifdef _OUTPUTI2CLOG
		{
			char szTemp[32];
			snprintf( szTemp, 32, "NG(%d)", errno );
			strcat( szLog, szTemp );
		}

	}
	SPCommon::OutputLog( szLog );
#else
	}
#endif

	return bRet;
}

bool CSPI2CManager::ReadI2C( uint8_t ui8DeviceAddress, CSPI2CDataElement * pDataElement )
{
	bool bRet = true;

	uint8_t ui8RegisterAddress = pDataElement->GetRegisterAddress();
	uint8_t ui8DataLength;
	uint8_t * pui8Data = pDataElement->GetData( ui8DataLength );

	i2c_msg i2cMessage[] = {
			{ ui8DeviceAddress, 0, 1, &ui8RegisterAddress},
			{ ui8DeviceAddress, I2C_M_RD, ui8DataLength, pui8Data }
	};
	i2c_rdwr_ioctl_data ioctlData = { i2cMessage, 2 };

#ifdef _OUTPUTI2CLOG
	char szLog[1024];
	snprintf( szLog, 1024, "[I2C][R][DEV:%02X][REG:%02X][Length:%u]"
			, ui8DeviceAddress
			, pDataElement->GetRegisterAddress()
			, ui8DataLength );
#endif

	if( ioctl( this->m_iFD, I2C_RDWR, &ioctlData ) != 2 )
	{
		bRet = false;
#ifdef _OUTPUTI2CLOG
		strcat( szLog, "NG" );
	}
	else
	{
		strcat( szLog, "=>[" );
		for( int i=0; i<(int)ui8DataLength; i++ )
		{
			char szTemp[16];
			snprintf( szTemp, 16, "%02X ", pui8Data[i] );
			strcat( szLog, szTemp );
		}
		szLog[ strlen(szLog)-1 ] = ']';
	}

	SPCommon::OutputLog( szLog );
#else
	}
#endif

	return bRet;
}


