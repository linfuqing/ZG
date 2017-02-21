#include "TileManager.h"

static LPZGTILEMANAGERHANDLER sg_pHead;
static LPZGTILEMANAGERHANDLER sg_pTail;
static LPZGTILEMANAGERHANDLER sg_pPool;
static ZGTILEMANAGERSET sg_pfnTileManagerSet;

ZGUINT __ZGTileManagerSet(ZGUINT uElapsedTime, void* pTileActionObjectData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uCount, LPZGTILENODE* ppTileNodes)
{
	if (sg_pfnTileManagerSet == ZG_NULL)
		return 0;

	void* pUserData = sg_pPool == ZG_NULL ? ZG_NULL : sg_pPool->pUserData;
	ZGUINT uTime = sg_pfnTileManagerSet(uElapsedTime, pTileActionObjectData, pTileNodeData, pTileMap, uIndex, uCount, ppTileNodes, &pUserData);
	if (sg_pPool != ZG_NULL)
	{
		if(sg_pHead == ZG_NULL)
			sg_pHead = sg_pPool;

		if (sg_pTail != ZG_NULL)
			sg_pTail->pNext = sg_pPool;

		sg_pTail = sg_pPool;
		sg_pPool = sg_pTail->pNext;

		sg_pTail->pUserData = pUserData;
		sg_pTail->pNext = ZG_NULL;
	}

	return uTime;
}

ZGBOOLEAN ZGTileManagerSet(
	LPZGTILEMANAGER pTileManager,
	LPZGTILEMANAGEROBJECT pTileManagerObject,
	void* pTileActionObjectData,
	ZGUINT uIndex,
	ZGUINT uTime, 
	ZGTILEMANAGERSET pfnTileManagerSet)
{
	if (pTileManager == ZG_NULL || pTileManagerObject == ZG_NULL)
		return ZG_FALSE;

	pTileManagerObject->Instance.nTime += uTime;
	if (pTileManagerObject->Instance.nTime < 0)
		return ZG_FALSE;

	sg_pHead = ZG_NULL;
	sg_pTail = pTileManager->pQueue;
	sg_pPool = pTileManager->pPool;
	sg_pfnTileManagerSet = pfnTileManagerSet;

	if (sg_pTail != ZG_NULL)
	{
		while (sg_pTail->pNext != ZG_NULL)
			sg_pTail = sg_pTail->pNext;
	}

	pTileManagerObject->Instance.nTime -= __ZGTileManagerSet(
		0, 
		pTileActionObjectData,
		pTileManagerObject->Instance.Instance.pData,
		pTileManagerObject->Instance.Instance.pTileMap,
		uIndex,
		0, 
		ZG_NULL);

	if (pTileManager->pQueue == ZG_NULL)
		pTileManager->pQueue = sg_pHead;

	pTileManager->pPool = sg_pPool;

	if(pTileManagerObject->Instance.nTime > 0)
		pTileManagerObject->Instance.nTime = 0;

	return ZG_TRUE;
}

void ZGTileManagerRun(
	LPZGTILEMANAGER pTileManager,
	ZGUINT uTime,
	ZGUINT uBufferLength,
	PZGUINT8 puBuffer,
	ZGTILEACTIONTEST pfnTileActionTest,
	ZGTILEOBJECTMOVE pfnTileObjectMove,
	ZGTILEMANAGERDELAY pfnTileManagerDelay,
	ZGTILEMANAGERSET pfnTileManagerSet,
	ZGTILEMANAGERHAND pfnTileManagerHand)
{
	if (pTileManager == ZG_NULL)
		return;

	sg_pTail = pTileManager->pQueue;
	sg_pPool = pTileManager->pPool;
	LPZGTILEMANAGERHANDLER pTail;
	if (sg_pTail != ZG_NULL)
	{
		if (pfnTileManagerHand != ZG_NULL)
		{
			while (sg_pTail != ZG_NULL && !pfnTileManagerHand(uTime, sg_pTail->pUserData))
			{
				pTail = sg_pTail;
				sg_pTail = sg_pTail->pNext;

				pTail->pNext = sg_pPool;
				sg_pPool = pTail;
			}

			pTileManager->pQueue = sg_pTail;
		}

		if (sg_pTail != ZG_NULL)
		{
			while(sg_pTail->pNext != ZG_NULL)
			{
				if (pfnTileManagerHand != ZG_NULL && !pfnTileManagerHand(uTime, sg_pTail->pNext->pUserData))
				{
					pTail = sg_pTail->pNext;
					sg_pTail->pNext = pTail->pNext;

					pTail->pNext = sg_pPool;
					sg_pPool = pTail;
				}

				sg_pTail = sg_pTail->pNext;
			}
		}
	}

	sg_pHead = ZG_NULL;

	pTail = sg_pTail;

	sg_pfnTileManagerSet = pfnTileManagerSet;

	ZGUINT uDelayTime;
	for (LPZGTILEMANAGEROBJECT pTileManagerObject = pTileManager->pObjects; pTileManagerObject != ZG_NULL; pTileManagerObject = pTileManagerObject->pNext)
	{
		uDelayTime = ZGTileObjectRun(
			&pTileManagerObject->Instance,
			uTime,
			uBufferLength,
			puBuffer,
			pfnTileActionTest,
			pfnTileObjectMove,
			__ZGTileManagerSet);

		if (uDelayTime > 0 && pfnTileManagerDelay != ZG_NULL)
			pfnTileManagerDelay(pTileManagerObject->Instance.Instance.pData, uTime - uDelayTime, uDelayTime);
	}

	if (pTail == ZG_NULL)
		pTail = sg_pHead;
	else
		pTail = pTail->pNext;

	if(pfnTileManagerHand != ZG_NULL)
	{
		sg_pTail = pTail;
		while (sg_pTail != ZG_NULL && !pfnTileManagerHand(uTime, sg_pTail->pUserData))
		{
			pTail = sg_pTail;
			sg_pTail = sg_pTail->pNext;

			pTail->pNext = sg_pPool;
			sg_pPool = pTail;

			if(sg_pHead == pTail)
				sg_pHead = sg_pTail;
		}

		if (sg_pTail != ZG_NULL)
		{
			while (sg_pTail->pNext != ZG_NULL)
			{
				if (!pfnTileManagerHand(uTime, sg_pTail->pNext->pUserData))
				{
					pTail = sg_pTail->pNext;
					sg_pTail->pNext = pTail->pNext;

					pTail->pNext = sg_pPool;
					sg_pPool = pTail;
				}

				sg_pTail = sg_pTail->pNext;
			}
		}
	}

	if (pTileManager->pQueue == ZG_NULL)
		pTileManager->pQueue = sg_pHead;

	pTileManager->pPool = sg_pPool;
}