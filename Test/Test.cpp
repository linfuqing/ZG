// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../ZG/RTS.h"

int main()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	

    return _CrtDumpMemoryLeaks();
}

