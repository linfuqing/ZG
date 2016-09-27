#pragma once

#include "RBTree.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct ZGRBListNode
	{
		ZGRBTREENODE Instance;
		void* pValue;
	}ZGRBLISTNODE, *LPZGRBLISTNODE;

	typedef struct ZGRBList
	{
		ZGRBTREE Instance;
		LPZGRBLISTNODE pHead;
	}ZGRBLIST, *LPZGRBLIST;

	LPZGRBLISTNODE ZGRBListNodeNext(LPZGRBLISTNODE pNode);

	LPZGRBLISTNODE ZGRBListGet(LPZGRBLIST pList, const void* pKey);

	void ZGRBListInit(LPZGRBLIST pList, ZGCOMPARSION pfnComparsion);

	ZGBOOLEAN ZGRBListAdd(LPZGRBLIST pList, LPZGRBLISTNODE pNode, void* pValue, const void* pKey, ZGBOOLEAN bIsAllowDuplicate);

	ZGBOOLEAN ZGRBListRemove(LPZGRBLIST pList, LPZGRBLISTNODE pNode);

	ZGBOOLEAN ZGRBListRemoveAt(LPZGRBLIST pList, const void* pKey);

	void ZGRBListClear(LPZGRBLIST pList);
#ifdef __cplusplus
}
#endif

