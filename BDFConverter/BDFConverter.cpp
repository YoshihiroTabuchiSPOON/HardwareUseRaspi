#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CharsetTable.h"
#include "../SPBitmapFont/SPBitmapFont.h"
#include "../SPCommon/SPCommon.h"

static bool OutputBitmapData( FILE * fpOutput
								, SPBitmapFont::CSPBitmapElement * pBitmapElement
							    , SPBitmapFont::CSPBitmapIndexElement * pBitmapIndexElement );

int main( int argc, char * argv[] )
{
	if( argc < 3 )
	{
		printf( "Usage: ./BDFConverter [-c InputCharset] [-t CharsetConvertTable] InputBDFFile OutputBinFile\n" );
		return 1;
	}

	const char * pcszInputCharset = NULL;
	const char * pcszCharsetConvertTable = NULL;

	for( int i=0; i<argc-2; i++ )
	{
		if( strcmp( argv[i], "-c" ) == 0 &&
			i+1 < argc-2 )
		{
			pcszInputCharset = argv[i+1];
		}

		if( strcmp( argv[i] , "-t" ) == 0 &&
			i+1 < argc-2 )
		{
			pcszCharsetConvertTable = argv[i+1];
		}
	}

	const char * pcszInputBDFFile  = argv[argc - 2];
	const char * pcszOutputBinFile = argv[argc - 1];

	if( pcszInputCharset != NULL &&
		pcszCharsetConvertTable == NULL )
	{
		printf( "CharsetConvertTable must set, if you set InputCHarset\n" );
		return 2;
	}

	CCharsetTable::ECharsetType charsetType;
	charsetType = CCharsetTable::CharsetUnknown;
	if( pcszInputCharset )
	{
		if( strcmp( pcszInputCharset, "SJIS" ) == 0 )
		{
			charsetType = CCharsetTable::CharsetSJIS;
		}
		else if( strcmp( pcszInputCharset, "JIS" ) == 0 )
		{
			charsetType = CCharsetTable::CharsetJIS;
		}
		else if( strcmp( pcszInputCharset, "UTF16" ) == 0 )
		{
			charsetType = CCharsetTable::CharsetUTF16;
		}
		else
		{
			printf( "Unknown charset %s\n", pcszInputCharset );
		}
	}


	CCharsetTable charsetTable;
	if( !charsetTable.StructCharsetTable( pcszCharsetConvertTable, charsetType ) )
	{
		printf( "CharsetConvertTable Cannot create  %s\n", pcszCharsetConvertTable );
		return 3;
	}

	FILE * fpInput = fopen( pcszInputBDFFile, "r" );
	if( fpInput == NULL )
	{
		printf( "Cannot open InputBDFFile %s\n", pcszInputBDFFile );
		return 4;
	}

	char szOutputFontFile[1024];

	strcpy( szOutputFontFile, pcszOutputBinFile );	strcat( szOutputFontFile,  ".font" );

	FILE * fpOutputFont = fopen( szOutputFontFile, "w" );
	if( fpOutputFont == NULL )
	{
		printf( "Cannot open OutputFontFile %s\n", szOutputFontFile );
		fclose( fpInput );
		return 6;
	}

	const char * pcszStartChar = "STARTCHAR";
	const char * pcszEncoding = "ENCODING";
	const char * pcszBBX = "BBX";
	const char * pcszBitmap = "BITMAP";
	const char * pcszEndChar = "ENDCHAR";

	bool bCharacterStarted = false;
	bool bBitmapStarted = false;

	SPBitmapFont::CSPBitmapIndexManager indexManager;
	char szLineData[4096];

	SPBitmapFont::CSPBitmapElement * pBitmapElement = NULL;
	SPBitmapFont::CSPBitmapIndexElement * pBitmapIndexElement = NULL;
	uint8_t ui8LineIndex = 0;

	while( fgets( szLineData, 4096, fpInput ) )
	{
		if( strncmp( szLineData, pcszStartChar, strlen( pcszStartChar ) ) == 0 )
		{
			bCharacterStarted = true;

			if( pBitmapElement )
			{
				delete pBitmapElement;
			}
			if( pBitmapIndexElement )
			{
				delete pBitmapIndexElement;
			}

			pBitmapElement = new SPBitmapFont::CSPBitmapElement();
			pBitmapIndexElement = new SPBitmapFont::CSPBitmapIndexElement();
		}
		else if( bCharacterStarted )
		{
			if( strncmp( szLineData, pcszEndChar, strlen( pcszEndChar ) ) == 0 )
			{
				bCharacterStarted = false;
				bBitmapStarted = false;

				if( pBitmapElement && pBitmapIndexElement &&
					 pBitmapElement->GetWidth() != 0 &&
					 pBitmapElement->GetHeight() != 0 &&
					 pBitmapIndexElement->IsSetCode() )
				{
					if( OutputBitmapData(fpOutputFont, pBitmapElement, pBitmapIndexElement) )
					{
						indexManager.AddElement( pBitmapIndexElement );
						pBitmapIndexElement = NULL;
						delete pBitmapElement;
						pBitmapElement = NULL;
					}
				}
			}
			else if( bBitmapStarted )
			{
				for( int i=0; szLineData[i]; i++ )
				{
					if( szLineData[i] == '\r' ||
						szLineData[i] == '\n' )
					{
						szLineData[i] = '\0';
						break;
					}
				}
				pBitmapElement->SetBitmapDataText( ui8LineIndex, szLineData );
				ui8LineIndex++;
			}
			else if( strncmp( szLineData, pcszBitmap, strlen( pcszBitmap )) == 0 )
			{
				bBitmapStarted = true;
				ui8LineIndex = (uint8_t)0;
			}
			else
			{
				const char * pcszData[10];
				int iColumnIndex = 0;
				bool bPrevIsSpace = false;

				for( int i=0; szLineData[i]; i++ )
				{
					char cOneChar = szLineData[i];

					if( cOneChar == ' ' ||
						cOneChar == '\r' ||
						cOneChar == '\n' )
					{
						szLineData[i] = '\0';
						if( cOneChar != ' ' )
						{
							break;
						}
						bPrevIsSpace = true;
					}
					else if( i == 0 || bPrevIsSpace )
					{
						pcszData[iColumnIndex++] = szLineData+i;
						bPrevIsSpace = false;
					}
				}

				if( strncmp( szLineData, pcszBBX, strlen(pcszBBX) ) == 0 )
				{
					int iWidth = atoi( pcszData[1] );
					int iHeight = atoi( pcszData[2] );

					pBitmapElement->SetBitmapSize( (uint8_t)iWidth, (uint8_t)iHeight );
				}
				if( strncmp( szLineData, pcszEncoding, strlen( pcszEncoding )) == 0 )
				{
					uint16_t ui16InputCharcode = SPCommon::Get10Or16DigitsValue(pcszData[1] );

					CCharsetElement * pCharsetElement = charsetTable.SearchCharsetSortedBinary(ui16InputCharcode);

					if( pCharsetElement )
					{
						pBitmapIndexElement->SetCode( pCharsetElement->GetUTF16() );
					}
				}
			}
		}
	}

	fclose( fpOutputFont );
	fclose( fpInput );

	FILE * fpOutput = fopen( pcszOutputBinFile, "w" );
	if( fpOutput == NULL )
	{
		printf( "Cannot open Font Bin File %s\n", pcszOutputBinFile );
		fclose( fpOutputFont );
		return 6;
	}

	SPBitmapFont::CSPBitmapFileHeader bitmapFileHeader;
	bitmapFileHeader.SetCharset( "UTF-16" );
	bitmapFileHeader.SetIndexNumber( (uint32_t)indexManager.GetElementNumber() );
	bitmapFileHeader.SetSeekSize( 4 );
	bitmapFileHeader.SetCharcodeSize( 2 );

	//ヘッダーの出力
	bitmapFileHeader.WriteHeader( fpOutput );

	//インデックスの出力
	indexManager.WriteIndexListToFile( fpOutput );

	//後ろにバイナリーフォント情報をくっつける
	FILE * fpInputFont = fopen( szOutputFontFile, "r" );
	if( fpInputFont == NULL )
	{
		printf( "Cannot open input font %s...Why??\n", szOutputFontFile );
		fclose( fpOutput );
		return 7;
	}

	while( !feof( fpInputFont ) )
	{
		char szBuff[4096];
		size_t sizeRead = fread( szBuff, 1, 4096, fpInputFont );
		if( sizeRead > 0 )
		{
			fwrite( szBuff, 1, sizeRead, fpOutput );
		}
	}

	fclose( fpInputFont );
	fclose( fpOutput );

	remove( szOutputFontFile );

	printf( "Process ended!!" );

	return 0;
}

bool OutputBitmapData( FILE * fpOutput
						, SPBitmapFont::CSPBitmapElement * pBitmapElement
					    , SPBitmapFont::CSPBitmapIndexElement * pBitmapIndexElement )
{
	pBitmapIndexElement->SetSeek( (uint32_t)ftell( fpOutput ) );

	return pBitmapElement->WriteToBinFile( fpOutput );
}

