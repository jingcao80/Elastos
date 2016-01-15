//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#include <elatypes.h>
#include <string.h>
#include <clsinfo.h>

_ELASTOS_NAMESPACE_USING

CAR_INLINE void RelocateUnalignedPtr(
    /* [in] */ void *ptr,
    /* [in] */ size_t offset)
{
#ifdef _x86
    *(size_t *)ptr += offset;
#else
    union
    {
        size_t  s;
        byte_t  bytes[4];
    } u;
    u.bytes[0] = ((byte_t *)ptr)[0];
    u.bytes[1] = ((byte_t *)ptr)[1];
    u.bytes[2] = ((byte_t *)ptr)[2];
    u.bytes[3] = ((byte_t *)ptr)[3];

    u.s += offset;

    ((byte_t *)ptr)[0] = u.bytes[0];
    ((byte_t *)ptr)[1] = u.bytes[1];
    ((byte_t *)ptr)[2] = u.bytes[2];
    ((byte_t *)ptr)[3] = u.bytes[3];
#endif
}

void RelocateModuleInfo(
    /* [in] */ CIModuleInfo *srcModInfo,
    /* [out] */ CIModuleInfo *destModInfo)
{
    CIClassInfo * classInfo;
    CIInterfaceInfo ** interfaceInfos;
    CIInterfaceInfo * interfaceInfo;
    CIMethodInfo * methodInfo;

    memcpy(destModInfo, srcModInfo, srcModInfo->mTotalSize);

    destModInfo->mInterfaces = (CIInterfaceInfo*)((Byte *)destModInfo +
        (UInt32)destModInfo->mInterfaces);
    interfaceInfo = destModInfo->mInterfaces;
    for (Int32 i = 0; i < destModInfo->mInterfaceNum; i++) {
        interfaceInfo[i].mMethods = (CIMethodInfo *) \
              ((Byte *)destModInfo + (UInt32)interfaceInfo[i].mMethods);
        methodInfo = interfaceInfo[i].mMethods;
        for (Int32 j = 0; j < interfaceInfo[i].mMethodNumMinus4; j++) {
            methodInfo[j].mParams = (CIBaseType*) \
                 ((Byte *)destModInfo + (UInt32)methodInfo[j].mParams);
        }
    }

    destModInfo->mClasses = (CIClassInfo *)((Byte *)destModInfo +
       (UInt32)destModInfo->mClasses);
    classInfo = destModInfo->mClasses;
    for (Int32 i = 0; i < destModInfo->mClassNum; i++) {
        classInfo[i].mInterfaces = (CIInterfaceInfo **) \
                ((Byte *)destModInfo + (Int32)(classInfo[i].mInterfaces));
        interfaceInfos = classInfo[i].mInterfaces;
        for (Int32 j = 0; j < classInfo[i].mInterfaceNum; j++) {
            RelocateUnalignedPtr(interfaceInfos + j, (size_t)destModInfo);
        }
        classInfo[i].mUunm =
                (char*)((Byte *)destModInfo + (UInt32)classInfo[i].mUunm);
    }
}

CAR_INLINE void  FlatUnalignedPtr(
    /* [in] */ void *ptr,
    /* [in] */ size_t offset)
{
#ifdef _x86
    *(size_t *)ptr -= offset;
#elif defined(_arm)
    union
    {
        size_t  s;
        byte_t  bytes[4];
    } u;
    u.bytes[0] = ((byte_t *)ptr)[0];
    u.bytes[1] = ((byte_t *)ptr)[1];
    u.bytes[2] = ((byte_t *)ptr)[2];
    u.bytes[3] = ((byte_t *)ptr)[3];

    u.s -= offset;

    ((byte_t *)ptr)[0] = u.bytes[0];
    ((byte_t *)ptr)[1] = u.bytes[1];
    ((byte_t *)ptr)[2] = u.bytes[2];
    ((byte_t *)ptr)[3] = u.bytes[3];
#else
#error unsupported architecure.
#endif
}

void FlatModuleInfo(
    /* [in] */ CIModuleInfo *srcModInfo,
    /* [out] */ CIModuleInfo *destModInfo)
{
    Int32 i, j;
    CIClassInfo * classInfo;
    CIInterfaceInfo ** interfaceInfos;
    CIInterfaceInfo * interfaceInfo;
    CIMethodInfo * methodInfo;

    memcpy(destModInfo, srcModInfo, srcModInfo->mTotalSize);
    destModInfo->mInterfaces = (CIInterfaceInfo*) \
            ((Byte*)destModInfo->mInterfaces - (UInt32)srcModInfo);
    interfaceInfo = (CIInterfaceInfo*)((Byte*) destModInfo->mInterfaces + \
                      (UInt32)destModInfo);

    for (i = 0; i < destModInfo->mInterfaceNum; i++) {
        interfaceInfo[i].mMethods = (CIMethodInfo *) \
              ((Byte*)interfaceInfo[i].mMethods - (UInt32)srcModInfo);

        methodInfo = (CIMethodInfo*)((Byte*)interfaceInfo[i].mMethods + \
                            (UInt32)destModInfo);
        for (j = 0; j < interfaceInfo[i].mMethodNumMinus4; j++) {
            methodInfo[j].mParams = (CIBaseType*) \
                 ((Byte*)methodInfo[j].mParams - (UInt32)srcModInfo);
        }
    }

    destModInfo->mClasses = (CIClassInfo*) \
            ((Byte*)destModInfo->mClasses - (UInt32)srcModInfo);

    classInfo = (CIClassInfo*)((Byte*)destModInfo->mClasses + (UInt32)destModInfo);
    for (i = 0; i < destModInfo->mClassNum; i++) {
        classInfo[i].mInterfaces = (CIInterfaceInfo**) \
                ((Byte*)classInfo[i].mInterfaces - (UInt32)srcModInfo);

        interfaceInfos = (CIInterfaceInfo**)((Byte*)classInfo[i].mInterfaces + \
                (UInt32)destModInfo);
        for (j = 0; j < classInfo[i].mInterfaceNum; j++) {
            FlatUnalignedPtr(interfaceInfos + j, (size_t)srcModInfo);
        }
        classInfo[i].mUunm =  \
            (char*)((Byte*)classInfo[i].mUunm - (UInt32)srcModInfo);
    }
}
