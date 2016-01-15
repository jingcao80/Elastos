
#ifndef __SERVICEMANAGER_H__
#define __SERVICEMANAGER_H__

#include <elastos/coredef.h>
#include <elastos/core/Mutex.h>
#include <elastos/utility/etl/HashMap.h>

using namespace Elastos;
using Elastos::Core::Mutex;
using Elastos::Utility::Etl::HashMap;

class ServiceManager : public ElLightRefBase
{
public:
    static CARAPI_(AutoPtr<ServiceManager>) GetInstance();

    CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service);

    CARAPI GetService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** service);

private:
    ServiceManager();

    CARAPI_(Boolean) ConnectServer();

private:
    static AutoPtr<ServiceManager> sInstance;

    Boolean mIsConnected;
    Mutex mConnectedLock;
    String mServerIpAddress;
    Int32 mServerPort;
};

#endif //__SERVICEMANAGER_H__
