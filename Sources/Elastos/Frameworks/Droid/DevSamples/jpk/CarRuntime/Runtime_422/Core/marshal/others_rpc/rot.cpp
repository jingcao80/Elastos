
#include <pthread.h>
#include "rot.h"

pthread_mutex_t g_importTableLock;
pthread_mutex_t g_exportTableLock;
pthread_mutex_t g_marshalLock;

#define ROT_HASHTABLE_SIZE 32

static ImportObject s_hashImportObjects[ROT_HASHTABLE_SIZE];
static ExportObject s_hashExportObjects[ROT_HASHTABLE_SIZE];

static UInt32 Hash(const char* key)
{
    UInt32 val = 0;
    UInt32 pos = 0;
    UInt32 len = strlen(key);
    
    while(pos < len) {
        val += key[pos] * pos;
        pos++; 
    }
    
    return val % ROT_HASHTABLE_SIZE;
}

ECode InitROT()
{
    ECode ec = NOERROR;

    for (int n = 0; n < ROT_HASHTABLE_SIZE; n++) {
        s_hashImportObjects[n].Initialize();
        s_hashImportObjects[n].m_pNext = NULL;
        s_hashExportObjects[n].Initialize();
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

void UninitROT()
{
    pthread_mutex_destroy(&g_importTableLock);
    pthread_mutex_destroy(&g_exportTableLock);
    pthread_mutex_destroy(&g_marshalLock);
}

ECode RegisterExportObject(
    /* [in] */ const char* connName,
    /* [in] */ IInterface *pObject,
    /* [in] */ IStub *pIStub)
{
    ExportObject* pObjNode;

    pObject = pObject->Probe(EIID_IInterface);
    if (!pObject) return E_NO_INTERFACE;

    pObjNode = new ExportObject;
    if (NULL == pObjNode) return E_OUT_OF_MEMORY;
    assert(connName != NULL && connName[0] != '\0');
    pObjNode->m_connName = (char*)malloc(strlen(connName) + 1);
    strcpy(pObjNode->m_connName, connName);
    pObjNode->m_pObject = pObject;
    pObjNode->m_pIStub = pIStub;

    pthread_mutex_lock(&g_exportTableLock);
    s_hashExportObjects[Hash(connName)].InsertFirst(pObjNode);
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
        pIter = s_hashExportObjects + n;
        for (; NULL != pIter; pIter = (ExportObject*)(pIter->Next())) {
            if (pIter->m_pObject == pObject) {
                memcpy(pExport, pIter, sizeof(ExportObject));
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
    /* [in] */ const char* connName,
    /* [out] */ ExportObject *pExport)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* pHead = s_hashExportObjects + Hash(connName);

    for (ExportObject* pIter = (ExportObject *)pHead->Next(); NULL != pIter; \
        pIter = (ExportObject*)(pIter->Next())) {
        if (!strcmp(pIter->m_connName, connName)) {
            memcpy(pExport, pIter, sizeof(ExportObject));
            pExport->m_pIStub->AddRef();
            pthread_mutex_unlock(&g_exportTableLock);
            return NOERROR;
        }
    }

    pthread_mutex_unlock(&g_exportTableLock);
    return E_NO_EXPORT_SERVER;
}

ECode UnregisterExportObject(
    /* [in] */ const char* connName)
{
    pthread_mutex_lock(&g_exportTableLock);
    ExportObject* pHead = s_hashExportObjects + Hash(connName);
    ExportObject* pPrev = pHead;

    for (ExportObject* pIter = (ExportObject *)pHead->Next(); NULL != pIter; \
        pIter = (ExportObject*)(pIter->Next())) {
        if (!strcmp(pIter->m_connName, connName)) {
            if ((Int32)(((CObjectStub*)(pIter->m_pIStub))->m_cRef) != 0) {
                pthread_mutex_unlock(&g_exportTableLock);
                return S_FALSE;
            }

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
    /* [in] */ const char* stubConnName,
    /* [in] */ IProxy *pIProxy)
{
    ImportObject * pObjNode;

    pObjNode = new ImportObject;
    if (NULL == pObjNode) {
        return E_OUT_OF_MEMORY;
    }
    assert(stubConnName != NULL && stubConnName[0] != '\0');
    pObjNode->m_stubConnName = (char*)malloc(strlen(stubConnName) + 1);
    strcpy(pObjNode->m_stubConnName, stubConnName);
    pObjNode->m_pIProxy = pIProxy;
    pthread_mutex_lock(&g_importTableLock);
    s_hashImportObjects[Hash(stubConnName)].InsertFirst(pObjNode);
    pthread_mutex_unlock(&g_importTableLock);

    return NOERROR;
}

ECode FindImportObject(
    /* [in] */ const char* stubConnName,
    /* [out] */ ImportObject *pImport)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* pHead = s_hashImportObjects + Hash(stubConnName);

    for (ImportObject* pIter = (ImportObject *)pHead->Next(); NULL != pIter; \
        pIter = (ImportObject*)(pIter->Next())) {
        if (!strcmp(pIter->m_stubConnName, stubConnName)) {
            memcpy(pImport, pIter, sizeof(ImportObject));
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
    /* [in] */ const char* stubConnName)
{
    pthread_mutex_lock(&g_importTableLock);
    ImportObject* pHead = s_hashImportObjects + Hash(stubConnName);
    ImportObject* pPrev = pHead;

    for (ImportObject* pIter = (ImportObject *)pHead->Next(); NULL != pIter; \
        pIter = (ImportObject*)(pIter->Next())) {
        if (!strcmp(pIter->m_stubConnName, stubConnName)) {
            if ((Int32)(((CObjectProxy *)(pIter->m_pIProxy))->m_cRef) != 0) {
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
