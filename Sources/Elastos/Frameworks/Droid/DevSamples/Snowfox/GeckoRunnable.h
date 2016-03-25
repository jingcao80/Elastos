
#ifndef __GECKORUNNABLE_H__
#define __GECKORUNNABLE_H__

#include <elastos.h>

using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

class GeckoRunnable
    : public ElRefBase
    , public IRunnable
{
public:
    GeckoRunnable(
        /* [in] */ Void* pHost)
        : mHost(pHost)
    {
    }

    UInt32 AddRef()
    {
        return ElRefBase::AddRef();
    }

    UInt32 Release()
    {
        return ElRefBase::Release();
    }

    PInterface Probe(
        /* [in] */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return (PInterface)this;
        }
        else if (riid == EIID_IRunnable) {
            return this;
        }

        return NULL;
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* pObject,
        /* [in] */ InterfaceID* pIID)
    {
        return E_NOT_IMPLEMENTED;
    }

    ECode Run()
    {
        return NOERROR;
    }

protected:
    void* mHost;
};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__GECKORUNNABLE_H__
