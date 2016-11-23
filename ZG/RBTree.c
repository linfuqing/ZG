#include "RBTree.h"

ZG_INLINE ZGBOOLEAN __ZGRBTreeLeftRotate(LPZGRBTREENODE pNode, LPZGRBTREENODE* ppRoot)
{
	if (pNode == ZG_NULL || pNode->pRightChild == ZG_NULL)
		return ZG_NULL;

	LPZGRBTREENODE pRightChild = pNode->pRightChild;
	pNode->pRightChild = pRightChild->pLeftChild;

	if (pNode->pRightChild != ZG_NULL)
		pNode->pRightChild->pParent = pNode;

	pRightChild->pParent = pNode->pParent;

	if (pNode->pParent == ZG_NULL)
	{
		if (ppRoot != ZG_NULL)
			*ppRoot = pRightChild;
	}
	else
	{
		if (pNode->pParent->pLeftChild == pNode)
			pNode->pParent->pLeftChild = pRightChild;

		if (pNode->pParent->pRightChild == pNode)
			pNode->pParent->pRightChild = pRightChild;
	}

	pRightChild->pLeftChild = pNode;

	pNode->pParent = pRightChild;

	return ZG_TRUE;
}

ZG_INLINE ZGBOOLEAN __ZGRBTreeRightRotate(LPZGRBTREENODE pNode, LPZGRBTREENODE* ppRoot)
{
	if (pNode->pLeftChild == ZG_NULL)
		return ZG_FALSE;

	LPZGRBTREENODE pLeftChild = pNode->pLeftChild;
	pNode->pLeftChild = pLeftChild->pRightChild;

	if (pNode->pLeftChild != ZG_NULL)
		pNode->pLeftChild->pParent = pNode;

	pLeftChild->pParent = pNode->pParent;

	if (pNode->pParent == ZG_NULL)
	{
		if (ppRoot != ZG_NULL)
			*ppRoot = pLeftChild;
	}
	else
	{
		if (pNode->pParent->pLeftChild == pNode)
			pNode->pParent->pLeftChild = pLeftChild;

		if (pNode->pParent->pRightChild == pNode)
			pNode->pParent->pRightChild = pLeftChild;
	}

	pLeftChild->pRightChild = pNode;

	pNode->pParent = pLeftChild;

	return ZG_TRUE;
}

ZGBOOLEAN __ZGRBTreeInsertFiexup(LPZGRBTREENODE pNode, LPZGRBTREENODE* ppRoot)
{
	if (pNode == ZG_NULL || !pNode->bIsRed)
		return ZG_FALSE;

	if (pNode->pParent == ZG_NULL)
	{
		pNode->bIsRed = ZG_FALSE;

		if (ppRoot != ZG_NULL)
			*ppRoot = pNode;

		return ZG_TRUE;
	}

	if (!pNode->pParent->bIsRed)
		return ZG_TRUE;

	if (pNode->pParent->pParent->pLeftChild == pNode->pParent)
	{
		LPZGRBTREENODE pSibling = pNode->pParent->pParent->pRightChild;
		if (pSibling != ZG_NULL && pSibling->bIsRed)
		{
			pNode->pParent->bIsRed = ZG_FALSE;
			pSibling->bIsRed = ZG_FALSE;

			pNode->pParent->pParent->bIsRed = ZG_TRUE;

			return __ZGRBTreeInsertFiexup(pNode->pParent->pParent, ppRoot);
		}

		LPZGRBTREENODE pTemp;
		if (pNode->pParent->pRightChild == pNode)
		{
			pTemp = pNode->pParent;
			if (!__ZGRBTreeLeftRotate(pTemp, ppRoot))
				return ZG_FALSE;
		}
		else
			pTemp = pNode;

		pTemp->pParent->bIsRed = ZG_FALSE;
		pTemp->pParent->pParent->bIsRed = ZG_TRUE;

		if (__ZGRBTreeRightRotate(pTemp->pParent->pParent, ppRoot))
			return __ZGRBTreeInsertFiexup(pTemp, ppRoot);
	}

	if (pNode->pParent->pParent->pRightChild == pNode->pParent)
	{
		LPZGRBTREENODE pSibling = pNode->pParent->pParent->pLeftChild;
		if (pSibling != ZG_NULL && pSibling->bIsRed)
		{
			pNode->pParent->bIsRed = ZG_FALSE;
			pSibling->bIsRed = ZG_FALSE;

			pNode->pParent->pParent->bIsRed = ZG_TRUE;

			return __ZGRBTreeInsertFiexup(pNode->pParent->pParent, ppRoot);
		}

		LPZGRBTREENODE pTemp;
		if (pNode->pParent->pLeftChild == pNode)
		{
			pTemp = pNode->pParent;
			if (!__ZGRBTreeRightRotate(pTemp, ppRoot))
				return ZG_FALSE;
		}
		else
			pTemp = pNode;

		pTemp->pParent->bIsRed = ZG_FALSE;
		pTemp->pParent->pParent->bIsRed = ZG_TRUE;

		if (__ZGRBTreeLeftRotate(pTemp->pParent->pParent, ppRoot))
			return __ZGRBTreeInsertFiexup(pTemp, ppRoot);
	}

	return ZG_FALSE;
}

ZGBOOLEAN __ZGRBTreeRemoveFiexup(LPZGRBTREENODE pNode, LPZGRBTREENODE* ppRoot)
{
	if (pNode->bIsRed)
	{
		pNode->bIsRed = ZG_FALSE;

		return ZG_TRUE;
	}

	if (pNode->pParent == ZG_NULL)
	{
		if (ppRoot != ZG_NULL)
			*ppRoot = pNode;

		return ZG_TRUE;
	}

	if (pNode->pParent->pLeftChild == pNode)
	{
		LPZGRBTREENODE pSibling = pNode->pParent->pRightChild;
		if (pSibling->bIsRed)
		{
			pSibling->bIsRed = ZG_FALSE;
			pNode->pParent->bIsRed = ZG_TRUE;

			if (!__ZGRBTreeLeftRotate(pNode->pParent, ppRoot))
				return ZG_FALSE;

			pSibling = pNode->pParent->pRightChild;
		}

		if ((pSibling->pLeftChild == ZG_NULL || !pSibling->pLeftChild->bIsRed) &&
			(pSibling->pRightChild == ZG_NULL || !pSibling->pRightChild->bIsRed))
		{
			pSibling->bIsRed = ZG_TRUE;

			if (__ZGRBTreeRemoveFiexup(pNode->pParent, ppRoot))
				return ZG_TRUE;
		}
		else if (pSibling->pRightChild == ZG_NULL || !pSibling->pRightChild->bIsRed)
		{
			pSibling->pLeftChild->bIsRed = ZG_FALSE;
			pSibling->bIsRed = ZG_TRUE;

			if (!__ZGRBTreeRightRotate(pSibling, ppRoot))
				return ZG_FALSE;

			pSibling = pNode->pParent->pRightChild;
		}

		pSibling->bIsRed = pNode->pParent->bIsRed;
		pNode->pParent->bIsRed = ZG_FALSE;
		pSibling->pRightChild->bIsRed = ZG_FALSE;

		if (!__ZGRBTreeLeftRotate(pNode->pParent, ppRoot))
			return ZG_FALSE;
	}

	if (pNode->pParent->pRightChild == pNode)
	{
		LPZGRBTREENODE pSibling = pNode->pParent->pLeftChild;
		if (pSibling->bIsRed)
		{
			pSibling->bIsRed = ZG_FALSE;
			pNode->pParent->bIsRed = ZG_TRUE;

			if (!__ZGRBTreeRightRotate(pNode->pParent, ppRoot))
				return ZG_FALSE;

			pSibling = pNode->pParent->pLeftChild;
		}

		if ((pSibling->pLeftChild == ZG_NULL || !pSibling->pLeftChild->bIsRed) &&
			(pSibling->pRightChild == ZG_NULL || !pSibling->pRightChild->bIsRed))
		{
			pSibling->bIsRed = ZG_TRUE;

			if (__ZGRBTreeRemoveFiexup(pNode->pParent, ppRoot))
				return ZG_TRUE;
		}
		else if (pSibling->pLeftChild == ZG_NULL || !pSibling->pLeftChild->bIsRed)
		{
			pSibling->pRightChild->bIsRed = ZG_FALSE;
			pSibling->bIsRed = ZG_TRUE;

			if (!__ZGRBTreeLeftRotate(pSibling, ppRoot))
				return ZG_FALSE;

			pSibling = pNode->pParent->pLeftChild;
		}

		pSibling->bIsRed = pNode->pParent->bIsRed;
		pNode->pParent->bIsRed = ZG_FALSE;
		pSibling->pLeftChild->bIsRed = ZG_FALSE;

		if (!__ZGRBTreeRightRotate(pNode->pParent, ppRoot))
			return ZG_FALSE;
	}

	return ZG_TRUE;
}

ZGBOOLEAN __ZGRBTreeReplace(LPZGRBTREENODE pDestination, const ZGRBTREENODE* pSource)
{
	if (pDestination == ZG_NULL || pSource == ZG_NULL)
		return ZG_FALSE;

	if (pDestination == pSource)
		return ZG_TRUE;

	pDestination->bIsRed = pSource->bIsRed;
	pDestination->pParent = pSource->pParent;
	pDestination->pLeftChild = pSource->pLeftChild;
	pDestination->pRightChild = pSource->pRightChild;

	if (pSource->pParent != ZG_NULL)
	{
		if (pSource->pParent->pLeftChild == pSource)
			pSource->pParent->pLeftChild = pDestination;

		if (pSource->pParent->pRightChild == pSource)
			pSource->pParent->pRightChild = pDestination;
	}

	if (pSource->pLeftChild != ZG_NULL)
		pSource->pLeftChild->pParent = pDestination;

	if (pSource->pRightChild != ZG_NULL)
		pSource->pRightChild->pParent = pDestination;

	return ZG_TRUE;
}

ZGBOOLEAN __ZGRBTreeAdded(LPZGRBTREENODE pNode, LPZGRBTREENODE* ppRoot, ZGCOMPARSION pfnComparsion, ZGBOOLEAN bIsAllowDuplicate)
{
	if (pNode == ZG_NULL || ppRoot == ZG_NULL)
		return ZG_FALSE;

	pNode->pParent = ZG_NULL;
	pNode->pLeftChild = ZG_NULL;
	pNode->pRightChild = ZG_NULL;

	LPZGRBTREENODE pParent = *ppRoot;
	if (pParent == ZG_NULL)
	{
		pNode->bIsRed = ZG_FALSE;

		*ppRoot = pNode;

		return ZG_TRUE;
	}

	if (pfnComparsion == ZG_NULL)
		return ZG_FALSE;

	while (ZG_TRUE)
	{
		ZGINT nComparsionValue = pfnComparsion(pNode->pKey, pParent->pKey);
		if (nComparsionValue < 0)
		{
			if (pParent->pLeftChild == ZG_NULL)
			{
				pNode->pParent = pParent;

				pParent->pLeftChild = pNode;

				break;
			}
			else
				pParent = pParent->pLeftChild;
		}
		else
		{
			if (nComparsionValue == 0 && !bIsAllowDuplicate)
				return ZG_FALSE;

			if (pParent->pRightChild == ZG_NULL)
			{
				pNode->pParent = pParent;

				pParent->pRightChild = pNode;

				break;
			}
			else
				pParent = pParent->pRightChild;
		}
	}

	return __ZGRBTreeInsertFiexup(pNode, ppRoot);
}

ZGBOOLEAN __ZGRBTreeRemoved(LPZGRBTREENODE pNode, LPZGRBTREENODE* ppRoot)
{
	if (pNode == ZG_NULL)
		return ZG_FALSE;

	LPZGRBTREENODE pTemp;
	if (pNode->pLeftChild == ZG_NULL || pNode->pRightChild == ZG_NULL)
		pTemp = pNode;
	else
	{
		pTemp = pNode->pRightChild;
		while (pTemp->pLeftChild != ZG_NULL)
			pTemp = pTemp->pLeftChild;
	}

	if (pTemp->pLeftChild == ZG_NULL && pTemp->pRightChild == ZG_NULL)
	{
		__ZGRBTreeRemoveFiexup(pTemp, ppRoot);

		if (pTemp->pParent == ZG_NULL)
		{
			if (ppRoot != ZG_NULL)
				*ppRoot = ZG_NULL;
		}
		else
		{
			if (pTemp->pParent->pLeftChild == pTemp)
				pTemp->pParent->pLeftChild = ZG_NULL;

			if (pTemp->pParent->pRightChild == pTemp)
				pTemp->pParent->pRightChild = ZG_NULL;
		}
	}
	else
	{
		LPZGRBTREENODE pChild = ZG_NULL;
		if (pTemp->pLeftChild != ZG_NULL)
		{
			pChild = pTemp->pLeftChild;
			pTemp->pLeftChild = ZG_NULL;
		}

		if (pTemp->pRightChild != ZG_NULL)
		{
			pChild = pTemp->pRightChild;
			pTemp->pRightChild = ZG_NULL;
		}

		if (__ZGRBTreeReplace(pChild, pTemp))
		{
			if (ppRoot != ZG_NULL && *ppRoot == pTemp)
				*ppRoot = pChild;
		}
		else
			return ZG_FALSE;
	}

	if (__ZGRBTreeReplace(pTemp, pNode))
	{
		if (ppRoot != ZG_NULL && *ppRoot == pNode)
			*ppRoot = pTemp;
	}
	else
		return ZG_FALSE;

	return ZG_TRUE;
}

LPZGRBTREENODE ZGRBTreeNodeNext(LPZGRBTREENODE pNode)
{
	if (pNode->pRightChild == ZG_NULL)
	{
		while (pNode->pParent != ZG_NULL)
		{
			if (pNode->pParent->pLeftChild == pNode)
				return pNode->pParent;

			pNode = pNode->pParent;
		}

		return ZG_NULL;
	}

	pNode = pNode->pRightChild;
	while (pNode->pLeftChild != ZG_NULL)
		pNode = pNode->pLeftChild;

	return pNode;
}

LPZGRBTREENODE ZGRBTreeNodeGet(LPZGRBTREENODE pNode, const void* pKey, ZGCOMPARSION pfnComparsion)
{
	if (pKey == ZG_NULL || pfnComparsion == ZG_NULL)
		return ZG_NULL;

	ZGINT nComparsionValue;
	LPZGRBTREENODE pResult = ZG_NULL;
	while (ZG_TRUE)
	{
		nComparsionValue = pfnComparsion(pKey, pNode->pKey);
		if (nComparsionValue < 0)
		{
			if (pResult != ZG_NULL)
				break;

			if (pNode->pLeftChild == ZG_NULL)
				break;
			else
				pNode = pNode->pLeftChild;
		}
		else
		{
			if (nComparsionValue == 0)
				pResult = pNode;

			if (pNode->pRightChild == ZG_NULL)
				break;
			else
				pNode = pNode->pRightChild;
		}
	}

	return pResult;
}

LPZGRBTREENODE ZGRBTreeGet(const ZGRBTREE* pTree, const void* pKey)
{
	if (pTree == ZG_NULL || pTree->pRoot == ZG_NULL)
		return ZG_NULL;

	return ZGRBTreeNodeGet(pTree->pRoot, pKey, pTree->pfnComparsion);
}

void ZGRBTreeInit(LPZGRBTREE pTree, ZGCOMPARSION pfnComparsion)
{
	if (pTree == ZG_NULL)
		return;

	pTree->uCount = 0;
	pTree->pRoot = ZG_NULL;
	pTree->pfnComparsion = pfnComparsion;
}

ZGBOOLEAN ZGRBTreeAdd(LPZGRBTREE pTree, LPZGRBTREENODE pNode, void* pValue, const void* pKey, ZGBOOLEAN bIsAllowDuplicate)
{
	if (pTree == ZG_NULL || pNode == ZG_NULL)
		return ZG_FALSE;

	pNode->pValue = pValue;
	pNode->pKey = pKey;

	pNode->bIsRed = ZG_TRUE;

	if (__ZGRBTreeAdded(pNode, &pTree->pRoot, pTree->pfnComparsion, bIsAllowDuplicate))
	{
		++pTree->uCount;

		return ZG_TRUE;
	}

	return ZG_FALSE;
}

ZGBOOLEAN ZGRBTreeRemove(LPZGRBTREE pTree, LPZGRBTREENODE pNode)
{
	if (__ZGRBTreeRemoved(pNode, &pTree->pRoot))
	{
		pNode->bIsRed = ZG_TRUE;
		pNode->pParent = ZG_NULL;
		pNode->pLeftChild = ZG_NULL;
		pNode->pRightChild = ZG_NULL;

		--pTree->uCount;

		return ZG_TRUE;
	}

	return ZG_FALSE;
}

ZGBOOLEAN ZGRBTreeRemoveAt(LPZGRBTREE pTree, const void* pKey)
{
	return ZGRBTreeRemove(pTree, ZGRBTreeGet(pTree, pKey));
}

void ZGRBTreeClear(LPZGRBTREE pTree)
{
	LPZGRBTREENODE pNode = pTree->pRoot;
	while (__ZGRBTreeRemoved(pNode, &pTree->pRoot))
	{
		pNode->bIsRed = ZG_TRUE;
		pNode->pParent = ZG_NULL;
		pNode->pLeftChild = ZG_NULL;
		pNode->pRightChild = ZG_NULL;

		pNode = pTree->pRoot;
	}

	pTree->uCount = 0;
}