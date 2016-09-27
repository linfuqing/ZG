#include "Map.h"
#include "Math.h"

ZGBOOLEAN ZGMapTest(
	const ZGMAP* pSource, 
	const ZGMAP* pDestination, 
	ZGUINT uIndex, 
	ZGUINT uOffset, 
	PZGUINT puCount, 
	PZGUINT puIndices)
{
	if (pSource == ZG_NULL || pDestination == ZG_NULL)
		return 0;

	uIndex += pSource->Instance.uOffset;
	uOffset += pDestination->Instance.uOffset;

	ZGUINT uSourceX = uIndex % pSource->uPitch, 
		uSourceY = uIndex / pSource->uPitch, 
		uDestinationX = uOffset % pDestination->uPitch, 
		uDestinationY = uOffset / pDestination->uPitch, 
		x = uSourceX > uDestinationX ? uSourceX - uDestinationX : 0,
		y = uSourceY > uDestinationY ? uSourceY - uDestinationY : 0,
		uMinX = uDestinationX - (uSourceX - x),
		uMinY = uDestinationY - (uSourceY - y), 
		uPitch = pDestination->uPitch - uMinX, 
		uSourceOffset = pSource->Instance.uOffset + x + y * pSource->uPitch, 
		uDestinationOffset = pDestination->Instance.uOffset + uMinX + uMinY * pDestination->uPitch;

	uPitch = ZG_MIN(pSource->uPitch, uPitch);
	ZGBITFLAG Source, Destination;
	Source.puFlags = pSource->Instance.puFlags;
	Destination.puFlags = pDestination->Instance.puFlags;
	
	ZGUINT uSourceLength = pSource->Instance.uOffset + pSource->Instance.uCount,
		uDestinationLength = pDestination->Instance.uOffset + pDestination->Instance.uCount,
		uCount = puCount == ZG_NULL ? 0 : *puCount, 
		uResult = 0, 
		uTemp;
	while (uSourceOffset < uSourceLength && uDestinationOffset < uDestinationLength)
	{
		Source.uOffset = uSourceOffset;
		Destination.uOffset = uDestinationOffset;

		Source.uCount = uPitch;
		Destination.uCount = uPitch;
		uTemp = ZGBitFlagIndexOf(&Source, &Destination);
		while (uTemp > 0)
		{
			++uResult;

			Source.uOffset += uTemp;
			if (uCount > 0)
			{
				--uCount;

				*puIndices++ = Source.uOffset - 1;
			}
			else
				return ZG_TRUE;

			Destination.uOffset += uTemp;
			Source.uCount -= uTemp;
			Destination.uCount -= uTemp;
			uTemp = ZGBitFlagIndexOf(&Source, &Destination);
		}

		uSourceOffset += pSource->uPitch;
		uDestinationOffset += pDestination->uPitch;
	}

	if (puCount != ZG_NULL)
		*puCount = uResult;

	return uResult > 0;
}

void ZGMapAssign(
	LPZGMAP pSource, 
	const ZGMAP* pDestination, 
	ZGUINT uIndex, 
	ZGUINT uOffset, 
	ZGBOOLEAN bValue)
{
	if (pSource == ZG_NULL || pDestination == ZG_NULL)
		return;

	uIndex += pSource->Instance.uOffset;
	uOffset += pDestination->Instance.uOffset;

	ZGUINT uSourceX = uIndex % pSource->uPitch,
		uSourceY = uIndex / pSource->uPitch,
		uDestinationX = uOffset % pDestination->uPitch,
		uDestinationY = uOffset / pDestination->uPitch,
		x = uSourceX > uDestinationX ? uSourceX - uDestinationX : 0,
		y = uSourceY > uDestinationY ? uSourceY - uDestinationY : 0,
		uMinX = uDestinationX - (uSourceX - x),
		uMinY = uDestinationY - (uSourceY - y),
		uPitch = pDestination->uPitch - uMinX,
		uSourceOffset = pSource->Instance.uOffset + x + y * pSource->uPitch,
		uDestinationOffset = pDestination->Instance.uOffset + uMinX + uMinY * pDestination->uPitch;

	uPitch = ZG_MIN(pSource->uPitch, uPitch);
	ZGBITFLAG Source, Destination;
	Source.puFlags = pSource->Instance.puFlags;
	Destination.puFlags = pDestination->Instance.puFlags;

	ZGUINT uSourceLength = pSource->Instance.uOffset + pSource->Instance.uCount,
		uDestinationLength = pDestination->Instance.uOffset + pDestination->Instance.uCount;
	while (uSourceOffset < uSourceLength && uDestinationOffset < uDestinationLength)
	{
		Source.uOffset = uSourceOffset;
		Destination.uOffset = uDestinationOffset;

		Source.uCount = uPitch;
		Destination.uCount = uPitch;

		ZGBitFlagAssign(&Source, &Destination, bValue);

		uSourceOffset += pSource->uPitch;
		uDestinationOffset += pDestination->uPitch;
	}
}