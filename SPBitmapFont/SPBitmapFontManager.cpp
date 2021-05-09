#include <string.h>
#include "SPBitmapFontManager.h"

using namespace SPBitmapFont;

CSPBitmapFontListElement::CSPBitmapFontListElement()
{
	this->m_pNextElement = NULL;
}
CSPBitmapFontListElement::~CSPBitmapFontListElement()
{
}

///////////////////////////////////////////////////////////////////////////////

CSPBitmapFontList::CSPBitmapFontList()
{
	this->m_pFirstElement = this->m_pLastElement = NULL;
}

CSPBitmapFontList::~CSPBitmapFontList()
{
	this->ReleaseAllElement();
}

void CSPBitmapFontList::ReleaseAllElement()
{
	while( this->m_pFirstElement )
	{
		CSPBitmapFontListElement * pDelete = this->m_pFirstElement;
		this->m_pFirstElement = this->m_pFirstElement->m_pNextElement;
		delete pDelete;
	}
	this->m_pLastElement = NULL;
}

CSPBitmapFont * CSPBitmapFontList::GetFont( const char * pcszFontName )
{
	for( CSPBitmapFontListElement * pElement = this->m_pFirstElement
			; pElement
			; pElement = pElement->m_pNextElement )
	{
		if( strcmp( pElement->GetFontName(), pcszFontName ) == 0 )
		{
			return pElement;
		}
	}

	CSPBitmapFontListElement * pElement = new CSPBitmapFontListElement();
	pElement->SetFontName( pcszFontName );
	this->AddElement( pElement );

	return pElement;
}

void CSPBitmapFontList::AddElement( CSPBitmapFontListElement * pElement )
{
	if( this->m_pFirstElement )
	{
		this->m_pLastElement->m_pNextElement = pElement;
		this->m_pLastElement = pElement;
	}
	else
	{
		this->m_pFirstElement = this->m_pLastElement = pElement;
	}
}
