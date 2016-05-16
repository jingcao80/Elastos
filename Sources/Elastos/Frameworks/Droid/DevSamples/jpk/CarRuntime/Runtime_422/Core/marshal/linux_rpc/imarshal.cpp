
#include <pthread.h>
#include "prxstub.h"
#include "rot.h"
#include "elasys_server.h"

_ELASTOS_NAMESPACE_USING

EXTERN_C const InterfaceID EIID_IProxy;

#define ASSERT_PROXY(pProxy) \
    do {assert(((CObjectProxy*)pProxy)->m_cInterfaces > 0);} while (0);

namespace Elastos {
namespace RPC {

extern pthread_mutex_t g_marshalLock;

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
        pInterfacePack->m_sNetAddress = ex.m_sNetAddress;
        ex.m_pIStub->GetClassID(&pInterfacePack->m_clsid);
        ec = ex.m_pIStub->GetInterfaceIndex(pObj, &pInterfacePack->m_uIndex);
        if (ec == E_NO_INTERFACE) {
            ec = E_CONFLICT_WITH_LOCAL_KEYWORD;
        }
        pthread_mutex_unlock(&g_marshalLock);
        if (SUCCEEDED(ec)) ex.m_pIStub->AddRef();
    }
    else {
        pv = (IProxy*)pObj->Probe(EIID_IProxy);
        if (pv) {
            pthread_mutex_unlock(&g_marshalLock);
            pInterfacePack->m_sNetAddress = ((CObjectProxy*)pv)->m_sNetAddress;
            ASSERT_PROXY(pv);
            pv->GetClassID(&pInterfacePack->m_clsid);
            ec = pv->GetInterfaceIndex(pObj, &(pInterfacePack->m_uIndex));
        }
        else {
            ec = CObjectStub::S_CreateObject(pObj, &pIStub);
            pthread_mutex_unlock(&g_marshalLock);
            if (SUCCEEDED(ec)) {
                pInterfacePack->m_sNetAddress = ((CObjectStub*)pIStub)->m_sNetAddress;
                pIStub->GetClassID(&(pInterfacePack->m_clsid));
                ec = pIStub->GetInterfaceIndex(pObj, &(pInterfacePack->m_uIndex));
                if (ec == E_NO_INTERFACE) {
                    ec = E_CONFLICT_WITH_LOCAL_KEYWORD;
                }
                if (SUCCEEDED(ec)) pIStub->AddRef();
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
    *ppObj = NULL;

    ECode ec;

    ClassID proxyClsid;
    char uunmbuf[80];
    proxyClsid.pUunm = uunmbuf;

    ec = pCustomMarshal->GetClsid(&proxyClsid);
    if (SUCCEEDED(ec)) {
        AutoPtr<ICustomMarshal> pITempCustMsh;
        ec = _CObject_CreateInstance(proxyClsid, RGM_SAME_DOMAIN,
                EIID_ICustomMarshal, (IInterface **)&pITempCustMsh);
        if (SUCCEEDED(ec)) {
            ec = pITempCustMsh->CreateObject(pCustomMarshal, ppObj);
        }
    }

    return ec;
}

ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack *pInterfacePack,
    /* [out] */ IInterface **ppObj)
{
    *ppObj = NULL;

    ImportObject im;
    ExportObject ex;
    ECode ec;
    ICustomMarshal * pCustomMarshal;

    pthread_mutex_lock(&g_marshalLock);
    ec = FindExportObject(pInterfacePack->m_sNetAddress, &ex);
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

    ec = FindImportObject(pInterfacePack->m_sNetAddress, &im);
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
        AutoPtr<IProxy> pIProxy;
        ec = CObjectProxy::S_CreateObject(
            pInterfacePack->m_clsid,
            pInterfacePack->m_sNetAddress,
            (IProxy **)&pIProxy);
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
        }
    }

    return ec;
}

} // namespace RPC
} // namespace Elastos
