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

#include "elastos/droid/server/location/MockProvider.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Os::CBundle;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String MockProvider::TAG("MockProvider");

CAR_INTERFACE_IMPL(MockProvider, Object, ILocationProviderInterface)

MockProvider::MockProvider(
    /* [in] */ const String& name,
    /* [in] */ IILocationManager* locationManager,
    /* [in] */ IProviderProperties* properties)
    : mStatus(0)
    , mStatusUpdateTime(0L)
    , mHasLocation(FALSE)
    , mHasStatus(FALSE)
    , mEnabled(FALSE)
{
    if (properties == NULL) {
        Logger::E(TAG, "properties is null");
    }
    mName = name;
    mLocationManager = locationManager;
    mProperties = properties;
    CLocation::New(name, (ILocation**)&mLocation);
    CBundle::New((IBundle**)&mExtras);
}

ECode MockProvider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode MockProvider::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties);
    *properties = mProperties;
    REFCOUNT_ADD(*properties);

    return NOERROR;
}

ECode MockProvider::Disable()
{
    mEnabled = FALSE;
    return NOERROR;
}

ECode MockProvider::Enable()
{
    mEnabled = TRUE;
    return NOERROR;
}

ECode MockProvider::IsEnabled(
    /* [out] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    *enable = mEnabled;
    return NOERROR;
}

ECode MockProvider::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    if (mHasStatus) {
        extras->Clear();
        extras->PutAll(mExtras);
        *status = mStatus;
    } else {
        *status = ILocationProvider::AVAILABLE;
    }
    return NOERROR;
}

ECode MockProvider::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mStatusUpdateTime;
    return NOERROR;
}

ECode MockProvider::SetLocation(
    /* [in] */ ILocation* l)
{
    mLocation->Set(l);
    mHasLocation = TRUE;
    if (mEnabled) {
        ECode ec = mLocationManager->ReportLocation(mLocation, FALSE);
        if (FAILED(ec)) {
            Logger::E(TAG, "RemoteException calling reportLocation");
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode MockProvider::ClearLocation()
{
    mHasLocation = FALSE;
    return NOERROR;
}

ECode MockProvider::SetStatus(
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 updateTime)
{
    mStatus = status;
    mStatusUpdateTime = updateTime;
    mExtras->Clear();
    if (extras != NULL) {
        mExtras->PutAll(extras);
    }
    mHasStatus = TRUE;
    return NOERROR;
}

ECode MockProvider::ClearStatus()
{
    mHasStatus = FALSE;
    mStatusUpdateTime = 0;
    return NOERROR;
}

ECode MockProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    return Dump(pw, String(""));
}

ECode MockProvider::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Println(prefix + mName);
    pw->Println(prefix + "mHasLocation=" + (mHasLocation ? "TRUE" : "FALSE"));
    pw->Println(prefix + "mLocation:");
    // mLocation->Dump(new PrintWriterPrinter(pw), prefix + "  ");
    pw->Println(prefix + "mHasStatus=" + (mHasStatus ? "TRUE" : "FALSE"));
    // pw->Println(prefix + "mStatus=" + mStatus);
    // pw->Println(prefix + "mStatusUpdateTime=" + mStatusUpdateTime);
    // pw->Println(prefix + "mExtras=" + mExtras);
    return NOERROR;
}

ECode MockProvider::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    return NOERROR;
}

ECode MockProvider::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
