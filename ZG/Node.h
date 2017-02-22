#pragma once

#include "Core.h"
#include "RBList.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef ZGUINT(*ZGNODEPREDICATION)(const void*, const void*);

	typedef ZGUINT(*ZGNODEEVALUATION)(void*);

	/*typedef enum ZGNodeSearchType
	{
		ZG_NODE_SEARCH_TYPE_ONCE, 
		ZG_NODE_SEARCH_TYPE_MIN, 
		ZG_NODE_SEARCH_TYPE_MAX
	}ZGNODESEARCHTYPE, *LPZGNODESEARCHTYPE;*/

	typedef struct ZGNode
	{
		ZGRBLISTNODE Instance;

		ZGUINT uDistance;
		ZGUINT uValue;
		ZGUINT uEvaluation;
		ZGUINT uDepth;

		ZGUINT uCount;

		struct ZGNode** ppChildren;
		struct ZGNode* pForward;
		struct ZGNode* pBackward;
		struct ZGNode* pPrevious;
		struct ZGNode* pNext;

		LPZGRBLIST pList;
		
		void* pData;
	}ZGNODE, *LPZGNODE;

	ZGUINT ZGNodeSearch(
		LPZGNODE pNode,
		ZGNODEPREDICATION pfnPredication,
		ZGNODEEVALUATION pfnEvaluation,
		ZGUINT uMinEvaluation,
		ZGUINT uMaxEvaluation,
		ZGUINT uMaxDistance,
		ZGUINT uMaxDepth/*,
		ZGNODESEARCHTYPE type*/);

	void ZGNodeEnable(LPZGNODE pNode);

	void ZGNodeDisable(LPZGNODE pNode);
#ifdef __cplusplus
}
#endif