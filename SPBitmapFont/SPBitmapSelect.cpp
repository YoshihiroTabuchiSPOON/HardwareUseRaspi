#include <string.h>
#include "SPBitmapSelect.h"

using namespace SPBitmapFont;

CSPBitmapSelect::CSPBitmapSelect()
{
	this->m_pFirstBitmap = this->m_pLastBitmap = NULL;
}

CSPBitmapSelect::~CSPBitmapSelect()
{
	this->ReleaseAllElement();
}

void CSPBitmapSelect::ReleaseAllElement()
{
	while( this->m_pFirstBitmap )
	{
		CSPBitmapElement * pDelete = this->m_pFirstBitmap;
		this->m_pFirstBitmap = this->m_pFirstBitmap->m_pNextBitmap;
		delete pDelete;
	}
	this->m_pLastBitmap = NULL;
}

bool CSPBitmapSelect::GetFonts( CSPBitmapFont & bitmapFont
		, const wchar_t * pcwszString, int iDefaultWidth, int iDefaultHeight )
{
	for( int i=0; pcwszString[i]; i++ )
	{
		CSPBitmapElement * pSelectElement = bitmapFont.GetBitmapData( pcwszString[i] );
		CSPBitmapElement * pCharElement;
		if( pSelectElement )
		{
			pCharElement = pSelectElement->Clone();
		}
		else
		{
			pCharElement = this->CreateDefaultChar(iDefaultWidth, iDefaultHeight);
			pCharElement->SetChar( pcwszString[i] );
		}

		this->AddElement( pCharElement );
	}

	return true;
}

void CSPBitmapSelect::AddElement( CSPBitmapElement * pElement )
{
	if( this->m_pFirstBitmap )
	{
		this->m_pLastBitmap->m_pNextBitmap = pElement;
		this->m_pLastBitmap = pElement;
	}
	else
	{
		this->m_pFirstBitmap = this->m_pLastBitmap = pElement;
	}
}

CSPBitmapElement * CSPBitmapSelect::CreateDefaultChar( int iDefaultWidth, int iDefaultHeight )
{
	CSPBitmapElement * pBitmap = new CSPBitmapElement();
	pBitmap->SetBitmapSize( (uint8_t)iDefaultWidth
							, (uint8_t)iDefaultHeight );
	uint8_t ui8Width = 0;
	uint8_t ui8Height = 0;

	uint8_t * puiBitmap = pBitmap->GetBitmapData( ui8Width, ui8Height );
	memset( puiBitmap, 0xff, pBitmap->GetDataSize() );

	return pBitmap;
}
