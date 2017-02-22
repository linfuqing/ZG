#pragma once

#include "TileAction.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef ZGUINT(*ZGTILEOBJECTCHECK)(
		void* pTileObjectActionData, 
		LPZGTILENODE pTileNode);

	typedef ZGUINT(*ZGTILEOBJECTMOVE)(
		ZGUINT uElapsedTime, 
		void* pTileObjectActionData, 
		void* pTileNodeData, 
		LPZGTILEMAP pTileMap, 
		ZGUINT uFromIndex, 
		ZGUINT uToIndex);

	typedef ZGUINT(*ZGTILEOBJECTSET)(
		ZGUINT uElapsedTime, 
		void* pTileObjectActionData, 
		void* pTileNodeData, 
		LPZGTILEMAP pTileMap, 
		ZGUINT uIndex, 
		void** ppUserData/*,
		ZGUINT uCount, 
		LPZGTILENODE* ppTileNodes*/);

	typedef void*(*ZGTILEOBJECTPOP)();
	typedef void(*ZGTILEOBJECTPUSH)(void* pUserData);

	typedef struct ZGTileObjectAction
	{
		ZGTILEOBJECTCHECK pfnCheck;
		ZGTILEOBJECTMOVE pfnMove;
		ZGTILEOBJECTSET pfnSet;

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
		ZGUINT uTime, 
		ZGTILEOBJECTPOP pfnTileObjectPop, 
		ZGTILEOBJECTPUSH pfnTileObjectPush);
#ifdef __cplusplus
}
#endif
