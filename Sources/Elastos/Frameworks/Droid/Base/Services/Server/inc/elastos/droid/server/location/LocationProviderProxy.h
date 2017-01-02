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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERPROXY_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERPROXY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/server/ServiceWatcher.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Location::IILocationProvider;
using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Internal::Location::IProviderProperties;
using Elastos::Droid::Internal::Location::IProviderRequest;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Core::Runnable;
using Elastos::Droid::Server::Location::ILocationProviderInterface;
using Elastos::Droid::Server::ServiceWatcher;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * Proxy for ILocationProvider implementations.
 */
class LocationProviderProxy
    : public Object
    , public ILocationProviderInterface
{
private:
    class NewServiceWorkRunnable
        : public Runnable
    {
    public:
        NewServiceWorkRunnable(
            /* [in] */ LocationProviderProxy* host);

        //@Override
        CARAPI Run();

    private:
        LocationProviderProxy* mHost;
    };

public:
    static CARAPI_(AutoPtr<LocationProviderProxy>) CreateAndBind(
        /* [in] */ IContext* context,
        /* [in] */ const String& name,
        /* [in] */ const String& action,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNamesResId,
        /* [in] */ IHandler* handler);

private:
    LocationProviderProxy(
        /* [in] */ IContext* context,
        /* [in] */ const String& name,
        /* [in] */ const String& action,
        /* [in] */ Int32 overlaySwitchResId,
        /* [in] */ Int32 defaultServicePackageNameResId,
        /* [in] */ Int32 initialPackageNamesResId,
        /* [in] */ IHandler* handler);

    CARAPI_(Boolean) Bind();

    CARAPI_(AutoPtr<IILocationProvider>) GetService();

public:
    CAR_INTERFACE_DECL()

    CARAPI GetConnectedPackageName(
        /* [out] */ String* name);

    //@Override
    CARAPI GetName(
        /* [out] */ String* name);

    //@Override
    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    //@Override
    CARAPI Enable();

    //@Override
    CARAPI Disable();

    //@Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* enable);

    //@Override
    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    //@Override
    CARAPI GetStatus(
        /* [in] */ IBundle* extras,
        /* [out] */ Int32* status);

    //@Override
    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* time);

    //@Override
    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

private:
    static const String TAG;
    static const Boolean D;

    AutoPtr<IContext> mContext;
    String mName;
    AutoPtr<ServiceWatcher> mServiceWatcher;

    // cached values set by the location manager, synchronized on mLock
    AutoPtr<IProviderProperties> mProperties;
    Boolean mEnabled;
    AutoPtr<IProviderRequest> mRequest;
    AutoPtr<IWorkSource> mWorksource;

    /**
     * Work to apply current state to a newly connected provider.
     * Remember we can switch the service that implements a providers
     * at run-time, so need to apply current state.
     */
    AutoPtr<IRunnable> mNewServiceWork;
 };

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONPROVIDERPROXY_H__
