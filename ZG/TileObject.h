#pragma once

#include "TileAction.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef void (*ZGTILEOBJECTSET)(void* pTileActionData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uCount, LPZGTILENODE* ppTileNodes);

	typedef struct ZGTileObject
	{
		ZGTILENODE Instance;
		ZGUINT uActionCount;
		LPZGTILEACTION* ppActions;
	}ZGTILEOBJECT, *LPZGTILEOBJECT;

	ZGUINT ZGTileObjectRun(
		LPZGTILEOBJECT pTileObject,
		LPZGTILEMAP pTileMap,
		ZGUINT uEvaluation,
		ZGUINT uMinEvaluation,
		ZGUINT uMaxEvaluation, 
		ZGUINT uMaxDistance,
		ZGUINT uMaxDepth,
		ZGUINT uBufferLength,
		PZGUINT8 puBuffer,
		PZGUINT puIndex, 
		ZGTILEACTIONTEST pfnTileTest,
		ZGTILEOBJECTSET pfnTileSet);
#ifdef __cplusplus
}
#endif
