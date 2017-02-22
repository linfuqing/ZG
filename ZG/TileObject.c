#include "TileObject.h"
#include "Math.h"

ZGUINT ZGTileObjectRun(
	LPZGTILEOBJECT pTileObject,
	ZGUINT uTime, 
	ZGTILEOBJECTPOP pfnTileObjectPop,
	ZGTILEOBJECTPUSH pfnTileObjectPush)
{
	if (pTileObject == ZG_NULL || 
		pTileObject->Instance.pInstance == ZG_NULL || 
		pTileObject->pActions == ZG_NULL/* || 
		pfnTileObjectSet == ZG_NULL*/)
		return 0;

	pTileObject->nTime += uTime;

	ZGBOOLEAN bIsActive;
	ZGUINT uDepth = 0, i;
	LPZGTILEOBJECTACTION pTileObjectAction;
	LPZGTILEMAPNODE pTileMapNode;
	void* pUserData;
	while (pTileObject->nTime > 0)
	{
		pTileObjectAction = ZG_NULL;
		for (i = 0; i < pTileObject->pActions->uChildCount; ++ i)
		{
			pTileObjectAction = pTileObject->pActions->ppChildren[i];
			if (pTileObjectAction == ZG_NULL)
				continue;

			uDepth = pTileObjectAction->pfnCheck == ZG_NULL ? 0 : pTileObjectAction->pfnCheck(pTileObjectAction->pData, &pTileObject->Instance);
			if (uDepth > 0)
			{
				if (pTileObject->Instance.pTileMap != ZG_NULL)
				{
					uDepth = 0;
					LPZGNODE pNode = pTileObject->Instance.pTileMap->pNodes + pTileObject->Instance.uIndex;
					ZGUINT uMaxDepth = pTileObject->Instance.pInstance->uRange * uTime, 
						uMaxDistance = pTileObject->Instance.pInstance->uDistance * uTime,
						uDistance = 0, 
						uIndex = pTileObject->Instance.uIndex;
					uTime = 0;
					while (pNode->pNext != ZG_NULL)
					{
						if (++uDepth > uMaxDepth)
							break;

						uDistance += pNode->pNext->uDistance;
						if (uDistance > uMaxDistance)
							break;

						if (pNode->pNext->pData != ZG_NULL && pTileObjectAction->pfnMove != ZG_NULL)
						{
							pTileMapNode = (LPZGTILEMAPNODE)pNode->pNext->pData;
							uTime += pTileObjectAction->pfnMove(
								uTime, 
								pTileObjectAction->pData,
								pTileObject->Instance.pData,
								pTileObject->Instance.pTileMap,
								uIndex,
								pTileMapNode->uIndex);

							if (uTime > (ZGUINT)pTileObject->nTime)
							{
								pNode = pNode->pNext;

								break;
							}

							uIndex = pTileMapNode->uIndex;
						}

						pNode = pNode->pNext;
					}

					if (pNode->pData != ZG_NULL)
					{
						pTileMapNode = (LPZGTILEMAPNODE)pNode->pData;
						bIsActive = pTileObject->Instance.uIndex != pTileMapNode->uIndex && 
							ZGTileNodeSetTo(
								&pTileObject->Instance, 
								pTileObject->Instance.pTileMap, 
								pTileMapNode->uIndex);
						if (pTileObject->nTime > (ZGINT)uTime && pNode->pNext == ZG_NULL && pTileObjectAction->pfnSet != ZG_NULL)
						{
							pUserData = pfnTileObjectPop == ZG_NULL ? ZG_NULL : pfnTileObjectPop();
							uTime += pTileObjectAction->pfnSet(
								uTime,
								pTileObjectAction->pData,
								pTileObject->Instance.pData,
								pTileObject->Instance.pTileMap,
								pTileMapNode->uIndex, 
								&pUserData
								/*pTileActionMapNode->uMaxIndex,
								ZG_MIN(pTileActionMapNode->uMaxCount, pTileActionMapNode->uCount),
								pTileActionMapNode->ppNodes*/);
							if (pfnTileObjectPush != ZG_NULL)
								pfnTileObjectPush(pUserData);

							bIsActive = ZG_TRUE;
						}

						pTileObject->nTime -= uTime;

						if(bIsActive)
							break;
					}
				}
			}
		}

		if(i < pTileObject->pActions->uChildCount)
			pTileObject->pActions = pTileObjectAction;
		else
		{
			ZGUINT uTime = (ZGUINT)pTileObject->nTime;
			pTileObject->nTime = 0;

			return uTime;
		}
	}

	return 0;
}