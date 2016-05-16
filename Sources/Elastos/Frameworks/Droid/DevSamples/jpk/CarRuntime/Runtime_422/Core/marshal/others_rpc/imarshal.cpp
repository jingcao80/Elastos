
#include <pthread.h>
#include "prxstub.h"
#include "rot.h"
#include "elasys_server.h"

_ELASTOS_NAMESPACE_USING

EXTERN_C const InterfaceID EIID_IProxy;
extern pthread_mutex_t g_marshalLock;

#define ASSERT_PROXY(pProxy) \
    do {assert(((CObjectProxy*)pProxy)->m_cInterfaces > 0);} while (0);

ECode StdMarshalInterface(
    /* [in] */ IInterface *pObj,
    /* [out] */ InterfacePack *pInterfacePack)
{
    ExportObject ex;
    ECode ec;
    IStub *pIStub;
    IProxy *pv;

    pthread_mutex_lock(&g_marshalLock);
    ec = FindExportObject(pObj, &ex);
    if (SUCCEEDED(ec)) {
        strcpy(pInterfacePack->m_stubConnName, 
                ((CObjectStub*)ex.m_pIStub)->m_connName);
        ex.m_pIStub->GetClassID(&pInterfacePack->m_clsid);
        ec = ex.m_pIStub->GetInterfaceIndex(pObj, &pInterfacePack->m_uIndex);
        if (ec == E_NO_INTERFACE) {
            ec = E_CONFLICT_WITH_LOCAL_KEYWORD;
        }
        pthread_mutex_unlock(&g_marshalLock);
    }
    else {
        pv = (IProxy*)pObj->Probe(EIID_IProxy);
        if (pv) {
            pthread_mutex_unlock(&g_marshalLock);
            strcpy(pInterfacePack->m_stubConnName, 
                    ((CObjectProxy*)pv)->m_stubConnName);        
            ASSERT_PROXY(pv);
            pv->GetClassID(&pInterfacePack->m_clsid);
            ec = pv->GetInterfaceIndex(pObj, &(pInterfacePack->m_uIndex));
        }
        else {
            ec = CObjectStub::S_CreateObject(pObj, &pIStub);
            pthread_mutex_unlock(&g_marshalLock);
            if (SUCCEEDED(ec)) {
                strcpy(pInterfacePack->m_stubConnName, 
                        ((CObjectStub*)pIStub)->m_connName);         
                pIStub->GetClassID(&pInterfacePack->m_clsid);
                ec = pIStub->GetInterfaceIndex(pObj, &pInterfacePack->m_uIndex);
                if (ec == E_NO_INTERFACE) {
                    ec = E_CONFLICT_WITH_LOCAL_KEYWORD;
                }
            }
        }
    }

    return ec;
}

static ECode StdUnmarshalCustomInterface(
    /* [in] */ ICustomMarshal *pCustomMarshal,
    /* [in] */ InterfacePack *pInterfacePack,
    /* [out] */ IInterface **ppObj)
{
    ECode ec;
    ICustomMarshal * pITempCustMsh;
    ClassID proxyClsid;
    char uunmbuf[80];
    proxyClsid.pUunm = uunmbuf;

    ec = pCustomMarshal->GetClsid(&proxyClsid);
    if (SUCCEEDED(ec)) {
        ec = _CObject_CreateInstance(proxyClsid, RGM_SAME_DOMAIN,
                EIID_ICustomMarshal, (IInterface **)&pITempCustMsh);
        if (SUCCEEDED(ec)) {
            ec = pITempCustMsh->CreateObject(pCustomMarshal, ppObj);
            pITempCustMsh->Release();
        }
    }

    return ec;
}

ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack *pInterfacePack,
    /* [out] */ IInterface **ppObj)
{
    ImportObject im;
    ExportObject ex;
    ECode ec;
    IProxy *pIProxy;
    ICustomMarshal * pCustomMarshal;

    pthread_mutex_lock(&g_marshalLock);
    ec = FindExportObject(pInterfacePack->m_stubConnName, &ex);
    if (SUCCEEDED(ec) && (flag == UnmarshalFlag_Noncoexisting)) {
        pthread_mutex_unlock(&g_marshalLock);
        pCustomMarshal = ICustomMarshal::Probe(ex.m_pObject);
        if (!pCustomMarshal) {
            CObjectStub* pSt = (CObjectStub*)ex.m_pIStub;
            *ppObj = pSt->m_pInterfaces[pInterfacePack->m_uIndex].m_pObject;
            (*ppObj)->AddRef();
            ex.m_pIStub->Release();

            return NOERROR;
        }
        else {
            ec = StdUnmarshalCustomInterface(\
                pCustomMarshal, pInterfacePack, ppObj);
            return ec;
        }
    }

    ec = FindImportObject(pInterfacePack->m_stubConnName, &im);
    if (SUCCEEDED(ec)) {
        pthread_mutex_unlock(&g_marshalLock);
        pCustomMarshal = ICustomMarshal::Probe(im.m_pIProxy);
        if (!pCustomMarshal) {
            ec = im.m_pIProxy->GetInterface(pInterfacePack->m_uIndex, ppObj);
        }
        else {
            ec = StdUnmarshalCustomInterface(\
                pCustomMarshal, pInterfacePack, ppObj);
        }
        im.m_pIProxy->Release();
    }
    else {
        ec = CObjectProxy::S_CreateObject(
            pInterfacePack->m_clsid,
            pInterfacePack->m_stubConnName,
            &pIProxy);
        pthread_mutex_unlock(&g_marshalLock);
        if (SUCCEEDED(ec)) {
            pCustomMarshal = ICustomMarshal::Probe(pIProxy);
            if (!pCustomMarshal) {
                ec = pIProxy->GetInterface(pInterfacePack->m_uIndex, ppObj);
            }
            else {
                ec = StdUnmarshalCustomInterface(\
                    pCustomMarshal, pInterfacePack, ppObj);
            }
            pIProxy->Release(); // Release IProxy's ref count
        }
    }

    return ec;
}
