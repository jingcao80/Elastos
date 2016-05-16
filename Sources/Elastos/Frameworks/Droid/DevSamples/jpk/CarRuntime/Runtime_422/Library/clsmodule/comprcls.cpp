//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <malloc.h>

#include <clsbase.h>


int CompressCLS(void *pvDest)
{
    int nDataSize;
    CLSModule *pSrc, *pDest;

    pDest = (CLSModule *)pvDest;
    pSrc = (CLSModule *)alloca(pDest->nSize);
    if (!pSrc) _ReturnError (CLSError_OutOfMemory);

    memcpy(pSrc, pDest, pDest->nSize);
    nDataSize = pSrc->nSize - sizeof(CLSModule);

    if (compress(
        (BYTE *)pDest + sizeof(CLSModule),
        (uLongf *)&nDataSize,
        (BYTE *)pSrc + sizeof(CLSModule),
        (uLongf)nDataSize) != Z_OK) {
        _ReturnError (CLSError_Compress);
    }

    _ReturnOK (nDataSize + sizeof(CLSModule));
}

int UncompressCLS(const void *pvSrc, int nSize, CLSModule *pDest)
{
    int nDataSize;
    CLSModule *pSrc;

    pSrc = (CLSModule *)pvSrc;
    nDataSize = pSrc->nSize - sizeof(CLSModule);

    if (uncompress(
        (BYTE *)pDest + sizeof(CLSModule),
        (uLongf *)&nDataSize,
        (BYTE *)pSrc + sizeof(CLSModule),
        (uLongf)nSize - sizeof(CLSModule)) != Z_OK) {
        _ReturnError (CLSError_Uncompress);
    }

    memcpy(pDest, pSrc, sizeof(CLSModule));
    _ReturnOK (nDataSize + sizeof(CLSModule));
}
