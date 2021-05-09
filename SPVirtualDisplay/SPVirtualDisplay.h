#ifndef _SPVIRTUALDISPLAY_H_
#define _SPVIRTUALDISPLAY_H_

#include <stdint.h>
#include "../SPBitmapFont/SPBitmapFontManager.h"

namespace SPVirtualDisplay
{
class CSPVirtualDisplay
{
public:
	CSPVirtualDisplay();
	virtual ~CSPVirtualDisplay();

	void SetDisplaySize( uint16_t ui16DisplayWidth
						  , uint16_t ui16DisplayHeight );

	virtual void DrawHorzLine( uint16_t ui16XPosStart
								, uint16_t ui16YPos
								, uint16_t ui16XPosEnd
								, uint32_t ui32PointInfo );
	virtual void DrawVertLine( uint16_t ui16XPos
								, uint16_t ui16YPosStart
								, uint16_t ui16YPosEnd
								, uint32_t ui32PointInfo );



	virtual void SetPoint( uint16_t ui16XPos, uint16_t ui16YPos
							, uint32_t ui32PointInfo ) = 0;
	virtual uint32_t GetPoint( uint16_t ui16XPos, uint16_t ui16YPos ) = 0;
	virtual void ClearDisplay() = 0;

	virtual void DrawString( uint16_t ui16XPos
								,uint16_t ui16YPos
								,const char * pcszFontName
								,const wchar_t * pcwszString
								,int iDefaultHeight
								,uint32_t ui32TextPointInfo = 0xffffffff
								,uint32_t ui32BackPointInfo = 0x00000000);

protected:
	void ReleaseDisplayBuffer();

	virtual uint32_t GetDisplayBufferSize() = 0;
	uint16_t m_ui16DisplayWidth;
	uint16_t m_ui16DisplayHeight;
	uint8_t * m_pDisplayBuffer;

	bool InDisplay( uint16_t ui16XPos, uint16_t ui16YPos );
	SPBitmapFont::CSPBitmapFontList m_fontList;
};
}

#endif
