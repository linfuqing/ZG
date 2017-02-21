#include "TileObject.h"
#include "Math.h"

ZGUINT ZGTileObjectRun(
	LPZGTILEOBJECT pTileObject,
	ZGUINT uTime,
	ZGUINT uBufferLength, 
	PZGUINT8 puBuffer, 
	ZGTILEACTIONTEST pfnTileActionTest,
	ZGTILEOBJECTMOVE pfnTileObjectMove,
	ZGTILEOBJECTSET pfnTileObjectSet)
{
	if (pTileObject == ZG_NULL || 
		pTileObject->Instance.pInstance == ZG_NULL || 
		pTileObject->pActions == ZG_NULL || 
		pfnTileObjectSet == ZG_NULL)
		return 0;

	pTileObject->nTime += uTime;

	ZGUINT uDepth = 0, i;
	LPZGTILEOBJECTACTION pTileObjectAction;
	LPZGTILEMAPNODE pTileMapNode;
	LPZGTILEACTIONMAPNODE pTileActionMapNode;
	while (pTileObject->nTime > 0)
	{
		pTileObjectAction = ZG_NULL;
		for (i = 0; i < pTileObject->pActions->uChildCount; ++ i)
		{
			pTileObjectAction = pTileObject->pActions->ppChildren[i];
			if (pTileObjectAction == ZG_NULL)
				continue;

			uDepth = ZGTileActionSearchBreadth(
				pTileObjectAction->pInstance,
				&pTileObject->Instance,
				uBufferLength,
				puBuffer,
				pfnTileActionTest);

			if (uDepth > 0)
			{
				if (pTileObject->Instance.pTileMap != ZG_NULL)
				{
					uDepth = 0;
					LPZGNODE pNode = pTileObject->Instance.pTileMap->pNodes + pTileObject->Instance.uIndex;
					ZGUINT uDistance = 0, uIndex = pTileObject->Instance.uIndex;
					uTime = 0;
					while (pNode->pNext != ZG_NULL)
					{
						uDistance += pNode->pNext->uDistance;
						if (uDistance > pTileObject->Instance.pInstance->uDistance)
							break;

						if (++uDepth > pTileObject->Instance.pInstance->uRange)
							break;

						if (pNode->pNext->pData != ZG_NULL && pfnTileObjectMove != ZG_NULL)
						{
							pTileMapNode = (LPZGTILEMAPNODE)pNode->pNext->pData;
							uTime += pfnTileObjectMove(
								uTime, 
								pTileObjectAction->pInstance == ZG_NULL ? ZG_NULL : pTileObjectAction->pData,
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

					pTileMapNode = (LPZGTILEMAPNODE)pNode->pData;
					if (ZGTileNodeSetTo(&pTileObject->Instance, pTileObject->Instance.pTileMap, pTileMapNode->uIndex))
					{
						pTileObject->nTime -= uTime;
						if (pTileObject->nTime > 0 && pNode->pNext == ZG_NULL)
						{
							pTileActionMapNode = (LPZGTILEACTIONMAPNODE)(((LPZGTILENODEMAPNODE)pTileMapNode->pData)->pData);
							pTileObject->nTime -= pfnTileObjectSet(
								uTime, 
								pTileObjectAction->pInstance == ZG_NULL ? ZG_NULL : pTileObjectAction->pData,
								pTileObject->Instance.pData,
								pTileObject->Instance.pTileMap,
								pTileActionMapNode->uMaxIndex,
								ZG_MIN(pTileActionMapNode->uMaxCount, pTileActionMapNode->uCount),
								pTileActionMapNode->ppNodes);
						}

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