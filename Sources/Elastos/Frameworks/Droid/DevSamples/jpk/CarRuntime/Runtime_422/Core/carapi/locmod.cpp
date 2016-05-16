//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
#define __NO_LINKNODE_CONSTRUCTOR
#include <elastos.h>
#include "locmod.h"
#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <utils/Log.h>

#define ENABLE_DUMP_CLSID    0    // debug info switch
#if ENABLE_DUMP_CLSID
#define DUMP_CLSID(CLSID, info) \
    do { \
        ALOGD("> %s\n", info); \
        ALOGD("======== DUMP_CLSID ========\n"); \
        ALOGD("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }\n", \
                CLSID.clsid.Data1, CLSID.clsid.Data2, CLSID.clsid.Data3, \
                CLSID.clsid.Data4[0], CLSID.clsid.Data4[1], \
                CLSID.clsid.Data4[2], CLSID.clsid.Data4[3], \
                CLSID.clsid.Data4[4], CLSID.clsid.Data4[5], \
                CLSID.clsid.Data4[6], CLSID.clsid.Data4[7]); \
        ALOGD("============================\n"); \
    } while(0);
#else
#define DUMP_CLSID(CLSID, info);
#endif

EXTERN_C CARAPI DllGetClassObject(REMuid, REIID, PInterface *);

DLinkNode g_LocModList = { &g_LocModList, &g_LocModList };
pthread_mutex_t g_LocModListLock;

CAR_INLINE _ELASTOS Boolean IsElastosUunm(const char *pUunm)
{
    return !strcmp("Elastos.Runtime.CarRuntime.eco", pUunm);
}

ECode AcquireClassObjectFromLocalModule(
    /* [in] */ RClassID rclsid,
    /* [in] */ REIID riid,
    /* [out] */ PInterface *ppObject)
{
    const char *pszUunm = rclsid.pUunm;
    PDLLGETCLASSOBJECT pDllGetClassObject;

    assert(pszUunm);

    ECode ec = NOERROR;
    Void *pIModule = NULL;
    LocalModule *pLocalModule;

    pthread_mutex_lock(&g_LocModListLock);
    pLocalModule = (LocalModule *)(g_LocModList.m_pNext);
    while ((DLinkNode *)pLocalModule != &g_LocModList) {
        if (IsEqualUunm(pLocalModule->m_pszUunm, pszUunm)) {
            ec = (*pLocalModule->m_pDllGetClassObject)(rclsid.clsid,
                            EIID_IClassObject, ppObject);
            pLocalModule->m_nAskCount = 0;
            pthread_mutex_unlock(&g_LocModListLock);
            return ec;
        }
        pLocalModule = (LocalModule *)pLocalModule->m_pNext;
    }
    pthread_mutex_unlock(&g_LocModListLock);

    if (IsElastosUunm(pszUunm)) {
        return DllGetClassObject(rclsid.clsid, EIID_IClassObject,
                        ppObject);
    }

    pLocalModule  = NULL;
    char path[260];
    strcpy(path, pszUunm);
    DUMP_CLSID(rclsid, path)
    pIModule = dlopen(path, RTLD_NOW);
    if(NULL == pIModule){
        ec = E_FILE_NOT_FOUND;
        ALOGE("<%s, %d> dlopen '%s' failed.\n", __FILE__, __LINE__, path);
        ALOGE("error: %s\n", dlerror());
        goto ErrorExit;
    }

    // TODO(mips): wchar_t
#ifndef _GNUC_MIPS_KERNEL_
    strcpy(path, "DllGetClassObject");
    pDllGetClassObject = (PDLLGETCLASSOBJECT)dlsym(pIModule, path);
#else
    pDllGetClassObject = (PDLLGETCLASSOBJECT)dlsym(
            pIModule, "D\0l\0l\0G\0e\0t\0C\0l\0a\0s\0s\0O\0b\0j\0e\0c\0t\0");
#endif
    if (NULL == pDllGetClassObject) {
        ec = E_DOES_NOT_EXIST;
        goto ErrorExit;
    }

    pLocalModule = new LocalModule;
    if (!pLocalModule) {
        ec = E_OUT_OF_MEMORY;
        goto ErrorExit;
    }

    pLocalModule->m_pszUunm = new char[strlen(pszUunm) + 1];
    if (!pLocalModule->m_pszUunm) {
        ec = E_OUT_OF_MEMORY;
        goto ErrorExit;
    }
    strcpy(pLocalModule->m_pszUunm, pszUunm);

    pLocalModule->m_pIModule = pIModule;
    pLocalModule->m_pDllGetClassObject = pDllGetClassObject;
    pLocalModule->m_nAskCount = 0;

    //TODO(mips): wchar_t
#ifndef _GNUC_MIPS_KERNEL_
    pLocalModule->m_pDllCanUnloadNow = (PDLLCANUNLOADNOW)dlsym(pIModule, path);
#else
    pLocalModule->m_pDllCanUnloadNow = (PDLLCANUNLOADNOW)dlsym(
            pIModule, "D\0l\0l\0C\0a\0n\0U\0n\0l\0o\0a\0d\0N\0o\0w\0");
#endif

    ec = (*pDllGetClassObject)(rclsid.clsid, riid, ppObject);

    if (FAILED(ec)) goto ErrorExit;

    pthread_mutex_lock(&g_LocModListLock);
    g_LocModList.InsertFirst(pLocalModule);
    pthread_mutex_unlock(&g_LocModListLock);
    return ec;

ErrorExit:
    if (pIModule) {
        dlclose(pIModule);
    }
    if (pLocalModule) {
        if (pLocalModule->m_pszUunm) {
            delete pLocalModule->m_pszUunm;
        }
        delete pLocalModule;
    }
    return ec;
}

ELAPI_(_ELASTOS Boolean) _CModule_CanUnloadAllModules()
{
    LocalModule *pLocalModule;

    pthread_mutex_lock(&g_LocModListLock);
    pLocalModule = (LocalModule *)(g_LocModList.m_pNext);

    while ((DLinkNode *)pLocalModule != &g_LocModList) {
        if (NULL != pLocalModule->m_pDllCanUnloadNow) {
            if (NOERROR != (*pLocalModule->m_pDllCanUnloadNow)()) {
                pthread_mutex_unlock(&g_LocModListLock);
                return FALSE;
            }
        }
        pLocalModule = (LocalModule *)pLocalModule->m_pNext;
    }

    pthread_mutex_unlock(&g_LocModListLock);
    return TRUE;
}

ELAPI _CProcess_FreeUnusedModule(String szDllName)
{
    ECode ec = NOERROR;
    LocalModule *pLocalModule;
    Int16 nLen1 = 0;
    Int16 nLen2 = 0;

    if (szDllName.IsEmpty()) return E_INVALID_ARGUMENT;

    nLen2 = szDllName.GetByteLength();

    pthread_mutex_lock(&g_LocModListLock);
    pLocalModule = (LocalModule *)(g_LocModList.m_pNext);

    while (pLocalModule != (LocalModule *)&g_LocModList) {
        LocalModule *pnext = (LocalModule *)(pLocalModule->Next());
        assert(pLocalModule->m_pszUunm);
        nLen1 = strlen(pLocalModule->m_pszUunm);

        if (nLen1 < nLen2) {
            pLocalModule = pnext;
            continue;
        }
        else if (szDllName.CompareIgnoreCase(pLocalModule->m_pszUunm+nLen1-nLen2)) {
            pLocalModule = pnext;
            continue;
        }
        else if (nLen1 != nLen2 &&
                            pLocalModule->m_pszUunm[nLen1-nLen2] != '/') {
            pLocalModule = pnext;
            continue;
        }
        else if (NULL == pLocalModule->m_pDllCanUnloadNow) {
            ec = E_NOT_CAR_DLL;
        }
        else if (NOERROR == (*pLocalModule->m_pDllCanUnloadNow)()) {
            pLocalModule->Detach();
            dlclose(pLocalModule->m_pIModule);
            delete pLocalModule->m_pszUunm;
            delete pLocalModule;
            pLocalModule = NULL;
            ec = NOERROR;
        }
        else {
            ec = E_DLL_CANNOT_UNLOAD_NOW;
        }
        pthread_mutex_unlock(&g_LocModListLock);
        return ec;
    }

    pthread_mutex_unlock(&g_LocModListLock);

    return E_DLL_NOT_LOADED;
}

ELAPI_(void) _CProcess_FreeUnusedModules(Boolean bImmediate)
{
    LocalModule *pLocalModule;

    pthread_mutex_lock(&g_LocModListLock);
    pLocalModule = (LocalModule *)(g_LocModList.m_pNext);

    while (pLocalModule != (LocalModule *)&g_LocModList) {
        LocalModule *pnext = (LocalModule *)(pLocalModule->Next());
        if (NULL != pLocalModule->m_pDllCanUnloadNow) {
            if (NOERROR == (*pLocalModule->m_pDllCanUnloadNow)()) {
                ++pLocalModule->m_nAskCount;
                /*
                * If DllCanUnloadNow() return TRUE more than or
                * equal to 3 successive times, then release the DLL.
                * If caller specify 'bImmediate' is TRUE, then release
                * the DLL immediately.
                */
                if (bImmediate || pLocalModule->m_nAskCount > 2) {
                    pLocalModule->Detach();
                    dlclose(pLocalModule->m_pIModule);
                    delete pLocalModule->m_pszUunm;
                    delete pLocalModule;
                    pLocalModule = NULL;
                }
            }
            else
                pLocalModule->m_nAskCount = 0;
        }
        pLocalModule = pnext;
    }

    pthread_mutex_unlock(&g_LocModListLock);
}

void FreeCurProcComponents()
{
    LocalModule *pCurModule = NULL;

    pthread_mutex_lock(&g_LocModListLock);
    pCurModule = (LocalModule *)(g_LocModList.m_pNext);

    while (pCurModule != (LocalModule *)&g_LocModList) {
        LocalModule *pnext = (LocalModule *)(pCurModule->Next());
        ECode ecCur = S_FALSE;
        if (NULL != pCurModule->m_pDllCanUnloadNow) {
            ecCur = (*pCurModule->m_pDllCanUnloadNow)();
            if (NOERROR == ecCur) {
                pCurModule->Detach();
                dlclose(pCurModule->m_pIModule);
                delete pCurModule->m_pszUunm;
                delete pCurModule;
                pCurModule = NULL;
            }
        }

        if (S_FALSE != ecCur) {
            pCurModule = (LocalModule *)(g_LocModList.m_pNext);
        }
        else {
            pCurModule = pnext;
        }
    }

    pthread_mutex_unlock(&g_LocModListLock);
}
