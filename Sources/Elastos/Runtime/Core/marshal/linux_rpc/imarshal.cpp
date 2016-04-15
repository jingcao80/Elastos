
#include <pthread.h>
#include <prxstub.h>
#include <rot.h>

_ELASTOS_NAMESPACE_USING

EXTERN_C const InterfaceID EIID_IProxy;

#define ASSERT_PROXY(proxy) \
    do {assert(((CObjectProxy*)proxy)->mInterfaceNum > 0);} while (0);

namespace Elastos {
namespace RPC {

extern pthread_mutex_t g_marshalLock;

ECode StdMarshalInterface(
    /* [in] */ IInterface* object,
    /* [out] */ InterfacePack* interfacePack)
{
    ExportObject ex;

    pthread_mutex_lock(&g_marshalLock);
    ECode ec = FindExportObject(object, &ex);
    if (SUCCEEDED(ec)) {
        interfacePack->mNetAddress = ex.mNetAddress;
        ex.mIStub->GetClassID(&interfacePack->mClsid);
        ec = ex.mIStub->GetInterfaceIndex(object, &interfacePack->mIndex);
        if (ec == E_NO_INTERFACE) {
            ec = E_CONFLICT_WITH_LOCAL_KEYWORD;
        }
        pthread_mutex_unlock(&g_marshalLock);
        if (SUCCEEDED(ec)) ex.mIStub->AddRef();
    }
    else {
        IProxy* proxy = (IProxy*)object->Probe(EIID_IProxy);
        if (proxy) {
            pthread_mutex_unlock(&g_marshalLock);
            interfacePack->mNetAddress = ((CObjectProxy*)proxy)->mNetAddress;
            ASSERT_PROXY(proxy);
            proxy->GetClassID(&interfacePack->mClsid);
            ec = proxy->GetInterfaceIndex(object, &(interfacePack->mIndex));
        }
        else {
            IStub* stub;
            ec = CObjectStub::S_CreateObject(object, &stub);
            pthread_mutex_unlock(&g_marshalLock);
            if (SUCCEEDED(ec)) {
                interfacePack->mNetAddress = ((CObjectStub*)stub)->mNetAddress;
                stub->GetClassID(&(interfacePack->mClsid));
                ec = stub->GetInterfaceIndex(object, &(interfacePack->mIndex));
                if (ec == E_NO_INTERFACE) {
                    ec = E_CONFLICT_WITH_LOCAL_KEYWORD;
                }
                if (SUCCEEDED(ec)) stub->AddRef();
            }
        }
    }

    return ec;
}

static ECode StdUnmarshalCustomInterface(
    /* [in] */ ICustomMarshal* customMarshal,
    /* [in] */ InterfacePack* interfacePack,
    /* [out] */ IInterface** object)
{
    *object = NULL;

    ClassID proxyClsid;
    char uunmbuf[80];
    proxyClsid.mUunm = uunmbuf;

    ECode ec = customMarshal->GetClsid(&proxyClsid);
    if (SUCCEEDED(ec)) {
        AutoPtr<IInterface> obj;
        ec = _CObject_CreateInstance(proxyClsid, RGM_SAME_DOMAIN,
                EIID_ICustomMarshal, (IInterface**)&obj);
        if (SUCCEEDED(ec)) {
            ICustomMarshal* tempCustMsh = ICustomMarshal::Probe(obj);
            ec = tempCustMsh->CreateObject(customMarshal, object);
        }
    }

    return ec;
}

ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack* interfacePack,
    /* [out] */ IInterface** object)
{
    *object = NULL;

    ExportObject ex;
    ICustomMarshal* customMarshal;

    pthread_mutex_lock(&g_marshalLock);
    ECode ec = FindExportObject(interfacePack->mNetAddress, &ex);
    if (SUCCEEDED(ec) && (flag == UnmarshalFlag_Noncoexisting)) {
        pthread_mutex_unlock(&g_marshalLock);
        customMarshal = ICustomMarshal::Probe(ex.mObject);
        if (!customMarshal) {
            CObjectStub* stub = (CObjectStub*)ex.mIStub;
            *object = stub->mInterfaces[interfacePack->mIndex].mObject;
            (*object)->AddRef();
            ex.mIStub->Release();

            return NOERROR;
        }
        else {
            ec = StdUnmarshalCustomInterface(
                    customMarshal, interfacePack, object);
            return ec;
        }
    }

    ImportObject im;
    ec = FindImportObject(interfacePack->mNetAddress, &im);
    if (SUCCEEDED(ec)) {
        pthread_mutex_unlock(&g_marshalLock);
        customMarshal = ICustomMarshal::Probe(im.mIProxy);
        if (!customMarshal) {
            ec = im.mIProxy->GetInterface(interfacePack->mIndex, object);
        }
        else {
            ec = StdUnmarshalCustomInterface(
                    customMarshal, interfacePack, object);
        }
        im.mIProxy->Release();
    }
    else {
        AutoPtr<IProxy> proxy;
        ec = CObjectProxy::S_CreateObject(
            interfacePack->mClsid,
            interfacePack->mNetAddress,
            (IProxy**)&proxy);
        pthread_mutex_unlock(&g_marshalLock);
        if (SUCCEEDED(ec)) {
            customMarshal = ICustomMarshal::Probe(proxy);
            if (!customMarshal) {
                ec = proxy->GetInterface(interfacePack->mIndex, object);
            }
            else {
                ec = StdUnmarshalCustomInterface(
                        customMarshal, interfacePack, object);
            }
        }
    }

    return ec;
}

} // namespace RPC
} // namespace Elastos
