#include "TileManager.h"

static LPZGTILEMANAGEROBJECT sg_pObject;
static LPZGTILEMANAGERHANDLER sg_pHead;
static LPZGTILEMANAGERHANDLER sg_pTail;
static LPZGTILEMANAGERHANDLER sg_pPool;

void __ZGTileManagerDetach(LPZGTILEMANAGERHANDLER pHandler, ZGBOOLEAN bIsFree)
{
	if (pHandler->pBackward != ZG_NULL)
		pHandler->pBackward->pForward = pHandler->pForward;

	if (pHandler->pForward != ZG_NULL)
		pHandler->pForward->pBackward = pHandler->pBackward;

	if (pHandler->pObject != ZG_NULL && pHandler->pObject->pHandler == pHandler)
		pHandler->pObject->pHandler = pHandler->pForward;

	pHandler->pObject = ZG_NULL;
	pHandler->pBackward = ZG_NULL;
	pHandler->pForward = ZG_NULL;

	if (bIsFree)
	{
		pHandler->pNext = sg_pPool;

		sg_pPool = pHandler;
	}
}

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

	sg_pTail->pBackward = ZG_NULL;
	if (sg_pObject == ZG_NULL)
		sg_pTail->pForward = ZG_NULL;
	else
	{
		sg_pTail->pForward = sg_pObject->pHandler;
		if(sg_pObject->pHandler != ZG_NULL)
			sg_pObject->pHandler->pBackward = sg_pTail;

		sg_pObject->pHandler = sg_pTail;
	}

	sg_pTail->pObject = sg_pObject;
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

				__ZGTileManagerDetach(pTail, ZG_TRUE);
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

					__ZGTileManagerDetach(pTail, ZG_TRUE);
				}
				else
					sg_pTail = sg_pTail->pNext;
			}
		}
	}

	sg_pHead = ZG_NULL;

	pTail = sg_pTail;

	ZGUINT uDelayTime;
	for (LPZGTILEMANAGEROBJECT pTileManagerObject = pTileManager->pObjects; pTileManagerObject != ZG_NULL; pTileManagerObject = pTileManagerObject->pNext)
	{
		sg_pObject = pTileManagerObject;

		uDelayTime = ZGTileObjectRun(
			&pTileManagerObject->Instance,
			uTime,
			__ZGTileManagerPop,
			__ZGTileManangerPush);

		if (uDelayTime > 0 && pfnTileManagerDelay != ZG_NULL)
			pfnTileManagerDelay(pTileManagerObject->Instance.Instance.pData, uTime - uDelayTime, uDelayTime);
	}

	if(pfnTileManagerHand != ZG_NULL)
	{
		sg_pTail = pTail;
		if (sg_pTail == ZG_NULL)
		{
			sg_pTail = sg_pHead;
			while (sg_pTail != ZG_NULL && !pfnTileManagerHand(uTime, sg_pTail->pUserData))
			{
				pTail = sg_pTail;
				sg_pTail = sg_pTail->pNext;

				__ZGTileManagerDetach(pTail, ZG_TRUE);
			}
			sg_pHead = sg_pTail;
		}

		if (sg_pTail != ZG_NULL)
		{
			while (sg_pTail->pNext != ZG_NULL)
			{
				if (pfnTileManagerHand(uTime, sg_pTail->pNext->pUserData))
					sg_pTail = sg_pTail->pNext;
				else
				{
					pTail = sg_pTail->pNext;
					sg_pTail->pNext = pTail->pNext;

					__ZGTileManagerDetach(pTail, ZG_TRUE);
				}
			}
		}
	}

	if (pTileManager->pQueue == ZG_NULL)
		pTileManager->pQueue = sg_pHead;

	pTileManager->pPool = sg_pPool;
}

void ZGTileManagerBreak(LPZGTILEMANAGEROBJECT pTileManagerObject, ZGUINT uTime, ZGTILEMANAGERCHECK pfnCheck)
{
	if (pTileManagerObject == ZG_NULL)
		return;

	while (pTileManagerObject->pHandler != ZG_NULL)
		__ZGTileManagerDetach(pTileManagerObject->pHandler, pfnCheck(pTileManagerObject->pHandler->pUserData));

	pTileManagerObject->Instance.nTime = 0;
	pTileManagerObject->Instance.nTime -= uTime;
}