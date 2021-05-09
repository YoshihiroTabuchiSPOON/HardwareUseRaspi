#include <stdlib.h>
#include "SPBitmapFont.h"

using namespace SPBitmapFont;

CSPBitmapIndexElement::CSPBitmapIndexElement()
{
	this->m_pNextElement = NULL;
	this->m_ui32Code = this->m_ui32Seek = 0;
	this->m_bCodeSet = false;
	this->m_bSeekSet = false;
}

CSPBitmapIndexElement::~CSPBitmapIndexElement()
{
}

///////////////////////////////////////////////////////////////////////////////

CSPBitmapIndexManager::CSPBitmapIndexManager()
{
	this->m_pFirstElement = this->m_pLastElement = NULL;
	this->m_iElementNumber = 0;
	this->m_ppSortedElement = NULL;
	this->m_iSortedNumber = 0;
	this->m_ui8CodeSize = 2;
	this->m_ui8SeekSize = 4;
}

CSPBitmapIndexManager::~CSPBitmapIndexManager()
{
	this->ReleaseSortedArray();
	this->ReleaseIndexElement();
}

void CSPBitmapIndexManager::ReleaseIndexElement()
{
	while( this->m_pFirstElement )
	{
		CSPBitmapIndexElement * pDelete = this->m_pFirstElement;
		this->m_pFirstElement = this->m_pFirstElement->m_pNextElement;
		delete pDelete;
	}
	this->m_pLastElement = NULL;
}

void CSPBitmapIndexManager::ReleaseSortedArray()
{
	if( this->m_ppSortedElement )
	{
		delete[] this->m_ppSortedElement;
		this->m_ppSortedElement = NULL;
		this->m_iSortedNumber = 0;
	}
}

void CSPBitmapIndexManager::AddElement( CSPBitmapIndexElement * pElement )
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

	this->m_iElementNumber++;
}

void CSPBitmapIndexManager::CreateSortedArray()
{
	this->ReleaseSortedArray();

	if( this->m_iElementNumber == 0 )
	{
		return;
	}

	this->m_iSortedNumber = this->m_iElementNumber;
	this->m_ppSortedElement = new CSPBitmapIndexElement*[this->m_iSortedNumber];

	int iIndex = 0;
	for( CSPBitmapIndexElement * pElement = this->m_pFirstElement
			; pElement && iIndex < this->m_iSortedNumber
			; pElement = pElement->m_pNextElement, iIndex++ )
	{
		this->m_ppSortedElement[iIndex] = pElement;
	}

	qsort( this->m_ppSortedElement, this->m_iSortedNumber, sizeof(CSPBitmapIndexElement*)
			, CSPBitmapIndexManager::CompareForCodeSortedArray );

}

int CSPBitmapIndexManager::CompareForCodeSortedArray( const void * pVoid1, const void * pVoid2 )
{
	CSPBitmapIndexElement ** ppElement1 = (CSPBitmapIndexElement**)pVoid1;
	CSPBitmapIndexElement ** ppElement2 = (CSPBitmapIndexElement**)pVoid2;

	if( (*ppElement1)->GetCode() < (*ppElement2)->GetCode() )
	{
		return -1;
	}
	else if( (*ppElement1)->GetCode() > (*ppElement2)->GetCode() )
	{
		return 1;
	}
	return 0;
}

bool CSPBitmapIndexManager::WriteIndexListToFile(FILE * fpOutput)
{
	this->CreateSortedArray();

	for( int i=0; i<this->m_iSortedNumber; i++ )
	{
		uint32_t ui32Code = this->m_ppSortedElement[i]->GetCode();
		if( this->m_ui8CodeSize == 2 )
		{
			uint16_t ui16Code = (uint16_t)ui32Code;
			fwrite( &ui16Code, sizeof(uint16_t), 1, fpOutput );
		}
		else
		{
			fwrite( &ui32Code, sizeof(uint32_t), 1, fpOutput );
		}

		uint32_t ui32Seek = this->m_ppSortedElement[i]->GetSeek();
		if( this->m_ui8SeekSize == 2 )
		{
			uint16_t ui16Seek = (uint16_t)ui32Seek;
			fwrite( &ui16Seek, sizeof(uint16_t), 1, fpOutput );
		}
		else
		{
			fwrite( &ui32Seek, sizeof(uint32_t), 1, fpOutput );
		}
	}

	return true;
}

