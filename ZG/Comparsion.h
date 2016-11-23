#pragma once

#include "Core.h"

typedef ZGINT(*ZGCOMPARSION)(const void*, const void*);

ZG_INLINE ZGINT ZGComparsionPointer(const void* pSource, const void* pDestination)
{
	return (ZGINT)((ZGLONG)pSource - (ZGLONG)pDestination);
}

ZG_INLINE ZGINT ZGComparsionInt(const void* pSource, const void* pDestination)
{
	return *((PZGINT)pSource) - *((PZGINT)pDestination);
}

ZG_INLINE ZGINT ZGComparsionIntInverce(const void* pSource, const void* pDestination)
{
	return *((PZGINT)pDestination) - *((PZGINT)pSource);
}