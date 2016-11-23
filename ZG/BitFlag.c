#include "BitFlag.h"
#include "Math.h"

ZG_INLINE ZGUINT8 __ZGBitFlagGetFlag(PZGUINT8 puFlags, ZGUINT uIndex, ZGUINT uCount)
{
	ZGUINT uTemp = uIndex << 3;
	if (uTemp < uCount)
		uTemp = uCount - uTemp;
	else
		return 0;

	return puFlags[uIndex] & (uTemp < 8 ? (1 << uTemp) - 1 : 255);
}

ZGUINT ZGBitFlagMinBit(const ZGBITFLAG* pBitFlag)
{
	if (pBitFlag == ZG_NULL || pBitFlag->uCount <= 0)
		return 0;

	ZGUINT uOffset = pBitFlag->uOffset >> 3, uCount = pBitFlag->uOffset + pBitFlag->uCount;
	ZGUINT8 uFlag = __ZGBitFlagGetFlag(pBitFlag->puFlags, uOffset, uCount) >> (pBitFlag->uOffset & 7);
	if (uFlag > 0)
		return ZGMininumBit8(uFlag);

	ZGUINT uLength = (uCount + 7) >> 3, i;
	for (i = uOffset + 1; i < uLength; ++i)
	{
		uFlag = __ZGBitFlagGetFlag(pBitFlag->puFlags, i, uCount);
		if (uFlag > 0)
			return (i << 3) + ZGMininumBit8(uFlag) - pBitFlag->uOffset;
	}

	return 0;
}

ZGUINT ZGBitFlagIndexOf(const ZGBITFLAG* pSource, const ZGBITFLAG* pDestination)
{
	if(pSource == ZG_NULL || pSource->uCount <= 0)
		return ZGBitFlagMinBit(pDestination);

	if (pDestination == ZG_NULL || pDestination->uCount <= 0)
		return ZGBitFlagMinBit(pSource);

	ZGUINT uFlag,
		uSourceCount = pSource->uOffset + pSource->uCount,
		uDestinationCount = pDestination->uOffset + pDestination->uCount, 
		uSourceFlag = __ZGBitFlagGetFlag(pSource->puFlags, pSource->uOffset >> 3, uSourceCount),
		uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, pDestination->uOffset >> 3, uDestinationCount),
		uSourceShift = pSource->uOffset & 7,
		uDestinationShift = pDestination->uOffset & 7;

	uFlag = (uSourceFlag >> uSourceShift) & (uDestinationFlag >> uDestinationShift);
	if (uFlag > 0)
		return ZGMininumBit8(uFlag);

	ZGUINT uOffset = uSourceShift > uDestinationShift ? uSourceShift - uDestinationShift : uDestinationShift - uSourceShift, 
		uSourceOffset, 
		uDestinationOffset;
	if (uSourceShift > uDestinationShift)
	{
		for (uSourceOffset = pSource->uOffset + 8, uDestinationOffset = pDestination->uOffset + 8;
			uSourceOffset < uSourceCount && uDestinationOffset < uDestinationCount;
			uSourceOffset += 8, uDestinationOffset += 8)
		{
			uSourceFlag = __ZGBitFlagGetFlag(pSource->puFlags, uSourceOffset >> 3, uSourceCount);
			uFlag = uSourceFlag & (uDestinationFlag >> uSourceShift);
			if (uFlag > 0)
				return (uSourceOffset & ~7) + ZGMininumBit8(uFlag) - pSource->uOffset;

			uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, uDestinationOffset >> 3, uDestinationCount);
			uFlag = uSourceFlag & (uDestinationFlag << uSourceShift);
			if (uFlag > 0)
				return (uSourceOffset & ~7) + ZGMininumBit8(uFlag) - pSource->uOffset;
		}
	}
	else if (uSourceShift < uDestinationShift)
	{
		for (uSourceOffset = pSource->uOffset + 8, uDestinationOffset = pDestination->uOffset + 8;
			uSourceOffset < uSourceCount && uDestinationOffset < uDestinationCount;
			uSourceOffset += 8, uDestinationOffset += 8)
		{
			uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, uDestinationOffset >> 3, uDestinationCount);
			uFlag = uSourceFlag & (uDestinationFlag << uDestinationShift);
			if (uFlag > 0)
				return ((uSourceOffset - 8) & ~7) + ZGMininumBit8(uFlag) - pSource->uOffset;

			uSourceFlag = __ZGBitFlagGetFlag(pSource->puFlags, uSourceOffset >> 3, uSourceCount);
			uFlag = uSourceFlag & (uDestinationFlag >> uSourceShift);
			if (uFlag > 0)
				return (uSourceOffset & ~7) + ZGMininumBit8(uFlag) - pSource->uOffset;
		}
	}
	else
	{
		for (uSourceOffset = pSource->uOffset + 8, uDestinationOffset = pDestination->uOffset + 8;
			uSourceOffset < uSourceCount && uDestinationOffset < uDestinationCount;
			uSourceOffset += 8, uDestinationOffset += 8)
		{
			uSourceFlag = __ZGBitFlagGetFlag(pSource->puFlags, uSourceOffset >> 3, uSourceCount);
			uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, uDestinationOffset >> 3, uDestinationCount);
			uFlag = uSourceFlag & uDestinationFlag;
			if (uFlag > 0)
				return (uSourceOffset & ~7) + ZGMininumBit8(uFlag) - pSource->uOffset;
		}
	}

	return 0;
}

void ZGBitFlagAssign(LPZGBITFLAG pSource, const ZGBITFLAG* pDestination, ZGBOOLEAN bValue)
{
	if (pSource == ZG_NULL || pSource->uCount <= 0)
		return;

	if (pDestination == ZG_NULL || pDestination->uCount <= 0)
		return;

	ZGUINT uIndex,
		uDestinationCount = pDestination->uOffset + pDestination->uCount, 
		uSourceShift = pSource->uOffset & 7,
		uDestinationShift = pDestination->uOffset & 7;

	ZGUINT8 uFlag, uSourceFlag, uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, pDestination->uOffset >> 3, uDestinationCount);

	uFlag = uDestinationFlag >> uDestinationShift;
	if (uFlag > 0)
	{
		uIndex = pSource->uOffset >> 3;
		uSourceFlag = pSource->puFlags[uIndex];

		uFlag <<= uSourceShift;
		if (bValue)
			uSourceFlag |= uFlag;
		else
			uSourceFlag &= ~uFlag;

		pSource->puFlags[uIndex] = uSourceFlag;
	}

	ZGUINT uOffset = uSourceShift > uDestinationShift ? uSourceShift - uDestinationShift : uDestinationShift - uSourceShift,
		uSourceCount = pSource->uOffset + pSource->uCount, 
		uSourceOffset,
		uDestinationOffset;
	if (uSourceShift > uDestinationShift)
	{
		for (uSourceOffset = pSource->uOffset + 8, uDestinationOffset = pDestination->uOffset + 8;
			uSourceOffset < uSourceCount && uDestinationOffset < uDestinationCount;
			uSourceOffset += 8, uDestinationOffset += 8)
		{
			uIndex = uSourceOffset >> 3;
			uSourceFlag = pSource->puFlags[uIndex];

			uFlag = uDestinationFlag >> uSourceShift;
			if (uFlag > 0)
			{
				if (bValue)
					uSourceFlag |= uFlag;
				else
					uSourceFlag &= ~uFlag;

				pSource->puFlags[uIndex] = uSourceFlag;
			}

			uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, uDestinationOffset >> 3, uDestinationCount);
			uFlag = uDestinationFlag << uSourceShift;
			if (uFlag > 0)
			{
				if (bValue)
					uSourceFlag |= uFlag;
				else
					uSourceFlag &= ~uFlag;

				pSource->puFlags[uIndex] = uSourceFlag;
			}
		}
	}
	else if (uSourceShift < uDestinationShift)
	{
		for (uSourceOffset = pSource->uOffset + 8, uDestinationOffset = pDestination->uOffset + 8;
			uSourceOffset < uSourceCount && uDestinationOffset < uDestinationCount;
			uSourceOffset += 8, uDestinationOffset += 8)
		{
			uIndex = uSourceOffset >> 3;
			uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, uDestinationOffset >> 3, uDestinationCount);
			uFlag = uDestinationFlag << uDestinationShift;
			if (uFlag > 0)
			{
				if (bValue)
					uSourceFlag |= uFlag;
				else
					uSourceFlag &= ~uFlag;

				pSource->puFlags[uIndex - 1] = uSourceFlag;
			}

			uSourceFlag = pSource->puFlags[uIndex];
			uFlag = uDestinationFlag >> uSourceShift;
			if (uFlag > 0)
			{
				if (bValue)
					uSourceFlag |= uFlag;
				else
					uSourceFlag &= ~uFlag;

				pSource->puFlags[uIndex] = uSourceFlag;
			}
		}
	}
	else
	{
		for (uSourceOffset = pSource->uOffset + 8, uDestinationOffset = pDestination->uOffset + 8;
			uSourceOffset < uSourceCount && uDestinationOffset < uDestinationCount;
			uSourceOffset += 8, uDestinationOffset += 8)
		{
			uDestinationFlag = __ZGBitFlagGetFlag(pDestination->puFlags, uDestinationOffset >> 3, uDestinationCount);
			uFlag = uDestinationFlag;
			if (uFlag > 0)
			{
				uIndex = uSourceOffset >> 3;

				uSourceFlag = pSource->puFlags[uIndex];

				if (bValue)
					uSourceFlag |= uFlag;
				else
					uSourceFlag &= ~uFlag;

				pSource->puFlags[uIndex] = uSourceFlag;
			}
		}
	}
}
