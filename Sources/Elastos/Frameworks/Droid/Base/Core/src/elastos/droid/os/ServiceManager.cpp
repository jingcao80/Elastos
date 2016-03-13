
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

const String ServiceManager::TAG("elserviceManager");
AutoPtr<IServiceManager> ServiceManager::sServiceManager;

AutoPtr<IServiceManager> ServiceManager::GetIServiceManager()
{
    if (sServiceManager != NULL) {
        return sServiceManager;
    }

    // Find the service manager
    ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sServiceManager));
    return sServiceManager;
}

AutoPtr<IInterface> ServiceManager::GetService(
    /* [in] */ const String& name)
{
//    try {
    AutoPtr<IInterface> obj;
    GetIServiceManager()->GetService(name, (IInterface**)&obj);
    return obj;
//    } catch (RemoteException e) {
//        Log.e(TAG, "error in getService", e);
//    }
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ IInterface* service)
{
//    try {
    return GetIServiceManager()->AddService(name, service, FALSE);
//    } catch (RemoteException e) {
//        Log.e(TAG, "error in addService", e);
//    }
}

ECode ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ IInterface* service,
    /* [in] */ Boolean allowIsolated)
{
    // try {
    return GetIServiceManager()->AddService(name, service, allowIsolated);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "error in addService", e);
    // }
}

AutoPtr<IInterface> ServiceManager::CheckService(
    /* [in] */ const String& name)
{
//    try {
//        IBinder service = sCache.get(name);
//        if (service != null) {
//            return service;
//        } else {
//            return getIServiceManager().checkService(name);
//        }
//    } catch (RemoteException e) {
//        Log.e(TAG, "error in checkService", e);
//        return null;
//    }
    AutoPtr<IInterface> obj;
    GetIServiceManager()->GetService(name, (IInterface**)&obj);
    return obj;
}

AutoPtr< ArrayOf<String> > ServiceManager::ListServices()
{
//    try {
//        return getIServiceManager().listServices();
//    } catch (RemoteException e) {
//        Log.e(TAG, "error in listServices", e);
//        return null;
//    }
    AutoPtr< ArrayOf<String> > services;
    GetIServiceManager()->ListServices((ArrayOf<String>**)&services);
    return services;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
