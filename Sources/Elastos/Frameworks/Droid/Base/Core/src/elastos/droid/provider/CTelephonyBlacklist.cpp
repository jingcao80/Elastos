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

#include "elastos/droid/provider/CTelephonyBlacklist.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyBlacklist, Singleton, ITelephonyBlacklist, IBaseColumns);
CAR_SINGLETON_IMPL(CTelephonyBlacklist);

ECode CTelephonyBlacklist::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Blacklist::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_FILTER_BYNUMBER_URI(
     /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Telephony::Blacklist::CONTENT_FILTER_BYNUMBER_URI;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_PHONE_URI(
     /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Telephony::Blacklist::CONTENT_PHONE_URI;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyBlacklist::GetCONTENT_MESSAGE_URI(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Telephony::Blacklist::CONTENT_MESSAGE_URI;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
