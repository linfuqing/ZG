// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../ZG/SLG.h"

int main()
{
	LPZGTILEMAP pMap0;
	pMap0 = ZGSLGCreateMap(8, 8, 0);
	LPZGRBLIST pQueue0;
	pQueue0 = ZGSLGCreateQueue();
	LPZGRBLISTNODE pObject0;
	pObject0 = ZGSLGCreateObject(1, 1, 0, 0, 1, pQueue0);
	LPZGRBLISTNODE pObject1;
	pObject1 = ZGSLGCreateObject(1, 1, 0, 1, 1, pQueue0);
	LPZGTILEACTION pAction0;
	pAction0 = ZGSLGCreateAction(1, 0);
	ZGSLGGetAttributeFromObjectUINT(pObject0, 1);
	ZGSLGSetAttributeToObjectUINT(pObject0, 1, 1);
	ZGSLGSetDistanceToObject(pObject0, 10);
	ZGSLGSetRangeToObject(pObject0, 10);
	ZGSLGSetActionToObject(pObject0, pAction0, 0);
	ZGSLGSetAttributeToObjectUINT(pObject1, 2, 1);
	ZGSLGSetAttributeToObjectUINT(pObject0, 5, 1);
	ZGSLGSetAttributeToObjectUINT(pObject1, 7, 0);
	ZGSLGSetAttributeToObjectUINT(pObject0, 6, 1);
	ZGSLGSetAttributeToObjectUINT(pObject1, 8, 0);
	ZGSLGSetMap(pMap0, 3, 1);
	ZGSLGSetMap(pMap0, 11, 1);
	ZGSLGMoveObjectToMap(pObject0, pMap0, 0);
	ZGSLGMoveObjectToMap(pObject1, pMap0, 4);
	LPZGRBLISTNODE pTemp0;
	pTemp0 = ZGSLGGetObjectFromQueue(pQueue0);
	pTemp0 = ZGSLGGetNextFromObject(pTemp0);
	ZGSLGGetIndexFromObject(pObject1);
	LPZGNODE pNode0;
	ZGUINT uActionIndex;
	ZGUINT uMapIndex;
	ZGUINT uInfoCount;
	LPZGSLGINFO pInfos;
	ZGSLGRun(pQueue0, pMap0, 10, 10, 100, 100, 100, &uActionIndex, &uMapIndex, &uInfoCount, &pInfos);
	pNode0 = ZGSLGGetMapNodeFromMap(pMap0, 4);
	ZGSLGDestroyMap(pMap0);
	ZGSLGDestroy(pQueue0);
	ZGSLGDestroy(pObject1);
	ZGSLGDestroy(pObject0);
	ZGSLGDestroy(pAction0);

    return _CrtDumpMemoryLeaks();
}

