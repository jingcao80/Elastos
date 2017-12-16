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

#ifndef __ELASTOS_DROID_OS_SERVICEMANAGER_H__
#define __ELASTOS_DROID_OS_SERVICEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <binder/Binder.h>

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC ServiceManager
{
public:
    /**
     * Returns a reference to a service with the given name.
     *
     * @param name the name of the service to get
     * @return a reference to the service, or <code>null</code> if the service doesn't exist
     */
    static CARAPI_(AutoPtr<IInterface>) GetService(
        /* [in] */ const String& name);

    static android::sp<android::IBinder> GetAndroidService(
        /* [in] */ const String& name);

    /**
     * Place a new @a service called @a name into the service
     * manager.
     *
     * @param name the name of the new service
     * @param service the service object
     */
    static CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service);

    /**
     * Place a new @a service called @a name into the service
     * manager.
     *
     * @param name the name of the new service
     * @param service the service object
     * @param allowIsolated set to true to allow isolated sandboxed processes
     * to access this service
     */
    static CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service,
        /* [in] */ Boolean allowIsolated);

    /**
     * Retrieve an existing service called @a name from the
     * service manager.  Non-blocking.
     */
    static CARAPI_(AutoPtr<IInterface>) CheckService(
        /* [in] */ const String& name);

    /**
     * Return a list of all currently running services.
     */
    static CARAPI_(AutoPtr< ArrayOf<String> >) ListServices();

private:
    static CARAPI_(AutoPtr<IServiceManager>) GetIServiceManager();

private:
    static const String TAG;

    static AutoPtr<IServiceManager> sServiceManager;

private:
    ServiceManager();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_SERVICEMANAGER_H__
