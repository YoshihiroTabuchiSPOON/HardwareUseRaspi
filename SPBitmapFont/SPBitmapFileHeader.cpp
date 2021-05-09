#include <string.h>
#include "SPBitmapFont.h"

using namespace SPBitmapFont;

CSPBitmapFileHeader::CSPBitmapFileHeader()
{
	this->m_ui16Version = 1;
	this->m_ui16HeaderSize = 8 + sizeof(uint32_t) + sizeof(uint8_t)*3;

	memset( this->m_pszCharset, 0, 9 );
	this->m_ui32IndexNumber = 0;
	this->m_ui8IndexRecordSize = 0;
	this->m_ui8CharcodeSize = 0;
	this->m_ui8SeekSize = 0;
}

CSPBitmapFileHeader::~CSPBitmapFileHeader()
{
}

void CSPBitmapFileHeader::SetCharset( const char * pcszCharset )
{
	memset( this->m_pszCharset, 0, 9 );
	if( strlen( pcszCharset ) < 8 )
	{
		strcpy( this->m_pszCharset, pcszCharset );
	}
	else
	{
		strncpy( this->m_pszCharset, pcszCharset, 9 );
		this->m_pszCharset[8] = '\0';
	}
}

bool CSPBitmapFileHeader::WriteHeader( FILE * fpOutput )
{
	fwrite( &this->m_ui16Version, sizeof(uint16_t), 1, fpOutput );
	fwrite( &this->m_ui16HeaderSize, sizeof(uint16_t), 1, fpOutput );
	fwrite( this->m_pszCharset, sizeof(char), 8, fpOutput );
	fwrite( &this->m_ui32IndexNumber, sizeof(uint32_t), 1, fpOutput );
	fwrite( &this->m_ui8IndexRecordSize, sizeof(uint8_t), 1, fpOutput );
	fwrite( &this->m_ui8CharcodeSize, sizeof(uint8_t), 1, fpOutput );
	fwrite( &this->m_ui8SeekSize, sizeof(uint8_t), 1, fpOutput );

	return true;
}

bool CSPBitmapFileHeader::ReadHeader( FILE * fpInput )
{
	int iSeekPos;

	fread( &this->m_ui16Version, sizeof(uint16_t), 1, fpInput );
	fread( &this->m_ui16HeaderSize, sizeof(uint16_t), 1, fpInput );

	fread( this->m_pszCharset, sizeof(char), 8, fpInput );
	this->m_pszCharset[8] = '\0';

	fread( &this->m_ui32IndexNumber, sizeof(uint32_t), 1, fpInput );
	fread( &this->m_ui8IndexRecordSize, sizeof(uint8_t), 1, fpInput );
	fread( &this->m_ui8CharcodeSize, sizeof(uint8_t), 1, fpInput );
	fread( &this->m_ui8SeekSize, sizeof(uint8_t), 1, fpInput );

	iSeekPos = sizeof(uint16_t)*2 + this->m_ui16HeaderSize;
	fseek( fpInput, iSeekPos, SEEK_SET );

	return true;
}
