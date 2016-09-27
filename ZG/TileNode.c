#include "TileNode.h"

static const ZGTILERANGE* sg_pTileRange;
static const void* sg_pTileNodeData;
ZGTILENODETEST sg_pfnTest;

ZGBOOLEAN __ZGTileNodeTest(LPZGTILEMAP pTileMap, const ZGTILERANGE* pTileRange, ZGUINT uIndex)
{
	return sg_pfnTest == ZG_NULL ? ZGMapTest(
		&pTileMap->Instance, 
		&sg_pTileRange->Instance, 
		uIndex, 
		sg_pTileRange->uOffset, 
		ZG_NULL, 
		ZG_NULL) : 
		sg_pfnTest(sg_pTileNodeData, pTileRange, pTileMap, uIndex);
}

ZGBOOLEAN ZGTileNodeUnset(
	LPZGTILENODE pTileNode,
	LPZGTILEMAP pTileMap)
{
	if (pTileNode == ZG_NULL || pTileMap == ZG_NULL)
		return ZG_FALSE;

	if (pTileNode->uIndex >= pTileMap->Instance.Instance.uCount)
		return ZG_FALSE;

	ZGMapAssign(
		&pTileMap->Instance,
		&pTileNode->Instance.Instance,
		pTileNode->uIndex,
		pTileNode->Instance.uOffset,
		ZG_FALSE);

	ZGUINT uMaxNodeX = pTileNode->Instance.Instance.uPitch - 1,
		uMaxNodeY = pTileNode->Instance.Instance.Instance.uCount / pTileNode->Instance.Instance.uPitch - 1,
		uMaxMapX = pTileMap->Instance.uPitch - 1,
		uMaxMapY = pTileMap->Instance.Instance.uCount / pTileMap->Instance.uPitch - 1,
		uNodeX = pTileNode->Instance.uOffset % pTileNode->Instance.Instance.uPitch,
		uNodeY = pTileNode->Instance.uOffset / pTileNode->Instance.Instance.uPitch,
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
		for (j = uFromX; j <= uToY; ++j)
		{
			if (ZGMapGet(&pTileNode->Instance.Instance, i + j * pTileNode->Instance.Instance.uPitch))
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

	if (uIndex >= pTileMap->Instance.Instance.uCount)
		return ZG_FALSE;

	ZGTileNodeUnset(pTileNode, pTileMap);

	ZGMapAssign(
		&pTileMap->Instance,
		&pTileNode->Instance.Instance,
		uIndex,
		pTileNode->Instance.uOffset,
		ZG_TRUE);

	ZGUINT uMaxNodeX = pTileNode->Instance.Instance.uPitch - 1,
		uMaxNodeY = pTileNode->Instance.Instance.Instance.uCount / pTileNode->Instance.Instance.uPitch - 1,
		uMaxMapX = pTileMap->Instance.uPitch - 1,
		uMaxMapY = pTileMap->Instance.Instance.uCount / pTileMap->Instance.uPitch - 1,
		uNodeX = pTileNode->Instance.uOffset % pTileNode->Instance.Instance.uPitch,
		uNodeY = pTileNode->Instance.uOffset / pTileNode->Instance.Instance.uPitch,
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
		for (j = uFromX; j <= uToY; ++j)
		{
			if (ZGMapGet(&pTileNode->Instance.Instance, i + j * pTileNode->Instance.Instance.uPitch))
				((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, (i + uMapX - uNodeX) + (j + uMapY - uNodeY) * pTileMap->Instance.uPitch))->pNode = pTileNode;
		}
	}

	pTileNode->uIndex = uIndex;

	return ZG_TRUE;
}

ZGUINT ZGTileNodeSearch(
	const ZGTILENODE* pTileNode,
	LPZGTILEMAP pTileMap,
	ZGBOOLEAN bIsTest,
	ZGUINT uIndex, 
	ZGTILENODETEST pfnTest)
{
	if (pTileNode == ZG_NULL || pTileMap == ZG_NULL)
		return 0;

	sg_pTileRange = &pTileNode->Instance;
	sg_pTileNodeData = pTileNode->pData;
	sg_pfnTest = pfnTest;

	return ZGTileMapSearch(
		pTileMap,
		&pTileNode->Instance,
		bIsTest, 
		pTileNode->uIndex,
		uIndex,
		pTileNode->uRange,
		pTileNode->uDistance,
		ZGTilePredicate, 
		__ZGTileNodeTest);
}
