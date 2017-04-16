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
static ZGUINT sg_uLevelSize;

ZGUINT __ZGTileActionEvaluate(void* pMapNode)
{
	LPZGTILEMAPNODE pTemp = (LPZGTILEMAPNODE)pMapNode;

	ZGUINT uLevel = pTemp->uIndex / sg_uLevelSize,
		uOffset = uLevel * sg_uLevelSize,
		uIndex = pTemp->uIndex - uOffset;

	ZGMAP Map = sg_pTileMap->Instance;
	Map.Instance.uOffset += uOffset;
	Map.Instance.uCount = sg_uLevelSize;

	ZGUINT uTemp = sg_uBufferLength * sizeof(ZGUINT8) / sizeof(ZGUINT), uCount, uLength, uSize, i, j;
	PZGUINT puIndices;
	LPZGTILENODE pTileNode, *ppTileNodes;
	if (sg_pfnTileActionTest == ZG_NULL)
	{
		if (ZGMapTest(&Map, &sg_pTileRange->Instance, uIndex, sg_pTileRange->uOffset, ZG_NULL, ZG_NULL))
			return sg_pTileAction->uMaxEvaluation;
	}
	else
	{
		uCount = uTemp;
		puIndices = (PZGUINT)sg_puBuffer;
		if (ZGMapTest(&Map, &sg_pTileRange->Instance, uIndex, sg_pTileRange->uOffset, &uCount, puIndices))
		{
			uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE);
			if (uSize <= sg_uBufferLength)
			{
				uLength = 0;
				ppTileNodes = (LPZGTILENODE*)(puIndices + uCount);
				for (i = 0; i < uCount; ++i)
				{
					pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(sg_pTileMap, puIndices[i] + uOffset))->pNode;
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

	ZGUINT uLevelCount = sg_pTileAction->pInstance == ZG_NULL ? 0 : sg_pTileAction->pInstance->uLevelCount;
	PZGUINT puLevelIndices;
	if (uLevelCount > 0)
		puLevelIndices = sg_pTileAction->pInstance->puLevelIndices;
	else
	{
		puLevelIndices = &uLevel;

		uLevelCount = 1;
	}

	ZGUINT uMaxLength = 0, uMinBit, uMapIndex, uLevelIndex, k;
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
			uMapIndex = ZGTileConvert(
				sg_pTileMap->Instance.uPitch,
				uIndex,
				sg_pTileAction->pInstance->Distance.Instance.uPitch,
				BitFlag.uOffset - 1,
				sg_pTileAction->pInstance->Distance.uOffset);
			if (uMapIndex > 0)
			{
				--uMapIndex;
				for (i = 0; i < uLevelCount; ++i)
				{
					uLevelIndex = puLevelIndices[i];
					uOffset = uLevelIndex * sg_uLevelSize;

					Map.Instance.uOffset = sg_pTileMap->Instance.Instance.uOffset + uOffset;

					uCount = uTemp;
					puIndices = (PZGUINT)sg_puBuffer;
					if (ZGMapTest(
						&Map,
						&sg_pTileAction->pInstance->Instance.Instance,
						uMapIndex,
						sg_pTileAction->pInstance->Instance.uOffset,
						&uCount,
						puIndices))
					{
						uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE);
						uLength = 0;
						ppTileNodes = (LPZGTILENODE*)(puIndices + uCount);
						for (j = 0; j < uCount; ++j)
						{
							pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(sg_pTileMap, puIndices[j] + uOffset))->pNode;
							if (pTileNode == ZG_NULL)
								continue;

							if (sg_pTileAction->pfnAnalyzation != ZG_NULL && !sg_pTileAction->pfnAnalyzation(sg_pTileAction->pData, sg_pTileNodeData, pTileNode->pData))
								continue;

							for (k = 0; k < uLength; ++k)
							{
								if (ppTileNodes[k] == pTileNode)
									break;
							}

							if (k < uLength)
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
							pTileActionMapNode->uMaxIndex = uMapIndex;

							uCount = ZG_MIN(uLength, pTileActionMapNode->uCount);
							for (j = 0; j < uCount; ++j)
								pTileActionMapNode->ppNodes[j] = ppTileNodes[j];
						}
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

	sg_puBuffer = puBuffer;
	sg_uBufferLength = uBufferLength;

	sg_uLevelSize = pTileNode->pTileMap->Instance.Instance.uCount / pTileNode->pTileMap->uLevel;

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