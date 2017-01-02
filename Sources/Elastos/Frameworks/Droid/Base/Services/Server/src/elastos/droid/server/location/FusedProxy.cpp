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

#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/server/location/CFusedLocationHardwareSecure.h"
#include "elastos/droid/server/location/FusedProxy.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Location::EIID_IIFusedLocationHardware;
using Elastos::Droid::Location::IIFusedProvider;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Manifest;
using Elastos::Core::IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//============================
// FusedProxy::NewServiceWork
//============================

FusedProxy::NewServiceWork::NewServiceWork(
    /* [in] */ FusedProxy* host)
    : mHost(host)
{}

ECode FusedProxy::NewServiceWork::Run()
{
    return mHost->BindProvider(mHost->mLocationHardware);
}

//============================
// FusedProxy
//============================

FusedProxy::FusedProxy(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IIFusedLocationHardware* locationHardware,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId)
    : TAG("FusedProxy")
{
    AutoPtr<IIFusedLocationHardware> flh;
    CFusedLocationHardwareSecure::New(locationHardware, context,
        Manifest::permission::LOCATION_HARDWARE,
        (IIFusedLocationHardware**)&flh);
    mLocationHardware = (FusedLocationHardwareSecure*)flh.Get();

    AutoPtr<IRunnable> newServiceWork = (IRunnable*)(new NewServiceWork(this));

    // prepare the connection to the provider
    mServiceWatcher = new ServiceWatcher(
            context,
            TAG,
            String("com.android.location.service.FusedProvider"),
            overlaySwitchResId,
            defaultServicePackageNameResId,
            initialPackageNameResId,
            newServiceWork.Get(),
            handler);
}

AutoPtr<FusedProxy> FusedProxy::CreateAndBind(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IIFusedLocationHardware* locationHardware,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId)
{
    AutoPtr<FusedProxy> fusedProxy = new FusedProxy(
        context,
        handler,
        locationHardware,
        overlaySwitchResId,
        defaultServicePackageNameResId,
        initialPackageNameResId);

    // try to bind the Fused provider
    if (!fusedProxy->mServiceWatcher->Start()) {
        return NULL;
    }

    return fusedProxy;
}

ECode FusedProxy::BindProvider(
    /* [in] */ IIFusedLocationHardware* locationHardware)
{
    AutoPtr<IIFusedProvider> provider = IIFusedProvider::Probe(mServiceWatcher->GetBinder());

    if (provider == NULL) {
        Logger::E(TAG, "No instance of FusedProvider found on FusedLocationHardware connected.");
        return E_NULL_POINTER_EXCEPTION;
    }

    ECode ec = provider->OnFusedLocationHardwareChange(locationHardware);
    if (FAILED(ec)) {
        Logger::E(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos