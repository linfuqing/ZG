// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../ZG/RTS.h"

int main()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	
	ZGUINT uFuck = 10, uShit = -11;
	ZGINT nFuck = uFuck += uShit;
	printf("%d", nFuck);

    return _CrtDumpMemoryLeaks();
}

