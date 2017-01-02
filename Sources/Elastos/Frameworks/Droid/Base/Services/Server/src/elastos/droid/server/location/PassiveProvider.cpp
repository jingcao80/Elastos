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

#include "elastos/droid/server/location/PassiveProvider.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

// using Elastos::Droid::Internal::Location::CProviderProperties;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String PassiveProvider::TAG("PassiveProvider");

CAR_INTERFACE_IMPL(PassiveProvider, Object, ILocationProviderInterface)

static AutoPtr<IProviderProperties> PROPERTIES_Init()
{
    // TODO:
    // AutoPtr<IProviderProperties> temp;
    // CProviderProperties::New(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    //     ICriteria::Criteria_POWER_LOW, ICriteria::Criteria_ACCURACY_COARSE,
    //     (IProviderProperties**)&temp);
    // return temp;
    return NULL;
}

AutoPtr<IProviderProperties> PassiveProvider::PROPERTIES = PROPERTIES_Init();

PassiveProvider::PassiveProvider(
    /* [in] */ IILocationManager* locationManager)
    : mLocationManager(locationManager)
    , mReportLocation(FALSE)
{}

ECode PassiveProvider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = ILocationManager::PASSIVE_PROVIDER;
    return NOERROR;
}

ECode PassiveProvider::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties);
    *properties = PROPERTIES;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

ECode PassiveProvider::IsEnabled(
    /* [out] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    *enable = TRUE;
    return NOERROR;
}

ECode PassiveProvider::Enable()
{
    return NOERROR;
}

ECode PassiveProvider::Disable()
{
    return NOERROR;
}

ECode PassiveProvider::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    if (mReportLocation) {
        *status = ILocationProvider::AVAILABLE;
    } else {
        *status = ILocationProvider::TEMPORARILY_UNAVAILABLE;
    }
    return NOERROR;
}

ECode PassiveProvider::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = -1;
    return NOERROR;
}

ECode PassiveProvider::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    Boolean rl;
    request->GetReportLocation(&rl);
    mReportLocation = rl;
    return NOERROR;
}

ECode PassiveProvider::UpdateLocation(
    /* [in] */ ILocation* location)
{
    if (mReportLocation) {
        // pass the location back to the location manager
        ECode ec = mLocationManager->ReportLocation(location, TRUE);
        if (FAILED(ec)) {
            Logger::E(TAG, "RemoteException calling reportLocation");
            return E_REMOTE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode PassiveProvider::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode PassiveProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StringBuilder sb("mReportLocation=");
    sb += (mReportLocation ? "TRUE" : "FALSE");
    pw->Println(sb.ToString());
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
