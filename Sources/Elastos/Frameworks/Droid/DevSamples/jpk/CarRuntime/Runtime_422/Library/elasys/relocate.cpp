//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include <elatypes.h>
#include <string.h>
#include <clsinfo.h>

_ELASTOS_NAMESPACE_USING

CAR_INLINE void RelocateUnalignedPtr(void *pPtr, size_t offset)
{
#ifdef _x86
    *(size_t *)pPtr += offset;
#else
    union
    {
        size_t  s;
        byte_t  bytes[4];
    } u;
    u.bytes[0] = ((byte_t *)pPtr)[0];
    u.bytes[1] = ((byte_t *)pPtr)[1];
    u.bytes[2] = ((byte_t *)pPtr)[2];
    u.bytes[3] = ((byte_t *)pPtr)[3];

    u.s += offset;

    ((byte_t *)pPtr)[0] = u.bytes[0];
    ((byte_t *)pPtr)[1] = u.bytes[1];
    ((byte_t *)pPtr)[2] = u.bytes[2];
    ((byte_t *)pPtr)[3] = u.bytes[3];
#endif
}

void RelocateModuleInfo(
    /* [in] */ CIModuleInfo *pSrcModInfo,
    /* [out] */ CIModuleInfo *pDestModInfo)
{
    Int32 i, j;
    CIClassInfo * pClassInfo;
    CIInterfaceInfo ** ppInterfaceInfo;
    CIInterfaceInfo * pInterfaceInfo;
    CIMethodInfo * pMethodInfo;

    memcpy(pDestModInfo, pSrcModInfo, pSrcModInfo->totalSize);

    pDestModInfo->interfaces = (CIInterfaceInfo*)((Byte *)pDestModInfo +
        (UInt32)pDestModInfo->interfaces);
    pInterfaceInfo = pDestModInfo->interfaces;
    for (i = 0; i < pDestModInfo->interfaceNum; i++) {
        pInterfaceInfo[i].methods = (CIMethodInfo *) \
              ((Byte *)pDestModInfo + (UInt32)pInterfaceInfo[i].methods);
        pMethodInfo = pInterfaceInfo[i].methods;
        for (j = 0; j < pInterfaceInfo[i].methodNumMinus4; j++) {
            pMethodInfo[j].params = (CIBaseType*) \
                 ((Byte *)pDestModInfo + (UInt32)pMethodInfo[j].params);
        }
    }

    pDestModInfo->classes = (CIClassInfo *)((Byte *)pDestModInfo +
       (UInt32)pDestModInfo->classes);
    pClassInfo = pDestModInfo->classes;
    for (i = 0; i < pDestModInfo->classNum; i++) {
        pClassInfo[i].interfaces = (CIInterfaceInfo **) \
                ((Byte *)pDestModInfo + (Int32)(pClassInfo[i].interfaces));
        ppInterfaceInfo = pClassInfo[i].interfaces;
        for (j = 0; j < pClassInfo[i].interfaceNum; j++) {
            RelocateUnalignedPtr(ppInterfaceInfo + j, (size_t)pDestModInfo);
        }
        pClassInfo[i].pszUunm =
                (char*)((Byte *)pDestModInfo + (UInt32)pClassInfo[i].pszUunm);
    }
}

CAR_INLINE void  FlatUnalignedPtr(void *pPtr, size_t offset)
{
#ifdef _x86
    *(size_t *)pPtr -= offset;
#elif defined(_arm)
    union
    {
        size_t  s;
        byte_t  bytes[4];
    } u;
    u.bytes[0] = ((byte_t *)pPtr)[0];
    u.bytes[1] = ((byte_t *)pPtr)[1];
    u.bytes[2] = ((byte_t *)pPtr)[2];
    u.bytes[3] = ((byte_t *)pPtr)[3];

    u.s -= offset;

    ((byte_t *)pPtr)[0] = u.bytes[0];
    ((byte_t *)pPtr)[1] = u.bytes[1];
    ((byte_t *)pPtr)[2] = u.bytes[2];
    ((byte_t *)pPtr)[3] = u.bytes[3];
#else
#error unsupported architecure.
#endif
}

void FlatModuleInfo(
    /* [in] */ CIModuleInfo *pSrcModInfo,
    /* [out] */ CIModuleInfo *pDestModInfo)
{
    Int32 i, j;
    CIClassInfo * pClassInfo;
    CIInterfaceInfo ** ppInterfaceInfo;
    CIInterfaceInfo * pInterfaceInfo;
    CIMethodInfo * pMethodInfo;

    memcpy(pDestModInfo, pSrcModInfo, pSrcModInfo->totalSize);
    pDestModInfo->interfaces = (CIInterfaceInfo*) \
            ((Byte*)pDestModInfo->interfaces - (UInt32)pSrcModInfo);
    pInterfaceInfo = (CIInterfaceInfo*)((Byte*) pDestModInfo->interfaces + \
                      (UInt32)pDestModInfo);

    for (i = 0; i < pDestModInfo->interfaceNum; i++) {
        pInterfaceInfo[i].methods = (CIMethodInfo *) \
              ((Byte*)pInterfaceInfo[i].methods - (UInt32)pSrcModInfo);

        pMethodInfo = (CIMethodInfo*)((Byte*)pInterfaceInfo[i].methods + \
                            (UInt32)pDestModInfo);
        for (j = 0; j < pInterfaceInfo[i].methodNumMinus4; j++) {
            pMethodInfo[j].params = (CIBaseType*) \
                 ((Byte*)pMethodInfo[j].params - (UInt32)pSrcModInfo);
        }
    }

    pDestModInfo->classes = (CIClassInfo*) \
            ((Byte*)pDestModInfo->classes - (UInt32)pSrcModInfo);

    pClassInfo = (CIClassInfo*)((Byte*)pDestModInfo->classes + (UInt32)pDestModInfo);
    for (i = 0; i < pDestModInfo->classNum; i++) {
        pClassInfo[i].interfaces = (CIInterfaceInfo**) \
                ((Byte*)pClassInfo[i].interfaces - (UInt32)pSrcModInfo);

        ppInterfaceInfo = (CIInterfaceInfo**)((Byte*)pClassInfo[i].interfaces + \
                (UInt32)pDestModInfo);
        for (j = 0; j < pClassInfo[i].interfaceNum; j++) {
            FlatUnalignedPtr(ppInterfaceInfo + j, (size_t)pSrcModInfo);
        }
        pClassInfo[i].pszUunm =  \
            (char*)((Byte*)pClassInfo[i].pszUunm - (UInt32)pSrcModInfo);
    }
}
