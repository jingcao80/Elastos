
#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <elastos.h>
#include "clsinfo.h"
#include <cutils/log.h>

_ELASTOS_NAMESPACE_USING

void RelocateUnalignedPtr(void *pPtr, size_t offset);

void RelocateModuleInfo(
    /* [in] */ CIModuleInfo* pSrcModInfo,
    /* [out] */ CIModuleInfo* pDestModInfo);

static CIModuleInfoNode *s_pModuleInfoList = NULL;

static pthread_mutex_t s_moduleInfoLock;

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
int _DumpGUID(REIID riid)
{
    ALOGD("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2],
        riid.Data4[3], riid.Data4[4], riid.Data4[5],
        riid.Data4[6], riid.Data4[7]);
    return printf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
        riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2],
        riid.Data4[3], riid.Data4[4], riid.Data4[5],
        riid.Data4[6], riid.Data4[7]);
}

int _DumpCLSID(RClassID rclsid)
{
    _DumpGUID((REIID)rclsid);
    ALOGD("%s\n", rclsid.pUunm);
    return printf("%s\n", rclsid.pUunm);
}
#endif // _DEBUG || _MARSHAL_DEBUG

ECode InitMIL()
{
    pthread_mutexattr_t recursiveAttr;
    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&s_moduleInfoLock, &recursiveAttr)) {
        return E_FAIL;
    }
    pthread_mutexattr_destroy(&recursiveAttr);

    return NOERROR;
}

void UninitMIL()
{
    pthread_mutex_destroy(&s_moduleInfoLock);
}

ECode RegisterModuleInfo(
    /* [in] */ CIModuleInfo *pSrcModuleInfo)
{
    CIModuleInfo * pModuleInfo;
    CIModuleInfoNode *pNewNode;
    CIModuleInfoNode *pCurNode;
    CIClassInfo    *pClassInfo;
    CIInterfaceInfo *pInterfaceInfo;

    assert (NULL != pSrcModuleInfo);

    // TODO: compare component uuid, need car support
    // prevent from registering same ClassInfo again
    //
    pthread_mutex_lock(&s_moduleInfoLock);
    if (pSrcModuleInfo->classNum != 0) {
        pClassInfo = (CIClassInfo *)((UInt32)pSrcModuleInfo->classes \
                + (UInt32)pSrcModuleInfo);
        pCurNode = s_pModuleInfoList;
        while (pCurNode != NULL) {
            if (pCurNode->m_pModInfo->classNum != 0 \
                && pClassInfo[0].clsid == \
                pCurNode->m_pModInfo->classes[0].clsid) {
                break;
            }
            pCurNode = pCurNode->m_pNext;
        }
    }
    else if (pSrcModuleInfo->interfaceNum != 0) {
        pInterfaceInfo = (CIInterfaceInfo *)((UInt32)pSrcModuleInfo->interfaces \
                + (UInt32)pSrcModuleInfo);
        pCurNode = s_pModuleInfoList;
        while (pCurNode != NULL) {
            if (pCurNode->m_pModInfo->interfaceNum != 0 \
                && pInterfaceInfo[0].iid == \
                pCurNode->m_pModInfo->interfaces[0].iid) {
                break;
            }
            pCurNode = pCurNode->m_pNext;
        }
    }
    else {
        pthread_mutex_unlock(&s_moduleInfoLock);
        // No class info need to register
        return NOERROR;
    }

    if (pCurNode != NULL) {
        pthread_mutex_unlock(&s_moduleInfoLock);
        return S_ALREADY_EXISTS;
    }

    pModuleInfo = (CIModuleInfo *)malloc(pSrcModuleInfo->totalSize);
    if (NULL == pModuleInfo) {
        pthread_mutex_unlock(&s_moduleInfoLock);
        return E_OUT_OF_MEMORY;
    }
    RelocateModuleInfo(pSrcModuleInfo, pModuleInfo);

    pNewNode = (CIModuleInfoNode *)malloc(sizeof(CIModuleInfoNode));
    if (pNewNode == NULL) {
        free(pModuleInfo);
        pthread_mutex_unlock(&s_moduleInfoLock);
        return E_OUT_OF_MEMORY;
    }
    pNewNode->m_pModInfo = (CIModuleInfo *)pModuleInfo;
    pNewNode->m_pNext = NULL;
    if (s_pModuleInfoList == NULL) {
        s_pModuleInfoList = pNewNode;
    }
    else {
        pNewNode->m_pNext = s_pModuleInfoList->m_pNext;
        s_pModuleInfoList->m_pNext = pNewNode;
    }
    pthread_mutex_unlock(&s_moduleInfoLock);

    return NOERROR;
}

ECode UnregisterModuleInfo(
    /* [in] */ CIModuleInfo *pModuleInfo)
{
    CIModuleInfoNode *pCurNode, *pPreNode;

    assert(pModuleInfo != NULL);

    pthread_mutex_lock(&s_moduleInfoLock);
    pPreNode = NULL;
    pCurNode = s_pModuleInfoList;
    while (pCurNode != NULL) {
        if ((*pCurNode).m_pModInfo == pModuleInfo) {
            break;
        }
        pPreNode = pCurNode;
        pCurNode = pCurNode->m_pNext;
    }

    if (pCurNode != NULL) {
        if (pPreNode == NULL) {
            s_pModuleInfoList = pCurNode->m_pNext;
        }
        else {
            pPreNode->m_pNext = pCurNode->m_pNext;
        }
        pthread_mutex_unlock(&s_moduleInfoLock);
        free(pCurNode);

        return NOERROR;
    }

    pthread_mutex_unlock(&s_moduleInfoLock);
    return E_DOES_NOT_EXIST;
}

ECode LookupClassInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIClassInfo **ppClassInfo)
{
    CIModuleInfoNode *pCurNode;
    CIModuleInfo *pModInfo;
    int m;

    assert(ppClassInfo != NULL);

    pthread_mutex_lock(&s_moduleInfoLock);
    pCurNode = s_pModuleInfoList;
    while (pCurNode != NULL) {
        pModInfo = pCurNode->m_pModInfo;
        for (m = 0; m < pModInfo->classNum; m++) {
            if (pModInfo->classes[m].clsid == rclsid) {
                *ppClassInfo = &(pModInfo->classes[m]);
                pthread_mutex_unlock(&s_moduleInfoLock);
                return NOERROR;
            }
        }
        pCurNode = pCurNode->m_pNext;
    }
    pthread_mutex_unlock(&s_moduleInfoLock);

    return E_DOES_NOT_EXIST;
}

ECode LookupModuleInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIModuleInfo **ppModuleInfo)
{
    CIModuleInfoNode *pCurNode;
    CIModuleInfo *pModInfo;
    int m;

    assert(ppModuleInfo != NULL);

    pthread_mutex_lock(&s_moduleInfoLock);
    pCurNode = s_pModuleInfoList;
    while (pCurNode != NULL) {
        pModInfo = pCurNode->m_pModInfo;
        for (m = 0; m < pModInfo->classNum; m++) {
            if (pModInfo->classes[m].clsid == rclsid) {
                *ppModuleInfo = pModInfo;
                pthread_mutex_unlock(&s_moduleInfoLock);
                return NOERROR;
            }
        }
        pCurNode = pCurNode->m_pNext;
    }
    pthread_mutex_unlock(&s_moduleInfoLock);

    return E_DOES_NOT_EXIST;
}

// TODO:
void *GetUnalignedPtr(void *pPtr)
{
#ifdef _x86
    return *(void **)pPtr;
#else
    union
    {
        void    *p;
        byte_t  bytes[4];
    } u;
    u.bytes[0] = ((byte_t *)pPtr)[0];
    u.bytes[1] = ((byte_t *)pPtr)[1];
    u.bytes[2] = ((byte_t *)pPtr)[2];
    u.bytes[3] = ((byte_t *)pPtr)[3];

    return u.p;
#endif
}

ECode AcquireClassInfo(
    /* [in] */ const ClassID & classId,
    /* [out] */ CIClassInfo **ppClsInfo)
{
    ECode ec;
    CIModuleInfo *pModInfo;
    void *pModule;

    typedef ECode (STDCALL *CarDllGetClassObject_t)(
            REMuid clsid, REIID riid, IInterface **ppObj);

    if (ppClsInfo == NULL) return E_INVALID_ARGUMENT;

    char path[260];
    strcpy(path, classId.pUunm);
    pModule = dlopen(path, RTLD_NOW);

    strcpy(path, "DllGetClassObject");
    CarDllGetClassObject_t func = (CarDllGetClassObject_t)dlsym(pModule, path);

    func(ECLSID_ClassInfo, EIID_IInterface, (IInterface**)&pModInfo);

    ec = RegisterModuleInfo(pModInfo);
    if (FAILED(ec)) {
#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
        _DumpCLSID(classId);
        ALOGD("Failed to RegisterModuleInfo in %s", classId.pUunm);
#endif
        return ec;
    }

    ec = LookupModuleInfo(classId.clsid, &pModInfo);
    if (FAILED(ec)) {
#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
        _DumpCLSID(classId);
        ALOGD("Failed to LookupModuleInfo in %s", classId.pUunm);
#endif
        return ec;
    }

    for (Int32 m = 0; m < pModInfo->classNum; m++) {
        if (pModInfo->classes[m].clsid == classId.clsid) {
            *ppClsInfo = &(pModInfo->classes[m]);
            return NOERROR;
        }
    }

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
    _DumpCLSID(classId);
    ALOGD("Failed to AcquireClassInfo in %s", classId.pUunm);
#endif
    return E_DOES_NOT_EXIST;
}
