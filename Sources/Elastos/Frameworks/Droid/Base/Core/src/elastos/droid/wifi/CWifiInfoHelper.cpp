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

#include "elastos/droid/wifi/CWifiInfoHelper.h"
#include "elastos/droid/wifi/CWifiInfo.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiInfoHelper, Singleton, IWifiInfoHelper)

CAR_SINGLETON_IMPL(CWifiInfoHelper)

ECode CWifiInfoHelper::GetDetailedStateOf(
    /* [in] */ ISupplicantState* suppState,
    /* [out] */ NetworkInfoDetailedState* detailedState)
{
    VALIDATE_NOT_NULL(detailedState);
    SupplicantState ss;
    suppState->Get(&ss);
    *detailedState = CWifiInfo::GetDetailedStateOf(ss);
    return NOERROR;
}

ECode CWifiInfoHelper::RemoveDoubleQuotes(
    /* [in] */ const String& str,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    CWifiInfo::RemoveDoubleQuotes(str, value);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
