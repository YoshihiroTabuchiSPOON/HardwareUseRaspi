#ifndef _SPI2C_H_
#define _SPI2C_H_

#include <pthread.h>
#include <stdint.h>

namespace SPI2C
{

class CSPI2CData
{
public:
	CSPI2CData();
	virtual ~CSPI2CData();

	void AddElement( class CSPI2CDataElement * pDataElement );
	inline class CSPI2CDataElement * GetFirstElement()
	{
		return this->m_pFirstElement;
	}
	void ReleaseAllElement();

protected:
	class CSPI2CDataElement * m_pFirstElement;
	class CSPI2CDataElement * m_pLastElement;
};

class CSPI2CDataElement
{
	friend class CSPI2CData;
public:
	enum EReadWriteType
	{
		TypeRead=0,
		TypeWrite=1,
	};
	CSPI2CDataElement(const char * pcszAddressType);
	virtual ~CSPI2CDataElement();

	virtual void ResetData( const uint8_t * pcui8Data, uint8_t ui8DatLength );
	virtual void SetData( uint8_t ui8DataOffset, const uint8_t * pcui8Data, uint8_t ui8DataLength );
	virtual void SetData( uint8_t ui8DataOffset, uint8_t ui8DataByte );

	uint8_t * GetData( uint8_t & ui8DataLength );
	virtual EReadWriteType GetReadWriteType() = 0;

	virtual uint8_t GetReadAddrss()
	{
		return this->m_ui8RegisterAddress;
	}

	inline CSPI2CDataElement * GetNextElement()
	{
		return this->m_pNextElement;
	}
	inline uint8_t GetRegisterAddress()
	{
		return this->m_ui8RegisterAddress;
	}

	virtual int GetAfterProcessWaitMS()
	{
		return 0;
	}

	const char * GetAddressType()
	{
		return this->m_pszAddressType;
	}

protected:
	void ReleaseData();

	CSPI2CDataElement * m_pNextElement;
	uint8_t * m_pui8Data;
	uint8_t m_ui8DataLength;

	uint8_t m_ui8RegisterAddress;
	char * m_pszAddressType;
};

class CSPI2CDataRegisterWriter : public CSPI2CDataElement
{
public:
	CSPI2CDataRegisterWriter( const char * pcszAddressType, uint8_t ui8RegisterAddress );
	virtual ~CSPI2CDataRegisterWriter();

	virtual void ResetData( const uint8_t * pui8Data, uint8_t ui8DataLength );
	virtual void SetData( uint8_t ui8DataOffset, const uint8_t * pcui8Data, uint8_t ui8DataLength );
	virtual void SetData( uint8_t ui8DataOffset, uint8_t ui8DataByte );

	virtual EReadWriteType GetReadWriteType()
	{
		return CSPI2CDataElement::TypeWrite;
	}

protected:
};

class CSPI2CDataCommandWriter : public CSPI2CDataElement
{
public:
	CSPI2CDataCommandWriter( const char * pcszAddressType )
	: CSPI2CDataElement( pcszAddressType ){}
	virtual ~CSPI2CDataCommandWriter(){}

	virtual EReadWriteType GetReadWriteType()
	{
		return CSPI2CDataElement::TypeWrite;
	}
};

class CSPI2CDataRegisterReader : public CSPI2CDataElement
{
public:
	CSPI2CDataRegisterReader( const char * pcszAddressType, uint8_t ui8RegisterAddress );
	virtual ~CSPI2CDataRegisterReader();

	virtual EReadWriteType GetReadWriteType()
	{
		return CSPI2CDataElement::TypeRead;
	}
protected:
};

class CSPI2CManager
{
public:
	static void * ThreadProc( void * pVoidParameter );

	CSPI2CManager(const char * pcszI2CName);
	virtual ~CSPI2CManager();

	void AddDeviceAccess( class CSPI2CDeviceAccessBase * pDevice );
	inline void StopThread(bool bForced = false)
	{
		this->m_bStopLoop = true;

		if( bForced )
		{
			this->ReleaseAllDevice();
		}
	}
	inline bool IsThreadStopped()
	{
		return this->m_bStopLoop;
	}

protected:
	void ReleaseAllDevice();
	class CSPI2CDeviceAccessBase * CheckoutAccess();

	class CSPI2CDeviceAccessBase * m_pFirstAccess;
	class CSPI2CDeviceAccessBase * m_pLastAccess;

	bool CommunicationLoop();

	char * m_pszI2CName;
	bool m_bStopLoop;
	int m_iFD;
	uint8_t m_ui8NowDeviceAddress;

	int m_iWaitMSNoAccessBuffer;

	pthread_mutex_t m_mutex;

	bool WriteI2C( uint8_t ui8DeviceAddress, CSPI2CDataElement * pDataElement );
	bool ReadI2C( uint8_t ui8DeviceAddress, CSPI2CDataElement * pDataElement );
};

class CSPI2CDeviceAccessBase
{
	friend class CSPI2CManager;
public:
	CSPI2CDeviceAccessBase(uint8_t ui8DeviceAddress);
	virtual ~CSPI2CDeviceAccessBase();

	inline uint8_t GetDeviceAddress()
	{
		return this->m_ui8DeviceAddress;
	}

	inline bool GetLoopAccess()
	{
		return this->m_bLoopAccess;
	}

	virtual bool AfterProcess();

protected:
	CSPI2CDeviceAccessBase * m_pNextAccess;

	CSPI2CData m_listDataProcess;
	CSPI2CManager * m_pManager;

	bool m_bLoopAccess;

	uint8_t m_ui8DeviceAddress;
};

}

#endif
