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

#include "elastos/droid/wifi/CWifiConfigurationKeyMgmt.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > InitStrings()
{
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(5);
    (*strings)[0] = "NONE";
    (*strings)[1] = "WPA_PSK";
    (*strings)[2] = "WPA_EAP";
    (*strings)[3] = "IEEE8021X";
    (*strings)[4] = "WPA2_PSK";
    return strings;
}

const AutoPtr< ArrayOf<String> > CWifiConfigurationKeyMgmt::sStrings = InitStrings();

CAR_INTERFACE_IMPL(CWifiConfigurationKeyMgmt, Singleton, IWifiConfigurationKeyMgmt)

CAR_SINGLETON_IMPL(CWifiConfigurationKeyMgmt)

ECode CWifiConfigurationKeyMgmt::GetStrings(
    /* [out, callee] */ ArrayOf<String>** strings)
{
    VALIDATE_NOT_NULL(strings);
    *strings = sStrings;
    REFCOUNT_ADD(*strings);
    return NOERROR;
}

ECode CWifiConfigurationKeyMgmt::ToString(
    /* [in] */ Int32 index,
    /* [out] */ String* strValue)
{
    VALIDATE_NOT_NULL(strValue);
    if (index < 0 || index > 4) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    *strValue = (*sStrings)[index];
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
