//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdio.h>
#include <assert.h>

#include "clsbase.h"

int GetOriginalType(const CLSModule *pModule,
    const TypeDescriptor *pSrc, TypeDescriptor *pDest)
{
    pDest->nPointer = pSrc->nPointer;
    pDest->bUnsigned = pSrc->bUnsigned;

    while (pSrc->type == Type_alias) {
        pSrc = &pModule->ppAliasDir[pSrc->sIndex]->type;
        pDest->nPointer += pSrc->nPointer;
        pDest->bUnsigned |= pSrc->bUnsigned;
    }

    pDest->type = pSrc->type;
    pDest->sIndex = pSrc->sIndex;
    pDest->pNestedType = pSrc->pNestedType;

    _ReturnOK (CLS_NoError);
}

int GetArrayBaseType(const CLSModule *pModule,
    const TypeDescriptor *pSrc, TypeDescriptor *pDest)
{
    TypeDescriptor *pType = (TypeDescriptor *)pSrc;

    while (Type_Array == pType->type) {
        pType = &pModule->ppArrayDir[pType->sIndex]->type;
    }

    memcpy(pDest, pType, sizeof(TypeDescriptor));

    _ReturnOK (CLS_NoError);
}

BOOL IsEqualType(const CLSModule *pModule,
    const TypeDescriptor *pSrc, const TypeDescriptor *pDest)
{
    TypeDescriptor src, dest;

    GetOriginalType(pModule, pSrc, &src);
    GetOriginalType(pModule, pDest, &dest);

    _Return (!memcmp(&src, &dest, sizeof(src)));
}
