
#include <pthread.h>
#include <rot.h>

namespace Elastos {
namespace IPC {

pthread_mutex_t g_importTableLock;
pthread_mutex_t g_exportTableLock;
pthread_mutex_t g_marshalLock;

#define ROT_HASHTABLE_SIZE 24

static ImportObject s_hashImportObjects[ROT_HASHTABLE_SIZE];
static ExportObject s_hashExportObjects[ROT_HASHTABLE_SIZE];

static UInt32 Hash(android::IBinder* key)
{
    UInt32 mask = 0x01;
    UInt32 val = (UInt32)key;
    UInt32 res = 0;

    while(val > 0) {
        res += val & mask;
        val = val >> 1;
    }

    return res % ROT_HASHTABLE_SIZE;
}

ECode InitROT()
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

void UninitROT()
{
    pthread_mutex_destroy(&g_importTableLock);
    pthread_mutex_destroy(&g_exportTableLock);
    pthread_mutex_destroy(&g_marshalLock);
}

ECode RegisterExportObject(
    /* [in] */ android::IBinder* binder,
    /* [in] */ IInterface* object,
    /* [in] */ IStub* stub)
{
    object = object->Probe(EIID_IInterface);
    if (!object) return E_NO_INTERFACE;

    ExportObject* expObj = new ExportObject;
    if (NULL == expObj) return E_OUT_OF_MEMORY;
    expObj->mBinder = binder;
    expObj->mObject = object;
    expObj->mIStub = stub;

    pthread_mutex_lock(&g_exportTableLock);
    s_hashExportObjects[Hash(binder)].InsertFirst(expObj);
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
            if (it->mObject == object && it->mBinder->getWeakRefs()->attemptIncStrong(NULL)) {
                memcpy(expObj, it, sizeof(ExportObject));
                pthread_mutex_unlock(&g_exportTableLock);
                return NOERROR;
            }
        }
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_OBJECT;
}

ECode FindExportObject(
    /* [in] */ android::IBinder* binder,
    /* [out] */ ExportObject* expObj)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* head = s_hashExportObjects + Hash(binder);

    for (ExportObject* it = head; NULL != it; it = (ExportObject*)(it->Next())) {
        if (it->mBinder == binder && it->mBinder->getWeakRefs()->attemptIncStrong(NULL)) {
            memcpy(expObj, it, sizeof(ExportObject));
            pthread_mutex_unlock(&g_exportTableLock);
            return NOERROR;
        }
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_OBJECT;
}

ECode UnregisterExportObject(
    /* [in] */ android::IBinder* binder)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* head = s_hashExportObjects + Hash(binder);
    ExportObject* prev = NULL;

    for (ExportObject* it = head; NULL != it; it = (ExportObject*)(it->Next())) {
        if (it->mBinder == binder) {
            if ((Int32)(((CObjectStub*)(it->mIStub))->getStrongCount()) != 0) {
                pthread_mutex_unlock(&g_exportTableLock);
                return S_FALSE;
            }

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
    /* [in] */ android::IBinder* binder,
    /* [in] */ IProxy* proxy)
{
    ImportObject* impObj = new ImportObject;
    if (NULL == impObj) {
        return E_OUT_OF_MEMORY;
    }
    impObj->mBinder = binder;
    impObj->mIProxy = proxy;
    pthread_mutex_lock(&g_importTableLock);
    s_hashImportObjects[Hash(binder)].InsertFirst(impObj);
    pthread_mutex_unlock(&g_importTableLock);

    return NOERROR;
}

ECode FindImportObject(
    /* [in] */ android::IBinder* binder,
    /* [out] */ ImportObject* impObj)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* head = s_hashImportObjects + Hash(binder);

    for (ImportObject* it = head; NULL != it; it = (ImportObject*)(it->Next())) {
        if (it->mBinder == binder) {
            memcpy(impObj, it, sizeof(ImportObject));
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
    /* [in] */ android::IBinder* binder)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* head = s_hashImportObjects + Hash(binder);
    ImportObject* prev = NULL;

    for (ImportObject* it = head; NULL != it; it = (ImportObject*)(it->Next())) {
        if (it->mBinder == binder) {
            if ((Int32)(((CObjectProxy *)(it->mIProxy))->mRef) != 0) {
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

} // namespace IPC
} // namespace Elastos
