#include "TileNode.h"

ZGBOOLEAN ZGTileNodeUnset(
	LPZGTILENODE pTileNode,
	LPZGTILEMAP pTileMap)
{
	if (pTileNode == ZG_NULL || pTileMap == ZG_NULL)
		return ZG_FALSE;

	if (pTileNode->pInstance == ZG_NULL || pTileNode->uIndex >= pTileMap->Instance.Instance.uCount)
		return ZG_FALSE;

	ZGMapAssign(
		&pTileMap->Instance,
		&pTileNode->pInstance->Instance.Instance,
		pTileNode->uIndex,
		pTileNode->pInstance->Instance.uOffset,
		ZG_FALSE);

	ZGUINT uMaxNodeX = pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxNodeY = pTileNode->pInstance->Instance.Instance.Instance.uCount / pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxMapX = pTileMap->Instance.uPitch - 1,
		uMaxMapY = pTileMap->Instance.Instance.uCount / pTileMap->Instance.uPitch - 1,
		uNodeX = pTileNode->pInstance->Instance.uOffset % pTileNode->pInstance->Instance.Instance.uPitch,
		uNodeY = pTileNode->pInstance->Instance.uOffset / pTileNode->pInstance->Instance.Instance.uPitch,
		uMapX = pTileNode->uIndex % pTileMap->Instance.uPitch,
		uMapY = pTileNode->uIndex / pTileMap->Instance.uPitch,
		uToX = uMapX + uMaxNodeX - uNodeX,
		uToY = uMapY + uMaxNodeY - uNodeY,
		uFromX = uNodeX > uMapX ? uNodeX - uMapX : 0,
		uFromY = uNodeY > uMapY ? uNodeY - uMapY : 0,
		i, j;

	uToX = uToX > uMaxMapX ? uMaxNodeX + uMaxMapX - uToX : uMaxNodeX;
	uToY = uToY > uMaxMapY ? uMaxNodeY + uMaxMapY - uToY : uMaxNodeY;
	for (i = uFromX; i <= uToX; ++i)
	{
		for (j = uFromY; j <= uToY; ++j)
		{
			if (ZGMapGet(&pTileNode->pInstance->Instance.Instance, i + j * pTileNode->pInstance->Instance.Instance.uPitch))
				((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, (i + uMapX - uNodeX) + (j + uMapY - uNodeY) * pTileMap->Instance.uPitch))->pNode = ZG_NULL;
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

	ZGTileNodeUnset(pTileNode, pTileMap);

	ZGMapAssign(
		&pTileMap->Instance,
		&pTileNode->pInstance->Instance.Instance,
		uIndex,
		pTileNode->pInstance->Instance.uOffset,
		ZG_TRUE);

	ZGUINT uMaxNodeX = pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxNodeY = pTileNode->pInstance->Instance.Instance.Instance.uCount / pTileNode->pInstance->Instance.Instance.uPitch - 1,
		uMaxMapX = pTileMap->Instance.uPitch - 1,
		uMaxMapY = pTileMap->Instance.Instance.uCount / pTileMap->Instance.uPitch - 1,
		uNodeX = pTileNode->pInstance->Instance.uOffset % pTileNode->pInstance->Instance.Instance.uPitch,
		uNodeY = pTileNode->pInstance->Instance.uOffset / pTileNode->pInstance->Instance.Instance.uPitch,
		uMapX = uIndex % pTileMap->Instance.uPitch,
		uMapY = uIndex / pTileMap->Instance.uPitch,
		uToX = uMapX + uMaxNodeX - uNodeX,
		uToY = uMapY + uMaxNodeY - uNodeY,
		uFromX = uNodeX > uMapX ? uNodeX - uMapX : 0,
		uFromY = uNodeY > uMapY ? uNodeY - uMapY : 0,
		i, j;

	uToX = uToX > uMaxMapX ? uMaxNodeX + uMaxMapX - uToX : uMaxNodeX;
	uToY = uToY > uMaxMapY ? uMaxNodeY + uMaxMapY - uToY : uMaxNodeY;
	for (i = uFromX; i <= uToX; ++i)
	{
		for (j = uFromY; j <= uToY; ++j)
		{
			if (ZGMapGet(&pTileNode->pInstance->Instance.Instance, i + j * pTileNode->pInstance->Instance.Instance.uPitch))
				((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, (i + uMapX - uNodeX) + (j + uMapY - uNodeY) * pTileMap->Instance.uPitch))->pNode = pTileNode;
		}
	}

	pTileNode->uIndex = uIndex;

	return ZG_TRUE;
}

ZGUINT ZGTileNodeSearchDepth(
	const ZGTILENODE* pTileNode,
	LPZGTILEMAP pTileMap,
	ZGBOOLEAN bIsTest,
	ZGUINT uIndex, 
	ZGMAPTEST pfnMapTest)
{
	if (pTileNode == ZG_NULL || pTileMap == ZG_NULL)
		return 0;

	if (pTileNode->pInstance == ZG_NULL)
		return 0;

	return ZGTileMapSearchDepth(
		pTileMap,
		&pTileNode->pInstance->Instance,
		bIsTest, 
		pTileNode->uIndex,
		uIndex,
		pTileNode->pInstance->uRange,
		pTileNode->pInstance->uDistance,
		ZGTilePredicate, 
		pfnMapTest);
}

ZGUINT ZGTileNodeSearchBreadth(
	const ZGTILENODE* pTileNode,
	LPZGTILEMAP pTileMap,
	ZGMAPTEST pfnMapTest, 
	ZGTILEMAPTEST pfnTileMapTest)
{
	if (pTileNode == ZG_NULL || pTileMap == ZG_NULL)
		return 0;

	return ZGTileMapSearchBreadth(
		pTileMap,
		&pTileNode->pInstance->Instance,
		pTileNode->uIndex,
		pTileNode->pInstance->uRange,
		pTileNode->pInstance->uDistance,
		ZGTilePredicate,
		pfnMapTest, 
		pfnTileMapTest);
}
