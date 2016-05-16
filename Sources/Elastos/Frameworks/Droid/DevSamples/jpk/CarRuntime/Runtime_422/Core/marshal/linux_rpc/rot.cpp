
#include <pthread.h>
#include "rot.h"

namespace Elastos {
namespace RPC {

pthread_mutex_t g_importTableLock;
pthread_mutex_t g_exportTableLock;
pthread_mutex_t g_marshalLock;

#define ROT_HASHTABLE_SIZE 24

static ImportObject s_hashImportObjects[ROT_HASHTABLE_SIZE];
static ExportObject s_hashExportObjects[ROT_HASHTABLE_SIZE];

static UInt32 Hash(const String& key)
{
    assert(!key.IsNull());

    const char* s = key.string();
    UInt32 h = 0;
    if (s != NULL) {
        for ( ; *s; ++s) {
            h = 5 * h + *s;
        }
    }
    return h % ROT_HASHTABLE_SIZE;
}

ECode InitROT_RPC()
{
    ECode ec = NOERROR;

    for (int n = 0; n < ROT_HASHTABLE_SIZE; n++) {
        s_hashImportObjects[n].m_pNext = NULL;
        s_hashExportObjects[n].m_pNext = NULL;
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
    /* [in] */ IInterface *pObject,
    /* [in] */ IStub *pIStub)
{
    ExportObject* pObjNode;

    pObject = pObject->Probe(EIID_IInterface);
    if (!pObject) return E_NO_INTERFACE;

    pObjNode = new ExportObject;
    if (NULL == pObjNode) return E_OUT_OF_MEMORY;
    pObjNode->m_sNetAddress = netAddress;
    pObjNode->m_pObject = pObject;
    pObjNode->m_pIStub = pIStub;

    pthread_mutex_lock(&g_exportTableLock);
    s_hashExportObjects[Hash(netAddress)].InsertFirst(pObjNode);
    pthread_mutex_unlock(&g_exportTableLock);
    return NOERROR;
}

ECode FindExportObject(
    /* [in] */ IInterface *pObject,
    /* [out] */ ExportObject *pExport)
{
    pObject = pObject->Probe(EIID_IInterface);
    if (!pObject) return E_NO_INTERFACE;

    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* pIter;
    for (int n = 0; n < ROT_HASHTABLE_SIZE; n++) {
        pIter = &s_hashExportObjects[n];
        for (; NULL != pIter; pIter = (ExportObject*)(pIter->Next())) {
            if (pIter->m_pObject == pObject/* && pIter->m_pBinder->getWeakRefs()->attemptIncStrong(NULL)*/) {
                pExport->m_sNetAddress = pIter->m_sNetAddress;
                pExport->m_pObject = pIter->m_pObject;
                pExport->m_pIStub = pIter->m_pIStub;
                pExport->m_pIStub->AddRef();
                pthread_mutex_unlock(&g_exportTableLock);
                return NOERROR;
            }
        }
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_SERVER;
}

ECode FindExportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ExportObject *pExport)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* pHead = s_hashExportObjects + Hash(netAddress);

    for (ExportObject* pIter = pHead; NULL != pIter; \
        pIter = (ExportObject*)(pIter->Next())) {
        if (pIter->m_sNetAddress.Equals(netAddress)/* && pIter->m_pBinder->getWeakRefs()->attemptIncStrong(NULL)*/) {
            pExport->m_sNetAddress = pIter->m_sNetAddress;
            pExport->m_pObject = pIter->m_pObject;
            pExport->m_pIStub = pIter->m_pIStub;
            pExport->m_pIStub->AddRef();
            pthread_mutex_unlock(&g_exportTableLock);
            return NOERROR;
        }
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_SERVER;
}

ECode UnregisterExportObject(
    /* [in] */ const String& netAddress)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* pHead = s_hashExportObjects + Hash(netAddress);
    ExportObject* pPrev = NULL;

    for (ExportObject* pIter = pHead; NULL != pIter; \
        pIter = (ExportObject*)(pIter->Next())) {
        if (pIter->m_sNetAddress.Equals(netAddress)) {
            // if ((Int32)(((CObjectStub*)(pIter->m_pIStub))->getStrongCount()) != 0) {
            //     pthread_mutex_unlock(&g_exportTableLock);
            //     return S_FALSE;
            // }

            pIter->m_pObject->Release();

            if (pIter != pHead) {
                pIter->Detach(pPrev);
                delete pIter;
            }
            pthread_mutex_unlock(&g_exportTableLock);
            return NOERROR;
        }
        pPrev = pIter;
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_SERVER;
}

ECode RegisterImportObject(
    /* [in] */ const String& netAddress,
    /* [in] */ IProxy *pIProxy)
{
    ImportObject * pObjNode;

    pObjNode = new ImportObject;
    if (NULL == pObjNode) {
        return E_OUT_OF_MEMORY;
    }
    pObjNode->m_sNetAddress = netAddress;
    pObjNode->m_pIProxy = pIProxy;
    pthread_mutex_lock(&g_importTableLock);
    s_hashImportObjects[Hash(netAddress)].InsertFirst(pObjNode);
    pthread_mutex_unlock(&g_importTableLock);

    return NOERROR;
}

ECode FindImportObject(
    /* [in] */ const String& netAddress,
    /* [out] */ ImportObject *pImport)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* pHead = s_hashImportObjects + Hash(netAddress);

    for (ImportObject* pIter = pHead; NULL != pIter; \
        pIter = (ImportObject*)(pIter->Next())) {
        if (pIter->m_sNetAddress.Equals(netAddress)) {
            pImport->m_sNetAddress = pIter->m_sNetAddress;
            pImport->m_pIProxy = pIter->m_pIProxy;
            pImport->m_pIProxy->AddRef();
            pthread_mutex_unlock(&g_importTableLock);
            return NOERROR;
        }
    }

    pthread_mutex_unlock(&g_importTableLock);
    return E_NO_IMPORT_SERVER;
}

// BUGBUG: must atomic operation with delete proxy
ECode UnregisterImportObject(
    /* [in] */ const String& netAddress)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* pHead = s_hashImportObjects + Hash(netAddress);
    ImportObject* pPrev = NULL;

    for (ImportObject* pIter = pHead; NULL != pIter; \
        pIter = (ImportObject*)(pIter->Next())) {
        if (pIter->m_sNetAddress.Equals(netAddress)) {
            if ((Int32)(((Elastos::RPC::CObjectProxy *)(pIter->m_pIProxy))->m_cRef) != 0) {
                pthread_mutex_unlock(&g_importTableLock);
                return S_FALSE;
            }

            if (pIter != pHead) {
                pIter->Detach(pPrev);
                delete pIter;
            }

            pthread_mutex_unlock(&g_importTableLock);
            return NOERROR;
        }
        pPrev = pIter;
    }

    pthread_mutex_unlock(&g_importTableLock);
    return E_NO_IMPORT_SERVER;
}

} // namespace RPC
} // namespace Elastos
