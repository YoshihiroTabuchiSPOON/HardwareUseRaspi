#ifndef _SPBITMAPFONTMANAGER_H_
#define _SPBITMAPFONTMANAGER_H_

#include "SPBitmapFont.h"

namespace SPBitmapFont
{
class CSPBitmapFontListElement : public CSPBitmapFont
{
	friend class CSPBitmapFontList;
public:
	CSPBitmapFontListElement();
	virtual ~CSPBitmapFontListElement();

protected:
	CSPBitmapFontListElement * m_pNextElement;
};

class CSPBitmapFontList
{
public:
	CSPBitmapFontList();
	virtual ~CSPBitmapFontList();

	CSPBitmapFont * GetFont( const char * pcszFontName );

protected:
	void AddElement( CSPBitmapFontListElement * pElement );
	void ReleaseAllElement();

	CSPBitmapFontListElement * m_pFirstElement;
	CSPBitmapFontListElement * m_pLastElement;
};
}

#endif
