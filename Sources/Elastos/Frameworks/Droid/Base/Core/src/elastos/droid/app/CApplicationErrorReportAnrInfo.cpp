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
#include "elastos/droid/app/CApplicationErrorReportAnrInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace App {


CAR_INTERFACE_IMPL_2(CApplicationErrorReportAnrInfo, Object, IApplicationErrorReportAnrInfo, IParcelable)

CAR_OBJECT_IMPL(CApplicationErrorReportAnrInfo)

/**
 * Create an uninitialized instance of AnrInfo.
 */
ECode CApplicationErrorReportAnrInfo::constructor()
{
    return NOERROR;
}

/**
 * Activity name.
 */
ECode CApplicationErrorReportAnrInfo::GetActivity(
    /* [out] */ String* activity)
{
    VALIDATE_NOT_NULL(activity);
    *activity = mActivity;
    return NOERROR;
}

ECode CApplicationErrorReportAnrInfo::SetActivity(
    /* [in] */ const String& activity)
{
    mActivity = activity;
    return NOERROR;
}

/**
 * Description of the operation that timed out.
 */
ECode CApplicationErrorReportAnrInfo::GetCause(
    /* [out] */ String* cause)
{
    VALIDATE_NOT_NULL(cause);
    *cause = mCause;
    return NOERROR;
}

ECode CApplicationErrorReportAnrInfo::SetCause(
    /* [in] */ const String& cause)
{
    mCause = cause;
    return NOERROR;
}

/**
 * Additional info, including CPU stats.
 */
ECode CApplicationErrorReportAnrInfo::GetInfo(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = mInfo;
    return NOERROR;
}

ECode CApplicationErrorReportAnrInfo::SetInfo(
    /* [in] */ const String& info)
{
    mInfo = info;
    return NOERROR;
}

/**
 * Dump an AnrInfo instance to a Printer.
 */
ECode CApplicationErrorReportAnrInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    VALIDATE_NOT_NULL(pw);

    StringBuilder sb(30);
    sb.Append(prefix);
    sb.Append("activity:");
    sb.Append(mActivity);
    String str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("cause:");
    sb.Append(mCause);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    sb.Reset();
    sb.Append(prefix);
    sb.Append("info:");
    sb.Append(mInfo);
    str = sb.ToString();
    FAIL_RETURN(pw->Println(str));

    return NOERROR;
}

ECode CApplicationErrorReportAnrInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    FAIL_RETURN(parcel->ReadString(&mActivity));
    FAIL_RETURN(parcel->ReadString(&mCause));
    FAIL_RETURN(parcel->ReadString(&mInfo));

    return NOERROR;
}

ECode CApplicationErrorReportAnrInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mActivity));
    FAIL_RETURN(dest->WriteString(mCause));
    FAIL_RETURN(dest->WriteString(mInfo));

    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
