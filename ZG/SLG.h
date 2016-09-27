#pragma once

#include "TileObject.h"


# define ZG_SLG_EXPORT __declspec (dllexport)
# define ZG_SLG_OBJECT_ELEMENT_COUNT 2

#ifdef __cplusplus
extern "C" {
#endif
	typedef enum ZGSLGObjectFlag
	{
		ZG_SLG_OBJECT_FLAG_AUTO
	}ZGSLGOBJECTFLAG, *LPZGSLGOBJECTFLAG;

	typedef enum ZGSLGObjectAttribute
	{
		ZG_SLG_OBJECT_ATTRIBUTE_FLAG, 
		ZG_SLG_OBJECT_ATTRIBUTE_CAMP, 
		ZG_SLG_OBJECT_ATTRIBUTE_COOLDOWN, 
		ZG_SLG_OBJECT_ATTRIBUTE_HP, 
		ZG_SLG_OBJECT_ATTRIBUTE_ATTACK,
		ZG_SLG_OBJECT_ATTRIBUTE_DEFENSE = ZG_SLG_OBJECT_ATTRIBUTE_ATTACK + ZG_SLG_OBJECT_ELEMENT_COUNT,

		ZG_SLG_OBJECT_ATTRIBUTE_COUNT = ZG_SLG_OBJECT_ATTRIBUTE_DEFENSE + ZG_SLG_OBJECT_ELEMENT_COUNT
	}ZGSLGOBJECTATTRIBUTE, *LPZGSLGOBJECTATTRIBUTE;

	ZG_SLG_EXPORT void ZGSLGDestroy(void* pData);

	ZG_SLG_EXPORT LPZGRBLISTNODE ZGSLGCreateObject(
		ZGUINT uWidth,
		ZGUINT uHeight,
		ZGUINT uOffset,
		ZGUINT uTime, 
		ZGUINT uActionCount,
		LPZGRBLIST pRBList);

	ZG_SLG_EXPORT LPZGTILEACTION ZGSLGCreateAction(
		ZGUINT uDistance,
		ZGUINT uRange);

	ZG_SLG_EXPORT LPZGTILEMAP ZGSLGCreateMap(ZGUINT uWidth, ZGUINT uHeight, ZGBOOLEAN bIsOblique);

	ZG_SLG_EXPORT void ZGSLGDestroyMap(LPZGTILEMAP pTileMap);

	ZG_SLG_EXPORT LPZGRBLIST ZGSLGCreateQueue();

	ZG_SLG_EXPORT LPZGRBLISTNODE ZGSLGGetNextFromObject(LPZGRBLISTNODE pRBListNode);

	ZG_SLG_EXPORT void ZGSLGSetMap(LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGBOOLEAN bValue);

	ZG_SLG_EXPORT ZGBOOLEAN ZGSLGMoveObjectToMap(LPZGRBLISTNODE pRBListNode, LPZGTILEMAP pTileMap, ZGUINT uIndex);

	ZG_SLG_EXPORT ZGUINT ZGSLGSearch(LPZGRBLISTNODE pRBListNode, LPZGTILEMAP pTileMap, ZGUINT uIndex);

	ZG_SLG_EXPORT ZGUINT ZGSLGRun(
		LPZGRBLIST pRbList,
		LPZGTILEMAP pTileMap,
		ZGUINT uEvaluation,
		ZGUINT uMinEvaluation,
		ZGUINT uMaxEvaluation,
		ZGUINT uMaxDistance,
		ZGUINT uMaxDepth);

	ZG_SLG_EXPORT ZGUINT ZGSLGGetAttributeFromObjectUINT(LPZGRBLISTNODE pRBListNode, ZGUINT uAttribute)
	{
		if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
			return 0;

		LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)pRBListNode->pValue;
		if (pTileObject->Instance.pData == ZG_NULL)
			return 0;

		return (ZGUINT)((PZGLONG)pTileObject->Instance.pData)[uAttribute];
	}

	ZG_SLG_EXPORT ZGUINT ZGSLGGetIndexFromObject(LPZGRBLISTNODE pRBListNode)
	{
		if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
			return ZG_NULL;

		return ((LPZGTILEOBJECT)pRBListNode->pValue)->Instance.uIndex;
	}

	ZG_SLG_EXPORT LPZGNODE ZGSLGGetMapNodeFromMap(LPZGTILEMAP pTileMap, ZGUINT uIndex)
	{
		return pTileMap == ZG_NULL || pTileMap->Instance.Instance.uCount <= uIndex ? ZG_NULL : pTileMap->pNodes + uIndex;
	}

	ZG_SLG_EXPORT LPZGRBLISTNODE ZGSLGGetObjectFromQueue(LPZGRBLIST pRBList)
	{
		return pRBList == ZG_NULL ? ZG_NULL : pRBList->pHead;
	}

	ZG_SLG_EXPORT ZGUINT ZGSLGGetIndexFromMapNode(LPZGNODE pNode)
	{
		return pNode == ZG_NULL ? 0 : ((LPZGTILEMAPNODE)pNode->pData)->uIndex;
	}

	ZG_SLG_EXPORT LPZGNODE ZGSLGGetNextFromMapNode(LPZGNODE pNode)
	{
		return pNode == ZG_NULL ? ZG_NULL : pNode->pNext;
	}

	ZG_SLG_EXPORT void ZGSLGSetDistanceToObject(LPZGRBLISTNODE pRBListNode, ZGUINT uDistance)
	{
		if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
			return;

		((LPZGTILEOBJECT)pRBListNode->pValue)->Instance.uDistance = uDistance;
	}

	ZG_SLG_EXPORT void ZGSLGSetRangeToObject(LPZGRBLISTNODE pRBListNode, ZGUINT uRange)
	{
		if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
			return;

		((LPZGTILEOBJECT)pRBListNode->pValue)->Instance.uRange = uRange;
	}

	ZG_SLG_EXPORT void ZGSLGSetAttributeToObjectUINT(LPZGRBLISTNODE pRBListNode, ZGUINT uAttribute, ZGUINT uValue)
	{
		if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
			return;

		LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)pRBListNode->pValue;
		if (pTileObject->Instance.pData == ZG_NULL)
			return;

		((PZGLONG)pTileObject->Instance.pData)[uAttribute] = uValue;
	}

	ZG_SLG_EXPORT ZGBOOLEAN ZGSLGSetActionToObject(LPZGRBLISTNODE pRBListNode, LPZGTILEACTION pAction, ZGUINT uIndex)
	{
		if (pRBListNode == ZG_NULL || pRBListNode->pValue == ZG_NULL)
			return ZG_FALSE;

		LPZGTILEOBJECT pTileObject = (LPZGTILEOBJECT)(pRBListNode->pValue);
		if (pTileObject->uActionCount <= uIndex)
			return ZG_FALSE;

		pTileObject->ppActions[uIndex] = pAction;

		return ZG_TRUE;
	}

	ZG_SLG_EXPORT void ZGSLGSetDistanceToMap(LPZGTILEMAP pTileMap, ZGUINT uIndex, ZGUINT uDistance)
	{
		LPZGTILENODEMAPNODE pTileNodeMapNode = (LPZGTILENODEMAPNODE)ZGTileMapGetData(pTileMap, uIndex);
		pTileNodeMapNode->uDistance = uDistance;
	}
#ifdef __cplusplus
}
#endif