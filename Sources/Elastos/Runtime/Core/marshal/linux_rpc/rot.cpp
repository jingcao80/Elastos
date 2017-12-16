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
#include <rot.h>

namespace Elastos {
namespace RPC {

pthread_mutex_t g_importTableLock;
pthread_mutex_t g_exportTableLock;
pthread_mutex_t g_marshalLock;

#define ROT_HASHTABLE_SIZE 24

static ImportObject s_hashImportObjects[ROT_HASHTABLE_SIZE];
static ExportObject s_hashExportObjects[ROT_HASHTABLE_SIZE];

#ifdef __clang__
__attribute__((no_sanitize("integer")))
#endif
static UInt32 Hash(const String& key)
{
    assert(!key.IsNull());

    // BKDR Hash Function
    UInt32 seed = 31; // 31 131 1313 13131 131313 etc..
    UInt32 hash = 0;

    const char* s = key.string();
    if (s != NULL) {
        for ( ; *s; ++s) {
            hash = hash * seed + (*s);
        }
    }
    return hash % ROT_HASHTABLE_SIZE;
}

ECode InitROT_RPC()
{
    ECode ec = NOERROR;

    for (int n = 0; n < ROT_HASHTABLE_SIZE; n++) {
        s_hashImportObjects[n].mNext = NULL;
        s_hashExportObjects[n].mNext = NULL;
    }

    pthread_mutexattr_t recursiveAttr;
    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    if (FAILED(pthread_mutex_init(&g_importTableLock, &recursiveAttr))) {
        ec = E_FAIL;
    }
    else if (FAILED(pthread_mutex_init(&g_exportTableLock, &recursiveAttr))) {
        ec = E_FAIL;
    }
    else if (FAILED(pthread_mutex_init(&g_marshalLock, &recursiveAttr))) {
        ec = E_FAIL;
    }
    pthread_mutexattr_destroy(&recursiveAttr);

    return ec;
}

void UninitROT_RPC()
{
    pthread_mutex_destroy(&g_importTableLock);
    pthread_mutex_destroy(&g_exportTableLock);
    pthread_mutex_destroy(&g_marshalLock);
}

ECode RegisterExportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IInterface* object,
    /* [in] */ IStub* stub)
{
    object = object->Probe(EIID_IInterface);
    if (!object) return E_NO_INTERFACE;

    ExportObject* expObj = new ExportObject;
    if (NULL == expObj) return E_OUT_OF_MEMORY;
    expObj->mNetAddress = netAddress;
    expObj->mObject = object;
    expObj->mIStub = stub;

    pthread_mutex_lock(&g_exportTableLock);
    s_hashExportObjects[Hash(netAddress)].InsertFirst(expObj);
    pthread_mutex_unlock(&g_exportTableLock);
    return NOERROR;
}

ECode FindExportObject(
    /* [in] */ IInterface* object,
    /* [out] */ ExportObject* expObj)
{
    object = object->Probe(EIID_IInterface);
    if (!object) return E_NO_INTERFACE;

    pthread_mutex_lock(&g_exportTableLock);
    for (Int32 n = 0; n < ROT_HASHTABLE_SIZE; n++) {
        ExportObject* it = &s_hashExportObjects[n];
        for (; NULL != it; it = (ExportObject*)(it->Next())) {
            if (it->mObject == object/* && it->mBinder->getWeakRefs()->attemptIncStrong(NULL)*/) {
                expObj->mNetAddress = it->mNetAddress;
                expObj->mObject = it->mObject;
                expObj->mIStub = it->mIStub;
                expObj->mIStub->AddRef();
                pthread_mutex_unlock(&g_exportTableLock);
                return NOERROR;
            }
        }
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_OBJECT;
}

ECode FindExportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ExportObject* expObj)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* head = s_hashExportObjects + Hash(netAddress);

    for (ExportObject* it = head; NULL != it; \
        it = (ExportObject*)(it->Next())) {
        if (it->mNetAddress.Equals(netAddress)/* && it->mBinder->getWeakRefs()->attemptIncStrong(NULL)*/) {
            expObj->mNetAddress = it->mNetAddress;
            expObj->mObject = it->mObject;
            expObj->mIStub = it->mIStub;
            expObj->mIStub->AddRef();
            pthread_mutex_unlock(&g_exportTableLock);
            return NOERROR;
        }
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_OBJECT;
}

ECode UnregisterExportObject(
    /* [in] */ const String& netAddress)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* head = s_hashExportObjects + Hash(netAddress);
    ExportObject* prev = NULL;

    for (ExportObject* it = head; NULL != it; \
        it = (ExportObject*)(it->Next())) {
        if (it->mNetAddress.Equals(netAddress)) {
            // if ((Int32)(((CObjectStub*)(it->mIStub))->getStrongCount()) != 0) {
            //     pthread_mutex_unlock(&g_exportTableLock);
            //     return S_FALSE;
            // }

            it->mObject->Release();

            if (it != head) {
                it->Detach(prev);
                delete it;
            }
            pthread_mutex_unlock(&g_exportTableLock);
            return NOERROR;
        }
        prev = it;
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_OBJECT;
}

ECode RegisterImportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IProxy* proxy)
{
    ImportObject* impObj = new ImportObject;
    if (NULL == impObj) {
        return E_OUT_OF_MEMORY;
    }
    impObj->mNetAddress = netAddress;
    impObj->mIProxy = proxy;
    pthread_mutex_lock(&g_importTableLock);
    s_hashImportObjects[Hash(netAddress)].InsertFirst(impObj);
    pthread_mutex_unlock(&g_importTableLock);

    return NOERROR;
}

ECode FindImportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ImportObject* impObj)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* head = s_hashImportObjects + Hash(netAddress);

    for (ImportObject* it = head; NULL != it; \
        it = (ImportObject*)(it->Next())) {
        if (it->mNetAddress.Equals(netAddress)) {
            impObj->mNetAddress = it->mNetAddress;
            impObj->mIProxy = it->mIProxy;
            impObj->mIProxy->AddRef();
            pthread_mutex_unlock(&g_importTableLock);
            return NOERROR;
        }
    }

    pthread_mutex_unlock(&g_importTableLock);
    return E_NO_IMPORT_OBJECT;
}

// BUGBUG: must atomic operation with delete proxy
ECode UnregisterImportObject(
    /* [in] */ const String& netAddress)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* head = s_hashImportObjects + Hash(netAddress);
    ImportObject* prev = NULL;

    for (ImportObject* it = head; NULL != it; \
        it = (ImportObject*)(it->Next())) {
        if (it->mNetAddress.Equals(netAddress)) {
            if ((Int32)(((Elastos::RPC::CObjectProxy *)(it->mIProxy))->mRef) != 0) {
                pthread_mutex_unlock(&g_importTableLock);
                return S_FALSE;
            }

            if (it != head) {
                it->Detach(prev);
                delete it;
            }

            pthread_mutex_unlock(&g_importTableLock);
            return NOERROR;
        }
        prev = it;
    }

    pthread_mutex_unlock(&g_importTableLock);
    return E_NO_IMPORT_OBJECT;
}

} // namespace RPC
} // namespace Elastos
