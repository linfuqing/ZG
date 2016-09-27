#include "RBList.h"

LPZGRBLISTNODE ZGRBListNodeNext(LPZGRBLISTNODE pNode)
{
	LPZGRBTREENODE pTemp = pNode == ZG_NULL ? ZG_NULL : ZGRBTreeNodeNext(&pNode->Instance);
	return pTemp == ZG_NULL ? ZG_NULL : (LPZGRBLISTNODE)pTemp->pValue;
}

LPZGRBLISTNODE ZGRBListGet(LPZGRBLIST pList, const void* pKey)
{
	LPZGRBTREENODE pTemp = pList == ZG_NULL ? ZG_NULL : ZGRBTreeGet(&pList->Instance, pKey);
	return pTemp == ZG_NULL ? ZG_NULL : (LPZGRBLISTNODE)pTemp->pValue;
}

void ZGRBListInit(LPZGRBLIST pList, ZGCOMPARSION pfnComparsion)
{
	if (pList == ZG_NULL)
		return;

	ZGRBTreeInit(&pList->Instance, pfnComparsion);
	pList->pHead = ZG_NULL;
}

ZGBOOLEAN ZGRBListAdd(LPZGRBLIST pList, LPZGRBLISTNODE pNode, void* pValue, const void* pKey, ZGBOOLEAN bIsAllowDuplicate)
{
	if (pList == ZG_NULL || pNode == ZG_NULL)
		return ZG_FALSE;

	pNode->pValue = pValue;
	if (!ZGRBTreeAdd(&pList->Instance, &pNode->Instance, pNode, pKey, bIsAllowDuplicate))
		return ZG_FALSE;

	if (pList->pHead == ZG_NULL || (pList->Instance.pfnComparsion != ZG_NULL && pList->Instance.pfnComparsion(pList->pHead->Instance.pKey, pKey) > 0))
		pList->pHead = pNode;

	return ZG_TRUE;
}

ZGBOOLEAN ZGRBListRemove(LPZGRBLIST pList, LPZGRBLISTNODE pNode)
{
	if (pList == ZG_NULL || pNode == ZG_NULL)
		return ZG_FALSE;

	if (pList->pHead == pNode)
	{
		LPZGRBTREENODE pTemp = ZGRBTreeNodeNext(&pNode->Instance);
		pList->pHead = pTemp == ZG_NULL ? ZG_NULL : (LPZGRBLISTNODE)pTemp->pValue;
	}

	return ZGRBTreeRemove(&pList->Instance, &pNode->Instance);
}

ZGBOOLEAN ZGRBListRemoveAt(LPZGRBLIST pList, const void* pKey)
{
	return ZGRBListRemove(pList, ZGRBListGet(pList, pKey));
}

void ZGRBListClear(LPZGRBLIST pList)
{
	if (pList == ZG_NULL)
		return;

	ZGRBTreeClear(&pList->Instance);

	pList->pHead = ZG_NULL;
}