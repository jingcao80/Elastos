
#include <Elastos.CoreLibrary.h>
#include <ServiceManager.h>
#include <unistd.h>
#include <stdio.h>

#include <cutils/log.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CStringWrapper;

class DeathMonitor
    : public ElLightRefBase
    , public IProxyDeathRecipient
{
public:
    DeathMonitor() {}

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return this;
        }
        else if (riid == EIID_IProxyDeathRecipient) {
            return this;
        }
        return NULL;
    }

    CARAPI_(UInt32) AddRef()
    {
        return ElLightRefBase::AddRef();
    }

    CARAPI_(UInt32) Release()
    {
        return ElLightRefBase::Release();
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID)
    {
        return E_NOT_IMPLEMENTED;
    }

    //@Override
    CARAPI ProxyDied()
    {
        ALOGD("==== File: %s, Line: %d ====", __FILE__, __LINE__);
        return NOERROR;
    }
};

int main(int argv, char* argc[])
{
    AutoPtr<ICharSequence> cs;
    ServiceManager::GetInstance()->GetService(String("test"), (IInterface**)&cs);

    AutoPtr<DeathMonitor> m = new DeathMonitor();
    AutoPtr<IProxy> proxy = (IProxy*)cs->Probe(EIID_IProxy);
    proxy->LinkToDeath(m, 0);

    while (TRUE) {
        String str;
        cs->ToString(&str);
ALOGD("==== File: %s, Line: %d str: %s ====", __FILE__, __LINE__, str.string());
        sleep(1);
    }

    return 0;
}
