#include <stdio.h>
#include <assert.h>

#include <guidseed.h>

IMPL_GUID_ENCODE(RecodeSymbol)(
    void *pvArg, const char *pszSeed, DWORD *pResult)
{
    assert(0 != *pszSeed);

    const char *pszSeedStart;
    DWORD dwOffset, dwValue, cSize, cCount;

    cCount = dwOffset = 0;
    cSize = strlen(pszSeed);
    pszSeedStart = pszSeed;

    while (TRUE) {
        dwValue = EncodeCSymbol(*pszSeed++);

        *pResult |= dwValue << dwOffset;
        dwOffset += 6;

        if (dwOffset >= 32) {
            if (2 == cCount) break;
            cCount++;
            pResult++;
            dwOffset -= 32;
            *pResult |= dwValue >> (6 - dwOffset);
        }
        if (0 == *pszSeed) pszSeed = pszSeedStart;
    }
    return cSize > 16 ? 16 : cSize;
}

IMPL_GUID_DECODE(RecodeSymbol)(
    void *pvArg, const DWORD *pSource, char *pszResult)
{
    DWORD cCount, dwValue, dwOffset;

    cCount = dwOffset = 0;

    while (TRUE) {
        dwValue = (*pSource >> dwOffset) & 0x3f;
        dwOffset += 6;
        if (dwOffset >= 32) {
            if (2 == cCount) break;
            cCount++;
            pSource++;
            dwOffset -= 32;
            dwValue |= (*pSource << (6 - dwOffset)) & 0x3f;
        }
        *pszResult++ = (char)DecodeCSymbol(dwValue);
    }
    *pszResult = 0;
    return 0;
}
