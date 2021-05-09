#include <string.h>
#include "SPBitmapFont.h"

using namespace SPBitmapFont;


CSPBitmapFont::CSPBitmapFont()
{
	this->m_pFirstCachedBitmap = this->m_pLastCachedBitmap = NULL;
	this->m_pNextBitmapFont = NULL;
	this->m_pszFontName = NULL;
	this->m_iCachedNumber = 0;
	this->m_ppSortedCachedBitmap = NULL;
	this->m_iSortedCachedNumber = 0;
	this->m_iSortedCachedAllocatedNumber = 0;
	this->m_iAllocateStep = 100;
}

CSPBitmapFont::~CSPBitmapFont()
{
	this->ReleaseCachedBitmap();
	this->ReleaseSortedCachedBitmap();
	this->ReleaseFontName();
}

void CSPBitmapFont::ReleaseCachedBitmap()
{
	while( this->m_pFirstCachedBitmap )
	{
		CSPBitmapElement * pDelete = this->m_pFirstCachedBitmap;
		this->m_pFirstCachedBitmap = this->m_pFirstCachedBitmap->m_pNextBitmap;
		delete pDelete;
	}
	this->m_pLastCachedBitmap = NULL;
	this->m_iCachedNumber = 0;
}
void CSPBitmapFont::ReleaseSortedCachedBitmap()
{
	if( this->m_ppSortedCachedBitmap )
	{
		delete[] this->m_ppSortedCachedBitmap;
		this->m_ppSortedCachedBitmap = NULL;
		this->m_iSortedCachedNumber = 0;
	}
}

void CSPBitmapFont::ReleaseFontName()
{
	if( this->m_pszFontName )
	{
		delete[] this->m_pszFontName;
		this->m_pszFontName = NULL;
	}
}

void CSPBitmapFont::SetFontName( const char * pcszFontName )
{
	this->ReleaseCachedBitmap();
	this->ReleaseSortedCachedBitmap();
	this->ReleaseFontName();

	size_t sizeFontName = strlen( pcszFontName ) + 1;
	this->m_pszFontName = new char[ sizeFontName ];
	strcpy( this->m_pszFontName, pcszFontName );
}

void CSPBitmapFont::AddElement( CSPBitmapElement * pElement )
{
	if( this->m_pFirstCachedBitmap )
	{
		this->m_pLastCachedBitmap->m_pNextBitmap = pElement;
		this->m_pLastCachedBitmap = pElement;
	}
	else
	{
		this->m_pFirstCachedBitmap = this->m_pLastCachedBitmap = pElement;
	}

	this->m_iCachedNumber++;
}

CSPBitmapElement * CSPBitmapFont::GetBitmapData( wchar_t wcChar )
{
	int iIndex = 0;
	if( this->GetCachedFontIndex( wcChar, iIndex ) )
	{
		return this->m_ppSortedCachedBitmap[ iIndex ];
	}

	CSPBitmapElement * pBitmapLoad = this->GetFontFromFile( wcChar );
	if( !pBitmapLoad )
	{
		return NULL;
	}

	this->AddElement( pBitmapLoad );

	//必要に応じてソート配列を拡張する
	if( this->m_iCachedNumber > this->m_iSortedCachedAllocatedNumber)
	{
		int iAllocateNumber = this->m_iSortedCachedAllocatedNumber +
								 this->m_iAllocateStep;

		CSPBitmapElement ** ppTempSortedElement =
				new CSPBitmapElement*[iAllocateNumber];
		memset( ppTempSortedElement, 0, sizeof(CSPBitmapElement*)*iAllocateNumber );

		if( this->m_ppSortedCachedBitmap )
		{
			memcpy( ppTempSortedElement, this->m_ppSortedCachedBitmap
					, sizeof(CSPBitmapElement*) * this->m_iSortedCachedNumber );
			delete[] this->m_ppSortedCachedBitmap;
		}

		this->m_ppSortedCachedBitmap = ppTempSortedElement;
		this->m_iSortedCachedAllocatedNumber = iAllocateNumber;
	}

	//先程取得した直前配列のひとつ後ろに放り込む
	int iMoveSize = this->m_iSortedCachedNumber - (iIndex + 1);
	if( iMoveSize )
	{
		memmove( this->m_ppSortedCachedBitmap + (iIndex + 2)
				, this->m_ppSortedCachedBitmap + (iIndex + 1)
				, iMoveSize * sizeof(CSPBitmapElement*) );
	}

	this->m_ppSortedCachedBitmap[ iIndex+1 ] = pBitmapLoad;
	this->m_iSortedCachedNumber++;

	return pBitmapLoad;
}

//
//	２分検索を使用してキャッシュされた文字を取得する
//
//	引数：
//	wChar	(IN)	検索する文字（Unicode）
//	iHitOrPrevIndex	(OUT)	文字がキャッシュされている場合には文字のインデックス
//								キャッシュされていない場合には文字のひとつ前のインデックス
//
//	戻り値：
//	==true		：	キャッシュにデータあり
//	==false	：	キャッシュにデータなし
//
bool CSPBitmapFont::GetCachedFontIndex( wchar_t wcChar, int & iHitOrPrevIndex )
{
	bool bHit = false;

	if( !this->m_ppSortedCachedBitmap )
	{
		iHitOrPrevIndex = -1;
	}
	else
	{
		int iMin = 0;
		int iMax = this->m_iSortedCachedNumber-1;

		while( iMin <= iMax )
		{
			int iMid = (iMin + iMax) / 2;
			wchar_t wcRecord = this->m_ppSortedCachedBitmap[iMid]->GetChar();

			if( wcChar < wcRecord )
			{
				iMax = iMid - 1;
			}
			else if( wcChar > wcRecord )
			{
				iMin = iMid + 1;
			}
			else
			{
				bHit = true;
				iHitOrPrevIndex = iMid;
				break;
			}
		}

		if( !bHit )
		{
			iHitOrPrevIndex = iMax;
		}
	}

	return bHit;
}

CSPBitmapElement * CSPBitmapFont::GetFontFromFile( wchar_t wcChar )
{
	char szFontFile[1024];
	snprintf( szFontFile, 1024, "%s.bin", this->m_pszFontName );

	FILE * fp = fopen( szFontFile, "r" );
	if( !fp )
	{
		printf( "Cannot open font file %s\n", szFontFile );
		return NULL;
	}

	CSPBitmapFileHeader fileHeader;
	if( !fileHeader.ReadHeader( fp ) )
	{
		fclose( fp );
		printf( "Cannot read header FontHeader %s\n", szFontFile );
		return NULL;
	}

	//ヘッダーを読み取った直後はシークポイントがヘッダー直後に調整されている
	int iIndexTopPos = ftell( fp );

	//インデックスファイルを使用して２分検索を行う
	int iMin = 0;
	int iMax = (int)fileHeader.GetIndexNumber() - 1;

	uint8_t ui8IndexSize = fileHeader.GetIndexRecordSize();
	int iHitSeekPos= -1;

	while( iMin <= iMax )
	{
		int iMid = (iMin + iMax) / 2;

		int iSeekPos = iIndexTopPos + iMid * ui8IndexSize;
		fseek( fp, iSeekPos, SEEK_SET );

		uint8_t ui8CharcodeSize = fileHeader.GetCharcodeSize();
		uint8_t ui8SeekSize = fileHeader.GetSeekSize();

		uint32_t ui32Charcode = 0;
		if( ui8CharcodeSize == 1 )
		{
			uint8_t ui8Charcode = 0;
			fread( &ui8Charcode, 1, 1, fp );
			ui32Charcode = ui8Charcode;
		}
		else if( ui8CharcodeSize == 2 )
		{
			uint16_t ui16Charcode = 0;
			fread( &ui16Charcode, 2, 1, fp );
			ui32Charcode = ui16Charcode;
		}
		else
		{
			fread( &ui32Charcode, 4, 1, fp );
		}

		uint32_t ui32Seek = 0;
		if( ui8SeekSize == 2 )
		{
			uint16_t ui16Seek = 0;
			fread( &ui16Seek, 2, 1, fp );
			ui32Seek = ui16Seek;
		}
		else
		{
			fread( &ui32Seek, 4, 1, fp );
		}

		wchar_t wcReadCharcode = (wchar_t)ui32Charcode;
		if( wcChar < wcReadCharcode )
		{
			iMax = iMid - 1;
		}
		else if( wcChar > wcReadCharcode )
		{
			iMin = iMid + 1;
		}
		else
		{
			iHitSeekPos = (int)ui32Seek;
			break;
		}
	}

	if( iHitSeekPos < 0 )
	{
		return NULL;
	}

	//Index最終ポジション
	int iAfterIndexPos = iIndexTopPos + fileHeader.GetIndexNumber() * ui8IndexSize;
	int iFontDataPos = iAfterIndexPos + iHitSeekPos;

	fseek( fp, iFontDataPos, SEEK_SET );
	CSPBitmapElement * pElement = new CSPBitmapElement();
	pElement->ReadFromBinFile( fp );
	pElement->SetChar( wcChar );

	return pElement;
}
