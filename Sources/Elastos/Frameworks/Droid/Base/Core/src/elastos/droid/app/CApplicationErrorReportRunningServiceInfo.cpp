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

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/CApplicationErrorReportRunningServiceInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CApplicationErrorReportRunningServiceInfo, Object, IApplicationErrorReportRunningServiceInfo, IParcelable)

CAR_OBJECT_IMPL(CApplicationErrorReportRunningServiceInfo)

CApplicationErrorReportRunningServiceInfo::CApplicationErrorReportRunningServiceInfo()
    : mDurationMillis(0)
{
}

/**
 * Create an uninitialized instance of RunningServiceInfo.
 */
ECode CApplicationErrorReportRunningServiceInfo::constructor()
{
    return NOERROR;
}

/**
 * Duration in milliseconds that the service has been running.
 */
ECode CApplicationErrorReportRunningServiceInfo::GetDurationMillis(
    /* [out] */ Int64* durationMillis)
{
    VALIDATE_NOT_NULL(durationMillis);
    *durationMillis = mDurationMillis;
    return NOERROR;
}

ECode CApplicationErrorReportRunningServiceInfo::SetDurationMillis(
    /* [in] */ Int64 durationMillis)
{
    mDurationMillis = durationMillis;
    return NOERROR;
}

/**
 * Dump of debug information about the service.
 */
ECode CApplicationErrorReportRunningServiceInfo::GetServiceDetails(
    /* [out] */ String* serviceDetails)
{
    VALIDATE_NOT_NULL(serviceDetails);
    *serviceDetails = mServiceDetails;
    return NOERROR;
}

ECode CApplicationErrorReportRunningServiceInfo::SetServiceDetails(
    /* [in] */ const String& serviceDetails)
{
    mServiceDetails = serviceDetails;
    return NOERROR;
}

/**
 * Dump a RunningService instance to a Printer.
 */
ECode CApplicationErrorReportRunningServiceInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);

    StringBuilder sb(128);
    sb.Append(prefix);
    sb.Append("durationMillis:");
    sb.Append(mDurationMillis);
    String str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("serviceDetails:");
    sb.Append(mServiceDetails);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    return NOERROR;
}

ECode CApplicationErrorReportRunningServiceInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    FAIL_RETURN(parcel->ReadInt64(&mDurationMillis));
    FAIL_RETURN(parcel->ReadString(&mServiceDetails));

    return NOERROR;
}

ECode CApplicationErrorReportRunningServiceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt64(mDurationMillis));
    FAIL_RETURN(dest->WriteString(mServiceDetails));

    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
