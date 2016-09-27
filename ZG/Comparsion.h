#pragma once

#include "Core.h"

typedef ZGINT(*ZGCOMPARSION)(const void*, const void*);

inline ZGINT ZGComparsionPointer(const void* pSource, const void* pDestination)
{
	return (ZGINT)((ZGLONG)pSource - (ZGLONG)pDestination);
}

inline ZGINT ZGComparsionInt(const void* pSource, const void* pDestination)
{
	return *((PZGINT)pSource) - *((PZGINT)pDestination);
}

inline ZGINT ZGComparsionIntInverce(const void* pSource, const void* pDestination)
{
	return *((PZGINT)pDestination) - *((PZGINT)pSource);
}