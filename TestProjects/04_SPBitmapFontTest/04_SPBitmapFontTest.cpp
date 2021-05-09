#include <stdio.h>

#include "../../SPBitmapFont/SPBitmapSelect.h"

static void OutputFont( SPBitmapFont::CSPBitmapElement * pElement );

int main( int argc, char * argv[] )
{
	SPBitmapFont::CSPBitmapFont bitmapJiskan24;
	bitmapJiskan24.SetFontName( "ascii_jiskan_24" );

	SPBitmapFont::CSPBitmapSelect selectSet;
	selectSet.GetFonts( bitmapJiskan24, L"温度：5.3℃"
						, 24, 24 );

	for( int y=0; y<24;y++ )
	{
		for( SPBitmapFont::CSPBitmapElement * pElement = selectSet.GetFirstBitmap()
				; pElement
				; pElement = pElement->GetNextElement() )
		{
			for( int x=0; x<pElement->GetWidth(); x++ )
			{
				if( pElement->GetPoint( x, y ) )
				{
					printf( "■" );
				}
				else
				{
					printf( "□" );
				}
			}
		}
		printf( "\n" );
	}

	return 0;
}

void OutputFont( SPBitmapFont::CSPBitmapElement * pElement )
{
	uint8_t ui8Width = pElement->GetWidth();
	uint8_t ui8Height = pElement->GetHeight();

	for( uint8_t y=0; y<ui8Height; y++ )
	{
		for( uint8_t x=0; x<ui8Width; x++ )
		{
			if( pElement->GetPoint( x, y ) )
			{
				printf( "■" );
			}
			else
			{
				printf( "□" );
			}
		}
		printf( "\n" );
	}
	printf( "\n" );
}
