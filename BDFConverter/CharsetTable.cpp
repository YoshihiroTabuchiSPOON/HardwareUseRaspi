#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CharsetTable.h"
#include "../SPCommon/SPCommon.h"

CCharsetElement::CCharsetElement(uint32_t ui16SJIS
		, uint16_t ui16JIS, uint16_t ui16UTF16)
{
	this->m_ui16SJIS = ui16SJIS;
	this->m_ui16JIS = ui16JIS;
	this->m_ui16UTF16 = ui16UTF16;
	this->m_pNextElement = NULL;
	this->m_pParentTable = NULL;
}

CCharsetElement::~CCharsetElement()
{
}

///////////////////////////////////////////////////////////////////////////////

CCharsetTable::CCharsetTable()
{
	this->m_pFirstElement = this->m_pLastElement = NULL;
	this->m_iCharsetNumber = 0;
	this->m_charsetSorted = CharsetUnknown;
	this->m_iSortedNumber = 0;
	this->m_ppSortedArray = NULL;
}

CCharsetTable::~CCharsetTable()
{
	this->ReleaseSortedArray();
	this->ReleaseAllElement();
}

void CCharsetTable::ReleaseAllElement()
{
	while( this->m_pFirstElement )
	{
		CCharsetElement * pDelete = this->m_pFirstElement;
		this->m_pFirstElement = this->m_pFirstElement->m_pNextElement;
		delete pDelete;
	}
	this->m_pLastElement = NULL;
	this->m_iCharsetNumber = 0;
}

void CCharsetTable::ReleaseSortedArray()
{
	if( this->m_ppSortedArray )
	{
		delete[] this->m_ppSortedArray;
		this->m_ppSortedArray = NULL;
		this->m_iSortedNumber = 0;
	}
}

void CCharsetTable::AddElement( CCharsetElement * pElement )
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
	pElement->m_pParentTable = this;
	this->m_iCharsetNumber++;
}

void CCharsetTable::CreateSortedArray(ECharsetType charsetType)
{
	if( charsetType == CharsetUnknown )
	{
		return;
	}

	this->ReleaseSortedArray();

	this->m_iSortedNumber = this->m_iCharsetNumber;
	this->m_ppSortedArray = new CCharsetElement*[this->m_iSortedNumber];

	int iIndex = 0;
	for( CCharsetElement * pElement = this->m_pFirstElement
			; pElement && iIndex < this->m_iSortedNumber
			; pElement = pElement->m_pNextElement, iIndex++ )
	{
		this->m_ppSortedArray[iIndex] = pElement;
	}

	this->m_charsetSorted = charsetType;

	qsort( this->m_ppSortedArray, this->m_iSortedNumber, sizeof(CCharsetElement*)
			, CCharsetTable::CharsetSorter );
}

int CCharsetTable::CharsetSorter( const void * pVoid1, const void * pVoid2 )
{
	CCharsetElement ** ppElement1 = (CCharsetElement**)pVoid1;
	CCharsetElement ** ppElement2 = (CCharsetElement**)pVoid2;

	uint16_t ui16Charset1 = 0;
	uint16_t ui16Charset2 = 0;

	switch( (*ppElement1)->m_pParentTable->GetCharsetSorted() )
	{
	case CharsetSJIS:
		ui16Charset1 = (*ppElement1)->GetSJIS();
		ui16Charset2 = (*ppElement2)->GetSJIS();
		break;

	case CharsetJIS:
		ui16Charset1 = (*ppElement1)->GetJIS();
		ui16Charset2 = (*ppElement2)->GetJIS();
		break;

	case CharsetUTF16:
		ui16Charset1 = (*ppElement1)->GetUTF16();
		ui16Charset2 = (*ppElement2)->GetUTF16();
		break;

	default:
		break;
	}

	return (int32_t)ui16Charset1 - (int32_t)ui16Charset2;
}

CCharsetElement * CCharsetTable::SearchCharsetSortedBinary( uint16_t ui16Charcode )
{
	if( this->m_iSortedNumber == 0 )
	{
		return NULL;
	}

	int iMin = 0;
	int iMax = this->m_iSortedNumber-1;

	while( iMin <= iMax )
	{
		int iMid = (iMin+iMax) / 2;

		uint16_t ui16CheckCharcode;

		switch( this->m_charsetSorted )
		{
		case CharsetSJIS:
			ui16CheckCharcode = this->m_ppSortedArray[iMid]->GetSJIS();
			break;

		case CharsetJIS:
			ui16CheckCharcode = this->m_ppSortedArray[iMid]->GetJIS();
			break;

		case CharsetUTF16:
			ui16CheckCharcode = this->m_ppSortedArray[iMid]->GetUTF16();
			break;

		default:
			return NULL;
		}

		if( ui16Charcode < ui16CheckCharcode )
		{
			iMax = iMid-1;
		}
		else if( ui16Charcode > ui16CheckCharcode )
		{
			iMin = iMid+1;
		}
		else
		{
			return this->m_ppSortedArray[iMid];
		}
	}

	return NULL;
}

bool CCharsetTable::StructCharsetTable( const char * pcszFileName, ECharsetType charsetType )
{
	FILE * fp = fopen( pcszFileName, "r" );
	if( !fp )
	{
		return false;
	}

	char szBuff[4096];
	while( fgets( szBuff, 4096, fp ) )
	{
		if( szBuff[0] == '#' )
		{
			continue;
		}

		int iColumnNumber = 0;
		for( int i=0; szBuff[i]; i++ )
		{
			if( szBuff[i] == '\t' )
			{
				szBuff[i] = '\0';
				iColumnNumber++;
			}
			else if( szBuff[i] == '\n' ||
					  szBuff[i] == '\r' )
			{
				szBuff[i] = '\0';
				iColumnNumber++;

				break;
			}
		}

		if( iColumnNumber < 3 )
		{
			continue;
		}

		const char * pcszSJIS   = szBuff;
		const char * pcszJIS   = pcszSJIS + strlen(pcszSJIS) + 1;
		const char * pcszUTF16 = pcszJIS + strlen(pcszJIS) + 1;

		CCharsetElement * pElement = new CCharsetElement(
					SPCommon::Get10Or16DigitsValue( pcszSJIS )
				, 	SPCommon::Get10Or16DigitsValue( pcszJIS )
				,	SPCommon::Get10Or16DigitsValue( pcszUTF16 ) );
		this->AddElement( pElement );
	}

	fclose( fp );

	this->CreateSortedArray( charsetType );

	return true;
}

