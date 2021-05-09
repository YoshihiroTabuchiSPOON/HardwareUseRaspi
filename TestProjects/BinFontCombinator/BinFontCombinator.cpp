#include <stdio.h>
#include <unistd.h>

#include "../../SPBitmapFont/SPBitmapFont.h"

using namespace SPBitmapFont;

static SPBitmapFont::CSPBitmapIndexElement * ReadIndex
		( FILE * fp, uint8_t ui8CodeSize, uint8_t ui8SeekSize );

static bool CopyLeftData( FILE * fpInp, FILE * fpOut );

int main( int argc, char * argv[] )
{
	if( argc < 4 )
	{
		printf( "Usage: \n" );
		printf( "./BinFontCombinator [Combinate1 filepath] [Combinate2 filepath] [Combinated filepath]\n" );
		return 0;
	}

	FILE * fpCombinate1 = fopen( argv[1], "r" );
	if( !fpCombinate1 )
	{
		printf( "Cannot open combinate1 file %s\n", argv[1] );
		return 1;
	}

	FILE * fpCombinate2 = fopen( argv[2], "r" );
	if( !fpCombinate2 )
	{
		printf( "Cannot open combinate2 file %s\n", argv[2] );
		fclose( fpCombinate1 );
		return 2;
	}

	char szCombinatedBitmap[1024];
	snprintf( szCombinatedBitmap, 1024, "%s.bitmap", argv[3] );

	FILE * fpCombinatedBitmap = fopen( szCombinatedBitmap, "w" );
	if( !fpCombinatedBitmap )
	{
		printf( "Cannot open combinated_Bitmap file %s\n", argv[3] );
		fclose( fpCombinate1 );
		fclose( fpCombinate2 );
		return 3;
	}

	SPBitmapFont::CSPBitmapFileHeader headerCombinate1;
	SPBitmapFont::CSPBitmapFileHeader headerCombinate2;
	SPBitmapFont::CSPBitmapIndexManager indexManager;

	headerCombinate1.ReadHeader( fpCombinate1 );
	headerCombinate2.ReadHeader( fpCombinate2 );

	int iIndexNumber = (int)headerCombinate1.GetIndexNumber();
	for( int i=0; i<iIndexNumber; i++ )
	{
		SPBitmapFont::CSPBitmapIndexElement * pIndexElement =
				ReadIndex( fpCombinate1
						  , headerCombinate1.GetCharcodeSize()
						  , headerCombinate1.GetSeekSize() );

		indexManager.AddElement( pIndexElement );
	}

	CopyLeftData( fpCombinate1, fpCombinatedBitmap );

	int iSeekOffset = ftell( fpCombinatedBitmap );

	iIndexNumber = (int)headerCombinate2.GetIndexNumber();
	for( int i=0; i<iIndexNumber; i++ )
	{
		SPBitmapFont::CSPBitmapIndexElement * pIndexElement =
				ReadIndex( fpCombinate2
						, headerCombinate2.GetCharcodeSize()
						, headerCombinate2.GetSeekSize() );

		pIndexElement->SetSeek( pIndexElement->GetSeek() + iSeekOffset );
		indexManager.AddElement( pIndexElement );
	}

	CopyLeftData( fpCombinate2, fpCombinatedBitmap );

	fclose( fpCombinatedBitmap );
	fclose( fpCombinate1 );
	fclose( fpCombinate2 );

	uint8_t ui8SeekSize = headerCombinate1.GetSeekSize();
	if( ui8SeekSize < headerCombinate2.GetSeekSize() )
	{
		ui8SeekSize = headerCombinate2.GetSeekSize();
	}

	uint8_t ui8CodeSize = headerCombinate1.GetCharcodeSize();
	if( ui8CodeSize < headerCombinate2.GetCharcodeSize() )
	{
		ui8CodeSize = headerCombinate2.GetCharcodeSize();
	}

	headerCombinate1.SetCharcodeSize( ui8CodeSize );
	headerCombinate1.SetSeekSize( ui8SeekSize );
	headerCombinate1.SetIndexNumber
		(headerCombinate1.GetIndexNumber() + headerCombinate2.GetIndexNumber() );

	FILE * fpCombinated = fopen( argv[3], "w" );
	if( fpCombinated == NULL )
	{
		printf( "cannot open combinated file %s\n", argv[3] );
		return 5;
	}

	headerCombinate1.WriteHeader( fpCombinated );
	indexManager.WriteIndexListToFile( fpCombinated );

	fpCombinatedBitmap = fopen( szCombinatedBitmap, "r" );
	if( !fpCombinatedBitmap )
	{
		printf( "Cannot open CombinatedBitmap for read%s\n", szCombinatedBitmap );
		fclose( fpCombinated );
	}

	CopyLeftData( fpCombinatedBitmap, fpCombinated );

	fclose( fpCombinatedBitmap );

	remove( szCombinatedBitmap );

	fclose( fpCombinated );

	printf( "Process ended\n" );

	return 0;
}

SPBitmapFont::CSPBitmapIndexElement * ReadIndex
		( FILE * fp, uint8_t ui8CodeSize, uint8_t ui8SeekSize )
{
	uint32_t ui32Charcode = 0;
	if( ui8CodeSize == 1 )
	{
		uint8_t ui8Charcode = 0;
		fread( &ui8Charcode, 1, 1, fp );
		ui32Charcode = ui8Charcode;
	}
	else if( ui8CodeSize == 2 )
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

	SPBitmapFont::CSPBitmapIndexElement * pIndexElement =
			new SPBitmapFont::CSPBitmapIndexElement();

	pIndexElement->SetCode( ui32Charcode );
	pIndexElement->SetSeek( ui32Seek );

	return pIndexElement;
}

bool CopyLeftData( FILE * fpInp, FILE * fpOut )
{
	char szBuff[8192];

	while( !feof( fpInp ) )
	{
		size_t sizeRead = fread( szBuff, 1, 8192, fpInp );
		if( sizeRead > 0 )
		{
			fwrite( szBuff, 1, sizeRead, fpOut );
		}
	}

	return true;
}
