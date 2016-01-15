
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/BinderProxy.h"
#include "elastos/droid/ext/frameworkext.h"
#include <binder/IBinder.h>

namespace Elastos {
namespace Droid {
namespace Os {

// {C9B1505E-E42A-4175-A9AC-57D819F1428F}
extern const InterfaceID EIID_BinderProxy =
    { 0xc9b1505e, 0xe42a, 0x4175, { 0xa9, 0xac, 0x57, 0xd8, 0x19, 0xf1, 0x42, 0x8f } };

BinderProxy::BinderProxy()
    : mObject(0)
{}

BinderProxy::~BinderProxy()
{
    Destroy();
}

PInterface BinderProxy::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IBinder*)this;
    }
    else if (riid == EIID_IBinder) {
        return (IBinder*)this;
    }
    if (riid == EIID_BinderProxy) {
        return reinterpret_cast<PInterface>((BinderProxy*)this);
    }

    return NULL;
}

UInt32 BinderProxy::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BinderProxy::Release()
{
    return ElRefBase::Release();
}

ECode BinderProxy::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode BinderProxy::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)this;
    return NOERROR;
}

ECode BinderProxy::ToString(
    /* [out] */ String* str)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

void BinderProxy::Destroy()
{
    android::IBinder* b = (android::IBinder*)mObject;

    b->decStrong(this);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
