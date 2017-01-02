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
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/LocationProviderAdapter.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/LocationProviderAdapter_dec.h"
#include <elastos/utility/concurrent/FutureTask.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Utility::Concurrent::FutureTask;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                LocationProviderAdapter::StartRunnable
//=====================================================================

LocationProviderAdapter::StartRunnable::StartRunnable(
    /* [in] */ LocationProviderAdapter* owner,
    /* [in] */ const Boolean& gpsEnabled)
    : mOwner(owner)
    , mGpsEnabled(gpsEnabled)
{
}

ECode LocationProviderAdapter::StartRunnable::Run()
{
    mOwner->mImpl->Start(mGpsEnabled);
    return NOERROR;
}

//=====================================================================
//                LocationProviderAdapter::StopRunnable
//=====================================================================
LocationProviderAdapter::StopRunnable::StopRunnable(
    /* [in] */ LocationProviderAdapter* owner)
    : mOwner(owner)
{
}

ECode LocationProviderAdapter::StopRunnable::Run()
{
    mOwner->mImpl->Stop();
    return NOERROR;
}

//=====================================================================
//                       LocationProviderAdapter
//=====================================================================
AutoPtr<LocationProviderAdapter> LocationProviderAdapter::Create(
    /* [in] */ IContext* context)
{
    return new LocationProviderAdapter(context);
}

Boolean LocationProviderAdapter::Start(
    /* [in] */ const Boolean& gpsEnabled)
{
    AutoPtr<IRunnable> runnable = new StartRunnable(this, gpsEnabled);
    AutoPtr<FutureTask> task = new FutureTask(runnable, NULL);
    ThreadUtils::RunOnUiThread(task);
    return TRUE;
}

ECode LocationProviderAdapter::Stop()
{
    AutoPtr<IRunnable> runnable = new StopRunnable(this);
    AutoPtr<FutureTask> task = new FutureTask(runnable, NULL);
    ThreadUtils::RunOnUiThread(task);
    return NOERROR;
}

Boolean LocationProviderAdapter::IsRunning()
{
    assert(ThreadUtils::RunningOnUiThread());
    return mImpl->IsRunning();
}

ECode LocationProviderAdapter::NewLocationAvailable(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double timestamp,
    /* [in] */ Boolean hasAltitude,
    /* [in] */ Double altitude,
    /* [in] */ Boolean hasAccuracy,
    /* [in] */ Double accuracy,
    /* [in] */ Boolean hasHeading,
    /* [in] */ Double heading,
    /* [in] */ Boolean hasSpeed,
    /* [in] */ Double speed)
{
    NativeNewLocationAvailable(latitude, longitude, timestamp, hasAltitude, altitude,
             hasAccuracy, accuracy, hasHeading, heading, hasSpeed, speed);
    return NOERROR;
}

ECode LocationProviderAdapter::NewErrorAvailable(
    /* [in] */ const String& message)
{
    NativeNewErrorAvailable(message);
    return NOERROR;
}

LocationProviderAdapter::LocationProviderAdapter(
    /* [in] */ IContext* context)
{
    mImpl = LocationProviderFactory::Get(context);
}

ECode LocationProviderAdapter::NativeNewLocationAvailable(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double timeStamp,
    /* [in] */ Boolean hasAltitude,
    /* [in] */ Double altitude,
    /* [in] */ Boolean hasAccuracy,
    /* [in] */ Double accuracy,
    /* [in] */ Boolean hasHeading,
    /* [in] */ Double heading,
    /* [in] */ Boolean hasSpeed,
    /* [in] */ Double speed)
{
    Elastos_LocationProviderAdapter_nativeNewLocationAvailable(latitude, longitude, timeStamp, hasAltitude,
            altitude, hasAccuracy, accuracy, hasHeading, heading, hasSpeed, speed);
    return NOERROR;
}

ECode LocationProviderAdapter::NativeNewErrorAvailable(
    /* [in] */ const String& message)
{
    Elastos_LocationProviderAdapter_nativeNewErrorAvailable(message);
    return NOERROR;
}

AutoPtr<IInterface> LocationProviderAdapter::Create(
    /* [in] */ IInterface* context)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    return TO_IINTERFACE(Create(c));
}

Boolean LocationProviderAdapter::Start(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean gpsEnabled)
{
    AutoPtr<LocationProviderAdapter> mObj = (LocationProviderAdapter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("LocationProviderAdapter", "LocationProviderAdapter::Start, mObj is NULL");
        return FALSE;
    }
    return mObj->Start(gpsEnabled);
}

void LocationProviderAdapter::Stop(
    /* [in] */ IInterface* obj)
{
    AutoPtr<LocationProviderAdapter> mObj = (LocationProviderAdapter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("LocationProviderAdapter", "LocationProviderAdapter::Stop, mObj is NULL");
        return;
    }
    mObj->Stop();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
