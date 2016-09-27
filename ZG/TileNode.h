#pragma once

#include "TileMap.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef ZGBOOLEAN (*ZGTILENODETEST)(const void* pTileNodeData, const ZGTILERANGE* pTileRange, LPZGTILEMAP pTileMap, ZGUINT uIndex);

	typedef struct ZGTileNode
	{
		ZGTILERANGE Instance;
		ZGUINT uDistance;
		ZGUINT uRange;

		ZGUINT uIndex;

		void* pData;
	}ZGTILENODE, *LPZGTILENODE;

	typedef struct ZGTileNodeMapNode
	{
		ZGUINT uDistance;

		LPZGTILENODE pNode;

		void* pData;
	}ZGTILENODEMAPNODE, *LPZGTILENODEMAPNODE;

	inline ZGUINT ZGTilePredicate(const void* x, const void* y)
	{
		const ZGTILENODEMAPNODE* pTemp = (const ZGTILENODEMAPNODE*)((const ZGTILEMAPNODE*)x)->pData;

		return pTemp->uDistance;
	}

	ZGBOOLEAN ZGTileNodeUnset(
		LPZGTILENODE pTileNode,
		LPZGTILEMAP pTileMap);

	ZGBOOLEAN ZGTileNodeSetTo(
		LPZGTILENODE pTileNode,
		LPZGTILEMAP pTileMap,
		ZGUINT uIndex);

	ZGUINT ZGTileNodeSearch(
		const ZGTILENODE* pTileNode,
		LPZGTILEMAP pTileMap,
		ZGBOOLEAN bIsTest,
		ZGUINT uIndex, 
		ZGTILENODETEST pfnTest);
#ifdef __cplusplus
}
#endif

