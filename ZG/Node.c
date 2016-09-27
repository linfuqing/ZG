#include "Node.h"

static LPZGNODE sg_pHead = ZG_NULL;

ZGUINT __ZGNodeSearch(
	LPZGRBLIST pList,
	LPZGNODE pNode,
	ZGNODEPREDICATION pfnPredication,
	ZGNODEEVALUATION pfnEvaluation,
	ZGUINT uEvaluation,
	ZGUINT uMinEvaluation, 
	ZGUINT uMaxEvaluation, 
	ZGUINT uMaxDistance, 
	ZGUINT uMaxDepth,
	PZGUINT puMinDepth)
{
	if (pNode == ZG_NULL || 
		pList == ZG_NULL || 
		pfnEvaluation == ZG_NULL/* ||
		(pNode->uDepth + pNode->uEvaluation) > uMaxDepth*/)
		return uMaxDepth + 1;

	pNode->pNext = ZG_NULL;

	ZGUINT uMinDepth = puMinDepth == ZG_NULL ? 0 : *puMinDepth, uOutputDepth = uMaxDepth + 1, uCurrentDepth;
	LPZGNODE pTemp;
	if (pNode->uEvaluation > uMinEvaluation)
	{
		if (pNode->uValue < uMaxDistance && pNode->uDepth < uMaxDepth)
		{
			ZGUINT uDepth = pNode->uDepth + 1, uDistance;
			LPZGNODE pChild;
			for(ZGUINT i = 0; i < pNode->uCount; ++ i)
			{
				pChild = pNode->ppChildren[i];
				if (pChild != ZG_NULL)
				{
					if (pChild == pNode->pPrevious)
						continue;

					if (pChild->pPrevious != pNode)
					{
						if (pChild->pPrevious == ZG_NULL || pChild->pPrevious->uDepth < uMinDepth || pChild->pPrevious->uDepth > pNode->uDepth)
						{
							pChild->pPrevious = pNode;

							pChild->uDistance = pfnPredication == ZG_NULL ? 1 : pfnPredication(pNode->pData, pChild->pData);
						}
						else
							continue;
					}

					if (pChild->uDepth < uMinDepth)
					{
						if (pChild->pList == ZG_NULL)
							pChild->uEvaluation = pfnEvaluation(pChild->pData);
						else
						{
							pChild->pList = ZG_NULL;

							ZGRBListRemove(pChild->pList, &pChild->Instance);
						}

						if (pChild->uEvaluation < uMaxEvaluation)
						{
							//pChild->uValue = pChild->uDistance + pNode->uValue;
							uDistance = pChild->uDistance + pNode->uValue;

							if (uDistance < uMaxDistance)
							{
								pChild->pList = pList;

								ZGRBListAdd(pList, &pChild->Instance, pChild, (void*)(uDistance + pChild->uEvaluation), ZG_TRUE);
							}
						}
					}
					else if (uDepth < pChild->uDepth && (pNode->pNext == ZG_NULL || (uOutputDepth - pNode->uDepth) > pNode->uEvaluation))
					{
						uDistance = pNode->uValue;
						uCurrentDepth = pNode->uDepth;
						pTemp = pChild;
						while (pTemp->uEvaluation > uMinEvaluation)
						{
							uDistance = (pTemp->pPrevious == ZG_NULL ? 0 : pTemp->pPrevious->uValue) + pTemp->uDistance;

							++uCurrentDepth;

							pTemp->uValue = uDistance;
							pTemp->uDepth = uCurrentDepth;
							pTemp = pTemp->pNext;

							if (pTemp == ZG_NULL)
								break;
						}

						if (pTemp != ZG_NULL)
						{
							pTemp->uValue += pTemp->uEvaluation;

							if (pTemp->uValue > uMaxDistance || uCurrentDepth > uMaxDepth)
								pTemp = ZG_NULL;
						}

						if (pTemp != ZG_NULL)
						{
							pNode->pNext = pChild;

							if (pTemp->uValue <= uEvaluation)
								return uCurrentDepth;

							uMaxDistance = pTemp->uValue;

							uOutputDepth = uCurrentDepth;

							uMaxDepth = uOutputDepth - 1;
						}
					}
				}
			}
		}
	}
	else
	{
		pTemp = pNode;
		LPZGNODE pPrevious = pNode->pPrevious;
		while (pPrevious != ZG_NULL && /*previous != temp.__next && */pPrevious->uDepth >= uMinDepth && (pPrevious->pNext == ZG_NULL || pPrevious->pNext->uDepth > pTemp->uDepth))
		{
			pPrevious->pNext = pTemp;
			pTemp = pPrevious;

			pPrevious = pPrevious->pPrevious;
		}

		pNode->uValue += pNode->uEvaluation;
		if (pNode->uValue <= uEvaluation)
		{
			if(puMinDepth != ZG_NULL)
				*puMinDepth = pNode->uDepth + 1;

			return pNode->uDepth;
		}

		uMaxDistance = pNode->uValue;

		uOutputDepth = pNode->uDepth;

		uMaxDepth = uOutputDepth - 1;
	}

	ZGUINT uOutputMinDepth = pNode->uDepth + 1, uCurrentMinDepth;
	LPZGRBLISTNODE pHead = pList->pHead, pCurrent;
	while (pHead != ZG_NULL)
	{
		pCurrent = pHead;
		pHead = ZGRBListNodeNext(pCurrent);
		pTemp = (LPZGNODE)pCurrent->pValue;
		pTemp->pList = ZG_NULL;
		ZGRBListRemove(pList, pCurrent);

		if (pTemp != ZG_NULL &&
			pTemp->pPrevious != ZG_NULL && 
			pTemp->pPrevious->uValue < uMaxDistance && 
			pTemp->pPrevious->uDepth < uMaxDepth && 
			pTemp->pPrevious->uDepth >= uMinDepth)
		{
			uCurrentMinDepth = uMinDepth;
			pTemp->uValue = pTemp->pPrevious->uValue + pTemp->uDistance;
			pTemp->uDepth = pTemp->pPrevious->uDepth + 1;
			uCurrentDepth = __ZGNodeSearch(pList, pTemp, pfnPredication, pfnEvaluation, uEvaluation, uMinEvaluation, uMaxEvaluation, uMaxDistance, uMaxDepth, &uCurrentMinDepth);

			if(uCurrentMinDepth > uOutputMinDepth)
				uOutputMinDepth = uCurrentMinDepth;

			if (uCurrentDepth >= uMinDepth && uCurrentDepth <= uMaxDepth)
				uOutputDepth = uCurrentDepth;

			break;
		}
	}

	if(puMinDepth != ZG_NULL)
		*puMinDepth = uOutputMinDepth;

	return uOutputDepth;
}

ZGUINT ZGNodeSearch(
	LPZGNODE pNode, 
	ZGNODEPREDICATION pfnPredication,
	ZGNODEEVALUATION pfnEvaluation,
	ZGUINT uMinEvaluation,
	ZGUINT uMaxEvaluation,
	ZGUINT uMaxDistance,
	ZGUINT uMaxDepth, 
	ZGNODESEARCHTYPE type)
{
	if (pNode == ZG_NULL || pfnEvaluation == ZG_NULL)
		return 0;

	pNode->uDistance = 0;
	pNode->uValue = 0;

	pNode->uEvaluation = pfnEvaluation(pNode->pData);

	static ZGUINT s_uMinDepth = 1;

	if (s_uMinDepth >= (~0 - uMaxDepth))
	{
		s_uMinDepth = 1;

		for (LPZGNODE pTemp = sg_pHead; pTemp != ZG_NULL; pTemp = pTemp->pForward)
			pTemp->uDepth = 0;
	}

	pNode->uDepth = s_uMinDepth;

	pNode->pPrevious = ZG_NULL;


	static ZGRBLIST s_List;
	static ZGBOOLEAN s_bIsCreate = ZG_FALSE;

	if (s_bIsCreate)
	{
		for (LPZGRBLISTNODE pNode = s_List.pHead; pNode != ZG_NULL; pNode = ZGRBListNodeNext(pNode))
		{
			if (pNode->pValue != ZG_NULL)
				((LPZGNODE)pNode->pValue)->pList = ZG_NULL;
		}

		ZGRBListClear(&s_List);
	}
	else
	{
		ZGRBListInit(&s_List, ZGComparsionPointer);

		s_bIsCreate = ZG_TRUE;
	}

	uMaxDepth = uMaxDepth > 0 ? s_uMinDepth + uMaxDepth : ~0 - 1;

	ZGUINT uEvaluation;
	switch (type)
	{
	case ZG_NODE_SEARCH_TYPE_ONCE:
		uEvaluation = ~0;
		break;
	case ZG_NODE_SEARCH_TYPE_MIN:
		uEvaluation = pNode->uEvaluation;
		break;
	case ZG_NODE_SEARCH_TYPE_MAX:
		uEvaluation = 0;
		break;
	default:
		break;
	}

	ZGUINT uDepth = __ZGNodeSearch(
		&s_List, 
		pNode, 
		pfnPredication,
		pfnEvaluation, 
		uEvaluation,
		uMinEvaluation, 
		uMaxEvaluation, 
		uMaxDistance, 
		uMaxDepth, 
		&s_uMinDepth);
	return uDepth > uMaxDepth ? 0 : (uDepth - pNode->uDepth);
}

void ZGNodeEnable(LPZGNODE pNode)
{
	pNode->uDepth = 0;

	if (sg_pHead != ZG_NULL)
		sg_pHead->pBackward = pNode;

	pNode->pForward = sg_pHead;
	pNode->pBackward = ZG_NULL;
	pNode->pPrevious = ZG_NULL;
	pNode->pNext = ZG_NULL;
	pNode->pList = ZG_NULL;
	pNode->Instance.pValue = pNode;

	sg_pHead = pNode;
}

void ZGNodeDisable(LPZGNODE pNode)
{
	if (pNode->pForward != ZG_NULL)
		pNode->pForward->pBackward = pNode->pBackward;

	if (pNode->pBackward != ZG_NULL)
		pNode->pBackward->pForward = pNode->pForward;

	if (sg_pHead == pNode)
		sg_pHead = pNode->pForward;

	pNode->pForward = ZG_NULL;
	pNode->pBackward = ZG_NULL;

	if (pNode->pList != ZG_NULL)
	{
		ZGRBListRemove(pNode->pList, &pNode->Instance);

		pNode->pList = ZG_NULL;
	}
}