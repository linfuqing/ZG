#include "TileAction.h"
#include "Math.h"

static const ZGTILEACTION* sg_pTileAction;
static const ZGTILERANGE* sg_pTileRange;
static const ZGTILEMAP* sg_pTileMap;

static const void* sg_pTileNodeData;
static ZGTILEACTIONTEST sg_pfnTileActionTest;

static PZGUINT8 sg_puBuffer;
static ZGUINT sg_uBufferLength;
static ZGUINT sg_uEvaluation;
static ZGUINT sg_uMinEvaluation;
static ZGUINT sg_uMaxEvaluation;

ZGUINT __ZGTileActionEvaluateBreadth(void* pMapNode)
{
	LPZGTILEMAPNODE pTemp = (LPZGTILEMAPNODE)pMapNode;
	ZGUINT uTemp = sg_uBufferLength * sizeof(ZGUINT8) / sizeof(ZGUINT), uCount, uLength, uSize, i, j;
	PZGUINT puIndices;
	LPZGTILENODE pTileNode, *ppTileNodes;
	if (sg_pfnTileActionTest == ZG_NULL)
	{
		if (ZGMapTest(&sg_pTileMap->Instance, &sg_pTileRange->Instance, pTemp->uIndex, sg_pTileRange->uOffset, ZG_NULL, ZG_NULL))
			return sg_uMaxEvaluation;
	}
	else
	{
		uCount = uTemp;
		puIndices = (PZGUINT)sg_puBuffer;
		if (ZGMapTest(&sg_pTileMap->Instance, &sg_pTileRange->Instance, pTemp->uIndex, sg_pTileRange->uOffset, &uCount, puIndices))
		{
			uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE);
			if (uSize <= sg_uBufferLength)
			{
				uLength = 0;
				ppTileNodes = (LPZGTILENODE*)(puIndices + uCount);
				for (i = 0; i < uCount; ++i)
				{
					pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(sg_pTileMap, puIndices[i]))->pNode;
					if (pTileNode == ZG_NULL)
						return sg_uMaxEvaluation;

					if (&pTileNode->Instance == sg_pTileRange)
						continue;

					for (j = 0; j < uLength; ++j)
					{
						if (ppTileNodes[j] == pTileNode)
							break;
					}

					if (j < uLength)
						continue;

					ppTileNodes[uLength++] = pTileNode;

					uSize += sizeof(LPZGTILENODE);
					if (uSize > sg_uBufferLength)
						break;
				}

				if (uLength > 0)
					return sg_pfnTileActionTest(sg_pTileAction->pData, sg_pTileNodeData, ppTileNodes, uLength) ? sg_uMinEvaluation : sg_uMaxEvaluation;
			}
		}
	}

	ZGUINT uMaxLength = 0, uMinBit, uIndex;
	ZGBITFLAG BitFlag = sg_pTileAction->Distance.Instance.Instance;
	LPZGTILEACTIONMAPNODE pTileActionMapNode = (LPZGTILEACTIONMAPNODE)((LPZGTILENODEMAPNODE)ZGTileMapGetData(sg_pTileMap, pTemp->uIndex))->pData;
	pTileActionMapNode->uMaxCount = 0;
	while(ZG_TRUE)
	{
		uMinBit = ZGBitFlagMinBit(&BitFlag);
		if (uMinBit > 0)
		{
			BitFlag.uCount -= uMinBit;
			BitFlag.uOffset += uMinBit;
			uIndex = ZGTileConvert(
				sg_pTileMap->Instance.uPitch,
				pTemp->uIndex,
				sg_pTileAction->Distance.Instance.uPitch,
				BitFlag.uOffset - 1,
				sg_pTileAction->Distance.uOffset);
			if (uIndex > 0)
			{
				--uIndex;
				uCount = uTemp;
				puIndices = (PZGUINT)sg_puBuffer;
				if (ZGMapTest(
					&sg_pTileMap->Instance,
					&sg_pTileAction->Instance.Instance,
					uIndex,
					sg_pTileAction->Instance.uOffset,
					&uCount,
					puIndices))
				{
					uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE);
					uLength = 0;
					ppTileNodes = (LPZGTILENODE*)(puIndices + uCount);
					for (i = 0; i < uCount; ++i)
					{
						pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(sg_pTileMap, puIndices[i]))->pNode;
						if (pTileNode == ZG_NULL)
							continue;

						if (sg_pTileAction->pfnAnalyzation != ZG_NULL && !sg_pTileAction->pfnAnalyzation(sg_pTileAction->pData, sg_pTileNodeData, pTileNode->pData))
							continue;

						for (j = 0; j < uLength; ++j)
						{
							if (ppTileNodes[j] == pTileNode)
								break;
						}

						if (j < uLength)
							continue;

						if (uSize > sg_uBufferLength)
							++uLength;
						else
						{
							ppTileNodes[uLength++] = pTileNode;

							uSize += sizeof(LPZGTILENODE);
						}
					}

					uMaxLength = ZG_MAX(uMaxLength, uLength);

					if (uLength > pTileActionMapNode->uMaxCount)
					{
						pTileActionMapNode->uMaxCount = uLength;
						pTileActionMapNode->uMaxIndex = uIndex;

						uCount = ZG_MIN(uLength, pTileActionMapNode->uCount);
						for (i = 0; i < uCount; ++i)
							pTileActionMapNode->ppNodes[i] = ppTileNodes[i];
					}
				}
			}
		}
		else
			break;
	}

	ZGUINT uEvaluation = sg_uEvaluation * uMaxLength;

	return sg_uMinEvaluation > uEvaluation ? sg_uMinEvaluation - uEvaluation : 0;
}

ZGUINT ZGTileActionSearchBreadth(
	const ZGTILEACTION* pTileAction, 
	const ZGTILENODE* pTileNode,
	LPZGTILEMAP pTileMap,
	ZGUINT uEvaluation,
	ZGUINT uMinEvaluation,
	ZGUINT uMaxEvaluation, 
	ZGUINT uMaxDistance, 
	ZGUINT uMaxDepth, 
	ZGUINT uBufferLength,
	PZGUINT8 puBuffer, 
	ZGTILEACTIONTEST pfnTileActionTest)
{
	if (pTileAction == ZG_NULL || pTileNode == ZG_NULL || pTileMap == ZG_NULL)
		return 0;

	if (pTileMap->Instance.Instance.uCount <= pTileNode->uIndex)
		return 0;

	sg_pTileAction = pTileAction;
	sg_pTileRange = &pTileNode->Instance;
	sg_pTileMap = pTileMap;

	sg_pTileNodeData = pTileNode->pData;
	sg_pfnTileActionTest = pfnTileActionTest;

	sg_uEvaluation = uEvaluation;
	sg_uMinEvaluation = uMinEvaluation + 1;
	sg_uMaxEvaluation = uMaxEvaluation;
	sg_uBufferLength = uBufferLength;
	sg_puBuffer = puBuffer;

	return ZGNodeSearch(
		pTileMap->pNodes + pTileNode->uIndex,
		ZGTilePredicate,
		__ZGTileActionEvaluateBreadth,
		uMinEvaluation,
		uMaxEvaluation,
		uMaxDistance,
		uMaxDepth,
		ZG_NODE_SEARCH_TYPE_MAX);
}