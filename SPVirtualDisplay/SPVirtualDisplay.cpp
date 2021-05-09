#include <string.h>
#include "SPVirtualDisplay.h"
#include "../SPBitmapFont/SPBitmapSelect.h"

using namespace SPVirtualDisplay;

CSPVirtualDisplay::CSPVirtualDisplay()
{
	this->m_ui16DisplayWidth = 0;
	this->m_ui16DisplayHeight = 0;
	this->m_pDisplayBuffer = NULL;
}

CSPVirtualDisplay::~CSPVirtualDisplay()
{
	this->ReleaseDisplayBuffer();
}

void CSPVirtualDisplay::ReleaseDisplayBuffer()
{
	if( this->m_pDisplayBuffer )
	{
		delete[] this->m_pDisplayBuffer;
		this->m_pDisplayBuffer = NULL;
	}
}

void CSPVirtualDisplay::SetDisplaySize( uint16_t ui16DisplayWidth
										   , uint16_t ui16DisplayHeight )
{
	this->ReleaseDisplayBuffer();

	this->m_ui16DisplayWidth = ui16DisplayWidth;
	this->m_ui16DisplayHeight = ui16DisplayHeight;

	this->m_pDisplayBuffer = new uint8_t[ this->GetDisplayBufferSize() ];
	this->ClearDisplay();
}

void CSPVirtualDisplay::DrawHorzLine(uint16_t ui16XPosStart
		, uint16_t ui16YPos, uint16_t ui16XPosEnd
		, uint32_t ui32PointInfo )
{
}

void CSPVirtualDisplay::DrawVertLine( uint16_t ui16XPos
		, uint16_t ui16YPosStart, uint16_t ui16YPosEnd
		, uint32_t ui32PointInfo )
{
}

void CSPVirtualDisplay::DrawString( uint16_t ui16XPos
		,uint16_t ui16YPos,const char * pcszFontName
		,const wchar_t * pcwszString, int iDefaultHeight
		,uint32_t ui32TextPointInfo, uint32_t ui32BackPointInfo )
{
	SPBitmapFont::CSPBitmapFont * pBitmapFont = this->m_fontList.GetFont( pcszFontName );
	SPBitmapFont::CSPBitmapSelect selectFont;

	selectFont.GetFonts( *pBitmapFont, pcwszString, iDefaultHeight, iDefaultHeight );

	uint16_t ui16StartX = ui16XPos;
	uint16_t ui16StartY = ui16YPos;
	for( SPBitmapFont::CSPBitmapElement * pBitmapElement = selectFont.GetFirstBitmap()
			; pBitmapElement
			; pBitmapElement = pBitmapElement->GetNextElement() )
	{
		for( uint8_t ui8Line = 0
				; ui8Line < pBitmapElement->GetHeight()
				; ui8Line++ )
		{
			for( uint8_t ui8Col = 0
					; ui8Col < pBitmapElement->GetWidth()
					; ui8Col++ )
			{
				uint32_t ui32Point = ui32BackPointInfo;

				if( pBitmapElement->GetPoint( ui8Col, ui8Line ) )
				{
					ui32Point = ui32TextPointInfo;
				}

				this->SetPoint( ui16StartX + ui8Col
								, ui16StartY + ui8Line
								, ui32Point );
			}
		}

		ui16StartX += pBitmapElement->GetWidth();
	}
}
