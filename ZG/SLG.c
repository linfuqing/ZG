#include <malloc.h>
#include "SLG.h"

#define ZG_SLG_MAP_NODE_SIZE 16
#define ZG_SLG_BUFFER_SIZE 1024

static ZGUINT8 sg_auBuffer[ZG_SLG_BUFFER_SIZE];
static ZGUINT sg_uOffset;
static ZGUINT sg_uCount;
static ZGUINT sg_uIndex;
static ZGUINT sg_uData;
static LPZGRBLIST sg_pRBList;
static const ZGTILENODE* sg_pTileNode;
static const ZGTILEMAP* sg_pTileMap;

ZGBOOLEAN __ZGSLGAnalyzate(const void* pSourceTileNodeData, const void* pDestinationTileNodeData)
{
	return ((const ZGLONG*)pSourceTileNodeData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP] != ((const ZGLONG*)pDestinationTileNodeData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP];
}

ZGBOOLEAN __ZGSLGTestMap(LPZGMAP pMap, ZGUINT uIndex)
{
	LPZGTILENODE pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(sg_pTileMap, uIndex))->pNode;
	if (pTileNode == sg_pTileNode)
		return ZG_FALSE;

	if (uIndex == sg_uIndex || 
		pTileNode == ZG_NULL || 
		pTileNode->pData == ZG_NULL || 
		((PZGUINT)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP] != ((PZGUINT)sg_pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP])
		return ZG_TRUE;

	return ZG_FALSE;
}

ZGBOOLEAN __ZGSLGTestTileMap(LPZGTILEMAP pTileMap, const ZGTILERANGE* pTileRange, ZGUINT uIndex)
{
	((PZGUINT)sg_auBuffer)[sg_uCount++] = uIndex;

	return ZG_FALSE;
}

ZGBOOLEAN __ZGSLGTestAction(const void* pTileNodeData, const LPZGTILENODE* ppTileNodes, ZGUINT uNodeCount)
{
	ZGUINT uCamp = ((const ZGUINT*)pTileNodeData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP];
	LPZGTILENODE pTileNode;
	for (ZGUINT i = 0; i < uNodeCount; ++i)
	{
		pTileNode = ppTileNodes[i];
		if (pTileNode != ZG_NULL && pTileNode->pData != ZG_NULL && ((const ZGUINT*)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP] != uCamp)
			return ZG_FALSE;
	}

	return ZG_TRUE;
}

ZGUINT __ZGSLGAction(ZGUINT uElapsedTime, void* pTileActionObjectData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uCount, LPZGTILENODE* ppTileNodes)
{
	if (pTileActionObjectData == ZG_NULL)
		return 0;

	PZGLONG plAttributes = (PZGLONG)pTileActionObjectData;
	sg_uData = (ZGUINT)plAttributes[ZG_SLG_ACTION_ATTRIBUTE_DATA];

	sg_uIndex = uIndex;
	//sg_uOffset = (ZGUINT)((ZGLONG)(ppTileNodes + uCount) - (ZGLONG)sg_auBuffer);
	sg_uCount = 0;

	ZGUINT i, j, uLength = ZG_SLG_BUFFER_SIZE - sg_uOffset, uSize = 0;
	ZGLONG lDamage, lHp;
	LPZGTILENODE pTileNode;
	LPZGRBLISTNODE pRBListNode;
	LPZGSLGINFO pInfos = (LPZGSLGINFO)(sg_auBuffer + sg_uOffset);

	for (i = 0; i < uCount; ++i)
	{
		pTileNode = ppTileNodes[i];
		if (pTileNode == ZG_NULL)
			continue;

		lDamage = 0;
		for (j = 0; j < ZG_SLG_ELEMENT_COUNT; ++j)
		{
			lDamage += ((PZGLONG)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_DEFENSE + j] - ((PZGLONG)pTileNodeData)[ZG_SLG_OBJECT_ATTRIBUTE_ATTACK + j];
		}

		pRBListNode = (LPZGRBLISTNODE)((PZGLONG)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_PARENT];

		lHp = ((PZGLONG)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_HP] += lDamage;
		if (lHp <= 0)
		{
			ZGTileNodeUnset(pTileNode, pTileMap);

			ZGRBListRemove(sg_pRBList, pRBListNode);
		}

		uSize += sizeof(ZGSLGINFO);
		if (uSize <= uLength)
		{
			pInfos->pRBListNode = (LPZGRBLISTNODE)((PZGLONG)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_PARENT];
			pInfos->lHP = lDamage;

			++pInfos;

			++sg_uCount;
		}
	}

	return 1;
}

void ZGSLGDestroy(void* pData)
{
	free(pData);
}

LPZGRBLISTNODE ZGSLGCreateObject(
	ZGUINT uWidth,
	ZGUINT uHeight,
	ZGUINT uOffset, 
	ZGUINT uTime, 
	LPZGRBLIST pRBList)
{
	ZGUINT uCount = uWidth * uHeight, uLength = (uCount + 7) >> 3;
	LPZGRBLISTNODE pResult = (LPZGRBLISTNODE)malloc(
		sizeof(ZGRBLISTNODE) + 
		sizeof(ZGTILEOBJECT) + 
		sizeof(ZGTILENODEDATA) +
		sizeof(ZGUINT8) * uLength + 
		sizeof(ZGLONG) * ZG_SLG_OBJECT_ATTRIBUTE_COUNT);
	LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)(pResult + 1);
	LPZGTILENODEDATA pTileNodeData = (LPZGTILENODEDATA)(pTileObject + 1);
	PZGUINT8 puFlags = (PZGUINT8)(pTileNodeData + 1);
	PZGLONG plAttributes = (PZGLONG)(puFlags + uLength);

	pTileObject->Instance.pInstance = pTileNodeData;
	pTileObject->Instance.pData = plAttributes;
	pTileObject->Instance.uIndex = ~0;

	pTileObject->pActions = ZG_NULL;

	pTileNodeData->Instance.Instance.Instance.puFlags = puFlags;
	pTileNodeData->Instance.Instance.Instance.uOffset = 0;
	pTileNodeData->Instance.Instance.Instance.uCount = uCount;
	pTileNodeData->Instance.Instance.uPitch = uWidth;
	pTileNodeData->Instance.uOffset = uOffset;

	pTileNodeData->uDistance = 0;
	pTileNodeData->uRange = 0;

	ZGUINT i;
	--uLength;

	for (i = 0; i < uLength; ++i)
		puFlags[i] = ~0;

	uCount &= 7;
	puFlags[uLength] = uCount > 0 ? (1 << uCount) - 1 : ~0;

	for (ZGUINT i = 0; i < ZG_SLG_OBJECT_ATTRIBUTE_COUNT; ++i)
		plAttributes[i] = 0;

	plAttributes[ZG_SLG_OBJECT_ATTRIBUTE_PARENT] = (ZGLONG)pResult;

	ZGRBListAdd(pRBList, pResult, pTileObject, (const void*)(ZGLONG)uTime, ZG_TRUE);

	return pResult;
}

LPZGTILEOBJECTACTION ZGSLGCreateAction(
	ZGUINT uDistance,
	ZGUINT uRange)
{
	ZGUINT uDistanceLength = (uDistance << 1) + 1, uRangeLength = (uRange << 1) + 1;
	uDistanceLength = (uDistanceLength * uDistanceLength + 7) >> 3;
	uRangeLength = (uRangeLength * uRangeLength + 7) >> 3;
	LPZGTILEOBJECTACTION pResult = (LPZGTILEOBJECTACTION)malloc(
		sizeof(ZGTILEOBJECTACTION) + 
		sizeof(ZGTILEACTION) + 
		sizeof(ZGTILEACTIONDATA) +
		sizeof(ZGLONG) * (ZGUINT)(ZG_SLG_ACTION_ATTRIBUTE_COUNT) +
		sizeof(ZGUINT8) * uDistanceLength + 
		sizeof(ZGUINT8) * uRangeLength);
	LPZGTILEACTION pTileAction = (LPZGTILEACTION)(pResult + 1);
	LPZGTILEACTIONDATA pTileActionData = (LPZGTILEACTIONDATA)(pTileAction + 1);
	PZGLONG plAttributes = (PZGLONG)(pTileActionData + 1);
	PZGUINT8 puDistanceFlags = (PZGUINT8)(plAttributes + (ZGUINT)(ZG_SLG_ACTION_ATTRIBUTE_COUNT)), puRangeFlags = puDistanceFlags + uDistanceLength;
	pResult->pInstance = pTileAction;
	pResult->pData = plAttributes;
	pResult->ppChildren = ZG_NULL;
	pResult->uChildCount = 0;

	ZGTileRangeInitOblique(&pTileActionData->Distance, puDistanceFlags, uDistance);
	ZGTileRangeInitOblique(&pTileActionData->Instance, puRangeFlags, uRange);

	pTileAction->pInstance = pTileActionData;

	pTileAction->uEvaluation = 0;
	pTileAction->uMinEvaluation = 0;
	pTileAction->uMaxEvaluation = 0;
	pTileAction->uMaxDistance = 0;
	pTileAction->uMaxDepth = 0;

	pTileAction->pfnAnalyzation = __ZGSLGAnalyzate;

	ZGUINT i;
	for (i = 0; i < (ZGUINT)(ZG_SLG_ACTION_ATTRIBUTE_COUNT); ++i)
		plAttributes[i] = 0;

	return pResult;
}

LPZGRBLIST ZGSLGCreateQueue()
{
	LPZGRBLIST pResult = (LPZGRBLIST)malloc(sizeof(ZGRBLIST));
	ZGRBListInit(pResult, ZGComparsionPointer);

	return pResult;
}

LPZGTILEMAP ZGSLGCreateMap(ZGUINT uWidth, ZGUINT uHeight, ZGBOOLEAN bIsOblique)
{
	ZGUINT uCount = uWidth * uHeight, uFlagLength = (uCount + 7) >> 3, uChildCount = ZGTileChildCount(uWidth, uHeight, bIsOblique), uNodeCount = uCount * ZG_SLG_MAP_NODE_SIZE;
	LPZGTILEMAP pTileMap = (LPZGTILEMAP)malloc(sizeof(ZGTILEMAP) + 
		sizeof(ZGUINT8) * uFlagLength +
		sizeof(LPZGNODE) * uChildCount +
		sizeof(ZGNODE) * uCount + 
		sizeof(LPZGTILENODE) * uNodeCount +
		sizeof(ZGTILEMAPNODE) * uCount + 
		sizeof(ZGTILENODEMAPNODE) * uCount + 
		sizeof(ZGTILEACTIONMAPNODE) * uCount);
	PZGUINT8 puFlags = (PZGUINT8)(pTileMap + 1);
	LPZGNODE* ppNodes = (LPZGNODE*)(puFlags + uFlagLength), pNodes = (LPZGNODE)(ppNodes + uChildCount);
	LPZGTILENODE* ppTileNodes = (LPZGTILENODE*)(pNodes + uCount);
	LPZGTILEMAPNODE pTileMapNodes = (LPZGTILEMAPNODE)(ppTileNodes + uNodeCount);
	LPZGTILENODEMAPNODE pTileNodeMapNodes = (LPZGTILENODEMAPNODE)(pTileMapNodes + uCount);
	LPZGTILEACTIONMAPNODE pTileActionMapNodes = (LPZGTILEACTIONMAPNODE)(pTileNodeMapNodes + uCount);
	ZGTileMapEnable(pTileMap, puFlags, ppNodes, pNodes, pTileMapNodes, uWidth, uHeight, bIsOblique);

	ZGUINT i;
	for (i = 0; i < uFlagLength; ++i)
		puFlags[i] = 0;

	for (i = 0; i < uCount; ++i)
	{
		pTileActionMapNodes->ppNodes = ppTileNodes + i * ZG_SLG_MAP_NODE_SIZE;
		pTileActionMapNodes->uCount = ZG_SLG_MAP_NODE_SIZE;
		pTileActionMapNodes->pData = ZG_NULL;
		pTileNodeMapNodes->uDistance = 1;
		pTileNodeMapNodes->pNode = ZG_NULL;
		pTileNodeMapNodes->pData = pTileActionMapNodes++;
		((LPZGTILEMAPNODE)pTileMap->pNodes[i].pData)->pData = pTileNodeMapNodes++;
	}

	return pTileMap;
}

void ZGSLGDestroyMap(LPZGTILEMAP pTileMap)
{
	ZGTileMapDisable(pTileMap);
	free(pTileMap);
}

LPZGRBLISTNODE ZGSLGGetNextFromObject(LPZGRBLISTNODE pRBListNode)
{
	return ZGRBListNodeNext(pRBListNode);
}

void ZGSLGSetMap(LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGBOOLEAN bValue)
{
	ZGTileMapSet(pTileMap, uIndex, bValue);
}

ZGBOOLEAN ZGSLGMoveObjectToMap(LPZGRBLISTNODE pRBListNode, LPZGTILEMAP pTileMap, ZGUINT uIndex)
{
	if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
		return ZG_NULL;

	LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)(pRBListNode->pValue);
	if (pTileObject->Instance.uIndex < pTileMap->Instance.Instance.uCount && !ZGSLGSearchDepth(pRBListNode, pTileMap, uIndex))
		return ZG_FALSE;

	return ZGTileNodeSetTo(&pTileObject->Instance, pTileMap, uIndex);
}

ZGUINT ZGSLGSearchDepth(const ZGRBLISTNODE* pRBListNode, LPZGTILEMAP pTileMap, ZGUINT uIndex)
{
	if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
		return ZG_NULL;

	sg_uIndex = uIndex;
	sg_pTileNode = &((const ZGTILEOBJECT*)pRBListNode->pValue)->Instance;
	sg_pTileMap = pTileMap;
	return ZGTileNodeSearchDepth(sg_pTileNode, pTileMap, ZG_FALSE, uIndex, __ZGSLGTestMap);
}

ZGUINT ZGSLGSearchBreadth(const ZGRBLISTNODE* pRBListNode, LPZGTILEMAP pTileMap, PZGUINT* ppIndices)
{
	if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
		return ZG_NULL;

	sg_pTileNode = &((const ZGTILEOBJECT*)pRBListNode->pValue)->Instance;
	sg_pTileMap = pTileMap;
	sg_uCount = 0;
	ZGTileNodeSearchBreadth(sg_pTileNode, pTileMap, __ZGSLGTestMap, __ZGSLGTestTileMap);

	if (ppIndices != ZG_NULL)
		*ppIndices = (PZGUINT)sg_auBuffer;

	return sg_uCount;
}

ZGBOOLEAN ZGSLGAction(
	LPZGTILEOBJECTACTION pTileObjectAction,
	LPZGRBLISTNODE pRBListNode, 
	LPZGTILEMAP pTileMap, 
	LPZGRBLIST pRBList,  
	ZGUINT uMapIndex,
	PZGUINT puInfoCount,
	LPZGSLGINFO* ppInfos)
{
	if (pTileObjectAction == ZG_NULL || pRBListNode == ZG_NULL || pTileMap == ZG_NULL || pRBList == ZG_NULL)
		return ZG_FALSE;

	if (pTileObjectAction->pInstance == ZG_NULL || pTileObjectAction->pInstance->pInstance == ZG_NULL || pRBListNode->pValue == ZG_NULL)
		return ZG_FALSE;

	LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)pRBListNode->pValue;

	ZGUINT uCount = ZG_SLG_BUFFER_SIZE * sizeof(ZGUINT8) / sizeof(ZGUINT);
	PZGUINT puIndices = (PZGUINT)sg_auBuffer;
	if (ZGMapTest(&pTileMap->Instance, &pTileObjectAction->pInstance->pInstance->Instance.Instance, uMapIndex, pTileObjectAction->pInstance->pInstance->Instance.uOffset, &uCount, puIndices))
	{
		ZGUINT uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE), uLength = 0, i, j;
		LPZGTILENODE *ppTileNodes = (LPZGTILENODE*)(puIndices + uCount), pTileNode;
		for (i = 0; i < uCount; ++i)
		{
			pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, puIndices[i]))->pNode;
			if (pTileNode == ZG_NULL)
				continue;

			if (pTileObjectAction->pInstance->pfnAnalyzation != ZG_NULL && !pTileObjectAction->pInstance->pfnAnalyzation(pTileObject->Instance.pData, pTileNode->pData))
				continue;

			for (j = 0; j < uLength; ++j)
			{
				if (ppTileNodes[j] == pTileNode)
					break;
			}

			if (j < uLength)
				continue;

			if (uSize > ZG_SLG_BUFFER_SIZE)
				++uLength;
			else
			{
				ppTileNodes[uLength++] = pTileNode;

				uSize += sizeof(LPZGTILENODE);
			}
		}

		if (uLength > 0)
		{
			sg_uOffset = (ZGUINT)((ZGLONG)ppTileNodes - (ZGLONG)sg_auBuffer);
			sg_pRBList = pRBList;

			__ZGSLGAction(0, pTileObjectAction->pData, pTileObject->Instance.pData, pTileMap, uMapIndex, uLength, ppTileNodes);

			if (puInfoCount != ZG_NULL)
				*puInfoCount = sg_uCount;

			if (ppInfos != ZG_NULL)
				*ppInfos = (LPZGSLGINFO)(sg_auBuffer + sg_uOffset);

			return ZG_TRUE;
		}
	}

	return ZG_FALSE;
}

ZGUINT ZGSLGRun(
	LPZGRBLIST pRBList, 
	LPZGTILEMAP pTileMap,
	PZGUINT puData,
	PZGUINT puMapIndex, 
	PZGUINT puInfoCount, 
	LPZGSLGINFO* ppInfos)
{
	LPZGRBLISTNODE pRBListNode = pRBList == ZG_NULL ? ZG_NULL : pRBList->pHead;
	if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
		return 0;

	LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)pRBListNode->pValue;
	if (pTileObject->Instance.pData == ZG_NULL)
		return 0;

	ZGUINT uFlag = (ZGUINT)((PZGLONG)pTileObject->Instance.pData)[ZG_SLG_OBJECT_ATTRIBUTE_FLAG], uDepth = 0;
	if (ZG_TEST_BIT(uFlag, ZG_SLG_OBJECT_FLAG_AUTO))
	{
		sg_uOffset = 0;
		sg_pRBList = pRBList;

		uDepth = ZGTileObjectRun(
			pTileObject,
			pTileMap,
			1,
			ZG_SLG_BUFFER_SIZE,
			sg_auBuffer,
			__ZGSLGTestAction,
			ZG_NULL, 
			__ZGSLGAction);

		if (puData != ZG_NULL)
			*puData = sg_uData;
	}

	ZGRBListRemove(pRBList, pRBListNode);
	ZGRBListAdd(
		pRBList,
		pRBListNode, 
		pRBListNode->pValue, 
		(const void*)((ZGLONG)pRBListNode->Instance.pKey + ((PZGLONG)pTileObject->Instance.pData)[ZG_SLG_OBJECT_ATTRIBUTE_COOLDOWN]),
		ZG_TRUE);

	if (puMapIndex != ZG_NULL)
		*puMapIndex = sg_uIndex;

	if (puInfoCount != ZG_NULL)
		*puInfoCount = sg_uCount;

	if (ppInfos != ZG_NULL)
		*ppInfos = (LPZGSLGINFO)(sg_auBuffer + sg_uOffset);

	return uDepth;
}
