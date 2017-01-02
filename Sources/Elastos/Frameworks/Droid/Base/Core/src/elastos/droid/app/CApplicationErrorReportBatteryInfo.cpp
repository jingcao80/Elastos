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
#include "elastos/droid/app/CApplicationErrorReportBatteryInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CApplicationErrorReportBatteryInfo, Object, IApplicationErrorReportBatteryInfo, IParcelable)

CAR_OBJECT_IMPL(CApplicationErrorReportBatteryInfo)

CApplicationErrorReportBatteryInfo::CApplicationErrorReportBatteryInfo()
    : mUsagePercent(0)
    , mDurationMicros(0)
{

}
/**
 * Create an uninitialized instance of BatteryInfo.
 */
ECode CApplicationErrorReportBatteryInfo::constructor()
{
    return NOERROR;
}

/**
 * Percentage of the battery that was used up by the process.
 */
ECode CApplicationErrorReportBatteryInfo::GetUsagePercent(
    /* [out] */ Int32* usagePercent)
{
    VALIDATE_NOT_NULL(usagePercent);
    *usagePercent = mUsagePercent;
    return NOERROR;
}

ECode CApplicationErrorReportBatteryInfo::SetUsagePercent(
    /* [in] */ Int32 usagePercent)
{
    mUsagePercent = usagePercent;
    return NOERROR;
}

/**
 * Duration in microseconds over which the process used the above
 * percentage of battery.
 */
ECode CApplicationErrorReportBatteryInfo::GetDurationMicros(
    /* [out] */ Int64* durationMicros)
{
    VALIDATE_NOT_NULL(durationMicros);
    *durationMicros = mDurationMicros;
    return NOERROR;
}

ECode CApplicationErrorReportBatteryInfo::SetDurationMicros(
    /* [in] */ Int64 durationMicros)
{
    mDurationMicros = durationMicros;
    return NOERROR;
}

/**
 * Dump of various info impacting battery use.
 */
ECode CApplicationErrorReportBatteryInfo::GetUsageDetails(
    /* [out] */ String* usageDetails)
{
    VALIDATE_NOT_NULL(usageDetails);
    *usageDetails = mUsageDetails;
    return NOERROR;
}

ECode CApplicationErrorReportBatteryInfo::SetUsageDetails(
    /* [in] */ const String& usageDetails)
{
    mUsageDetails = usageDetails;
    return NOERROR;
}

/**
 * Checkin details.
 */
ECode CApplicationErrorReportBatteryInfo::GetCheckinDetails(
    /* [out] */ String* checkinDetails)
{
    VALIDATE_NOT_NULL(checkinDetails);
    *checkinDetails = mCheckinDetails;
    return NOERROR;
}

ECode CApplicationErrorReportBatteryInfo::SetCheckinDetails(
    /* [in] */ const String& checkinDetails)
{
    mCheckinDetails = checkinDetails;
    return NOERROR;
}

/**
 * Dump a BatteryInfo instance to a Printer.
 */
ECode CApplicationErrorReportBatteryInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);

    StringBuilder sb(30);
    sb.Append(prefix);
    sb.Append("usagePercent:");
    sb.Append(mUsagePercent);
    String str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("durationMicros:");
    sb.Append(mDurationMicros);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("usageDetails:");
    sb.Append(mUsageDetails);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("checkinDetails:");
    sb.Append(mCheckinDetails);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    return NOERROR;
}

ECode CApplicationErrorReportBatteryInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    FAIL_RETURN(parcel->ReadInt32(&mUsagePercent));
    FAIL_RETURN(parcel->ReadInt64(&mDurationMicros));
    FAIL_RETURN(parcel->ReadString(&mUsageDetails));
    FAIL_RETURN(parcel->ReadString(&mCheckinDetails));

    return NOERROR;
}

ECode CApplicationErrorReportBatteryInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt32(mUsagePercent));
    FAIL_RETURN(dest->WriteInt64(mDurationMicros));
    FAIL_RETURN(dest->WriteString(mUsageDetails));
    FAIL_RETURN(dest->WriteString(mCheckinDetails));

    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
