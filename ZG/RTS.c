#include <malloc.h>
#include <math.h>

#include "RTS.h"
#include "Math.h"

#define ZG_RTS_MAP_NODE_SIZE 16
#define ZG_RTS_BUFFER_SIZE 1024
#define ZG_RTS_OUTPUT_SIZE 1024
#define ZG_RTS_INFO_SIZE 1024
#define ZG_RTS_HANDLER_SIZE 1024

static ZGUINT8 sg_auBuffer[ZG_RTS_BUFFER_SIZE];
static ZGUINT8 sg_auOutput[ZG_RTS_OUTPUT_SIZE];
static ZGRTSINFO sg_aInfos[ZG_RTS_INFO_SIZE];
static ZGRTSHANDLER sg_aHandler[ZG_RTS_HANDLER_SIZE];
static ZGUINT sg_auHandlerIndices[ZG_RTS_HANDLER_SIZE];
static ZGUINT sg_uHandlerIndexCount;
static ZGUINT sg_uOffset;
static ZGUINT sg_uCount;

static LPZGTILEMAP sg_pTileMap;

ZGBOOLEAN __ZGRTSAnalyzate(const void* pSourceTileNodeData, const void* pDestinationTileNodeData)
{
	return ((const ZGLONG*)pSourceTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_CAMP] != ((const ZGLONG*)pDestinationTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_CAMP];
}

ZGBOOLEAN __ZGRTSTestAction(const void* pTileNodeData, const LPZGTILENODE* ppTileNodes, ZGUINT uNodeCount)
{
	ZGUINT uCamp = ((const ZGUINT*)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_CAMP];
	LPZGTILENODE pTileNode;
	for (ZGUINT i = 0; i < uNodeCount; ++i)
	{
		pTileNode = ppTileNodes[i];
		if (pTileNode != ZG_NULL && pTileNode->pData != ZG_NULL && ((const ZGUINT*)pTileNode->pData)[ZG_RTS_OBJECT_ATTRIBUTE_CAMP] != uCamp)
			return ZG_FALSE;
	}

	return ZG_TRUE;
}

void __ZGRTSDelay(void* pTileNodeData, ZGUINT uElapsedTime, ZGUINT uTime)
{
	if (pTileNodeData == ZG_NULL)
		return;

	if (sg_uCount < ZG_RTS_INFO_SIZE)
	{
		LPZGRTSINFO pInfo = &sg_aInfos[sg_uCount++];
		pInfo->eType = ZG_RTS_INFO_TYPE_DELAY;
		pInfo->uElapsedTime = uElapsedTime;
		pInfo->uTime = uTime;
		pInfo->pTileManagerObject = (LPZGTILEMANAGEROBJECT)((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_PARENT];
	}
}

ZGUINT __ZGRTSMove(ZGUINT uElapsedTime, void* pTileActionObjectData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uFromIndex, ZGUINT uToIndex)
{
	if (pTileActionObjectData == ZG_NULL || pTileNodeData == ZG_NULL)
		return 0;

	ZGUINT uTime = (ZGUINT)((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_MOVE_TIME], 
		uFromIndexX = uFromIndex % pTileMap->Instance.uPitch, 
		uFromIndexY = uFromIndex / pTileMap->Instance.uPitch, 
		uToIndexX = uToIndex % pTileMap->Instance.uPitch, 
		uToIndexY = uToIndex / pTileMap->Instance.uPitch, 
		uDistanceX = ZG_ABS(uFromIndexX, uToIndexX), 
		uDistanceY = ZG_ABS(uFromIndexY, uToIndexY);
	uTime = (ZGUINT)roundf(uTime * sqrtf((ZGFLOAT)(uDistanceX * uDistanceX + uDistanceY * uDistanceY)));
	if (sg_uCount < ZG_RTS_INFO_SIZE)
	{
		LPZGRTSINFO pInfo = &sg_aInfos[sg_uCount++];
		pInfo->eType = ZG_RTS_INFO_TYPE_MOVE;
		pInfo->uElapsedTime = uElapsedTime;
		pInfo->uTime = uTime;
		pInfo->pTileManagerObject = (LPZGTILEMANAGEROBJECT)((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_PARENT];
		LPZGRTSINFOMOVE pInfoMove = (LPZGRTSINFOMOVE)(sg_auOutput + sg_uOffset);

		sg_uOffset += sizeof(ZGRTSINFOMOVE);
		if (sg_uOffset > ZG_RTS_OUTPUT_SIZE)
			pInfo->pMove = ZG_NULL;
		else
		{
			pInfoMove->uFromIndex = uFromIndex;
			pInfoMove->uToIndex = uToIndex;

			pInfo->pMove = pInfoMove;
		}
	}

	return uTime;
}

ZGUINT __ZGRTSSet(ZGUINT uElapsedTime, void* pTileActionObjectData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uCount, LPZGTILENODE* ppTileNodes, void** ppUserData)
{
	if (pTileActionObjectData == ZG_NULL || pTileNodeData == ZG_NULL)
		return 0;

	LPZGTILEOBJECTACTION pTileObjectAction = (LPZGTILEOBJECTACTION)((PZGLONG)pTileActionObjectData)[ZG_RTS_ACTION_ATTRIBUTE_PARENT];
	LPZGTILEMANAGEROBJECT pTileManagerObject = (LPZGTILEMANAGEROBJECT)((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_PARENT];
	if (ppUserData != ZG_NULL)
	{
		static ZGBOOLEAN s_bIsInitHandlers = ZG_TRUE;
		if (s_bIsInitHandlers)
		{
			s_bIsInitHandlers = ZG_FALSE;

			for (ZGUINT i = 0; i < ZG_RTS_HANDLER_SIZE; ++i)
				sg_auHandlerIndices[i] = i;

			sg_uHandlerIndexCount = ZG_RTS_HANDLER_SIZE;
		}

		if (sg_uHandlerIndexCount > 0)
		{
			LPZGRTSHANDLER pHandler = sg_aHandler + sg_auHandlerIndices[--sg_uHandlerIndexCount];
			pHandler->uTime = uElapsedTime + (ZGUINT)((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_HAND_TIME];
			pHandler->uIndex = uIndex;
			pHandler->pTileObjectAction = pTileObjectAction;
			pHandler->pTileManagerObject = pTileManagerObject;
			
			*ppUserData = pHandler;
		}
	}

	ZGUINT uTime = (ZGUINT)((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_SET_TIME];
	if (sg_uCount < ZG_RTS_INFO_SIZE)
	{
		LPZGRTSINFO pInfo = &sg_aInfos[sg_uCount++];
		pInfo->eType = ZG_RTS_INFO_TYPE_SET;
		pInfo->uElapsedTime = uElapsedTime;
		pInfo->uTime = uTime;
		pInfo->pTileManagerObject = pTileManagerObject;
		LPZGRTSINFOSET pInfoSet = (LPZGRTSINFOSET)(sg_auOutput + sg_uOffset);

		sg_uOffset += sizeof(LPZGRTSINFOHAND);
		if (sg_uOffset > ZG_RTS_OUTPUT_SIZE)
			pInfoSet = ZG_NULL;
		else
		{
			pInfoSet->uIndex = uIndex;
			pInfoSet->pTileObjectAction = pTileObjectAction;
		}

		pInfo->pSet = pInfoSet;
	}

	return uTime;
}

/*ZGUINT __ZGRTSDo(void* pTileActionObjectData, void* pTileNodeData, LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uCount, LPZGTILENODE* ppTileNodes)
{
	if (pTileActionObjectData == ZG_NULL || pTileNodeData == ZG_NULL)
		return 0;

	ZGUINT uTime = (ZGUINT)((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_SET_TIME];
	LPZGRTSINFOHAND pInfoHand;
	if (sg_uCount < ZG_RTS_INFO_SIZE)
	{
		LPZGRTSINFO pInfo = &sg_aInfos[sg_uCount++];
		pInfo->eType = ZG_RTS_INFO_TYPE_HAND;
		pInfo->uTime = uTime;

		pInfoHand = (LPZGRTSINFOHAND)(sg_auOutput + sg_uOffset);

		sg_uOffset += sizeof(LPZGRTSINFOHAND);
		if (sg_uOffset > ZG_RTS_OUTPUT_SIZE)
			pInfoHand = ZG_NULL;
		else
		{
			pInfoHand->uIndex = uIndex;
			pInfoHand->pTileObjectAction = (LPZGTILEOBJECTACTION)((PZGLONG)pTileActionObjectData)[ZG_RTS_ACTION_ATTRIBUTE_PARENT];
			pInfoHand->uTargetCount = 0;
			pInfoHand->pTargets = ZG_NULL;
		}

		pInfo->pHand = pInfoHand;
	}
	else
		pInfoHand = ZG_NULL;

	ZGUINT i, j;
	ZGLONG lDamage, lHp;
	LPZGTILENODE pTileNode;
	LPZGRTSINFOTARGET pInfoTargets = (LPZGRTSINFOTARGET)(sg_auOutput + sg_uOffset);

	for (i = 0; i < uCount; ++i)
	{
		pTileNode = ppTileNodes[i];
		if (pTileNode == ZG_NULL || pTileNode->pData == ZG_NULL)
			continue;

		lDamage = 0;
		for (j = 0; j < ZG_RTS_ELEMENT_COUNT; ++j)
			lDamage += ((PZGLONG)pTileNode->pData)[ZG_RTS_OBJECT_ATTRIBUTE_DEFENSE + j] - ((PZGLONG)pTileNodeData)[ZG_RTS_OBJECT_ATTRIBUTE_ATTACK + j];

		lHp = ((PZGLONG)pTileNode->pData)[ZG_RTS_OBJECT_ATTRIBUTE_HP] += lDamage;
		if (lHp <= 0)
			ZGTileNodeUnset(pTileNode, pTileMap);

		if (pInfoHand != ZG_NULL)
		{
			sg_uOffset += sizeof(ZGRTSINFOTARGET);
			if (sg_uOffset <= ZG_RTS_BUFFER_SIZE)
			{
				pInfoTargets->pTileManagerObject = (LPZGTILEOBJECT)((PZGLONG)pTileNode->pData)[ZG_RTS_OBJECT_ATTRIBUTE_PARENT];
				pInfoTargets->lHP = lDamage;

				if (pInfoHand->pTargets == ZG_NULL)
					pInfoHand->pTargets = pInfoTargets;

				++pInfoTargets;

				++pInfoHand->uTargetCount;
			}
		}
	}

	return uTime;
}*/

ZGBOOLEAN __ZGRTSDo(
	LPZGTILEOBJECTACTION pTileObjectAction,
	LPZGTILEMANAGEROBJECT pTileManagerObject,
	LPZGTILEMAP pTileMap,
	ZGUINT uIndex, 
	ZGUINT uElapsedTime)
{
	if (pTileObjectAction == ZG_NULL || pTileManagerObject == ZG_NULL || pTileMap == ZG_NULL)
		return ZG_FALSE;

	if (pTileObjectAction->pInstance == ZG_NULL || pTileObjectAction->pInstance->pInstance == ZG_NULL)
		return ZG_FALSE;

	ZGUINT uCount = ZG_RTS_BUFFER_SIZE * sizeof(ZGUINT8) / sizeof(ZGUINT);
	PZGUINT puIndices = (PZGUINT)sg_auBuffer;
	if (ZGMapTest(
		&pTileMap->Instance,
		&pTileObjectAction->pInstance->pInstance->Instance.Instance,
		uIndex,
		pTileObjectAction->pInstance->pInstance->Instance.uOffset,
		&uCount,
		puIndices))
	{
		ZGUINT uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE), uLength = 0, i, j;
		LPZGTILENODE *ppTileNodes = (LPZGTILENODE*)(puIndices + uCount), pTileNode;
		for (i = 0; i < uCount; ++i)
		{
			pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, puIndices[i]))->pNode;
			if (pTileNode == ZG_NULL)
				continue;

			if (pTileObjectAction->pInstance->pfnAnalyzation != ZG_NULL &&
				!pTileObjectAction->pInstance->pfnAnalyzation(pTileManagerObject->Instance.Instance.pData, pTileNode->pData))
				continue;

			for (j = 0; j < uLength; ++j)
			{
				if (ppTileNodes[j] == pTileNode)
					break;
			}

			if (j < uLength)
				continue;

			if (uSize > ZG_RTS_BUFFER_SIZE)
				++uLength;
			else
			{
				ppTileNodes[uLength++] = pTileNode;

				uSize += sizeof(LPZGTILENODE);
			}
		}

		if (uLength > 0)
		{
			//__ZGRTSDo(pTileObjectAction->pData, pTileManagerObject->Instance.Instance.pData, pTileMap, uIndex, uLength, ppTileNodes);
			LPZGRTSINFOHAND pInfoHand;
			if (sg_uCount < ZG_RTS_INFO_SIZE)
			{
				LPZGRTSINFO pInfo = &sg_aInfos[sg_uCount++];
				pInfo->eType = ZG_RTS_INFO_TYPE_HAND;
				pInfo->uElapsedTime = uElapsedTime;
				pInfo->uTime = 0;
				pInfo->pTileManagerObject = pTileManagerObject;

				pInfoHand = (LPZGRTSINFOHAND)(sg_auOutput + sg_uOffset);

				sg_uOffset += sizeof(LPZGRTSINFOHAND);
				if (sg_uOffset > ZG_RTS_OUTPUT_SIZE)
					pInfoHand = ZG_NULL;
				else
				{
					pInfoHand->uIndex = uIndex;
					pInfoHand->pTileObjectAction = pTileObjectAction;
					pInfoHand->uTargetCount = 0;
					pInfoHand->pTargets = ZG_NULL;
				}

				pInfo->pHand = pInfoHand;
			}
			else
				pInfoHand = ZG_NULL;

			ZGUINT i, j;
			ZGLONG lDamage, lHp;
			LPZGTILENODE pTileNode;
			LPZGRTSINFOTARGET pInfoTargets = (LPZGRTSINFOTARGET)(sg_auOutput + sg_uOffset);

			for (i = 0; i < uCount; ++i)
			{
				pTileNode = ppTileNodes[i];
				if (pTileNode == ZG_NULL || pTileNode->pData == ZG_NULL)
					continue;

				lDamage = 0;
				for (j = 0; j < ZG_RTS_ELEMENT_COUNT; ++j)
					lDamage += ((PZGLONG)pTileNode->pData)[ZG_RTS_OBJECT_ATTRIBUTE_DEFENSE + j] - ((PZGLONG)pTileManagerObject->Instance.Instance.pData)[ZG_RTS_OBJECT_ATTRIBUTE_ATTACK + j];

				lHp = ((PZGLONG)pTileNode->pData)[ZG_RTS_OBJECT_ATTRIBUTE_HP] += lDamage;
				if (lHp <= 0)
					ZGTileNodeUnset(pTileNode, pTileMap);

				if (pInfoHand != ZG_NULL)
				{
					sg_uOffset += sizeof(ZGRTSINFOTARGET);
					if (sg_uOffset <= ZG_RTS_BUFFER_SIZE)
					{
						pInfoTargets->pTileManagerObject = (LPZGTILEMANAGEROBJECT)((PZGLONG)pTileNode->pData)[ZG_RTS_OBJECT_ATTRIBUTE_PARENT];
						pInfoTargets->lHP = lDamage;

						if (pInfoHand->pTargets == ZG_NULL)
							pInfoHand->pTargets = pInfoTargets;

						++pInfoTargets;

						++pInfoHand->uTargetCount;
					}
				}
			}

			return ZG_TRUE;
		}
	}

	return ZG_FALSE;
}

ZGBOOLEAN __ZGRTSHand(ZGUINT uTime, void* pUserData)
{
	if (pUserData == ZG_NULL)
		return ZG_FALSE;

	LPZGRTSHANDLER pHandler = (LPZGRTSHANDLER)pUserData;

	if (pHandler->uTime > uTime)
	{
		pHandler->uTime -= uTime;

		return ZG_TRUE;
	}

	__ZGRTSDo(pHandler->pTileObjectAction, pHandler->pTileManagerObject, sg_pTileMap, pHandler->uIndex, pHandler->uTime);

	return ZG_FALSE;
}

void ZGRTSDestroy(void* pData)
{
	free(pData);
}

LPZGTILEMANAGEROBJECT ZGRTSCreateObject(
	ZGUINT uWidth,
	ZGUINT uHeight,
	ZGUINT uOffset)
{
	ZGUINT uCount = uWidth * uHeight, uLength = (uCount + 7) >> 3;
	LPZGTILEMANAGEROBJECT pResult = (LPZGTILEMANAGEROBJECT)malloc(
		sizeof(ZGTILEMANAGEROBJECT) +
		sizeof(ZGTILENODEDATA) +
		sizeof(ZGUINT8) * uLength +
		sizeof(ZGLONG) * ZG_RTS_OBJECT_ATTRIBUTE_COUNT);
	LPZGTILENODEDATA pTileNodeData = (LPZGTILENODEDATA)(pResult + 1);
	PZGUINT8 puFlags = (PZGUINT8)(pTileNodeData + 1);
	PZGLONG plAttributes = (PZGLONG)(puFlags + uLength);

	pResult->Instance.Instance.pInstance = pTileNodeData;
	pResult->Instance.Instance.pData = plAttributes;
	pResult->Instance.Instance.uIndex = ~0;

	pResult->Instance.nTime = 0;

	pResult->Instance.pActions = ZG_NULL;
	pResult->pHandler = ZG_NULL;
	pResult->pPrevious = ZG_NULL;
	pResult->pNext = ZG_NULL;

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

	for (ZGUINT i = 0; i < ZG_RTS_OBJECT_ATTRIBUTE_COUNT; ++i)
		plAttributes[i] = 0;

	plAttributes[ZG_RTS_OBJECT_ATTRIBUTE_PARENT] = (ZGLONG)pResult;

	return pResult;
}

LPZGTILEOBJECTACTION ZGRTSCreateAction(
	ZGUINT uDistance,
	ZGUINT uRange, 
	ZGUINT uChildCount)
{
	ZGUINT uDistanceLength = (uDistance << 1) + 1, uRangeLength = (uRange << 1) + 1;
	uDistanceLength = (uDistanceLength * uDistanceLength + 7) >> 3;
	uRangeLength = (uRangeLength * uRangeLength + 7) >> 3;
	LPZGTILEOBJECTACTION pResult = (LPZGTILEOBJECTACTION)malloc(
		sizeof(ZGTILEOBJECTACTION) +
		sizeof(ZGTILEACTION) +
		sizeof(ZGTILEACTIONDATA) +
		sizeof(ZGLONG) * (ZGUINT)(ZG_RTS_ACTION_ATTRIBUTE_COUNT)+
		sizeof(ZGUINT8) * uDistanceLength +
		sizeof(ZGUINT8) * uRangeLength + 
		sizeof(LPZGTILEOBJECTACTION) * uChildCount);
	LPZGTILEACTION pTileAction = (LPZGTILEACTION)(pResult + 1);
	LPZGTILEACTIONDATA pTileActionData = (LPZGTILEACTIONDATA)(pTileAction + 1);
	PZGLONG plAttributes = (PZGLONG)(pTileActionData + 1);
	PZGUINT8 puDistanceFlags = (PZGUINT8)(plAttributes + (ZGUINT)(ZG_RTS_ACTION_ATTRIBUTE_COUNT)), puRangeFlags = puDistanceFlags + uDistanceLength;
	pResult->pInstance = pTileAction;
	pResult->pData = plAttributes;
	pResult->uChildCount = uChildCount;
	pResult->ppChildren = (LPZGTILEOBJECTACTION*)(puRangeFlags + uRangeLength);

	ZGTileRangeInitOblique(&pTileActionData->Distance, puDistanceFlags, uDistance);
	ZGTileRangeInitOblique(&pTileActionData->Instance, puRangeFlags, uRange);

	pTileAction->pInstance = pTileActionData;
	pTileAction->uEvaluation = 0;
	pTileAction->uMinEvaluation = 0;
	pTileAction->uMaxEvaluation = 0;
	pTileAction->uMaxDistance = 0;
	pTileAction->uMaxDepth = 0;

	pTileAction->pfnAnalyzation = __ZGRTSAnalyzate;

	ZGUINT i;
	for (i = 0; i < (ZGUINT)(ZG_RTS_ACTION_ATTRIBUTE_COUNT); ++i)
		plAttributes[i] = 0;

	for (i = 0; i < uChildCount; ++i)
		pResult->ppChildren[i] = ZG_NULL;

	return pResult;
}

LPZGTILEMANAGER ZGRTSCreateManager(ZGUINT uCapacity, ZGUINT uWidth, ZGUINT uHeight, ZGBOOLEAN bIsOblique)
{
	ZGUINT uCount = uWidth * uHeight, uFlagLength = (uCount + 7) >> 3, uChildCount = ZGTileChildCount(uWidth, uHeight, bIsOblique), uNodeCount = uCount * ZG_RTS_MAP_NODE_SIZE;
	LPZGTILEMANAGER pResult = (LPZGTILEMANAGER)malloc(
		sizeof(ZGTILEMANAGER) + 
		sizeof(ZGTILEMANAGERHANDLER) * uCapacity + 
		sizeof(ZGTILEMAP) +
		sizeof(ZGUINT8) * uFlagLength +
		sizeof(LPZGNODE) * uChildCount +
		sizeof(ZGNODE) * uCount +
		sizeof(LPZGTILENODE) * uNodeCount +
		sizeof(ZGTILEMAPNODE) * uCount +
		sizeof(ZGTILENODEMAPNODE) * uCount +
		sizeof(ZGTILEACTIONMAPNODE) * uCount);
	LPZGTILEMANAGERHANDLER pTileManagerHandlers = (LPZGTILEMANAGERHANDLER)(pResult + 1);
	LPZGTILEMAP pTileMap = (LPZGTILEMAP)(pTileManagerHandlers + uCapacity);
	PZGUINT8 puFlags = (PZGUINT8)(pTileMap + 1);
	LPZGNODE* ppNodes = (LPZGNODE*)(puFlags + uFlagLength), pNodes = (LPZGNODE)(ppNodes + uChildCount);
	LPZGTILENODE* ppTileNodes = (LPZGTILENODE*)(pNodes + uCount);
	LPZGTILEMAPNODE pTileMapNodes = (LPZGTILEMAPNODE)(ppTileNodes + uNodeCount);
	LPZGTILENODEMAPNODE pTileNodeMapNodes = (LPZGTILENODEMAPNODE)(pTileMapNodes + uCount);
	LPZGTILEACTIONMAPNODE pTileActionMapNodes = (LPZGTILEACTIONMAPNODE)(pTileNodeMapNodes + uCount);
	pResult->pTileMap = pTileMap;
	pResult->pObjects = ZG_NULL;
	pResult->pQueue = ZG_NULL;
	
	ZGUINT i;
	LPZGTILEMANAGERHANDLER pTileManagerHandler = pTileManagerHandlers;
	for (i = 1; i < uCapacity; ++i)
	{
		pTileManagerHandler = pTileManagerHandlers + i;
		pTileManagerHandler->pUserData = ZG_NULL;
		pTileManagerHandler->pNext = pTileManagerHandlers + i - 1;
	}

	if (uCapacity > 0)
	{
		pTileManagerHandlers->pUserData = ZG_NULL;
		pTileManagerHandlers->pNext = ZG_NULL;

		pResult->pPool = pTileManagerHandler;
	}
	else
		pResult->pPool = ZG_NULL;

	ZGTileMapEnable(pTileMap, puFlags, ppNodes, pNodes, pTileMapNodes, uWidth, uHeight, bIsOblique);

	for (i = 0; i < uFlagLength; ++i)
		puFlags[i] = 0;

	for (i = 0; i < uCount; ++i)
	{
		pTileActionMapNodes->ppNodes = ppTileNodes + i * ZG_RTS_MAP_NODE_SIZE;
		pTileActionMapNodes->uCount = ZG_RTS_MAP_NODE_SIZE;
		pTileActionMapNodes->pData = ZG_NULL;
		pTileNodeMapNodes->uDistance = 1;
		pTileNodeMapNodes->pNode = ZG_NULL;
		pTileNodeMapNodes->pData = pTileActionMapNodes++;
		((LPZGTILEMAPNODE)pTileMap->pNodes[i].pData)->pData = pTileNodeMapNodes++;
	}

	return pResult;
}

void ZGRTSDestroyManager(LPZGTILEMANAGER pTileManager)
{
	if(pTileManager != ZG_NULL)
		ZGTileMapDisable(pTileManager->pTileMap);

	free(pTileManager);
}

ZGBOOLEAN ZGRTSSetManager(LPZGTILEMANAGER pTileManager, ZGUINT uIndex, ZGBOOLEAN bValue)
{
	if (pTileManager == ZG_NULL)
		return ZG_FALSE;

	return ZGTileMapSet(pTileManager->pTileMap, uIndex, bValue);
}

ZGBOOLEAN ZGRTSSetObjectToManager(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMANAGER pTileManager, ZGUINT uIndex)
{
	if (pTileManagerObject == ZG_NULL || pTileManager == ZG_NULL)
		return ZG_FALSE;

	if (pTileManager->pTileMap == ZG_NULL)
		return ZG_FALSE;

	if (pTileManagerObject->Instance.Instance.uIndex < pTileManager->pTileMap->Instance.Instance.uCount)
		return ZG_FALSE;

	if (ZGMapTest(
		&pTileManager->pTileMap->Instance,
		&pTileManagerObject->Instance.Instance.pInstance->Instance.Instance,
		uIndex,
		pTileManagerObject->Instance.Instance.pInstance->Instance.uOffset,
		ZG_NULL,
		ZG_NULL))
		return ZG_FALSE;

	return ZGTileNodeSetTo(&pTileManagerObject->Instance.Instance, pTileManager->pTileMap, uIndex);
}

ZGBOOLEAN ZGRTSUnsetObjectFromManager(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMANAGER pTileManager)
{
	if (pTileManagerObject == ZG_NULL || pTileManager == ZG_NULL)
		return ZG_FALSE;

	return ZGTileNodeUnset(&pTileManagerObject->Instance.Instance, pTileManager->pTileMap);
}

ZGBOOLEAN ZGRTSAddObjectToManager(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMANAGER pTileManager)
{
	return ZGTileManagerAdd(pTileManager, pTileManagerObject);
}

ZGBOOLEAN ZGRTSRemoveObjectFromManager(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMANAGER pTileManager)
{
	return ZGTileManagerRemove(pTileManager, pTileManagerObject);
}

ZGBOOLEAN ZGRTSDo(
	LPZGTILEMANAGER pTileManager,
	LPZGTILEMANAGEROBJECT pTileManagerObject,
	ZGUINT uIndex,
	ZGUINT uTime, 
	PZGUINT puInfoCount,
	LPZGRTSINFO* ppInfos)
{
	if (pTileManagerObject == ZG_NULL)
		return ZG_NULL;

	if (pTileManagerObject->Instance.pActions == ZG_NULL)
		return ZG_NULL;

	sg_uOffset = 0;
	sg_uCount = 0;

	ZGBOOLEAN bResult = ZGTileManagerSet(pTileManager, pTileManagerObject, pTileManagerObject->Instance.pActions->pData, uIndex, uTime, __ZGRTSSet);

	if (puInfoCount != ZG_NULL)
		*puInfoCount = sg_uCount;

	if (ppInfos != ZG_NULL)
		*ppInfos = sg_aInfos;

	return bResult;
}

LPZGRTSINFO ZGRTSRun(LPZGTILEMANAGER pTileManager, ZGUINT uTime, PZGUINT puInfoCount)
{
	sg_uOffset = 0;
	sg_uCount = 0;

	sg_pTileMap = pTileManager == ZG_NULL ? ZG_NULL : pTileManager->pTileMap;

	ZGTileManagerRun(
		pTileManager,
		uTime,
		ZG_RTS_BUFFER_SIZE,
		sg_auBuffer,
		__ZGRTSTestAction,
		__ZGRTSMove,
		__ZGRTSDelay, 
		__ZGRTSSet, 
		__ZGRTSHand);

	if (puInfoCount != ZG_NULL)
		*puInfoCount = sg_uCount;

	return sg_aInfos;
}
