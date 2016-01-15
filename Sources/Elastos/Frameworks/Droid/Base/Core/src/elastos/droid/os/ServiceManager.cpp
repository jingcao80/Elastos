
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CServiceManager.h"

namespace Elastos {
namespace Droid {
namespace Os {

const String ServiceManager::TAG("ServiceManager");
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
    ECode ec = GetIServiceManager()->GetService(name, (IInterface**)&obj);
    if (SUCCEEDED(ec)) {
        return obj;
    }
    else return NULL;
//    } catch (RemoteException e) {
//        Log.e(TAG, "error in getService", e);
//    }
}

void ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ IInterface* service)
{
//    try {
    GetIServiceManager()->AddService(name, service, FALSE);
//    } catch (RemoteException e) {
//        Log.e(TAG, "error in addService", e);
//    }
}

void ServiceManager::AddService(
    /* [in] */ const String& name,
    /* [in] */ IInterface* service,
    /* [in] */ Boolean allowIsolated)
{
    // try {
    GetIServiceManager()->AddService(name, service, allowIsolated);
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
    // assert(0);
    PRINT_FILE_LINE_EX("TODO::ServiceManager::CheckService()");
    return NULL;
}

AutoPtr< ArrayOf<String> > ServiceManager::ListServices()
{
//    try {
//        return getIServiceManager().listServices();
//    } catch (RemoteException e) {
//        Log.e(TAG, "error in listServices", e);
//        return null;
//    }
    assert(0);
    return NULL;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
