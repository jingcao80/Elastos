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

#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <elastos.h>
#include <clsinfo.h>
#include <cutils/log.h>

_ELASTOS_NAMESPACE_USING

void RelocateUnalignedPtr(void* ptr, size_t offset);

void RelocateModuleInfo(
    /* [in] */ CIModuleInfo* srcModInfo,
    /* [out] */ CIModuleInfo* destModInfo);

static CIModuleInfoNode* sModuleInfoList = NULL;

static pthread_mutex_t sModuleInfoLock;

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
int _DumpGUID(REIID riid)
{
    ALOGD("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.mData1, riid.mData2, riid.mData3,
        riid.mData4[0], riid.mData4[1], riid.mData4[2],
        riid.mData4[3], riid.mData4[4], riid.mData4[5],
        riid.mData4[6], riid.mData4[7]);
    return printf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.mData1, riid.mData2, riid.mData3,
        riid.mData4[0], riid.mData4[1], riid.mData4[2],
        riid.mData4[3], riid.mData4[4], riid.mData4[5],
        riid.mData4[6], riid.mData4[7]);
}

int _DumpCLSID(RClassID rclsid)
{
    _DumpGUID((REIID)rclsid);
    ALOGD("%s\n", rclsid.mUunm);
    return printf("%s\n", rclsid.mUunm);
}
#endif // _DEBUG || _MARSHAL_DEBUG

ECode InitMIL()
{
    pthread_mutexattr_t recursiveAttr;
    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&sModuleInfoLock, &recursiveAttr)) {
        return E_FAIL;
    }
    pthread_mutexattr_destroy(&recursiveAttr);

    return NOERROR;
}

void UninitMIL()
{
    pthread_mutex_destroy(&sModuleInfoLock);
}

ECode RegisterModuleInfo(
    /* [in] */ CIModuleInfo* srcModuleInfo)
{
    CIModuleInfoNode* curNode;

    assert (NULL != srcModuleInfo);

    // TODO: compare component uuid, need car support
    // prevent from registering same ClassInfo again
    //
    pthread_mutex_lock(&sModuleInfoLock);
    if (srcModuleInfo->mClassNum != 0) {
        CIClassInfo* classInfo = (CIClassInfo *)((UInt32)srcModuleInfo->mClasses \
                + (UInt32)srcModuleInfo);
        curNode = sModuleInfoList;
        while (curNode != NULL) {
            if (curNode->mModInfo->mClassNum != 0 \
                && classInfo[0].mCLSID == \
                curNode->mModInfo->mClasses[0].mCLSID) {
                break;
            }
            curNode = curNode->mNext;
        }
    }
    else if (srcModuleInfo->mInterfaceNum != 0) {
        CIInterfaceInfo* interfaceInfo = (CIInterfaceInfo *)((UInt32)srcModuleInfo->mInterfaces \
                + (UInt32)srcModuleInfo);
        curNode = sModuleInfoList;
        while (curNode != NULL) {
            if (curNode->mModInfo->mInterfaceNum != 0 \
                && interfaceInfo[0].mIID == \
                curNode->mModInfo->mInterfaces[0].mIID) {
                break;
            }
            curNode = curNode->mNext;
        }
    }
    else {
        pthread_mutex_unlock(&sModuleInfoLock);
        // No class info need to register
        return NOERROR;
    }

    if (curNode != NULL) {
        pthread_mutex_unlock(&sModuleInfoLock);
        return NOERROR;
    }

    CIModuleInfo* moduleInfo = (CIModuleInfo *)malloc(srcModuleInfo->mTotalSize);
    if (NULL == moduleInfo) {
        pthread_mutex_unlock(&sModuleInfoLock);
        return E_OUT_OF_MEMORY;
    }
    RelocateModuleInfo(srcModuleInfo, moduleInfo);

    CIModuleInfoNode* newNode = (CIModuleInfoNode *)malloc(sizeof(CIModuleInfoNode));
    if (newNode == NULL) {
        free(moduleInfo);
        pthread_mutex_unlock(&sModuleInfoLock);
        return E_OUT_OF_MEMORY;
    }
    newNode->mModInfo = (CIModuleInfo *)moduleInfo;
    newNode->mNext = NULL;
    if (sModuleInfoList == NULL) {
        sModuleInfoList = newNode;
    }
    else {
        newNode->mNext = sModuleInfoList->mNext;
        sModuleInfoList->mNext = newNode;
    }
    pthread_mutex_unlock(&sModuleInfoLock);

    return NOERROR;
}

ECode UnregisterModuleInfo(
    /* [in] */ CIModuleInfo* moduleInfo)
{
    assert(moduleInfo != NULL);

    pthread_mutex_lock(&sModuleInfoLock);
    CIModuleInfoNode* preNode = NULL;
    CIModuleInfoNode* curNode = sModuleInfoList;
    while (curNode != NULL) {
        if ((*curNode).mModInfo == moduleInfo) {
            break;
        }
        preNode = curNode;
        curNode = curNode->mNext;
    }

    if (curNode != NULL) {
        if (preNode == NULL) {
            sModuleInfoList = curNode->mNext;
        }
        else {
            preNode->mNext = curNode->mNext;
        }
        pthread_mutex_unlock(&sModuleInfoLock);
        free(curNode);

        return NOERROR;
    }

    pthread_mutex_unlock(&sModuleInfoLock);
    return E_DOES_NOT_EXIST;
}

ELAPI ECO_PUBLIC LookupInterfaceInfo(
    /* [in] */ REMuid ritfid,
    /* [out] */ CIInterfaceInfo** interfaceInfo)
{
    assert(interfaceInfo != NULL);

    pthread_mutex_lock(&sModuleInfoLock);
    CIModuleInfoNode* curNode = sModuleInfoList;
    while (curNode != NULL) {
        CIModuleInfo* modInfo = curNode->mModInfo;
        for (Int32 m = 0; m < modInfo->mInterfaceNum; m++) {
            if (modInfo->mInterfaces[m].mIID == ritfid) {
                *interfaceInfo = &(modInfo->mInterfaces[m]);
                pthread_mutex_unlock(&sModuleInfoLock);
                return NOERROR;
            }
        }
        curNode = curNode->mNext;
    }
    pthread_mutex_unlock(&sModuleInfoLock);

    return E_DOES_NOT_EXIST;
}

ECode LookupClassInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIClassInfo** classInfo)
{
    assert(classInfo != NULL);

    pthread_mutex_lock(&sModuleInfoLock);
    CIModuleInfoNode* curNode = sModuleInfoList;
    while (curNode != NULL) {
        CIModuleInfo* modInfo = curNode->mModInfo;
        for (Int32 m = 0; m < modInfo->mClassNum; m++) {
            if (modInfo->mClasses[m].mCLSID == rclsid) {
                *classInfo = &(modInfo->mClasses[m]);
                pthread_mutex_unlock(&sModuleInfoLock);
                return NOERROR;
            }
        }
        curNode = curNode->mNext;
    }
    pthread_mutex_unlock(&sModuleInfoLock);

    return E_DOES_NOT_EXIST;
}

ECode LookupModuleInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIModuleInfo** moduleInfo)
{
    assert(moduleInfo != NULL);

    pthread_mutex_lock(&sModuleInfoLock);
    CIModuleInfoNode* curNode = sModuleInfoList;
    while (curNode != NULL) {
        CIModuleInfo* modInfo = curNode->mModInfo;
        for (Int32 m = 0; m < modInfo->mClassNum; m++) {
            if (modInfo->mClasses[m].mCLSID == rclsid) {
                *moduleInfo = modInfo;
                pthread_mutex_unlock(&sModuleInfoLock);
                return NOERROR;
            }
        }
        curNode = curNode->mNext;
    }
    pthread_mutex_unlock(&sModuleInfoLock);

    return E_DOES_NOT_EXIST;
}

// TODO:
void *GetUnalignedPtr(void* ptr)
{
#ifdef _x86
    return *(void **)ptr;
#else
    union
    {
        void    *p;
        byte_t  bytes[4];
    } u;
    u.bytes[0] = ((byte_t *)ptr)[0];
    u.bytes[1] = ((byte_t *)ptr)[1];
    u.bytes[2] = ((byte_t *)ptr)[2];
    u.bytes[3] = ((byte_t *)ptr)[3];

    return u.p;
#endif
}

ELAPI ECO_PUBLIC RegisterModuleInfo(
    /* [in] */ const String& moduleName)
{
    typedef ECode (STDCALL *CarDllGetClassObject_t)(
            REMuid clsid, REIID riid, IInterface** clsObj);

    char path[260];
    strcpy(path, moduleName.string());
#ifdef _DEBUG
    void* module = dlopen(path, RTLD_NOW);
#else
    void* module = dlopen(path, RTLD_LAZY);
#endif

    strcpy(path, "DllGetClassObject");
    CarDllGetClassObject_t func = (CarDllGetClassObject_t)dlsym(module, path);

    CIModuleInfo* modInfo;
    func(ECLSID_ClassInfo, EIID_IInterface, (IInterface**)&modInfo);

    ECode ec = RegisterModuleInfo(modInfo);
    if (FAILED(ec)) {
#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
        ALOGD("Failed to RegisterModuleInfo in %s", moduleName.string());
#endif
    }
    return ec;
}

ECode AcquireClassInfo(
    /* [in] */ const ClassID& classId,
    /* [out] */ CIClassInfo** clsInfo)
{
    typedef ECode (STDCALL *CarDllGetClassObject_t)(
            REMuid clsid, REIID riid, IInterface** clsObj);

    if (clsInfo == NULL) return E_INVALID_ARGUMENT;

    char path[260];
    strcpy(path, classId.mUunm);
#ifdef _DEBUG
    void* module = dlopen(path, RTLD_NOW);
#else
    void* module = dlopen(path, RTLD_LAZY);
#endif

    strcpy(path, "DllGetClassObject");
    CarDllGetClassObject_t func = (CarDllGetClassObject_t)dlsym(module, path);

    CIModuleInfo* modInfo;
    func(ECLSID_ClassInfo, EIID_IInterface, (IInterface**)&modInfo);

    ECode ec = RegisterModuleInfo(modInfo);
    if (FAILED(ec)) {
#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
        _DumpCLSID(classId);
        ALOGD("Failed to RegisterModuleInfo in %s", classId.mUunm);
#endif
        return ec;
    }

    ec = LookupModuleInfo(classId.mClsid, &modInfo);
    if (FAILED(ec)) {
#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
        _DumpCLSID(classId);
        ALOGD("Failed to LookupModuleInfo in %s", classId.mUunm);
#endif
        return ec;
    }

    for (Int32 m = 0; m < modInfo->mClassNum; m++) {
        if (modInfo->mClasses[m].mCLSID == classId.mClsid) {
            *clsInfo = &(modInfo->mClasses[m]);
            return NOERROR;
        }
    }

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
    _DumpCLSID(classId);
    ALOGD("Failed to AcquireClassInfo in %s", classId.mUunm);
#endif
    return E_DOES_NOT_EXIST;
}
