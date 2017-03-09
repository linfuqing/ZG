#include <malloc.h>
#include <stdlib.h>
#include <math.h>

#include "RTS.h"
#include "Math.h"

#define ZG_RTS_MAP_NODE_SIZE 16
#define ZG_RTS_BUFFER_SIZE 1024
#define ZG_RTS_OUTPUT_SIZE 1024
#define ZG_RTS_INFO_SIZE 1024

static ZGUINT8 sg_auBuffer[ZG_RTS_BUFFER_SIZE];
static ZGUINT8 sg_auOutput[ZG_RTS_OUTPUT_SIZE];
static ZGRTSINFO sg_aInfos[ZG_RTS_INFO_SIZE];
static ZGUINT sg_uOffset;
static ZGUINT sg_uCount;
static const ZGTILEMAP* sg_pTileMap;

ZGUINT __ZGRTSPredicate(const void* x, const void* y)
{
	if (x == ZG_NULL || y == ZG_NULL)
		return 0;

	ZGUINT uFromIndex = ((const ZGTILEMAPNODE*)x)->uIndex, uToIndex = ((const ZGTILEMAPNODE*)y)->uIndex;
	ZGFLOAT fDistance = (ZGRTSGetDistanceFromMap(sg_pTileMap, uFromIndex) + ZGRTSGetDistanceFromMap(sg_pTileMap, uToIndex)) * 0.5f;
	if (sg_pTileMap != ZG_NULL)
	{
		ZGUINT uFromIndexX = uFromIndex % sg_pTileMap->Instance.uPitch,
			uFromIndexY = uFromIndex / sg_pTileMap->Instance.uPitch,
			uToIndexX = uToIndex % sg_pTileMap->Instance.uPitch,
			uToIndexY = uToIndex / sg_pTileMap->Instance.uPitch,
			uDistanceX = ZG_ABS(uFromIndexX, uToIndexX),
			uDistanceY = ZG_ABS(uFromIndexY, uToIndexY);

		fDistance += sqrtf((ZGFLOAT)(uDistanceX * uDistanceX + uDistanceY * uDistanceY));
	}

	return (ZGUINT)roundf(fDistance);
}

ZGBOOLEAN __ZGRTSAnalyzate(const void* pTileActionData, const void* pSourceTileNodeData, const void* pDestinationTileNodeData)
{
	if (pTileActionData == ZG_NULL)
		return ZG_FALSE;

	const ZGRTSACTION* pAction = (const ZGRTSACTION*)pTileActionData;
	if (ZG_TEST_BIT(pAction->uFlag, ZG_RTS_ACTION_TYPE_SELF))
		return pSourceTileNodeData == pDestinationTileNodeData;

	if (pDestinationTileNodeData == ZG_NULL)
		return ZG_FALSE;

	const ZGRTSNODE* pDestination = (const ZGRTSNODE*)pDestinationTileNodeData;
	if (!ZG_TEST_FLAG(pAction->uSearchLabel, pDestination->uLabel))
		return ZG_FALSE;

	if (pSourceTileNodeData == ZG_NULL)
		return ZG_FALSE;

	const ZGRTSNODE* pSource = (const ZGRTSNODE*)pSourceTileNodeData;
	if(ZG_TEST_BIT(pAction->uFlag, ZG_RTS_ACTION_TYPE_ALLY))
		return pSource->uCamp == pDestination->uCamp;

	if (ZG_TEST_BIT(pAction->uFlag, ZG_RTS_ACTION_TYPE_ENEMY))
		return pSource->uCamp != pDestination->uCamp;

	return ZG_FALSE;
}

ZGBOOLEAN __ZGRTSCheck(const void* pTileActionData, const void* pSourceTileNodeData, const void* pDestinationTileNodeData)
{
	if (pTileActionData == ZG_NULL || pDestinationTileNodeData == ZG_NULL)
		return ZG_FALSE;

	return ZG_TEST_FLAG(((const ZGRTSACTION*)pTileActionData)->uSetLabel, ((const ZGRTSNODE*)pDestinationTileNodeData)->uLabel);
}

ZGBOOLEAN __ZGRTSTestAction(const void* pTileNodeData, const LPZGTILENODE* ppTileNodes, ZGUINT uNodeCount)
{
	LPZGTILENODE pTileNode;
	for (ZGUINT i = 0; i < uNodeCount; ++i)
	{
		pTileNode = ppTileNodes[i];
		if (pTileNode != ZG_NULL && pTileNode->pData != ZG_NULL && pTileNode->pData != pTileNodeData)
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
		pInfo->pTileManagerObject = ((LPZGRTSNODE)pTileNodeData)->pTileManagerObject;
	}
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

	if (pHandler->pfnInstance != ZG_NULL)
		pHandler->pfnInstance(pHandler->pTileObjectAction, pHandler->pTileManagerObject, pHandler->uIndex, pHandler->uTime);

	return ZG_FALSE;
}

ZGUINT __ZGRTSMove(
	ZGUINT uElapsedTime, 
	void* pTileObjectActionData,
	void* pTileNodeData, 
	LPZGTILEMAP pTileMap, 
	ZGUINT uFromIndex, 
	ZGUINT uToIndex)
{
	if (pTileNodeData == ZG_NULL)
		return 0;

	ZGUINT uTime = ((LPZGRTSNODE)pTileNodeData)->auAttributes[ZG_RTS_OBJECT_ATTRIBUTE_MOVE_TIME], 
		uFromIndexX = uFromIndex % pTileMap->Instance.uPitch, 
		uFromIndexY = uFromIndex / pTileMap->Instance.uPitch, 
		uToIndexX = uToIndex % pTileMap->Instance.uPitch, 
		uToIndexY = uToIndex / pTileMap->Instance.uPitch, 
		uDistanceX = ZG_ABS(uFromIndexX, uToIndexX), 
		uDistanceY = ZG_ABS(uFromIndexY, uToIndexY);
	uTime = (ZGUINT)roundf(
		uTime * 
		((ZGRTSGetDistanceFromMap(pTileMap, uFromIndex) + ZGRTSGetDistanceFromMap(pTileMap, uToIndex)) * 0.5f + 
			sqrtf((ZGFLOAT)(uDistanceX * uDistanceX + uDistanceY * uDistanceY))));
	if (sg_uCount < ZG_RTS_INFO_SIZE)
	{
		LPZGRTSINFO pInfo = &sg_aInfos[sg_uCount++];
		pInfo->eType = ZG_RTS_INFO_TYPE_MOVE;
		pInfo->uElapsedTime = uElapsedTime;
		pInfo->uTime = uTime;
		pInfo->pTileManagerObject = ((LPZGRTSNODE)pTileNodeData)->pTileManagerObject;
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

ZGBOOLEAN __ZGRTSActiveHand(
	LPZGTILEOBJECTACTION pTileObjectAction,
	LPZGTILEMANAGEROBJECT pTileManagerObject,
	ZGUINT uIndex,
	ZGUINT uElapsedTime)
{
	if (pTileObjectAction == ZG_NULL || pTileManagerObject == ZG_NULL)
		return ZG_FALSE;

	if (pTileManagerObject->Instance.Instance.pTileMap == ZG_NULL)
		return ZG_FALSE;


	if (pTileObjectAction->pData == ZG_NULL)
		return ZG_FALSE;

	LPZGRTSACTIONACTIVE pActionActive = (LPZGRTSACTIONACTIVE)pTileObjectAction->pData;

	ZGUINT uCount = ZG_RTS_BUFFER_SIZE * sizeof(ZGUINT8) / sizeof(ZGUINT);
	PZGUINT puIndices = (PZGUINT)sg_auBuffer;
	if (ZGMapTest(
		&pTileManagerObject->Instance.Instance.pTileMap->Instance,
		&pActionActive->Instance.pInstance->Instance.Instance,
		uIndex,
		pActionActive->Instance.pInstance->Instance.uOffset,
		&uCount,
		puIndices))
	{
		ZGUINT uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE), uLength = 0, i, j;
		LPZGTILENODE *ppTileNodes = (LPZGTILENODE*)(puIndices + uCount), pTileNode;
		for (i = 0; i < uCount; ++i)
		{
			pTileNode = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileManagerObject->Instance.Instance.pTileMap, puIndices[i]))->pNode;
			if (pTileNode == ZG_NULL)
				continue;

			if (pActionActive->pfnChecker != ZG_NULL &&
				!pActionActive->pfnChecker(pActionActive->Instance.pData, pTileManagerObject->Instance.Instance.pData, pTileNode->pData))
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

				sg_uOffset += sizeof(ZGRTSINFOHAND);
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

			if (pTileManagerObject->Instance.Instance.pData != ZG_NULL)
			{
				ZGUINT i, j, uSource, uDestination;
				ZGINT nDamage;
				LPZGTILENODE pTileNode;
				LPZGRTSNODE pSource, pDestination = (LPZGRTSNODE)pTileManagerObject->Instance.Instance.pData;
				LPZGRTSINFOTARGET pInfoTargets = (LPZGRTSINFOTARGET)(sg_auOutput + sg_uOffset);
				for (i = 0; i < uCount; ++i)
				{
					pTileNode = ppTileNodes[i];
					if (pTileNode == ZG_NULL || pTileNode->pData == ZG_NULL)
						continue;

					pSource = (LPZGRTSNODE)pTileNode->pData;

					nDamage = 0;
					for (j = 0; j < ZG_RTS_ELEMENT_COUNT; ++j)
					{
						uSource = pSource->auAttributes[ZG_RTS_OBJECT_ATTRIBUTE_DEFENSE + j];
						uDestination = pDestination->auAttributes[ZG_RTS_OBJECT_ATTRIBUTE_ATTACK + j];
						nDamage -= uSource < uDestination ? uDestination - uSource : 0;
					}

					pSource->auAttributes[ZG_RTS_OBJECT_ATTRIBUTE_HP] += nDamage;
					/*if (lHp <= 0)
					ZGTileNodeUnset(pTileNode, pTileMap);*/

					if (pInfoHand != ZG_NULL)
					{
						sg_uOffset += sizeof(ZGRTSINFOTARGET);
						if (sg_uOffset <= ZG_RTS_BUFFER_SIZE)
						{
							pInfoTargets->pTileManagerObject = pSource->pTileManagerObject;
							pInfoTargets->nDamage = nDamage;

							if (pInfoHand->pTargets == ZG_NULL)
								pInfoHand->pTargets = pInfoTargets;

							++pInfoTargets;

							++pInfoHand->uTargetCount;
						}
					}
				}
			}

			return ZG_TRUE;
		}
	}

	return ZG_FALSE;
}

ZGUINT __ZGRTSActiveSet(
	ZGUINT uElapsedTime, 
	void* pTileObjectActionData,
	void* pTileNodeData, 
	LPZGTILEMAP pTileMap, 
	ZGUINT uIndex, 
	void** ppUserData)
{
	if (pTileObjectActionData == ZG_NULL || pTileNodeData == ZG_NULL)
		return 0;

	LPZGTILEOBJECTACTION pTileObjectAction = ((LPZGRTSACTIONACTIVE)pTileObjectActionData)->pTileObjectAction;
	LPZGRTSNODE pNode = (LPZGRTSNODE)pTileNodeData;
	LPZGTILEACTIONMAPNODE pTileActionMapNode = (LPZGTILEACTIONMAPNODE)((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, uIndex))->pData;
	if (ppUserData != ZG_NULL && *ppUserData != ZG_NULL)
	{
		LPZGRTSHANDLER pHandler = (LPZGRTSHANDLER)(*ppUserData);
		pHandler->uTime = uElapsedTime + pNode->auAttributes[ZG_RTS_OBJECT_ATTRIBUTE_HAND_TIME];
		pHandler->uIndex = pTileActionMapNode->uMaxIndex;
		pHandler->pTileObjectAction = pTileObjectAction;
		pHandler->pTileManagerObject = pNode->pTileManagerObject;
		pHandler->pfnInstance = __ZGRTSActiveHand;
		*ppUserData = pHandler;
	}

	ZGUINT uTime = pNode->auAttributes[ZG_RTS_OBJECT_ATTRIBUTE_SET_TIME];
	if (sg_uCount < ZG_RTS_INFO_SIZE)
	{
		LPZGRTSINFO pInfo = &sg_aInfos[sg_uCount++];
		pInfo->eType = ZG_RTS_INFO_TYPE_SET;
		pInfo->uElapsedTime = uElapsedTime;
		pInfo->uTime = uTime;
		pInfo->pTileManagerObject = pNode->pTileManagerObject;
		LPZGRTSINFOSET pInfoSet = (LPZGRTSINFOSET)(sg_auOutput + sg_uOffset);

		sg_uOffset += sizeof(ZGRTSINFOSET);
		if (sg_uOffset > ZG_RTS_OUTPUT_SIZE)
			pInfoSet = ZG_NULL;
		else
		{
			pInfoSet->uIndex = pTileActionMapNode->uMaxIndex;
			pInfoSet->pTileObjectAction = pTileObjectAction;
		}

		pInfo->pSet = pInfoSet;
	}

	return uTime;
}

ZGUINT __ZGRTSActiveCheck(
	void* pTileObjectActionData,
	LPZGTILENODE pTileNode)
{
	if (pTileObjectActionData == ZG_NULL)
		return 0;

	sg_pTileMap = pTileNode == ZG_NULL ? ZG_NULL : pTileNode->pTileMap;

	return ZGTileActionSearch(
		&((LPZGRTSACTIONACTIVE)pTileObjectActionData)->Instance,
		pTileNode,
		ZG_RTS_BUFFER_SIZE,
		sg_auBuffer,
		__ZGRTSPredicate, 
		ZG_NULL, 
		__ZGRTSTestAction, 
		ZG_NODE_SEARCH_TYPE_ONCE);
}

ZGUINT __ZGRTSNormalCheck(
	void* pTileObjectActionData,
	LPZGTILENODE pTileNode)
{
	if (pTileObjectActionData == ZG_NULL || pTileNode == ZG_NULL)
		return ZG_NULL;

	if (pTileNode->pTileMap == ZG_NULL || 
		pTileNode->pTileMap->pNodes == ZG_NULL || 
		pTileNode->uIndex >= pTileNode->pTileMap->Instance.Instance.uCount)
		return ZG_NULL;

	LPZGRTSACTIONNORMAL pActionNormal = (LPZGRTSACTIONNORMAL)pTileObjectActionData;
	ZGUINT uSourceIndexX = pTileNode->uIndex % pTileNode->pTileMap->Instance.uPitch, 
		uSourceIndexY = pTileNode->uIndex / pTileNode->pTileMap->Instance.uPitch, 
		uDestinationX, 
		uDestinationY;
	if (pActionNormal->uIndex == pTileNode->uIndex || pActionNormal->uIndex >= pTileNode->pTileMap->Instance.Instance.uCount)
	{
		ZGUINT uWidth = pTileNode->pTileMap->Instance.uPitch, uHeight = pTileNode->pTileMap->Instance.Instance.uCount / uWidth;
		--uWidth;
		--uHeight;

		uDestinationX = uSourceIndexX + (ZGUINT)(rand() * 2.0f / RAND_MAX * pActionNormal->uRange);
		uDestinationX = uDestinationX > pActionNormal->uRange ? uDestinationX - pActionNormal->uRange : 0;
		uDestinationX = uDestinationX > uWidth ? uWidth : uDestinationX;
		uDestinationY = uSourceIndexY + (ZGUINT)(rand() * 2.0f / RAND_MAX * pActionNormal->uRange);
		uDestinationY = uDestinationY > pActionNormal->uRange ? uDestinationY - pActionNormal->uRange : 0;
		uDestinationY = uDestinationY > uHeight ? uHeight : uDestinationY;

		pActionNormal->uIndex = uSourceIndexX + uDestinationY * pTileNode->pTileMap->Instance.uPitch;
	}
	else
	{
		uDestinationX = pActionNormal->uIndex % pTileNode->pTileMap->Instance.uPitch;
		uDestinationY = pActionNormal->uIndex / pTileNode->pTileMap->Instance.uPitch;
	}

	const ZGUINT uCOUNT = ZG_RTS_BUFFER_SIZE * sizeof(ZGUINT8) / sizeof(ZGUINT);
	ZGUINT uDistanceX = ZG_ABS(uSourceIndexX, uDestinationX),
		uDistanceY = ZG_ABS(uSourceIndexY, uDestinationY),
		uDepth = 1, 
		uIndex, 
		uCount, 
		uSize, 
		uLength, 
		i, j;
	PZGINT puIndices = (PZGUINT)sg_auBuffer;
	LPZGNODE pNode = pTileNode->pTileMap->pNodes + pTileNode->uIndex;
	LPZGTILENODE pTemp;
	LPZGTILENODE* ppTileNodes;
	pNode->uValue = 0;
	pNode->uDistance = 0;
	pNode->uDepth = 0;
	pNode->pPrevious = ZG_NULL;
	while (uDistanceX > 0 || uDistanceY > 0)
	{
		if (uDistanceX > 0)
		{
			uSourceIndexX = uSourceIndexX > uDestinationX ? uSourceIndexX - 1 : uSourceIndexX + 1;

			--uDistanceX;
		}
		
		if (uDistanceY > 0)
		{
			uSourceIndexY = uSourceIndexY > uDestinationY ? uSourceIndexY - 1 : uSourceIndexY + 1;

			--uDistanceY;
		}

		uIndex = uSourceIndexX + uSourceIndexY * pTileNode->pTileMap->Instance.uPitch;
		uCount = uCOUNT;
		if (pTileNode->pInstance != ZG_NULL && 
			ZGMapTest(
				&pTileNode->pTileMap->Instance, 
				&pTileNode->pInstance->Instance.Instance, 
				uIndex,
				pTileNode->pInstance->Instance.uOffset,
				&uCount, 
				puIndices))
		{
			uSize = uCount * sizeof(ZGUINT) + sizeof(LPZGTILENODE);
			if (uSize <= ZG_RTS_BUFFER_SIZE)
			{
				uLength = 0;
				ppTileNodes = (LPZGTILENODE*)(puIndices + uCount);
				for (i = 0; i < uCount; ++i)
				{
					pTemp = ((LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileNode->pTileMap, puIndices[i]))->pNode;
					if (pTemp == ZG_NULL)
						break;

					if (pTemp == pTileNode)
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
					if (uSize > ZG_RTS_BUFFER_SIZE)
					{
						i = uCount;

						break;
					}
				}

				if (i < uCount || (uLength > 0 && !__ZGRTSTestAction(pTileNode->pData, ppTileNodes, uLength)))
					break;
			}
		}

		pNode->pNext = pTileNode->pTileMap->pNodes + uIndex;
		pNode->pNext->uDepth = pNode->uDepth + 1;
		pNode->pNext->uDistance = __ZGRTSPredicate(pNode->pData, pNode->pNext->pData);
		pNode->pNext->uValue = pNode->uValue + pNode->pNext->uDistance;
		pNode->pNext->uEvaluation = 0;
		pNode->pNext->pPrevious = pNode;
		pNode = pNode->pNext;

		++uDepth;
	}

	pNode->pNext = ZG_NULL;

	if(uDestinationX > 0 || uDestinationY > 0)
		pActionNormal->uIndex = ~0;

	return uDepth;
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
		sizeof(ZGRTSNODE));
	LPZGTILENODEDATA pTileNodeData = (LPZGTILENODEDATA)(pResult + 1);
	PZGUINT8 puFlags = (PZGUINT8)(pTileNodeData + 1);
	LPZGRTSNODE pNode = (LPZGRTSNODE)(puFlags + uLength);

	pResult->Instance.Instance.pInstance = pTileNodeData;
	pResult->Instance.Instance.pData = pNode;
	pResult->Instance.Instance.pTileMap = ZG_NULL;
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

	pNode->pTileManagerObject = pResult;
	pNode->uCamp = 0;
	pNode->uLabel = 0;

	for (ZGUINT i = 0; i < ZG_RTS_OBJECT_ATTRIBUTE_COUNT; ++i)
		pNode->auAttributes[i] = 0;

	return pResult;
}

LPZGTILEOBJECTACTION ZGRTSCreateActionNormal(ZGUINT uChildCount)
{
	LPZGTILEOBJECTACTION pResult = (LPZGTILEOBJECTACTION)malloc(
		sizeof(ZGTILEOBJECTACTION) +
		sizeof(ZGRTSACTIONNORMAL) +
		sizeof(LPZGTILEOBJECTACTION) * uChildCount);
	LPZGRTSACTIONNORMAL pActionNormal = (LPZGRTSACTIONNORMAL)(pResult + 1);
	pResult->pfnCheck = __ZGRTSNormalCheck;
	pResult->pfnMove = __ZGRTSMove;
	pResult->pfnSet = ZG_NULL;
	pResult->pData = pActionNormal;
	pResult->uChildCount = uChildCount;
	pResult->ppChildren = (LPZGTILEOBJECTACTION*)(pActionNormal + 1);

	pActionNormal->uIndex = ~0;
	pActionNormal->uRange = 5;

	for (ZGUINT i = 0; i < uChildCount; ++i)
		pResult->ppChildren[i] = ZG_NULL;

	return pResult;
}

LPZGTILEOBJECTACTION ZGRTSCreateActionActive(
	ZGUINT uDistance,
	ZGUINT uRange,
	ZGUINT uChildCount)
{
	ZGUINT uDistanceLength = (uDistance << 1) + 1, uRangeLength = (uRange << 1) + 1;
	uDistanceLength = (uDistanceLength * uDistanceLength + 7) >> 3;
	uRangeLength = (uRangeLength * uRangeLength + 7) >> 3;
	LPZGTILEOBJECTACTION pResult = (LPZGTILEOBJECTACTION)malloc(
		sizeof(ZGTILEOBJECTACTION) +
		sizeof(ZGTILEACTIONDATA) +
		sizeof(ZGRTSACTIONACTIVE) +
		sizeof(ZGUINT8) * uDistanceLength +
		sizeof(ZGUINT8) * uRangeLength +
		sizeof(LPZGTILEOBJECTACTION) * uChildCount);
	LPZGTILEACTIONDATA pTileActionData = (LPZGTILEACTIONDATA)(pResult + 1);
	LPZGRTSACTIONACTIVE pActionActive = (LPZGRTSACTIONACTIVE)(pTileActionData + 1);
	PZGUINT8 puDistanceFlags = (PZGUINT8)(pActionActive + 1), puRangeFlags = puDistanceFlags + uDistanceLength;
	pResult->pfnCheck = __ZGRTSActiveCheck;
	pResult->pfnMove = __ZGRTSMove;
	pResult->pfnSet = __ZGRTSActiveSet;
	pResult->pData = pActionActive;
	pResult->uChildCount = uChildCount;
	pResult->ppChildren = (LPZGTILEOBJECTACTION*)(puRangeFlags + uRangeLength);

	ZGTileRangeInitOblique(&pTileActionData->Distance, puDistanceFlags, uDistance);
	ZGTileRangeInitOblique(&pTileActionData->Instance, puRangeFlags, uRange);

	pActionActive->Instance.pInstance = pTileActionData;
	pActionActive->Instance.uEvaluation = 0;
	pActionActive->Instance.uMinEvaluation = 0;
	pActionActive->Instance.uMaxEvaluation = 0;
	pActionActive->Instance.uMaxDistance = 0;
	pActionActive->Instance.uMaxDepth = 0;
	pActionActive->Instance.pData = &pActionActive->Data;
	pActionActive->Instance.pfnAnalyzation = __ZGRTSAnalyzate;

	pActionActive->Data.uFlag = 1 << ZG_RTS_ACTION_TYPE_ENEMY;
	pActionActive->Data.uSearchLabel = 1;
	pActionActive->Data.uSetLabel = 1;

	pActionActive->pTileObjectAction = pResult;

	pActionActive->pfnChecker = __ZGRTSCheck;

	for (ZGUINT i = 0; i < uChildCount; ++i)
		pResult->ppChildren[i] = ZG_NULL;

	return pResult;
}

LPZGTILEMAP ZGRTSCreateMap(ZGUINT uWidth, ZGUINT uHeight, ZGBOOLEAN bIsOblique)
{
	ZGUINT uCount = uWidth * uHeight, uFlagLength = (uCount + 7) >> 3, uChildCount = ZGTileChildCount(uWidth, uHeight, bIsOblique), uNodeCount = uCount * ZG_RTS_MAP_NODE_SIZE;
	LPZGTILEMAP pResult = (LPZGTILEMAP)malloc(
		sizeof(ZGTILEMAP) +
		sizeof(ZGUINT8) * uFlagLength +
		sizeof(LPZGNODE) * uChildCount +
		sizeof(ZGNODE) * uCount +
		sizeof(LPZGTILENODE) * uNodeCount +
		sizeof(ZGTILEMAPNODE) * uCount +
		sizeof(ZGTILENODEMAPNODE) * uCount +
		sizeof(ZGTILEACTIONMAPNODE) * uCount + 
		sizeof(ZGRTSMAPNODE) * uCount);
	PZGUINT8 puFlags = (PZGUINT8)(pResult + 1);
	LPZGNODE* ppNodes = (LPZGNODE*)(puFlags + uFlagLength), pNodes = (LPZGNODE)(ppNodes + uChildCount);
	LPZGTILENODE* ppTileNodes = (LPZGTILENODE*)(pNodes + uCount);
	LPZGTILEMAPNODE pTileMapNodes = (LPZGTILEMAPNODE)(ppTileNodes + uNodeCount);
	LPZGTILENODEMAPNODE pTileNodeMapNodes = (LPZGTILENODEMAPNODE)(pTileMapNodes + uCount);
	LPZGTILEACTIONMAPNODE pTileActionMapNodes = (LPZGTILEACTIONMAPNODE)(pTileNodeMapNodes + uCount);
	LPZGRTSMAPNODE pMapNodes = (LPZGRTSMAPNODE)(pTileActionMapNodes + uCount);

	ZGTileMapEnable(pResult, puFlags, ppNodes, pNodes, pTileMapNodes, uWidth, uHeight, bIsOblique);

	ZGUINT i;
	for (i = 0; i < uFlagLength; ++i)
		puFlags[i] = 0;

	for (i = 0; i < uCount; ++i)
	{
		pMapNodes->uDistance = 1;

		pTileActionMapNodes->ppNodes = ppTileNodes + i * ZG_RTS_MAP_NODE_SIZE;
		pTileActionMapNodes->uCount = ZG_RTS_MAP_NODE_SIZE;
		pTileActionMapNodes->pData = pMapNodes++;

		pTileNodeMapNodes->pNode = ZG_NULL;
		pTileNodeMapNodes->pData = pTileActionMapNodes++;

		((LPZGTILEMAPNODE)pResult->pNodes[i].pData)->pData = pTileNodeMapNodes++;
	}

	return pResult;
}

LPZGTILEMANAGER ZGRTSCreateManager(ZGUINT uCapacity)
{
	LPZGTILEMANAGER pResult = (LPZGTILEMANAGER)malloc(
		sizeof(ZGTILEMANAGER) + 
		sizeof(ZGTILEMANAGERHANDLER) * uCapacity + 
		sizeof(ZGRTSHANDLER) * uCapacity);
	LPZGTILEMANAGERHANDLER pTileManagerHandlers = (LPZGTILEMANAGERHANDLER)(pResult + 1);
	LPZGRTSHANDLER pHandlers = (LPZGRTSHANDLER)(pTileManagerHandlers + uCapacity);
	pResult->pObjects = ZG_NULL;
	pResult->pQueue = ZG_NULL;
	
	ZGUINT i;
	LPZGTILEMANAGERHANDLER pTileManagerHandler = pTileManagerHandlers;
	for (i = 1; i < uCapacity; ++i)
	{
		pTileManagerHandler = pTileManagerHandlers + i;
		pTileManagerHandler->pUserData = pHandlers + i;
		pTileManagerHandler->pNext = pTileManagerHandlers + i - 1;
	}

	if (uCapacity > 0)
	{
		pTileManagerHandlers->pUserData = pHandlers;
		pTileManagerHandlers->pNext = ZG_NULL;

		pResult->pPool = pTileManagerHandler;
	}
	else
		pResult->pPool = ZG_NULL;

	return pResult;
}

void ZGRTSDestroyMap(LPZGTILEMAP pTileMap)
{
	ZGTileMapDisable(pTileMap);

	free(pTileMap);
}

ZGBOOLEAN ZGRTSGetMap(LPZGTILEMAP pTileMap, ZGUINT uIndex)
{
	return ZGTileMapGet(pTileMap, uIndex);
}

ZGBOOLEAN ZGRTSSetMap(LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGBOOLEAN bValue)
{
	return ZGTileMapSet(pTileMap, uIndex, bValue);
}

ZGBOOLEAN ZGRTSSetObjectToMap(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMAP pTileMap, ZGUINT uIndex)
{
	if (pTileManagerObject == ZG_NULL || pTileMap == ZG_NULL)
		return ZG_FALSE;

	if (pTileManagerObject->Instance.Instance.uIndex < pTileMap->Instance.Instance.uCount)
		return ZG_FALSE;

	if (ZGMapTest(
		&pTileMap->Instance,
		&pTileManagerObject->Instance.Instance.pInstance->Instance.Instance,
		uIndex,
		pTileManagerObject->Instance.Instance.pInstance->Instance.uOffset,
		ZG_NULL,
		ZG_NULL))
		return ZG_FALSE;

	return ZGTileNodeSetTo(&pTileManagerObject->Instance.Instance, pTileMap, uIndex);
}

ZGBOOLEAN ZGRTSUnsetObjectFromMap(LPZGTILEMANAGEROBJECT pTileManagerObject)
{
	if (pTileManagerObject == ZG_NULL)
		return ZG_FALSE;

	return ZGTileNodeUnset(&pTileManagerObject->Instance.Instance);
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

	void* pData = ZGTileMapGetData(pTileManagerObject->Instance.Instance.pTileMap, uIndex);
	pData = pData == ZG_NULL ? ZG_NULL : ((LPZGTILENODEMAPNODE)pData)->pData;
	if (pData != ZG_NULL)
	{
		LPZGTILEACTIONMAPNODE pTileActionMapNode = (LPZGTILEACTIONMAPNODE)pData;
		pTileActionMapNode->uMaxCount = 0;
		pTileActionMapNode->uMaxIndex = uIndex;
	}

	ZGBOOLEAN bResult = ZGTileManagerSet(pTileManager, pTileManagerObject, pTileManagerObject->Instance.pActions, uIndex, uTime);

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

	ZGTileManagerRun(
		pTileManager,
		uTime,
		__ZGRTSDelay, 
		__ZGRTSHand);

	if (puInfoCount != ZG_NULL)
		*puInfoCount = sg_uCount;

	return sg_aInfos;
}
