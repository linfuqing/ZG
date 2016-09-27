#pragma once

#include "Core.h"
#include "Comparsion.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct ZGRBTreeNode
	{
		void* pValue;
		const void* pKey;
		ZGBOOLEAN bIsRed;
		struct ZGRBTreeNode* pParent;
		struct ZGRBTreeNode* pLeftChild;
		struct ZGRBTreeNode* pRightChild;
	}ZGRBTREENODE, *LPZGRBTREENODE;

	typedef struct ZGRBTree
	{
		ZGUINT uCount;
		LPZGRBTREENODE pRoot;
		ZGCOMPARSION pfnComparsion;
	}ZGRBTREE, *LPZGRBTREE;

	LPZGRBTREENODE ZGRBTreeNodeNext(LPZGRBTREENODE pNode);

	LPZGRBTREENODE ZGRBTreeNodeGet(LPZGRBTREENODE pNode, const void* pKey, ZGCOMPARSION pfnComparsion);

	LPZGRBTREENODE ZGRBTreeGet(const ZGRBTREE* pTree, const void* pKey);

	void ZGRBTreeInit(LPZGRBTREE pTree, ZGCOMPARSION pfnComparsion);

	ZGBOOLEAN ZGRBTreeAdd(LPZGRBTREE pTree, LPZGRBTREENODE pNode, void* pValue, const void* pKey, ZGBOOLEAN bIsAllowDuplicate);

	ZGBOOLEAN ZGRBTreeRemove(LPZGRBTREE pTree, LPZGRBTREENODE pNode);

	ZGBOOLEAN ZGRBTreeRemoveAt(LPZGRBTREE pTree, const void* pKey);

	void ZGRBTreeClear(LPZGRBTREE pTree);
#ifdef __cplusplus
}
#endif
