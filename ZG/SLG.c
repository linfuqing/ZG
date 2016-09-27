#include <malloc.h>
#include "SLG.h"

#define ZG_SLG_MAP_NODE_SIZE 16
#define ZG_SLG_BUFFER_SIZE 1024

static ZGUINT8 sg_auBuffer[ZG_SLG_BUFFER_SIZE];
static ZGUINT sg_uIndex;
static ZGUINT sg_uOffset;
static ZGUINT sg_uCount;
static LPZGRBLIST sg_pRBList;

ZGBOOLEAN __ZGSLGAnalyzate(const void* pTileActionData, const void* pSourceTileNodeData, const void* pDestinationTileNodeData)
{
	return ((const ZGUINT*)pSourceTileNodeData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP] != ((const ZGUINT*)pDestinationTileNodeData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP];
}

ZGBOOLEAN __ZGSLGTestNode(const void* pTileNodeData, const ZGTILERANGE* pTileRange, LPZGTILEMAP pTileMap, ZGUINT uIndex)
{
	ZGUINT uCount = ZG_SLG_BUFFER_SIZE;
	PZGUINT puIndices = (PZGUINT)sg_auBuffer;
	if (ZGMapTest(&pTileMap->Instance, &pTileRange->Instance, uIndex, pTileRange->uOffset, &uCount, puIndices))
	{
		ZGUINT uCamp = ((PZGUINT)pTileNodeData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP];
		for (ZGUINT i = 0; i < uCount; ++i)
		{
			LPZGTILENODE pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, puIndices[i]))->pNode;
			if (uIndex == sg_uIndex || pTileNode == ZG_NULL || pTileNode->pData == ZG_NULL || ((PZGUINT)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_CAMP] != uCamp)
				return ZG_TRUE;
		}
	}

	return ZG_FALSE;
}

ZGBOOLEAN __ZGSLGTestAction(const void* pTileActionData, const void* pTileNodeData, const LPZGTILENODE* ppTileNodes, ZGUINT uNodeCount)
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

void __ZGSLGAction(void* pTileActionData, void* pTileNodeData, ZGUINT uIndex, ZGUINT uCount, LPZGTILENODE* ppTileNodes)
{
	sg_uIndex = uIndex;
	sg_uOffset = (ZGUINT)((ZGLONG)(ppTileNodes + uCount) - (ZGLONG)sg_auBuffer);
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
		if (lHp < 0)
			ZGRBListRemove(sg_pRBList, pRBListNode);

		uSize += sizeof(ZGSLGINFO);
		if (uSize <= uLength)
		{
			pInfos->pRBListNode = (LPZGRBLISTNODE)((PZGLONG)pTileNode->pData)[ZG_SLG_OBJECT_ATTRIBUTE_PARENT];
			pInfos->lDamage = lDamage;

			++pInfos;

			++sg_uCount;
		}
	}
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
	ZGUINT uActionCount, 
	LPZGRBLIST pRBList)
{
	ZGUINT uCount = uWidth * uHeight, uLength = (uCount + 7) >> 3;
	LPZGRBLISTNODE pResult = (LPZGRBLISTNODE)malloc(
		sizeof(ZGRBLISTNODE) + 
		sizeof(ZGTILEOBJECT) + 
		sizeof(ZGUINT8) * uLength + 
		sizeof(ZGLONG) * ZG_SLG_OBJECT_ATTRIBUTE_COUNT +
		sizeof(LPZGTILEACTION) * uActionCount);
	LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)(pResult + 1);
	PZGUINT8 puFlags = (PZGUINT8)(pTileObject + 1);
	PZGLONG plAttributes = (PZGLONG)(puFlags + uLength);
	LPZGTILEACTION* ppActions = (LPZGTILEACTION*)(plAttributes + ZG_SLG_OBJECT_ATTRIBUTE_COUNT);

	pTileObject->Instance.Instance.Instance.Instance.puFlags = puFlags;
	pTileObject->Instance.Instance.Instance.Instance.uOffset = 0;
	pTileObject->Instance.Instance.Instance.Instance.uCount = uCount;
	pTileObject->Instance.Instance.Instance.uPitch = uWidth;
	pTileObject->Instance.Instance.uOffset = uOffset;
	pTileObject->Instance.pData = plAttributes;
	pTileObject->Instance.uDistance = 0;
	pTileObject->Instance.uRange = 0;
	pTileObject->Instance.uIndex = ~0;
	pTileObject->uActionCount = uActionCount;
	pTileObject->ppActions = ppActions;

	ZGUINT i;
	for (i = 0; i < uActionCount; ++i)
		ppActions[i] = ZG_NULL;

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

LPZGTILEACTION ZGSLGCreateAction(
	ZGUINT uDistance,
	ZGUINT uRange)
{
	ZGUINT uDistanceLength = (uDistance << 1) + 1, uRangeLength = (uRange << 1) + 1;
	uDistanceLength = (uDistanceLength * uDistanceLength + 7) >> 3;
	uRangeLength = (uRangeLength * uRangeLength + 7) >> 3;
	LPZGTILEACTION pResult = (LPZGTILEACTION)malloc(sizeof(ZGTILEACTION) + sizeof(ZGUINT8) * uDistanceLength + sizeof(ZGUINT8) * uRangeLength);
	PZGUINT8 puDistanceFlags = (PZGUINT8)(pResult + 1), puRangeFlags = puDistanceFlags + uDistanceLength;
	ZGTileRangeInitOblique(&pResult->Distance, puDistanceFlags, uDistance);
	ZGTileRangeInitOblique(&pResult->Instance, puRangeFlags, uRange);
	pResult->pData = ZG_NULL;
	pResult->pfnAnalyzation = __ZGSLGAnalyzate;

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
	if (pTileObject->Instance.uIndex < pTileMap->Instance.Instance.uCount && !ZGSLGSearch(pRBListNode, pTileMap, uIndex))
		return ZG_FALSE;

	return ZGTileNodeSetTo(&pTileObject->Instance, pTileMap, uIndex);
}

ZGUINT ZGSLGSearch(LPZGRBLISTNODE pRBListNode, LPZGTILEMAP pTileMap, ZGUINT uIndex)
{
	if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
		return ZG_NULL;

	sg_uIndex = uIndex;
	return ZGTileNodeSearch(&((LPZGTILEOBJECT)(pRBListNode->pValue))->Instance, pTileMap, ZG_FALSE, uIndex, __ZGSLGTestNode);
}

ZGBOOLEAN ZGSLGAction(
	LPZGRBLISTNODE pRBListNode, 
	LPZGTILEMAP pTileMap, 
	LPZGRBLIST pRBList, 
	ZGUINT uActionIndex, 
	ZGUINT uMapIndex,
	PZGUINT puInfoCount,
	LPZGSLGINFO* ppInfos)
{
	if (pRBListNode == ZG_NULL || pTileMap == ZG_NULL || pRBList == ZG_NULL)
		return ZG_FALSE;

	if (pRBListNode->pValue == ZG_NULL)
		return ZG_FALSE;

	LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)pRBListNode->pValue;
	if (pTileObject->ppActions == ZG_NULL || pTileObject->uActionCount <= uActionIndex)
		return ZG_FALSE;

	LPZGTILEACTION pTileAction = pTileObject->ppActions[uActionIndex];
	if (pTileAction == ZG_NULL)
		return ZG_FALSE;

	ZGUINT uCount = ZG_SLG_BUFFER_SIZE * sizeof(ZGUINT8) / sizeof(ZGUINT);
	PZGUINT puIndices = (PZGUINT)sg_auBuffer;
	if (ZGMapTest(&pTileMap->Instance, &pTileAction->Instance.Instance, uMapIndex, pTileAction->Instance.uOffset, &uCount, puIndices))
	{
		ZGUINT uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE), uLength = 0, i, j;
		LPZGTILENODE *ppTileNodes = (LPZGTILENODE*)(puIndices + uCount), pTileNode;
		for (i = 0; i < uCount; ++i)
		{
			pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, puIndices[i]))->pNode;
			if (pTileNode == ZG_NULL)
				continue;

			if (pTileAction->pfnAnalyzation != ZG_NULL && !pTileAction->pfnAnalyzation(pTileAction->pData, pTileObject->Instance.pData, pTileNode->pData))
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
			sg_pRBList = pRBList;

			__ZGSLGAction(pTileAction->pData, pTileObject->Instance.pData, uMapIndex, uLength, ppTileNodes);

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
	ZGUINT uEvaluation,
	ZGUINT uMinEvaluation,
	ZGUINT uMaxEvaluation,
	ZGUINT uMaxDistance,
	ZGUINT uMaxDepth, 
	PZGUINT puActionIndex, 
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
		sg_pRBList = pRBList;

		uDepth = ZGTileObjectRun(
			pTileObject,
			pTileMap,
			uEvaluation,
			uMinEvaluation,
			uMaxEvaluation,
			uMaxDistance,
			uMaxDepth, 
			ZG_SLG_BUFFER_SIZE,
			sg_auBuffer,
			puActionIndex, 
			__ZGSLGTestAction,
			__ZGSLGAction);
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
