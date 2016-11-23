#pragma once

#include "Core.h"

#define ZG_TEST_BIT(uFlag, uBit) (((uFlag) & (1 << (uBit))) != 0)
#define ZG_SET_BIT(uFlag, uBit) (uFlag |= (1 << (uBit)))

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct ZGBitFlag
	{
		PZGUINT8 puFlags;
		ZGUINT uOffset;
		ZGUINT uCount;
	}ZGBITFLAG, *LPZGBITFLAG;

	//const ZGUINT8 g_MAXINUM_BIT_TABLE[];
	ZG_INLINE ZGUINT8 ZGMininumBit8(ZGUINT uFlag)
	{
		static const ZGUINT8 s_uMININUM_BIT_TABLE[] =
		{
			0, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
			7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
			8, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
			7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
			6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1
		};

		return s_uMININUM_BIT_TABLE[uFlag];
	}

	ZG_INLINE ZGUINT8 ZGMaxinumBit8(ZGUINT uFlag)
	{
		static const ZGUINT8 s_uMAXINUM_BIT_TABLE[] =
		{
			0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
		};

		return s_uMAXINUM_BIT_TABLE[uFlag];
	}

	ZG_INLINE ZGBOOLEAN ZGBitFlagSet(const ZGBITFLAG* pBitFlag, ZGUINT uIndex, ZGBOOLEAN bValue)
	{
		if (pBitFlag == ZG_NULL || pBitFlag->uCount <= uIndex)
			return ZG_FALSE;

		uIndex += pBitFlag->uOffset;
		if(bValue)
			pBitFlag->puFlags[uIndex >> 3] |= 1 << (uIndex & 7);
		else
			pBitFlag->puFlags[uIndex >> 3] &= ~(1 << (uIndex & 7));

		return ZG_TRUE;
	}

	ZG_INLINE ZGBOOLEAN ZGBitFlagGet(const ZGBITFLAG* pBitFlag, ZGUINT uIndex)
	{
		if (pBitFlag == ZG_NULL || pBitFlag->uCount <= uIndex)
			return ZG_FALSE;

		uIndex += pBitFlag->uOffset;
		return (pBitFlag->puFlags[uIndex >> 3] & (1 << (uIndex & 7))) != 0;
	}

	ZGUINT ZGBitFlagMinBit(const ZGBITFLAG* pBitFlag);

	ZGUINT ZGBitFlagIndexOf(const ZGBITFLAG* pSource, const ZGBITFLAG* pDestination);

	void ZGBitFlagAssign(LPZGBITFLAG pSource, const ZGBITFLAG* pDestination, ZGBOOLEAN bValue);
#ifdef __cplusplus
}
#endif