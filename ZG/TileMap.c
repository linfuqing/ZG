#include "TileMap.h"
#include "Math.h"

static LPZGTILEMAP sg_pTileMap;
static const ZGTILERANGE* sg_pTileRange;

static ZGTILEMAPTEST sg_pfnTest;

static ZGUINT sg_uToIndexX;
static ZGUINT sg_uToIndexY;

ZGUINT __ZGTileMapEvaluate(void* pMapNode)
{
	const ZGTILEMAPNODE* pTemp = (const ZGTILEMAPNODE*)pMapNode;
	
	if (sg_pfnTest == ZG_NULL ? 
		ZGMapTest(&sg_pTileMap->Instance, &sg_pTileRange->Instance, pTemp->uIndex, sg_pTileRange->uOffset, ZG_NULL, ZG_NULL) : 
		sg_pfnTest(sg_pTileMap, sg_pTileRange, pTemp->uIndex))
		return ~0;

	ZGUINT uFromX = pTemp->uIndex % sg_pTileMap->Instance.uPitch,
		uFromY = pTemp->uIndex / sg_pTileMap->Instance.uPitch;
	return ZG_ABS(uFromX, sg_uToIndexX) + ZG_ABS(uFromY, sg_uToIndexY);
}

void ZGTileRangeInitOblique(LPZGTILERANGE pRange, PZGUINT8 puFlags, ZGUINT uSize)
{
	if (pRange == ZG_NULL)
		return;

	pRange->Instance.uPitch = (uSize << 1) + 1;
	pRange->Instance.Instance.puFlags = puFlags;
	pRange->Instance.Instance.uOffset = 0;
	pRange->Instance.Instance.uCount = pRange->Instance.uPitch * pRange->Instance.uPitch;
	pRange->uOffset = pRange->Instance.Instance.uCount >> 1;

	ZGUINT i, j, uIndex = 0;
	for (i = 0; i < pRange->Instance.uPitch; ++i)
	{
		for (j = 0; j < pRange->Instance.uPitch; ++j)
		{
			ZGBitFlagSet(&pRange->Instance.Instance, uIndex, ZG_ABS(i, uSize) + ZG_ABS(j, uSize) <= uSize);

			++uIndex;
		}
	}
}

ZGUINT ZGTileMapSearch(
	LPZGTILEMAP pTileMap,
	const ZGTILERANGE* pTileRange,
	ZGBOOLEAN bIsTest, 
	ZGUINT uFromIndex,
	ZGUINT uToIndex,
	ZGUINT uMaxDepth,
	ZGUINT uMaxDistance,
	ZGNODEPREDICATION pfnPredication, 
	ZGTILEMAPTEST pfnTest)
{
	if (pTileMap == ZG_NULL || pTileRange == ZG_NULL)
		return 0;

	if (pTileMap->Instance.Instance.uCount <= uFromIndex || pTileMap->Instance.Instance.uCount <= uToIndex)
		return 0;

	sg_pTileMap = pTileMap;
	sg_pTileRange = pTileRange;

	sg_pfnTest = pfnTest;

	sg_uToIndexX = uToIndex % pTileMap->Instance.uPitch;
	sg_uToIndexY = uToIndex / pTileMap->Instance.uPitch;

	return ZGNodeSearch(
		pTileMap->pNodes + uFromIndex, 
		pfnPredication,
		__ZGTileMapEvaluate,
		0, 
		~0, 
		uMaxDistance, 
		uMaxDepth, 
		bIsTest ? ZG_NODE_SEARCH_TYPE_ONCE : ZG_NODE_SEARCH_TYPE_MIN);
}

void ZGTileMapEnable(
	LPZGTILEMAP pTileMap,
	PZGUINT8 puFlags,
	LPZGNODE* ppNodes,
	LPZGNODE pNodes,
	LPZGTILEMAPNODE pMapNodes,
	ZGUINT uWidth,
	ZGUINT uHeight,
	ZGBOOLEAN bIsOblique)
{
	if (pTileMap == ZG_NULL)
		return;

	pTileMap->Instance.Instance.puFlags = puFlags;
	pTileMap->Instance.Instance.uCount = uWidth * uHeight;
	pTileMap->Instance.Instance.uOffset = 0;
	pTileMap->Instance.uPitch = uWidth;
	pTileMap->pNodes = pNodes;

	ZGUINT uMaxX = uWidth - 1, uMaxY = uHeight - 1, uIndex = 0, i, j;
	LPZGNODE pNode;
	LPZGTILEMAPNODE pMapNode;
	for (j = 0; j < uHeight; ++j)
	{
		for (i = 0; i < uWidth; ++i)
		{
			pNode = pNodes + uIndex;
			pMapNode = pMapNodes + uIndex;

			pMapNode->uIndex = uIndex;

			pNode->pData = pMapNode;

			ZGNodeEnable(pNode);

			pNode->uCount = 0;
			pNode->ppChildren = ppNodes;

			if (i > 0)
			{
				++pNode->uCount;
				*ppNodes++ = pNodes + (i - 1) + j * uWidth;
			}

			if (i < uMaxX)
			{
				++pNode->uCount;
				*ppNodes++ = pNodes + (i + 1) + j * uWidth;
			}

			if (j > 0)
			{
				++pNode->uCount;
				*ppNodes++ = pNodes + i + (j - 1) * uWidth;
			}

			if (j < uMaxY)
			{
				++pNode->uCount;
				*ppNodes++ = pNodes + i + (j + 1) * uWidth;
			}

			if (bIsOblique)
			{
				if (i > 0)
				{
					if (j > 0)
					{
						++pNode->uCount;
						*ppNodes++ = pNodes + (i - 1) + (j - 1) * uWidth;
					}

					if (j < uMaxY)
					{
						++pNode->uCount;
						*ppNodes++ = pNodes + (i - 1) + (j + 1) * uWidth;
					}
				}

				if (i < uMaxX)
				{
					if (j > 0)
					{
						++pNode->uCount;
						*ppNodes++ = pNodes + (i + 1) + (j - 1) * uWidth;
					}

					if (j < uMaxY)
					{
						++pNode->uCount;
						*ppNodes++ = pNodes + (i + 1) + (j + 1) * uWidth;
					}
				}
			}

			++uIndex;
		}
	}
}

void ZGTileMapDisable(LPZGTILEMAP pTileMap)
{
	if (pTileMap == ZG_NULL || pTileMap->pNodes == ZG_NULL)
		return;

	for (ZGUINT i = 0; i < pTileMap->Instance.Instance.uCount; ++i)
		ZGNodeDisable(pTileMap->pNodes + i);
}