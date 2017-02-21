#pragma once

#include "TileNode.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef ZGBOOLEAN (*ZGTILEACTIONTEST)(const void* pTileNodeData, const LPZGTILENODE* ppTileNodes, ZGUINT uNodeCount);
	typedef ZGBOOLEAN (*ZGTILEACTIONANALYZATION)(const void* pSourceTileNodeData, const void* pDestinationTileNodeData);

	typedef struct ZGTileActionMapNode
	{
		LPZGTILENODE* ppNodes;
		ZGUINT uCount;
		ZGUINT uMaxCount;
		ZGUINT uMaxIndex;

		void* pData;
	}ZGTILEACTIONMAPNODE, *LPZGTILEACTIONMAPNODE;

	typedef struct ZGTileActionData
	{
		ZGTILERANGE Instance;
		ZGTILERANGE Distance;
	}ZGTILEACTIONDATA, *LPZGTILEACTIONDATA;

	typedef struct ZGTileAction
	{
		LPZGTILEACTIONDATA pInstance;

		ZGUINT uEvaluation;
		ZGUINT uMinEvaluation;
		ZGUINT uMaxEvaluation;
		ZGUINT uMaxDistance;
		ZGUINT uMaxDepth;

		ZGTILEACTIONANALYZATION pfnAnalyzation;
	}ZGTILEACTION, *LPZGTILEACTION;

	ZGUINT ZGTileActionSearchBreadth(
		const ZGTILEACTION* pTileAction,
		const ZGTILENODE* pTileNode,
		ZGUINT uBufferLength,
		PZGUINT8 puBuffer,
		ZGTILEACTIONTEST pfnTileActionTest);
#ifdef __cplusplus
}
#endif


