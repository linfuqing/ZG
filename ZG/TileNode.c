#include "TileNode.h"

ZGBOOLEAN ZGTileNodeUnset(LPZGTILENODE pTileNode)
{
	if (pTileNode == ZG_NULL || pTileNode->pTileMap == ZG_NULL)
		return ZG_FALSE;

	if (pTileNode->pInstance == ZG_NULL || pTileNode->uIndex >= pTileNode->pTileMap->Instance.Instance.uCount)
		return ZG_FALSE;

	ZGMAP Map = pTileNode->pTileMap->Instance;
	Map.Instance.uCount /= pTileNode->pTileMap->uLevel;
	ZGUINT uLevel = pTileNode->uIndex / Map.Instance.uCount, 
		uOffset = uLevel * Map.Instance.uCount, 
		uIndex = pTileNode->uIndex - uOffset;
	Map.Instance.uOffset += uOffset;
	
	ZGMapAssign(
		&Map,
		&pTileNode->pInstance->Instance.Instance,
		uIndex,
		pTileNode->pInstance->Instance.uOffset,
		ZG_FALSE);

	ZGUINT uMaxNodeX = pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxNodeY = pTileNode->pInstance->Instance.Instance.Instance.uCount / pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxMapX = Map.uPitch - 1,
		uMaxMapY = Map.Instance.uCount / Map.uPitch - 1,
		uNodeX = pTileNode->pInstance->Instance.uOffset % pTileNode->pInstance->Instance.Instance.uPitch,
		uNodeY = pTileNode->pInstance->Instance.uOffset / pTileNode->pInstance->Instance.Instance.uPitch,
		uMapX = uIndex % Map.uPitch,
		uMapY = uIndex / Map.uPitch,
		uToX = uMapX + uMaxNodeX - uNodeX,
		uToY = uMapY + uMaxNodeY - uNodeY,
		uFromX = uNodeX > uMapX ? uNodeX - uMapX : 0,
		uFromY = uNodeY > uMapY ? uNodeY - uMapY : 0,
		i, j;
	void* pData;

	uToX = uToX > uMaxMapX ? uMaxNodeX + uMaxMapX - uToX : uMaxNodeX;
	uToY = uToY > uMaxMapY ? uMaxNodeY + uMaxMapY - uToY : uMaxNodeY;
	for (i = uFromX; i <= uToX; ++i)
	{
		for (j = uFromY; j <= uToY; ++j)
		{
			if (ZGMapGet(&pTileNode->pInstance->Instance.Instance, i + j * pTileNode->pInstance->Instance.Instance.uPitch))
			{
				pData = ZGTileMapGetData(
					pTileNode->pTileMap,
					(i + uMapX - uNodeX) + (j + uMapY - uNodeY) * Map.uPitch + uOffset);

				if(pData != ZG_NULL)
					((LPZGTILENODEMAPNODE)pData)->pNode = ZG_NULL;
			}
		}
	}

	pTileNode->uIndex = ~0;

	return ZG_TRUE;
}

ZGBOOLEAN ZGTileNodeSetTo(
	LPZGTILENODE pTileNode,
	LPZGTILEMAP pTileMap,
	ZGUINT uIndex)
{
	if (pTileNode == ZG_NULL || pTileMap == ZG_NULL)
		return ZG_FALSE;

	if (pTileNode->pInstance == ZG_NULL || uIndex >= pTileMap->Instance.Instance.uCount)
		return ZG_FALSE;

	ZGTileNodeUnset(pTileNode);

	ZGMAP Map = pTileNode->pTileMap->Instance;
	Map.Instance.uCount /= pTileNode->pTileMap->uLevel;
	ZGUINT uLevel = uIndex / Map.Instance.uCount,
		uOffset = uLevel * Map.Instance.uCount, 
		uMapIndex = uIndex - uOffset;
	Map.Instance.uOffset += uOffset;

	ZGMapAssign(
		&Map,
		&pTileNode->pInstance->Instance.Instance,
		uMapIndex,
		pTileNode->pInstance->Instance.uOffset,
		ZG_TRUE);

	ZGUINT uMaxNodeX = pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxNodeY = pTileNode->pInstance->Instance.Instance.Instance.uCount / pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxMapX = Map.uPitch - 1,
		uMaxMapY = Map.Instance.uCount / Map.uPitch - 1,
		uNodeX = pTileNode->pInstance->Instance.uOffset % pTileNode->pInstance->Instance.Instance.uPitch,
		uNodeY = pTileNode->pInstance->Instance.uOffset / pTileNode->pInstance->Instance.Instance.uPitch,
		uMapX = uMapIndex % Map.uPitch,
		uMapY = uMapIndex / Map.uPitch,
		uToX = uMapX + uMaxNodeX - uNodeX,
		uToY = uMapY + uMaxNodeY - uNodeY,
		uFromX = uNodeX > uMapX ? uNodeX - uMapX : 0,
		uFromY = uNodeY > uMapY ? uNodeY - uMapY : 0,
		i, j;
	void* pData;

	uToX = uToX > uMaxMapX ? uMaxNodeX + uMaxMapX - uToX : uMaxNodeX;
	uToY = uToY > uMaxMapY ? uMaxNodeY + uMaxMapY - uToY : uMaxNodeY;
	for (i = uFromX; i <= uToX; ++i)
	{
		for (j = uFromY; j <= uToY; ++j)
		{
			if (ZGMapGet(&pTileNode->pInstance->Instance.Instance, i + j * pTileNode->pInstance->Instance.Instance.uPitch))
			{
				pData = ZGTileMapGetData(
					pTileMap,
					(i + uMapX - uNodeX) + (j + uMapY - uNodeY) * Map.uPitch + uOffset);

				if(pData != ZG_NULL)
					((LPZGTILENODEMAPNODE)pData)->pNode = pTileNode;
			}
		}
	}

	pTileNode->pTileMap = pTileMap;
	pTileNode->uIndex = uIndex;

	return ZG_TRUE;
}

ZGUINT ZGTileNodeSearchDepth(
	const ZGTILENODE* pTileNode,
	//ZGBOOLEAN bIsTest,
	ZGUINT uIndex, 
	ZGNODEPREDICATION pfnPredication,
	ZGMAPTEST pfnMapTest)
{
	if (pTileNode == ZG_NULL || pTileNode->pInstance == ZG_NULL)
		return 0;

	return ZGTileMapSearchDepth(
		pTileNode->pTileMap,
		&pTileNode->pInstance->Instance,
		//bIsTest, 
		pTileNode->uIndex,
		uIndex,
		pTileNode->pInstance->uRange,
		pTileNode->pInstance->uDistance,
		pfnPredication,
		pfnMapTest);
}

ZGUINT ZGTileNodeSearchBreadth(
	const ZGTILENODE* pTileNode,
	ZGNODEPREDICATION pfnPredication, 
	ZGMAPTEST pfnMapTest, 
	ZGTILEMAPTEST pfnTileMapTest)
{
	if (pTileNode == ZG_NULL)
		return 0;

	return ZGTileMapSearchBreadth(
		pTileNode->pTileMap,
		&pTileNode->pInstance->Instance,
		pTileNode->uIndex,
		pTileNode->pInstance->uRange,
		pTileNode->pInstance->uDistance,
		pfnPredication,
		pfnMapTest, 
		pfnTileMapTest);
}
