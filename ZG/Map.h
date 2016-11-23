#pragma once

#include "BitFlag.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct ZGMap
	{
		ZGBITFLAG Instance;
		ZGUINT uPitch;
	}ZGMAP, *LPZGMAP;

	typedef ZGBOOLEAN(*ZGMAPTEST)(LPZGMAP pMap, ZGUINT uIndex);

	ZG_INLINE ZGBOOLEAN ZGMapGet(const ZGMAP* pMap, ZGUINT uIndex)
	{
		return ZGBitFlagGet(&pMap->Instance, uIndex);
	}

	ZG_INLINE ZGBOOLEAN ZGMapSet(const ZGMAP* pMap, ZGUINT uIndex, ZGBOOLEAN bValue)
	{
		return ZGBitFlagSet(&pMap->Instance, uIndex, bValue);
	}

	ZGBOOLEAN ZGMapTest(
		const ZGMAP* pSource,
		const ZGMAP* pDestination,
		ZGUINT uIndex,
		ZGUINT uOffset,
		PZGUINT puCount,
		PZGUINT puIndices);

	ZGBOOLEAN ZGMapVisit(
		LPZGMAP pSource,
		const ZGMAP* pDestination,
		ZGUINT uIndex,
		ZGUINT uOffset,
		ZGMAPTEST pfnTest);

	void ZGMapAssign(
		LPZGMAP pSource, 
		const ZGMAP* pDestination, 
		ZGUINT uIndex, 
		ZGUINT uOffset, 
		ZGBOOLEAN bValue);
#ifdef __cplusplus
}
#endif