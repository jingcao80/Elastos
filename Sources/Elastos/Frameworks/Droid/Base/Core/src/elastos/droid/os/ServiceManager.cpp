//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CServiceManager.h"
#include <elastos/utility/logging/Logger.h>
#include <binder/IServiceManager.h>

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

android::sp<android::IBinder> ServiceManager::GetAndroidService(
    /* [in] */ const String& name)
{
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    if (sm == NULL) {
        Logger::E(TAG, "Failed to get android service_manager.");
        return NULL;
    }
    android::sp<android::IBinder> service = sm->getService(android::String16(name.string()));
    return service;
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
