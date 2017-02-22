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
		LPZGTILEMAP pTileMap;
		ZGUINT uIndex;

		void* pData;
	}ZGTILENODE, *LPZGTILENODE;

	typedef struct ZGTileNodeMapNode
	{
		//ZGUINT uDistance;

		LPZGTILENODE pNode;

		void* pData;
	}ZGTILENODEMAPNODE, *LPZGTILENODEMAPNODE;

	/*ZG_INLINE ZGUINT ZGTilePredicate(const void* x, const void* y)
	{
		const ZGTILENODEMAPNODE* pTemp = (const ZGTILENODEMAPNODE*)((const ZGTILEMAPNODE*)x)->pData;

		return pTemp->uDistance;
	}*/

	ZGBOOLEAN ZGTileNodeUnset(LPZGTILENODE pTileNode);

	ZGBOOLEAN ZGTileNodeSetTo(
		LPZGTILENODE pTileNode,
		LPZGTILEMAP pTileMap,
		ZGUINT uIndex);

	ZGUINT ZGTileNodeSearchDepth(
		const ZGTILENODE* pTileNode,
		//ZGBOOLEAN bIsTest,
		ZGUINT uIndex, 
		ZGNODEPREDICATION pfnPredication,
		ZGMAPTEST pfnMapTest);

	ZGUINT ZGTileNodeSearchBreadth(
		const ZGTILENODE* pTileNode,
		ZGNODEPREDICATION pfnPredication,
		ZGMAPTEST pfnMapTest,
		ZGTILEMAPTEST pfnTileMapTest);
#ifdef __cplusplus
}
#endif

