//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <elatypes.h>
#include <string.h>
#include <clsinfo.h>

_ELASTOS_NAMESPACE_USING

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
        (ptrdiff_t)destModInfo->mInterfaces);
    interfaceInfo = destModInfo->mInterfaces;
    for (Int32 i = 0; i < destModInfo->mInterfaceNum; i++) {
        interfaceInfo[i].mMethods = (CIMethodInfo *) \
              ((Byte *)destModInfo + (ptrdiff_t)interfaceInfo[i].mMethods);
        methodInfo = interfaceInfo[i].mMethods;
        for (Int32 j = 0; j < interfaceInfo[i].mMethodNumMinus4; j++) {
            methodInfo[j].mParams = (CIBaseType*) \
                 ((Byte *)destModInfo + (ptrdiff_t)methodInfo[j].mParams);
        }
    }

    destModInfo->mClasses = (CIClassInfo *)((Byte *)destModInfo +
       (ptrdiff_t)destModInfo->mClasses);
    classInfo = destModInfo->mClasses;
    for (Int32 i = 0; i < destModInfo->mClassNum; i++) {
        classInfo[i].mInterfaces = (CIInterfaceInfo **) \
                ((Byte *)destModInfo + (ptrdiff_t)(classInfo[i].mInterfaces));
        interfaceInfos = classInfo[i].mInterfaces;
        for (Int32 j = 0; j < classInfo[i].mInterfaceNum; j++) {
            interfaceInfos[j] = (CIInterfaceInfo*)((Byte*)destModInfo + (ptrdiff_t)interfaceInfos[j]);
        }
        classInfo[i].mUunm =
                (char*)((Byte *)destModInfo + (ptrdiff_t)classInfo[i].mUunm);
    }
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
            ((Byte*)destModInfo->mInterfaces - (Byte*)srcModInfo);
    interfaceInfo = (CIInterfaceInfo*)((ptrdiff_t) destModInfo->mInterfaces + \
                      (Byte*)destModInfo);

    for (i = 0; i < destModInfo->mInterfaceNum; i++) {
        interfaceInfo[i].mMethods = (CIMethodInfo *) \
              ((Byte*)interfaceInfo[i].mMethods - (Byte*)srcModInfo);

        methodInfo = (CIMethodInfo*)((ptrdiff_t)interfaceInfo[i].mMethods + \
                            (Byte*)destModInfo);
        for (j = 0; j < interfaceInfo[i].mMethodNumMinus4; j++) {
            methodInfo[j].mParams = (CIBaseType*) \
                 ((Byte*)methodInfo[j].mParams - (Byte*)srcModInfo);
        }
    }

    destModInfo->mClasses = (CIClassInfo*) \
            ((Byte*)destModInfo->mClasses - (Byte*)srcModInfo);

    classInfo = (CIClassInfo*)((ptrdiff_t)destModInfo->mClasses + (Byte*)destModInfo);
    for (i = 0; i < destModInfo->mClassNum; i++) {
        classInfo[i].mInterfaces = (CIInterfaceInfo**) \
                ((Byte*)classInfo[i].mInterfaces - (Byte*)srcModInfo);

        interfaceInfos = (CIInterfaceInfo**)((ptrdiff_t)classInfo[i].mInterfaces + \
                (Byte*)destModInfo);
        for (j = 0; j < classInfo[i].mInterfaceNum; j++) {
            interfaceInfos[j] = (CIInterfaceInfo*)((Byte*)interfaceInfos[j] - (Byte*)srcModInfo);
        }
        classInfo[i].mUunm =  \
            (char*)((Byte*)classInfo[i].mUunm - (Byte*)srcModInfo);
    }
}
