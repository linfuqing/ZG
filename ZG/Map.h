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

	inline ZGBOOLEAN ZGMapGet(const ZGMAP* pMap, ZGUINT uIndex)
	{
		return ZGBitFlagGet(&pMap->Instance, uIndex);
	}

	inline void ZGMapSet(const ZGMAP* pMap, ZGUINT uIndex, ZGBOOLEAN bValue)
	{
		ZGBitFlagSet(&pMap->Instance, uIndex, bValue);
	}

	ZGBOOLEAN ZGMapTest(
		const ZGMAP* pSource,
		const ZGMAP* pDestination,
		ZGUINT uIndex,
		ZGUINT uOffset,
		PZGUINT puCount,
		PZGUINT puIndices);

	void ZGMapAssign(
		LPZGMAP pSource, 
		const ZGMAP* pDestination, 
		ZGUINT uIndex, 
		ZGUINT uOffset, 
		ZGBOOLEAN bValue);
#ifdef __cplusplus
}
#endif