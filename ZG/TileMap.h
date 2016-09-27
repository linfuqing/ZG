#pragma once

#include "Map.h"
#include "Node.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct ZGTileRange
	{
		ZGMAP Instance;
		ZGUINT uOffset;
	}ZGTILERANGE, *LPZGTILERANGE;

	typedef struct ZGTileMapNode
	{
		ZGUINT uIndex;

		void* pData;
	}ZGTILEMAPNODE, *LPZGTILEMAPNODE;

	typedef struct ZGTileMap
	{
		ZGMAP Instance;
		LPZGNODE pNodes;
	}ZGTILEMAP, *LPZGTILEMAP;

	typedef ZGBOOLEAN(*ZGTILEMAPTEST)(LPZGTILEMAP pTileMap, const ZGTILERANGE* pTileRange, ZGUINT uIndex);

	inline ZGUINT ZGTileConvert(
		ZGUINT uSourcePitch,
		ZGUINT uSourceIndex, 
		ZGUINT uDestinationPitch, 
		ZGUINT uDestinationIndex,
		ZGUINT uDestinationOffset)
	{
		ZGUINT x = uSourceIndex % uSourcePitch + uDestinationIndex % uDestinationPitch, 
			y = uSourceIndex / uSourcePitch + uDestinationIndex / uDestinationPitch,
			uOffsetX = uDestinationOffset % uDestinationPitch,
			uOffsetY = uDestinationOffset / uDestinationPitch;

		if (x < uOffsetX || y < uOffsetY)
			return 0;

		return x - uOffsetX + (y - uOffsetY) * uSourcePitch + 1;
	}

	inline ZGUINT ZGTileChildCount(ZGUINT uWidth, ZGUINT uHeight, ZGBOOLEAN bIsOblique)
	{
		if (uWidth < 1 || uHeight < 1)
			return 0;

		if (uWidth == 1 && uHeight == 1)
			return 0;

		ZGUINT uCorner, uEdge, uNormal;
		if (bIsOblique)
		{
			uCorner = 3;
			uEdge = 5;
			uNormal = 8;
		}
		else
		{
			uCorner = 2;
			uEdge = 3;
			uNormal = 4;
		}

		return (uCorner << 2) + uEdge * ((uWidth + uHeight - 4) << 1) + uNormal * (uWidth - 1) * (uHeight - 1);
	}

	inline void ZGTileMapSet(const ZGTILEMAP* pTileMap, ZGUINT uIndex, ZGBOOLEAN bValue)
	{
		if (pTileMap == ZG_NULL)
			return;

		ZGMapSet(&pTileMap->Instance, uIndex, bValue);
	}

	inline ZGBOOLEAN ZGTileMapGet(const ZGTILEMAP* pTileMap, ZGUINT uIndex)
	{
		if (pTileMap == ZG_NULL)
			return ZG_FALSE;

		return ZGMapGet(&pTileMap->Instance, uIndex);
	}

	inline void ZGTileMapSetData(const ZGTILEMAP* pTileMap, ZGUINT uIndex, void* pData)
	{
		if (pTileMap == ZG_NULL || pTileMap->Instance.Instance.uCount <= uIndex)
			return;

		((LPZGTILEMAPNODE)(pTileMap->pNodes + uIndex)->pData)->pData = pData;
	}

	inline void* ZGTileMapGetData(const ZGTILEMAP* pTileMap, ZGUINT uIndex)
	{
		if (pTileMap == ZG_NULL || pTileMap->Instance.Instance.uCount <= uIndex)
			return ZG_NULL;

		return ((LPZGTILEMAPNODE)(pTileMap->pNodes + uIndex)->pData)->pData;
	}

	void ZGTileRangeInitOblique(LPZGTILERANGE pRange, PZGUINT8 puFlags, ZGUINT uSize);

	ZGUINT ZGTileMapSearch(
		LPZGTILEMAP pTileMap,
		const ZGTILERANGE* pTileRange,
		ZGBOOLEAN bIsTest,
		ZGUINT uFromIndex,
		ZGUINT uToIndex,
		ZGUINT uMaxDepth,
		ZGUINT uMaxDistance,
		ZGNODEPREDICATION pfnPredication, 
		ZGTILEMAPTEST pfnTest);

	void ZGTileMapEnable(
		LPZGTILEMAP pTileMap,
		PZGUINT8 puFlags,
		LPZGNODE* ppNodes,
		LPZGNODE pNodes,
		LPZGTILEMAPNODE pMapNodes,
		ZGUINT uWidth,
		ZGUINT uHeight,
		ZGBOOLEAN bIsOblique);

	void ZGTileMapDisable(LPZGTILEMAP pTileMap);
#ifdef __cplusplus
}
#endif