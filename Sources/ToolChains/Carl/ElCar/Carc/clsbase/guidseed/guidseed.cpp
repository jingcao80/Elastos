//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "guidseed.h"

extern const DictWord c_sysDict[c_cMaxDictWords];
extern const DictWord c_inscapeDict[c_cMaxDictWords];

DECL_GUID_ENCODER(RecodeSymbol);
DECL_GUID_ENCODER(StaticDict);

static const GuidEncoder c_encoders[] = {
    GUID_ENCODER_ENTRY(&RecodeSymbol, NULL),
    GUID_ENCODER_ENTRY(&StaticDict, c_sysDict),
    GUID_ENCODER_ENTRY(&StaticDict, c_inscapeDict),
};
static const int c_cEncoders = sizeof(c_encoders) / sizeof(GuidEncoder);

WORD GenerateCheckSum(const WORD *pData, int nSize)
{
    DWORD dwSum = 0;
    WORD wSize = nSize / sizeof(WORD);

    while (wSize > 0) {
        dwSum += *pData;
        pData++;
        wSize--;
    }
    if (nSize & 1) dwSum += (WORD)*(BYTE *)pData << 8;

    dwSum = (dwSum >> 16) + (dwSum & 0xffff);
    dwSum += (dwSum >> 16);

    wSize = (WORD)(dwSum & 0xffff);
    return ~wSize;
}

WORD GenerateGuidCheckSum(REFGUID guid)
{
    WORD wCheckSum;

    wCheckSum = GenerateCheckSum((WORD *)((BYTE *)&guid + 1), 15);
    while (0 != (wCheckSum & 0xffe0)) {
        wCheckSum = (wCheckSum >> 5) + (wCheckSum & 0x1f);
    }
    return wCheckSum;
}

int GuidFromSeedString(const char *pszSeed, GUID *pGuid)
{
    char szLongSeed[MAX_SEED_SIZE + 1];
    BYTE *p;
    DWORD result[3];
    int n, nEncoder, cEncoded, cPrevEncoded;

    n = strlen(pszSeed);
    if (n > MAX_SEED_SIZE) {
        memcpy(szLongSeed, pszSeed, MAX_SEED_SIZE);
        szLongSeed[MAX_SEED_SIZE] = 0;
        pszSeed = szLongSeed;
        n = MAX_SEED_SIZE;
    }
    p = (BYTE *)pGuid;
    p[1] = n;
    nEncoder = 0;
    cPrevEncoded = 0;

    for (n = 0; n < c_cEncoders; n++) {
        memset(result, 0, 12);
        cEncoded = (*c_encoders[n].fnEncode)(
                        c_encoders[n].pvArg, pszSeed, result);

        assert(cEncoded <= p[1] && "Guid Encoding Size Error");
        if (cEncoded > cPrevEncoded) {
            memcpy((BYTE *)pGuid + 4, result, 12);
            nEncoder = n;
            cPrevEncoded = cEncoded;
        }
        if (cEncoded == p[1]) break;
    }
    *(WORD *)&p[2] = GenerateCheckSum((WORD *)pszSeed, p[1]);

    p[0] = (BYTE)(nEncoder << 5);
    p[0] |= (BYTE)GenerateGuidCheckSum(*pGuid);

    return cPrevEncoded;
}

int SeedStringFromGuid(REFGUID guid, char *pszSeed)
{
    BYTE *p;
    BYTE checkSum;

    p = (BYTE *)&guid;
    checkSum = (BYTE)GenerateGuidCheckSum(guid);
    if (checkSum != (p[0] & 0x1f)) return -1;

    (*c_encoders[p[0] >> 5].fnDecode)(
                c_encoders[p[0] >> 5].pvArg, (DWORD *)&p[4], pszSeed);
    if (p[1] < strlen(pszSeed)) pszSeed[p[1]] = 0;
    return 0;
}

#ifdef _DEBUG
void GuidPrint(REFGUID guid)
{
    printf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
            guid.mData1, guid.mData2, guid.mData3,
            guid.mData4[0], guid.mData4[1], guid.mData4[2], guid.mData4[3],
            guid.mData4[4], guid.mData4[5], guid.mData4[6], guid.mData4[7]
            );
}
#endif // _DEBUG
