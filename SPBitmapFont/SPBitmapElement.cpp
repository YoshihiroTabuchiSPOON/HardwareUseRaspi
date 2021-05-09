#include <stdlib.h>
#include <string.h>
#include "SPBitmapFont.h"

#ifndef NULL
#define NULL (0)
#endif

using namespace SPBitmapFont;

CSPBitmapElement::CSPBitmapElement()
{
	this->m_pBitmap = NULL;
	this->m_ui8Width = this->m_ui8Height = 0;
	this->m_pNextBitmap = NULL;
	this->m_wcChar = L'\0';
}

CSPBitmapElement::~CSPBitmapElement()
{
	this->ReleaseBitmap();
}

void CSPBitmapElement::ReleaseBitmap()
{
	if( this->m_pBitmap )
	{
		delete[] this->m_pBitmap;
		this->m_pBitmap = NULL;
		this->m_ui8Width = this->m_ui8Height = 0;
	}
}

void CSPBitmapElement::SetBitmapSize( uint8_t ui8Width, uint8_t ui8Height )
{
	this->ReleaseBitmap();

	this->m_ui8Width = ui8Width;
	this->m_ui8Height = ui8Height;

	uint16_t ui16DataSize = this->GetDataSize();
	this->m_pBitmap = new uint8_t[ ui16DataSize ];
}

void CSPBitmapElement::SetBitmapDataText( uint8_t ui8Line, const char * pcszBitmapData )
{
	char szByte[3];
	uint8_t ui8ColBy8 = 0;

	for( int i=0; pcszBitmapData[i]; i+=2 )
	{
		szByte[0] = pcszBitmapData[i];
		szByte[1] = pcszBitmapData[i+1];
		szByte[2] = '\0';

		uint8_t ui8Offset = ui8Line * ((this->m_ui8Width-1) / 8 + 1)
							 + ui8ColBy8 ;

		if( ui8Offset < this->GetDataSize() )
		{
			this->m_pBitmap[ ui8Offset ] = (uint8_t)strtoul( szByte, NULL, 16 );
		}

		ui8ColBy8++;
	}
}

bool CSPBitmapElement::WriteToBinFile( FILE * fpOutput )
{
	fwrite( &this->m_ui8Width, 1, 1, fpOutput );
	fwrite( &this->m_ui8Height, 1, 1, fpOutput );

	fwrite( this->m_pBitmap, 1, this->GetDataSize(), fpOutput );

	return true;
}

bool CSPBitmapElement::ReadFromBinFile( FILE * fpInput )
{
	uint8_t ui8Width;
	uint8_t ui8Height;

	fread( &ui8Width, 1, 1, fpInput );
	fread( &ui8Height, 1, 1, fpInput );

	this->SetBitmapSize( ui8Width, ui8Height );
	fread( this->m_pBitmap, 1, this->GetDataSize(), fpInput );

	return true;
}

CSPBitmapElement * CSPBitmapElement::Clone()
{
	CSPBitmapElement * pClone = new CSPBitmapElement();
	pClone->m_ui8Width = this->m_ui8Width;
	pClone->m_ui8Height = this->m_ui8Height;

	pClone->m_pBitmap = new uint8_t[ this->GetDataSize() ];
	memcpy( pClone->m_pBitmap, this->m_pBitmap, this->GetDataSize() );

	pClone->m_wcChar = this->m_wcChar;

	return pClone;
}
