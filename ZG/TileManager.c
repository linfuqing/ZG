#include "TileManager.h"

static LPZGTILEMANAGERHANDLER sg_pHead;
static LPZGTILEMANAGERHANDLER sg_pTail;
static LPZGTILEMANAGERHANDLER sg_pPool;

void* __ZGTileManagerPop()
{
	return sg_pPool == ZG_NULL ? ZG_NULL : sg_pPool->pUserData;
}

void __ZGTileManangerPush(void* pUserData)
{
	if (sg_pPool == ZG_NULL)
		return;

	if (sg_pHead == ZG_NULL)
		sg_pHead = sg_pPool;

	if (sg_pTail != ZG_NULL)
		sg_pTail->pNext = sg_pPool;

	sg_pTail = sg_pPool;
	sg_pPool = sg_pTail->pNext;

	sg_pTail->pUserData = pUserData;
	sg_pTail->pNext = ZG_NULL;
}

ZGBOOLEAN ZGTileManagerSet(
	LPZGTILEMANAGER pTileManager,
	LPZGTILEMANAGEROBJECT pTileManagerObject,
	LPZGTILEOBJECTACTION pTileObjectAction,
	ZGUINT uIndex,
	ZGUINT uTime)
{
	if (pTileManager == ZG_NULL || pTileManagerObject == ZG_NULL || pTileObjectAction == ZG_NULL)
		return ZG_FALSE;

	if (pTileObjectAction->pfnSet == ZG_NULL)
		return ZG_FALSE;

	pTileManagerObject->Instance.nTime += uTime;
	if (pTileManagerObject->Instance.nTime < 0)
		return ZG_FALSE;

	sg_pHead = ZG_NULL;
	sg_pTail = pTileManager->pQueue;
	sg_pPool = pTileManager->pPool;

	if (sg_pTail != ZG_NULL)
	{
		while (sg_pTail->pNext != ZG_NULL)
			sg_pTail = sg_pTail->pNext;
	}

	void* pUserData = __ZGTileManagerPop();
	pTileManagerObject->Instance.nTime -= pTileObjectAction->pfnSet(
		0, 
		pTileObjectAction->pData,
		pTileManagerObject->Instance.Instance.pData,
		pTileManagerObject->Instance.Instance.pTileMap,
		uIndex,
		&pUserData);
	__ZGTileManangerPush(pUserData);

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
	ZGTILEMANAGERDELAY pfnTileManagerDelay,
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

	ZGUINT uDelayTime;
	for (LPZGTILEMANAGEROBJECT pTileManagerObject = pTileManager->pObjects; pTileManagerObject != ZG_NULL; pTileManagerObject = pTileManagerObject->pNext)
	{
		uDelayTime = ZGTileObjectRun(
			&pTileManagerObject->Instance,
			uTime,
			__ZGTileManagerPop,
			__ZGTileManangerPush);

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