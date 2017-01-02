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

#include "elastos/droid/wifi/CWifiEnterpriseConfigPhase2.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > strings_init()
{
    const char* array[] = { IWifiEnterpriseConfig::EMPTY_VALUE, "PAP", "MSCHAP",
                "MSCHAPV2", "GTC" };
    Int32 length = sizeof(array)/sizeof(array[0]);
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        (*strings)[i] = array[i];
    }

    return strings;
}

const String CWifiEnterpriseConfigPhase2::PREFIX("auth=");

CAR_INTERFACE_IMPL(CWifiEnterpriseConfigPhase2, Singleton, IWifiEnterpriseConfigEap)

CAR_SINGLETON_IMPL(CWifiEnterpriseConfigPhase2)

CWifiEnterpriseConfigPhase2::CWifiEnterpriseConfigPhase2()
    : mStrings(strings_init())
{
}

ECode CWifiEnterpriseConfigPhase2::GetStrings(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStrings;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
