#include "TileMap.h"
#include "Math.h"

static LPZGTILEMAP sg_pTileMap;
static const ZGTILERANGE* sg_pTileRange;

static ZGMAPTEST sg_pfnMapTest;
static ZGTILEMAPTEST sg_pfnTileMapTest;

static ZGUINT sg_uLevelSize;
static ZGUINT sg_uLevel;
static ZGUINT sg_uToIndexX;
static ZGUINT sg_uToIndexY;

ZGUINT __ZGTileMapEvaluateDepth(void* pMapNode)
{
	const ZGTILEMAPNODE* pTemp = (const ZGTILEMAPNODE*)pMapNode;
	
	ZGUINT uLevel = pTemp->uIndex / sg_uLevelSize,
		uOffset = uLevel * sg_uLevelSize,
		uIndex = pTemp->uIndex - uOffset;

	ZGMAP Map = sg_pTileMap->Instance;
	Map.Instance.uOffset += uOffset;
	Map.Instance.uCount = sg_uLevelSize;
	if (ZGMapVisit(&Map, &sg_pTileRange->Instance, uIndex, sg_pTileRange->uOffset, sg_pfnMapTest))
		return ~0;

	ZGUINT uFromX = uIndex % sg_pTileMap->Instance.uPitch,
		uFromY = uIndex / sg_pTileMap->Instance.uPitch;

	return ZG_ABS(uFromX, sg_uToIndexX) + ZG_ABS(uFromY, sg_uToIndexY) + ZG_ABS(uLevel, sg_uLevel);
}

ZGUINT __ZGTileMapEvaluateBreadth(void* pMapNode)
{
	const ZGTILEMAPNODE* pTemp = (const ZGTILEMAPNODE*)pMapNode;
	ZGUINT uLevel = pTemp->uIndex / sg_uLevelSize,
		uOffset = uLevel * sg_uLevelSize, 
		uIndex = pTemp->uIndex - uOffset;

	ZGMAP Map = sg_pTileMap->Instance;
	Map.Instance.uOffset += uOffset;
	Map.Instance.uCount = sg_uLevelSize;
	if (ZGMapVisit(&Map, &sg_pTileRange->Instance, uIndex, sg_pTileRange->uOffset, sg_pfnMapTest))
		return ~0;

	return sg_pfnTileMapTest == ZG_NULL || sg_pfnTileMapTest(sg_pTileMap, sg_pTileRange, pTemp->uIndex) ? 0 : 1;
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

ZGUINT ZGTileMapSearchDepth(
	LPZGTILEMAP pTileMap,
	const ZGTILERANGE* pTileRange,
	//ZGBOOLEAN bIsTest, 
	ZGUINT uFromIndex,
	ZGUINT uToIndex,
	ZGUINT uMaxDepth,
	ZGUINT uMaxDistance,
	ZGNODEPREDICATION pfnPredication, 
	ZGMAPTEST pfnMapTest)
{
	if (pTileMap == ZG_NULL || pTileRange == ZG_NULL)
		return 0;

	if (pTileMap->Instance.Instance.uCount <= uFromIndex || pTileMap->Instance.Instance.uCount <= uToIndex)
		return 0;

	sg_pTileMap = pTileMap;
	sg_pTileRange = pTileRange;

	sg_pfnMapTest = pfnMapTest;

	sg_uLevelSize = pTileMap->Instance.Instance.uCount / pTileMap->uLevel;
	sg_uLevel = uFromIndex / sg_uLevelSize;
	ZGUINT uIndex = uFromIndex - sg_uLevel * sg_uLevelSize;

	sg_uToIndexX = uIndex % pTileMap->Instance.uPitch;
	sg_uToIndexY = uIndex / pTileMap->Instance.uPitch;

	return ZGNodeSearch(
		pTileMap->pNodes + uFromIndex, 
		pfnPredication,
		__ZGTileMapEvaluateDepth,
		0, 
		~0, 
		uMaxDistance, 
		uMaxDepth, 
		ZG_NODE_SEARCH_TYPE_ONCE);
}

ZGUINT ZGTileMapSearchBreadth(
	LPZGTILEMAP pTileMap,
	const ZGTILERANGE* pTileRange,
	ZGUINT uIndex,
	ZGUINT uMaxDepth,
	ZGUINT uMaxDistance,
	ZGNODEPREDICATION pfnPredication,
	ZGMAPTEST pfnMapTest, 
	ZGTILEMAPTEST pfnTileMapTest)
{
	if (pTileMap == ZG_NULL || pTileRange == ZG_NULL)
		return 0;

	if (pTileMap->Instance.Instance.uCount <= uIndex)
		return 0;

	sg_pTileMap = pTileMap;
	sg_pTileRange = pTileRange;

	sg_pfnMapTest = pfnMapTest;
	sg_pfnTileMapTest = pfnTileMapTest;

	sg_uLevelSize = pTileMap->Instance.Instance.uCount / pTileMap->uLevel;

	return ZGNodeSearch(
		pTileMap->pNodes + uIndex,
		pfnPredication,
		__ZGTileMapEvaluateBreadth,
		0,
		~0,
		uMaxDistance,
		uMaxDepth, 
		ZG_NODE_SEARCH_TYPE_ONCE);
}

void ZGTileMapEnable(
	LPZGTILEMAP pTileMap,
	PZGUINT8 puFlags,
	LPZGNODE* ppNodes,
	LPZGNODE pNodes,
	LPZGTILEMAPNODE pMapNodes,
	LPZGTILEMAPBRIDGE pMapBridges,
	ZGUINT uBridgeLength,
	ZGUINT uWidth,
	ZGUINT uHeight,
	ZGUINT uDepth,
	ZGBOOLEAN bIsOblique)
{
	if (pTileMap == ZG_NULL)
		return;

	pTileMap->uLevel = uDepth;
	pTileMap->Instance.Instance.puFlags = puFlags;
	pTileMap->Instance.Instance.uCount = uWidth * uHeight * uDepth;
	pTileMap->Instance.Instance.uOffset = 0;
	pTileMap->Instance.uPitch = uWidth;
	pTileMap->pNodes = pNodes;

	ZGUINT uMaxX = uWidth - 1, uMaxY = uHeight - 1, uIndex = 0, i, j, k, l;
	LPZGNODE pNode;
	LPZGTILEMAPNODE pMapNode;
	LPZGTILEMAPBRIDGE pMapBridge;
	for (k = 0; k < uDepth; ++k)
	{
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

				for (l = 0; l < uBridgeLength; ++l)
				{
					pMapBridge = pMapBridges + l;
					if (pMapBridge->uFromIndex == uIndex)
					{
						++pNode->uCount;

						*ppNodes++ = pNodes + pMapBridge->uToIndex;
					}
				}

				++uIndex;
			}
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