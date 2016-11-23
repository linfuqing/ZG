#pragma once

#include "TileMap.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct ZGTileNodeData
	{
		ZGTILERANGE Instance;
		ZGUINT uDistance;
		ZGUINT uRange;
	}ZGTILENODEDATA, *LPZGTILENODEDATA;

	typedef struct ZGTileNode
	{
		LPZGTILENODEDATA pInstance;

		ZGUINT uIndex;

		void* pData;
	}ZGTILENODE, *LPZGTILENODE;

	typedef struct ZGTileNodeMapNode
	{
		ZGUINT uDistance;

		LPZGTILENODE pNode;

		void* pData;
	}ZGTILENODEMAPNODE, *LPZGTILENODEMAPNODE;

	ZG_INLINE ZGUINT ZGTilePredicate(const void* x, const void* y)
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

	ZGUINT ZGTileNodeSearchDepth(
		const ZGTILENODE* pTileNode,
		LPZGTILEMAP pTileMap,
		ZGBOOLEAN bIsTest,
		ZGUINT uIndex, 
		ZGMAPTEST pfnMapTest);

	ZGUINT ZGTileNodeSearchBreadth(
		const ZGTILENODE* pTileNode,
		LPZGTILEMAP pTileMap,
		ZGMAPTEST pfnMapTest,
		ZGTILEMAPTEST pfnTileMapTest);
#ifdef __cplusplus
}
#endif

