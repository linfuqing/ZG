#include "TileObject.h"

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
	typedef struct ZGTileManagerHandler
	{
		void* pUserData;
		struct ZGTileManagerHandler* pNext;
	}ZGTILEMANAGERHANDLER, *LPZGTILEMANAGERHANDLER;


	typedef struct ZGTileManagerObject
	{
		ZGTILEOBJECT Instance;
		LPZGTILEMANAGERHANDLER pHandler;
		struct ZGTileManagerObject* pPrevious;
		struct ZGTileManagerObject* pNext;
	}ZGTILEMANAGEROBJECT, *LPZGTILEMANAGEROBJECT;

	typedef struct ZGTileManager
	{
		LPZGTILEMANAGEROBJECT pObjects;
		LPZGTILEMANAGERHANDLER pQueue;
		LPZGTILEMANAGERHANDLER pPool;
	}ZGTILEMANAGER, *LPZGTILEMANAGER;

	typedef ZGUINT(*ZGTILEMANAGERSET)(
		ZGUINT uElapsedTime,
		void* pTileActionObjectData, 
		void* pTileNodeData, 
		LPZGTILEMAP pTileMap,
		ZGUINT uIndex, 
		ZGUINT uCount, 
		LPZGTILENODE* ppTileNodes, 
		void** ppUserData);

	typedef ZGBOOLEAN(*ZGTILEMANAGERHAND)(ZGUINT uTime, void* pUserData);

	typedef void(*ZGTILEMANAGERDELAY)(void* pTileNodeData, ZGUINT uElapsedTime, ZGUINT uTime);

	ZG_INLINE ZGBOOLEAN ZGTileManagerAdd(LPZGTILEMANAGER pTileManager, LPZGTILEMANAGEROBJECT pTileManagerObject)
	{
		if (pTileManager == ZG_NULL || pTileManagerObject == ZG_NULL)
			return ZG_FALSE;

		if (pTileManager->pObjects != ZG_NULL)
			pTileManager->pObjects->pPrevious = pTileManagerObject;

		pTileManagerObject->pPrevious = ZG_NULL;
		pTileManagerObject->pNext = pTileManager->pObjects;

		pTileManager->pObjects = pTileManagerObject;

		return ZG_TRUE;
	}

	ZG_INLINE ZGBOOLEAN ZGTileManagerRemove(LPZGTILEMANAGER pTileManager, LPZGTILEMANAGEROBJECT pTileManagerObject)
	{
		if (pTileManager == ZG_NULL || pTileManagerObject == ZG_NULL)
			return ZG_FALSE;

		if (pTileManager->pObjects == pTileManagerObject)
			pTileManager->pObjects = pTileManagerObject->pNext;

		if (pTileManagerObject->pPrevious != ZG_NULL)
			pTileManagerObject->pPrevious->pNext = pTileManagerObject->pNext;

		if (pTileManagerObject->pNext != ZG_NULL)
			pTileManagerObject->pNext->pPrevious = pTileManagerObject->pPrevious;

		pTileManagerObject->pPrevious = ZG_NULL;
		pTileManagerObject->pNext = ZG_NULL;

		return ZG_TRUE;
	}

	ZGBOOLEAN ZGTileManagerSet(
		LPZGTILEMANAGER pTileManager,
		LPZGTILEMANAGEROBJECT pTileManagerObject,
		void* pTileActionObjectData,
		ZGUINT uIndex,
		ZGUINT uTime,
		ZGTILEMANAGERSET pfnTileManagerSet);

	void ZGTileManagerRun(
		LPZGTILEMANAGER pTileManager, 
		ZGUINT uTime,
		ZGUINT uBufferLength,
		PZGUINT8 puBuffer,
		ZGTILEACTIONTEST pfnTileActionTest,
		ZGTILEOBJECTMOVE pfnTileObjectMove, 
		ZGTILEMANAGERDELAY pfnTileManagerDelay,
		ZGTILEMANAGERSET pfnTileManagerSet,
		ZGTILEMANAGERHAND pfnTileManagerHand);
#ifdef __cplusplus
}
#endif
