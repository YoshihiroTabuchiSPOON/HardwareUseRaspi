#ifndef _CHARSETTABLE_H_
#define _CHARSETTABLE_H_

#include <stdint.h>

class CCharsetElement
{
	friend class CCharsetTable;
public:
	CCharsetElement(uint32_t ui16SJIS, uint16_t ui16JIS, uint16_t ui16UTF16);
	virtual ~CCharsetElement();

	inline uint16_t GetSJIS()
	{
		return this->m_ui16SJIS;
	}
	inline uint16_t GetJIS()
	{
		return this->m_ui16JIS;
	}
	inline uint16_t GetUTF16()
	{
		return this->m_ui16UTF16;
	}

protected:
	uint16_t m_ui16SJIS;
	uint16_t m_ui16JIS;
	uint16_t m_ui16UTF16;

	CCharsetElement * m_pNextElement;
	class CCharsetTable * m_pParentTable;
};

class CCharsetTable
{
public:
	enum ECharsetType
	{
		CharsetUnknown,

		CharsetSJIS,
		CharsetJIS,
		CharsetUTF16,
	};
	CCharsetTable();
	virtual ~CCharsetTable();

	bool StructCharsetTable( const char * pcszFileName, ECharsetType charsetType );
	CCharsetElement * SearchCharsetSortedBinary( uint16_t ui16Charcode );

protected:
	CCharsetElement * m_pFirstElement;
	CCharsetElement * m_pLastElement;

	int m_iCharsetNumber;

	ECharsetType m_charsetSorted;
	inline ECharsetType GetCharsetSorted()
	{
		return this->m_charsetSorted;
	}
	int m_iSortedNumber;
	CCharsetElement ** m_ppSortedArray;

	void AddElement( CCharsetElement * pElement );
	void CreateSortedArray(ECharsetType charsetType);

	void ReleaseSortedArray();
	void ReleaseAllElement();

	static int CharsetSorter( const void * pVoid1
								, const void * pVoid2 );

};


#endif
