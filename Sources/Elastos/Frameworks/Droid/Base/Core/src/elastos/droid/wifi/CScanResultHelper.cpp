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

#include "elastos/droid/wifi/CScanResultHelper.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CScanResultHelper, Singleton, IScanResultHelper)

CAR_SINGLETON_IMPL(CScanResultHelper)

ECode CScanResultHelper::Is24GHz(
    /* [in] */ Int32 freq,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = freq > 2400 && freq < 2500;
    return NOERROR;
}

ECode CScanResultHelper::Is5GHz(
    /* [in] */ Int32 freq,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = freq > 4900 && freq < 5900;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
