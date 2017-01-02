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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_ACTIVITYRECOGNITIONPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_ACTIVITYRECOGNITIONPROXY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/server/ServiceWatcher.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Location::IActivityRecognitionHardware;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Server::ServiceWatcher;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Proxy class to bind GmsCore to the ActivityRecognitionHardware.
 *
 * @hide
 */
class ActivityRecognitionProxy
    : public Object
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ ActivityRecognitionProxy* host);

        // @Override
        CARAPI Run();

    private:
        ActivityRecognitionProxy* mHost;
    };

public:
    /**
     * Creates an instance of the proxy and binds it to the appropriate FusedProvider.
     *
     * @return An instance of the proxy if it could be bound, null otherwise.
     */
    static CARAPI_(AutoPtr<ActivityRecognitionProxy>) CreateAndBind(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IActivityRecognitionHardware* activityRecognitionHardware,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId);

private:
    ActivityRecognitionProxy(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IActivityRecognitionHardware* activityRecognitionHardware,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNameResId);

    /**
     * Helper function to bind the FusedLocationHardware to the appropriate FusedProvider instance.
     */
    CARAPI BindProvider(
        /* [in] */ IActivityRecognitionHardware* activityRecognitionHardware);

private:
    const static String TAG;

    AutoPtr<ServiceWatcher> mServiceWatcher;
    AutoPtr<IActivityRecognitionHardware> mActivityRecognitionHardware;

};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_ACTIVITYRECOGNITIONPROXY_H__