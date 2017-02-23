#include "TileAction.h"
#include "Math.h"

static const ZGTILEACTION* sg_pTileAction;
static const ZGTILERANGE* sg_pTileRange;
static const ZGTILEMAP* sg_pTileMap;

static const void* sg_pTileNodeData;
static ZGTILEACTIONEVALUATION sg_pfnTileActionEvaluation;
static ZGTILEACTIONTEST sg_pfnTileActionTest;

static PZGUINT8 sg_puBuffer;
static ZGUINT sg_uBufferLength;

ZGUINT __ZGTileActionEvaluate(void* pMapNode)
{
	LPZGTILEMAPNODE pTemp = (LPZGTILEMAPNODE)pMapNode;
	ZGUINT uTemp = sg_uBufferLength * sizeof(ZGUINT8) / sizeof(ZGUINT), uCount, uLength, uSize, i, j;
	PZGUINT puIndices;
	LPZGTILENODE pTileNode, *ppTileNodes;
	if (sg_pfnTileActionTest == ZG_NULL)
	{
		if (ZGMapTest(&sg_pTileMap->Instance, &sg_pTileRange->Instance, pTemp->uIndex, sg_pTileRange->uOffset, ZG_NULL, ZG_NULL))
			return sg_pTileAction->uMaxEvaluation;
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
					if (pTileNode == ZG_NULL || pTileNode->pInstance == ZG_NULL)
						return sg_pTileAction->uMaxEvaluation;

					if (&pTileNode->pInstance->Instance == sg_pTileRange)
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
					return sg_pfnTileActionTest(sg_pTileNodeData, ppTileNodes, uLength) ? 
					(sg_pTileAction->uMinEvaluation + 1 + (sg_pfnTileActionEvaluation == ZG_NULL ? 0 : sg_pfnTileActionEvaluation(sg_pTileMap, pTemp->uIndex))) :
					sg_pTileAction->uMaxEvaluation;
			}
		}
	}

	if (sg_pfnTileActionEvaluation != ZG_NULL)
	{
		ZGUINT uEvaluation = sg_pfnTileActionEvaluation(sg_pTileMap, pTemp->uIndex);
		if (uEvaluation > 0)
			return sg_pTileAction->uMinEvaluation + 1 + uEvaluation;
	}

	ZGUINT uMaxLength = 0, uMinBit, uIndex;
	ZGBITFLAG BitFlag = sg_pTileAction->pInstance->Distance.Instance.Instance;
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
				sg_pTileAction->pInstance->Distance.Instance.uPitch,
				BitFlag.uOffset - 1,
				sg_pTileAction->pInstance->Distance.uOffset);
			if (uIndex > 0)
			{
				--uIndex;
				uCount = uTemp;
				puIndices = (PZGUINT)sg_puBuffer;
				if (ZGMapTest(
					&sg_pTileMap->Instance,
					&sg_pTileAction->pInstance->Instance.Instance,
					uIndex,
					sg_pTileAction->pInstance->Instance.uOffset,
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

						if (sg_pTileAction->pfnAnalyzation != ZG_NULL && !sg_pTileAction->pfnAnalyzation(sg_pTileNodeData, pTileNode->pData))
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

	ZGUINT uEvaluation = sg_pTileAction->uEvaluation * uMaxLength;

	return sg_pTileAction->uMinEvaluation < uEvaluation ? 0 : sg_pTileAction->uMinEvaluation - uEvaluation + 1;
}

ZGUINT ZGTileActionSearch(
	const ZGTILEACTION* pTileAction, 
	LPZGTILENODE pTileNode,
	ZGUINT uBufferLength,
	PZGUINT8 puBuffer, 
	ZGNODEPREDICATION pfnPredication,
	ZGTILEACTIONEVALUATION pfnTileActionEvaluation,
	ZGTILEACTIONTEST pfnTileActionTest, 
	ZGNODESEARCHTYPE eType)
{
	if (pTileAction == ZG_NULL || pTileNode == ZG_NULL)
		return 0;

	if (pTileAction->pInstance == ZG_NULL || 
		pTileNode->pInstance == ZG_NULL || 
		pTileNode->pTileMap == ZG_NULL || 
		pTileNode->pTileMap->Instance.Instance.uCount <= pTileNode->uIndex)
		return 0;

	sg_pTileAction = pTileAction;
	sg_pTileRange = &pTileNode->pInstance->Instance;
	sg_pTileMap = pTileNode->pTileMap;

	sg_pTileNodeData = pTileNode->pData;
	sg_pfnTileActionEvaluation = pfnTileActionEvaluation;
	sg_pfnTileActionTest = pfnTileActionTest;

	sg_uBufferLength = uBufferLength;
	sg_puBuffer = puBuffer;

	return ZGNodeSearch(
		pTileNode->pTileMap->pNodes + pTileNode->uIndex,
		pfnPredication,
		__ZGTileActionEvaluate,
		pTileAction->uMinEvaluation,
		pTileAction->uMaxEvaluation,
		pTileAction->uMaxDistance,
		pTileAction->uMaxDepth, 
		eType);
}