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

#include "elastos/droid/wifi/CWpsInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWpsInfo, Object, IWpsInfo)

CAR_OBJECT_IMPL(CWpsInfo)

ECode CWpsInfo::constructor()
{
    mSetup = IWpsInfo::INVALID;
    return NOERROR;
}

ECode CWpsInfo::constructor(
    /* [in] */ IWpsInfo* source)
{
    if (source != NULL) {
        FAIL_RETURN(source->GetSetup(&mSetup));
        FAIL_RETURN(source->GetBSSID(&mBSSID));
        FAIL_RETURN(source->GetPin(&mPin));
    }

    return NOERROR;
}

ECode CWpsInfo::GetSetup(
    /* [out] */ Int32* setup)
{
    VALIDATE_NOT_NULL(setup);
    *setup = mSetup;

    return NOERROR;
}

ECode CWpsInfo::SetSetup(
    /* [in] */ Int32 setup)
{
    mSetup = setup;

    return NOERROR;
}

ECode CWpsInfo::GetBSSID(
    /* [out] */ String* BSSID)
{
    VALIDATE_NOT_NULL(BSSID);

    *BSSID = mBSSID;

    return NOERROR;
}

ECode CWpsInfo::SetBSSID(
    /* [in] */ const String& BSSID)
{
    mBSSID = BSSID;

    return NOERROR;
}

ECode CWpsInfo::GetPin(
    /* [out] */ String* pin)
{
    VALIDATE_NOT_NULL(pin);

    *pin = mPin;

    return NOERROR;
}

ECode CWpsInfo::SetPin(
    /* [in] */ const String& pin)
{
    mPin = pin;

    return NOERROR;
}

ECode CWpsInfo::ToString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    StringBuilder sb;
    sb += " setup: ";
    sb += mSetup;
    sb += "\n BSSID: ";
    sb += mBSSID;
    sb += "\n pin:";
    sb += mPin;
    sb += "\n";

    *value = sb.ToString();

    return NOERROR;
}

ECode CWpsInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadInt32(&mSetup));
    FAIL_RETURN(source->ReadString(&mBSSID));
    FAIL_RETURN(source->ReadString(&mPin));

    return NOERROR;
}

ECode CWpsInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt32(mSetup));
    FAIL_RETURN(dest->WriteString(mBSSID));
    FAIL_RETURN(dest->WriteString(mPin));

    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
