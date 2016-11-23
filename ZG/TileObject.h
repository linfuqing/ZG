#pragma once

#include "TileAction.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef ZGUINT(*ZGTILEOBJECTMOVE)(ZGUINT uElapsedTime, void* pTileActionObjectData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uFromIndex, ZGUINT uToIndex);
	typedef ZGUINT(*ZGTILEOBJECTSET)(ZGUINT uElapsedTime, void* pTileActionObjectData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uCount, LPZGTILENODE* ppTileNodes);

	typedef enum ZGTileObjectActionType
	{
		ZG_TILE_OBJECT_ACTION_TYPE_INDEX,
		ZG_TILE_OBJECT_ACTION_TYPE_DEPTH, 
		ZG_TILE_OBJECT_ACTION_TYPE_BREADTH
	}ZGTILEOBJECTACTIONTYPE;

	typedef struct ZGTileObjectAction
	{
		LPZGTILEACTION pInstance;

		void* pData;

		ZGUINT uChildCount;
		struct ZGTileObjectAction** ppChildren;
	}ZGTILEOBJECTACTION, *LPZGTILEOBJECTACTION;

	typedef struct ZGTileObject
	{
		ZGTILENODE Instance;
		ZGINT nTime;
		LPZGTILEOBJECTACTION pActions;
	}ZGTILEOBJECT, *LPZGTILEOBJECT;

	ZGUINT ZGTileObjectRun(
		LPZGTILEOBJECT pTileObject,
		LPZGTILEMAP pTileMap,
		ZGUINT uTime, 
		ZGUINT uBufferLength,
		PZGUINT8 puBuffer,
		ZGTILEACTIONTEST pfnTileActionTest,
		ZGTILEOBJECTMOVE pfnTileObjectMove, 
		ZGTILEOBJECTSET pfnTileObjectSet);
#ifdef __cplusplus
}
#endif
