#include "TileObject.h"

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
	ZGTILEACTIONTEST pfnTileActionTest,
	ZGTILEOBJECTSET pfnObjectSet)
{
	if (pTileObject == ZG_NULL)
		return 0;

	ZGUINT uDepth = 0;
	LPZGTILEACTION pTileAction;
	for (ZGUINT i = 0; i < pTileObject->uActionCount; ++i)
	{
		pTileAction = pTileObject->ppActions[i];
		uDepth = ZGTileActionSearchBreadth(
			pTileAction,
			&pTileObject->Instance, 
			pTileMap, 
			uEvaluation, 
			uMinEvaluation, 
			uMaxEvaluation,
			uMaxDistance, 
			uMaxDepth, 
			uBufferLength, 
			puBuffer, 
			pfnTileActionTest);

		if (uDepth > 0)
		{
			if (pTileMap != ZG_NULL)
			{
				ZGUINT uDepth = 0;
				LPZGNODE pNode = pTileMap->pNodes + pTileObject->Instance.uIndex;
				while (pNode->pNext != ZG_NULL && pNode->pNext->uValue <= pTileObject->Instance.uDistance)
				{
					if (uDepth++ > pTileObject->Instance.uRange)
						break;

					pNode = pNode->pNext;
				}

				LPZGTILEMAPNODE pTileMapNode = (LPZGTILEMAPNODE)pNode->pData;
				if (ZGTileNodeSetTo(&pTileObject->Instance, pTileMap, pTileMapNode->uIndex) && pfnObjectSet != ZG_NULL)
				{
					LPZGTILEACTIONMAPNODE pTileActionMapNode = (LPZGTILEACTIONMAPNODE)(((LPZGTILENODEMAPNODE)pTileMapNode->pData)->pData);
					pfnObjectSet(
						pTileAction == ZG_NULL ? ZG_NULL : pTileAction->pData,
						pTileObject->Instance.pData,
						pTileActionMapNode->uMaxIndex,
						pTileActionMapNode->uMaxCount,
						pTileActionMapNode->ppNodes);
				}
			}

			return uDepth;
		}
	}

	return 0;
}