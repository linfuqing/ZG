#pragma once

#include "TileNode.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef ZGBOOLEAN (*ZGTILEACTIONTEST)(const void* pTileActionData, const void* pTileNodeData, const LPZGTILENODE* ppTileNodes, ZGUINT uNodeCount);
	typedef ZGBOOLEAN (*ZGTILEACTIONANALYZATION)(const void* pTileActionData, const void* pSourceTileNodeData, const void* pDestinationTileNodeData);

	typedef struct ZGTileActionMapNode
	{
		LPZGTILENODE* ppNodes;
		ZGUINT uCount;
		ZGUINT uMaxCount;
		ZGUINT uMaxIndex;

		void* pData;
	}ZGTILEACTIONMAPNODE, *LPZGTILEACTIONMAPNODE;

	typedef struct ZGTileAction
	{
		ZGTILERANGE Instance;
		ZGTILERANGE Distance;

		void* pData;

		ZGTILEACTIONANALYZATION pfnAnalyzation;
	}ZGTILEACTION, *LPZGTILEACTION;

	ZGUINT ZGTileActionSearchBreadth(
		const ZGTILEACTION* pTileAction,
		const ZGTILENODE* pTileNode,
		LPZGTILEMAP pTileMap,
		ZGUINT uEvaluation,
		ZGUINT uMinEvaluation,
		ZGUINT uMaxEvaluation,
		ZGUINT uMaxDistance,
		ZGUINT uMaxDepth,
		ZGUINT uBufferLength,
		PZGUINT8 puBuffer,
		ZGTILEACTIONTEST pfnTileActionTest);
#ifdef __cplusplus
}
#endif


