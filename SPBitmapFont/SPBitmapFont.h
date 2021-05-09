#ifndef _SPBITMAPFONT_H_
#define _SPBITMAPFONT_H_

#include <stdio.h>
#include <stdint.h>

namespace SPBitmapFont
{
class CSPBitmapFont
{
public:
	CSPBitmapFont();
	virtual ~CSPBitmapFont();

	void SetFontName( const char * pcszFontName );
	class CSPBitmapElement * GetBitmapData( wchar_t wcChar );
	inline const char * GetFontName()
	{
		return this->m_pszFontName;
	}

protected:

	bool GetCachedFontIndex( wchar_t wcChar, int & iHitOrPrevIndex );
	class CSPBitmapElement * GetFontFromFile( wchar_t wcChar );

	void ReleaseCachedBitmap();
	void ReleaseSortedCachedBitmap();
	void ReleaseFontName();

	void AddElement( CSPBitmapElement * pElement );

	CSPBitmapFont * m_pNextBitmapFont;
	class CSPBitmapElement * m_pFirstCachedBitmap;
	class CSPBitmapElement * m_pLastCachedBitmap;
	int m_iCachedNumber;

	class CSPBitmapElement ** m_ppSortedCachedBitmap;
	int m_iSortedCachedNumber;
	int m_iSortedCachedAllocatedNumber;
	int m_iAllocateStep;

	char * m_pszFontName;
};

class CSPBitmapElement
{
	friend class CSPBitmapFont;
	friend class CSPBitmapSelect;
public:
	CSPBitmapElement();
	virtual ~CSPBitmapElement();

	void SetBitmapSize( uint8_t ui8Width, uint8_t ui8Height );

	void SetBitmapDataText( uint8_t ui8Line, const char * pcszBitmapData );
	inline uint8_t * GetBitmapData( uint8_t & ui8Width, uint8_t & ui8Height )
	{
		ui8Width = this->m_ui8Width;
		ui8Height = this->m_ui8Height;
		return this->m_pBitmap;
	}

	inline uint8_t GetWidth()
	{
		return this->m_ui8Width;
	}
	inline uint8_t GetHeight()
	{
		return this->m_ui8Height;
	}

	bool WriteToBinFile( FILE * fpOutput );
	bool ReadFromBinFile( FILE * fpInput );

	inline void SetChar( wchar_t wcChar )
	{
		this->m_wcChar = wcChar;
	}
	inline wchar_t GetChar( )
	{
		return this->m_wcChar;
	}

	inline CSPBitmapElement * GetNextElement()
	{
		return this->m_pNextBitmap;
	}

	CSPBitmapElement * Clone();
	inline bool GetPoint( uint8_t ui8XPos, uint8_t ui8YPos )
	{
		if( ui8XPos >= this->m_ui8Width ||
			 ui8YPos >= this->m_ui8Height )
		{
			return false;
		}

		uint16_t ui16Offset = ui8YPos * ( (this->m_ui8Width - 1) / 8 + 1 )
								+ ui8XPos / 8;

		uint8_t ui8Data = this->m_pBitmap[ ui16Offset ];

		uint8_t ui8Shift = 7 - (ui8XPos % 8);

		ui8Data = ui8Data >> ui8Shift;

		return ( (ui8Data & 0x01) != 0 );
	}
	inline uint16_t GetDataSize()
	{
		return ( (this->m_ui8Width - 1) / 8 + 1 ) * this->m_ui8Height;
	}

protected:
	void ReleaseBitmap();

	uint8_t * m_pBitmap;
	uint8_t m_ui8Width;
	uint8_t m_ui8Height;
	wchar_t m_wcChar;

	CSPBitmapElement * m_pNextBitmap;
};

class CSPBitmapIndexElement
{
	friend class CSPBitmapIndexManager;
public:
	CSPBitmapIndexElement();
	virtual ~CSPBitmapIndexElement();

	inline void SetCode( uint32_t ui32Code )
	{
		this->m_bCodeSet = true;
		this->m_ui32Code = ui32Code;
	}
	inline void SetSeek( uint32_t ui32Seek )
	{
		this->m_bSeekSet = true;
		this->m_ui32Seek = ui32Seek;
	}

	inline bool IsSetCodeAndSeek()
	{
		return this->m_bSeekSet & this->m_bCodeSet;
	}
	inline bool IsSetCode()
	{
		return this->m_bCodeSet;
	}

	inline uint32_t GetCode()
	{
		return this->m_ui32Code;
	}
	inline uint32_t GetSeek()
	{
		return this->m_ui32Seek;
	}

protected:
	uint32_t m_ui32Code;
	uint32_t m_ui32Seek;
	bool m_bCodeSet;
	bool m_bSeekSet;

	CSPBitmapIndexElement * m_pNextElement;
};

class CSPBitmapIndexManager
{
public:
	CSPBitmapIndexManager();
	virtual ~CSPBitmapIndexManager();

	void AddElement( CSPBitmapIndexElement * pElement );

	inline int GetElementNumber()
	{
		return this->m_iElementNumber;
	}
	bool WriteIndexListToFile( FILE * fpOutput );

protected:
	int m_iElementNumber;
	CSPBitmapIndexElement * m_pFirstElement;
	CSPBitmapIndexElement * m_pLastElement;

	CSPBitmapIndexElement ** m_ppSortedElement;
	int m_iSortedNumber;

	uint8_t m_ui8CodeSize;
	uint8_t m_ui8SeekSize;

	void ReleaseIndexElement();

	void ReleaseSortedArray();
	void CreateSortedArray();

	static int CompareForCodeSortedArray( const void * pVoid1, const void * pVoid2 );
};

class CSPBitmapFileHeader
{
public:
	CSPBitmapFileHeader();
	virtual ~CSPBitmapFileHeader();

	inline void SetVersion( uint16_t ui16Version )
	{
		this->m_ui16Version = ui16Version;
	}
	inline uint16_t GetVersion()
	{
		return this->m_ui16Version;
	}

	inline void SetHeaderSize( uint16_t ui16HeaderSize )
	{
		this->m_ui16HeaderSize = ui16HeaderSize;
	}
	inline uint16_t GetHeaderSize()
	{
		return this->m_ui16HeaderSize;
	}
	void SetCharset( const char * pcszCharset );
	inline const char * GetCharset()
	{
		return this->m_pszCharset;
	}
	inline void SetIndexNumber( uint32_t ui32IndexNumber )
	{
		this->m_ui32IndexNumber = ui32IndexNumber;
	}
	inline uint32_t GetIndexNumber()
	{
		return this->m_ui32IndexNumber;
	}

	inline void SetCharcodeSize( uint8_t ui8CharcodeSize )
	{
		this->m_ui8CharcodeSize = ui8CharcodeSize;
		this->CalcIndexRecordSize();
	}
	inline void SetSeekSize( uint8_t ui8SeekSize )
	{
		this->m_ui8SeekSize = ui8SeekSize;
		this->CalcIndexRecordSize();
	}

	inline uint8_t GetIndexRecordSize()
	{
		return this->m_ui8IndexRecordSize;
	}
	inline uint8_t GetCharcodeSize()
	{
		return this->m_ui8CharcodeSize;
	}
	inline uint8_t GetSeekSize()
	{
		return this->m_ui8SeekSize;
	}

	bool WriteHeader( FILE * fpOutput );
	bool ReadHeader( FILE * fpInput );

protected:
	uint16_t m_ui16Version;
	uint16_t m_ui16HeaderSize;
	char m_pszCharset[9];
	uint32_t m_ui32IndexNumber;
	uint8_t m_ui8IndexRecordSize;
	uint8_t m_ui8CharcodeSize;
	uint8_t m_ui8SeekSize;

	inline void CalcIndexRecordSize()
	{
		this->m_ui8IndexRecordSize =
				this->m_ui8CharcodeSize + this->m_ui8SeekSize;

	}
};
}

#endif
