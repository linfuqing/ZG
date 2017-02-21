#pragma once


#include "TileManager.h"

# define ZG_RTS_EXPORT __declspec (dllexport)
# define ZG_RTS_ELEMENT_COUNT 2

#ifdef __cplusplus
extern "C" {
#endif
	typedef enum ZGRTSObjectAttribute
	{
		ZG_RTS_OBJECT_ATTRIBUTE_PARENT,
		ZG_RTS_OBJECT_ATTRIBUTE_CAMP,
		ZG_RTS_OBJECT_ATTRIBUTE_MOVE_TIME,
		ZG_RTS_OBJECT_ATTRIBUTE_SET_TIME,
		ZG_RTS_OBJECT_ATTRIBUTE_HAND_TIME,
		ZG_RTS_OBJECT_ATTRIBUTE_HP,
		ZG_RTS_OBJECT_ATTRIBUTE_ATTACK,
		ZG_RTS_OBJECT_ATTRIBUTE_DEFENSE = ZG_RTS_OBJECT_ATTRIBUTE_ATTACK + ZG_RTS_ELEMENT_COUNT,

		ZG_RTS_OBJECT_ATTRIBUTE_COUNT = ZG_RTS_OBJECT_ATTRIBUTE_DEFENSE + ZG_RTS_ELEMENT_COUNT
	}ZGRTSOBJECTATTRIBUTE, *LPZGRTSOBJECTATTRIBUTE;

	typedef enum ZGRTSActionAttribute
	{
		ZG_RTS_ACTION_ATTRIBUTE_PARENT,
		ZG_RTS_ACTION_ATTRIBUTE_ATTACK,
		ZG_RTS_ACTION_ATTRIBUTE_DEFENSE = ZG_RTS_ACTION_ATTRIBUTE_ATTACK + ZG_RTS_ELEMENT_COUNT,
		ZG_RTS_ACTION_ATTRIBUTE_COUNT = ZG_RTS_OBJECT_ATTRIBUTE_DEFENSE + ZG_RTS_ELEMENT_COUNT
	}ZGRTSACTIONATTRIBUTE, *LPZGSLGACTIONATTRIBUTE;

	typedef enum ZGRTSInfoType
	{
		ZG_RTS_INFO_TYPE_DELAY, 
		ZG_RTS_INFO_TYPE_MOVE,
		ZG_RTS_INFO_TYPE_SET, 
		ZG_RTS_INFO_TYPE_HAND
	}ZGRTSINFOTYPE, *LPZGRTSINFOTYPE;

	typedef struct ZGRTSInfoTarget
	{
		LPZGTILEMANAGEROBJECT pTileManagerObject;
		ZGLONG lHP;
	}ZGRTSINFOTARGET, *LPZGRTSINFOTARGET;

	typedef struct ZGRTSInfoMove
	{
		ZGUINT uFromIndex;
		ZGUINT uToIndex;
	}ZGRTSINFOMOVE, *LPZGRTSINFOMOVE;

	typedef struct ZGRTSInfoSet
	{
		ZGUINT uIndex;
		LPZGTILEOBJECTACTION pTileObjectAction;
	}ZGRTSINFOSET, *LPZGRTSINFOSET;

	typedef struct ZGRTSInfoHand
	{
		ZGUINT uIndex;
		LPZGTILEOBJECTACTION pTileObjectAction;
		LPZGRTSINFOTARGET pTargets;
		ZGUINT uTargetCount;
	}ZGRTSINFOHAND, *LPZGRTSINFOHAND;

	typedef struct ZGRTSInfo
	{
		ZGRTSINFOTYPE eType;
		ZGUINT uElapsedTime;
		ZGUINT uTime;

		LPZGTILEMANAGEROBJECT pTileManagerObject;

		union
		{
			LPZGRTSINFOMOVE pMove;
			LPZGRTSINFOSET pSet;
			LPZGRTSINFOHAND pHand;
		};
	}ZGRTSINFO, *LPZGRTSINFO;

	typedef struct ZGRTSHandler
	{
		ZGUINT uTime;
		ZGUINT uIndex;
		LPZGTILEOBJECTACTION pTileObjectAction;
		LPZGTILEMANAGEROBJECT pTileManagerObject;
	}ZGRTSHANDLER, *LPZGRTSHANDLER;

	ZG_RTS_EXPORT void ZGRTSDestroy(void* pData);

	ZG_RTS_EXPORT LPZGTILEMANAGEROBJECT ZGRTSCreateObject(
		ZGUINT uWidth,
		ZGUINT uHeight,
		ZGUINT uOffset);

	ZG_RTS_EXPORT LPZGTILEOBJECTACTION ZGRTSCreateAction(
		ZGUINT uDistance,
		ZGUINT uRange,
		ZGUINT uChildCount);

	ZG_RTS_EXPORT LPZGTILEMAP ZGRTSCreateMap(ZGUINT uWidth, ZGUINT uHeight, ZGBOOLEAN bIsOblique);

	ZG_RTS_EXPORT LPZGTILEMANAGER ZGRTSCreateManager(ZGUINT uCapacity);

	ZG_RTS_EXPORT void ZGRTSDestroyMap(LPZGTILEMAP pTileMap);

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSGetMap(LPZGTILEMAP pTileMap, ZGUINT uIndex);

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetMap(LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGBOOLEAN bValue);

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetObjectToMap(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMAP pTileMap, ZGUINT uIndex);

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSUnsetObjectFromMap(LPZGTILEMANAGEROBJECT pTileManagerObject);

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSAddObjectToManager(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMANAGER pTileManager);

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSRemoveObjectFromManager(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEMANAGER pTileManager);

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSDo(
		LPZGTILEMANAGER pTileManager, 
		LPZGTILEMANAGEROBJECT pTileManagerObject,
		ZGUINT uIndex,
		ZGUINT uTime, 
		PZGUINT puInfoCount,
		LPZGRTSINFO* ppInfos);

	ZG_RTS_EXPORT LPZGRTSINFO ZGRTSRun(LPZGTILEMANAGER pTileManager, ZGUINT uTime, PZGUINT puInfoCount);

	ZG_RTS_EXPORT ZGUINT ZGRTSGetAttributeFromObjectUINT(LPZGTILEMANAGEROBJECT pTileManagerObject, ZGUINT uAttribute)
	{
		if (pTileManagerObject == ZG_NULL || pTileManagerObject->Instance.Instance.pData == ZG_NULL)
			return 0;

		return (ZGUINT)((PZGLONG)pTileManagerObject->Instance.Instance.pData)[uAttribute];
	}

	ZG_RTS_EXPORT ZGUINT ZGRTSGetIndexFromObject(LPZGTILEMANAGEROBJECT pTileManagerObject)
	{
		if (pTileManagerObject == ZG_NULL)
			return ZG_NULL;

		return pTileManagerObject->Instance.Instance.uIndex;
	}

	ZG_RTS_EXPORT LPZGNODE ZGRTSGetMapNodeFromMap(LPZGTILEMAP pTileMap, ZGUINT uIndex)
	{
		return pTileMap == ZG_NULL || pTileMap->Instance.Instance.uCount <= uIndex ?
			ZG_NULL : 
			pTileMap->pNodes + uIndex;
	}

	ZG_RTS_EXPORT ZGUINT ZGRTSGetIndexFromMapNode(LPZGNODE pNode)
	{
		return pNode == ZG_NULL ? 0 : ((LPZGTILEMAPNODE)pNode->pData)->uIndex;
	}

	ZG_RTS_EXPORT LPZGNODE ZGRTSGetNextFromMapNode(LPZGNODE pNode)
	{
		return pNode == ZG_NULL ? ZG_NULL : pNode->pNext;
	}

	ZG_RTS_EXPORT void ZGRTSSetAttributeToObjectUINT(LPZGTILEMANAGEROBJECT pTileManagerObject, ZGUINT uAttribute, ZGUINT uValue)
	{
		if (pTileManagerObject == ZG_NULL || pTileManagerObject->Instance.Instance.pData == ZG_NULL)
			return;

		((PZGLONG)pTileManagerObject->Instance.Instance.pData)[uAttribute] = uValue;
	}

	ZG_RTS_EXPORT void ZGRTSSetDistanceToObject(LPZGTILEMANAGEROBJECT pTileManagerObject, ZGUINT uDistance)
	{
		if (pTileManagerObject == ZG_NULL || pTileManagerObject->Instance.Instance.pInstance == ZG_NULL)
			return;

		pTileManagerObject->Instance.Instance.pInstance->uDistance = uDistance;
	}

	ZG_RTS_EXPORT void ZGRTSSetRangeToObject(LPZGTILEMANAGEROBJECT pTileManagerObject, ZGUINT uRange)
	{
		if (pTileManagerObject == ZG_NULL || pTileManagerObject->Instance.Instance.pInstance == ZG_NULL)
			return;

		pTileManagerObject->Instance.Instance.pInstance->uRange = uRange;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetActionToObject(LPZGTILEMANAGEROBJECT pTileManagerObject, LPZGTILEOBJECTACTION pActions)
	{
		if (pTileManagerObject == ZG_NULL)
			return ZG_FALSE;

		pTileManagerObject->Instance.pActions = pActions;

		return ZG_TRUE;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetEvaluationToAction(LPZGTILEOBJECTACTION pTileObjectAction, ZGUINT uEvaluation)
	{
		if (pTileObjectAction == ZG_NULL || pTileObjectAction->pInstance == ZG_NULL)
			return ZG_FALSE;

		pTileObjectAction->pInstance->uEvaluation = uEvaluation;

		return ZG_TRUE;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetMinEvaluationToAction(LPZGTILEOBJECTACTION pTileObjectAction, ZGUINT uMinEvaluation)
	{
		if (pTileObjectAction == ZG_NULL || pTileObjectAction->pInstance == ZG_NULL)
			return ZG_FALSE;

		pTileObjectAction->pInstance->uMinEvaluation = uMinEvaluation;

		return ZG_TRUE;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetMaxEvaluationToAction(LPZGTILEOBJECTACTION pTileObjectAction, ZGUINT uMaxEvaluation)
	{
		if (pTileObjectAction == ZG_NULL || pTileObjectAction->pInstance == ZG_NULL)
			return ZG_FALSE;

		pTileObjectAction->pInstance->uMaxEvaluation = uMaxEvaluation;

		return ZG_TRUE;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetMaxDistanceToAction(LPZGTILEOBJECTACTION pTileObjectAction, ZGUINT uMaxDistance)
	{
		if (pTileObjectAction == ZG_NULL || pTileObjectAction->pInstance == ZG_NULL)
			return ZG_FALSE;

		pTileObjectAction->pInstance->uMaxDistance = uMaxDistance;

		return ZG_TRUE;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetMaxDepthToAction(LPZGTILEOBJECTACTION pTileObjectAction, ZGUINT uMaxDepth)
	{
		if (pTileObjectAction == ZG_NULL || pTileObjectAction->pInstance == ZG_NULL)
			return ZG_FALSE;

		pTileObjectAction->pInstance->uMaxDepth = uMaxDepth;

		return ZG_TRUE;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetChildToAction(LPZGTILEOBJECTACTION pTileObjectAction, LPZGTILEOBJECTACTION pChild, ZGUINT uIndex)
	{
		if (pTileObjectAction == ZG_NULL || pTileObjectAction->uChildCount <= uIndex)
			return ZG_FALSE;

		pTileObjectAction->ppChildren[uIndex] = pChild;

		return ZG_TRUE;
	}

	ZG_RTS_EXPORT ZGBOOLEAN ZGRTSSetDistanceToMap(LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uDistance)
	{
		if (pTileMap == ZG_NULL)
			return ZG_FALSE;

		LPZGTILENODEMAPNODE pTileNodeMapNode = (LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, uIndex);
		if (pTileNodeMapNode == ZG_NULL)
			return ZG_FALSE;

		pTileNodeMapNode->uDistance = uDistance;

		return ZG_TRUE;
	}
#ifdef __cplusplus
}
#endif