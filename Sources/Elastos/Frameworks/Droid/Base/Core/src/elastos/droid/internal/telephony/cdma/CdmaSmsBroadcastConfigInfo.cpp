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

#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaSmsBroadcastConfigInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                      CdmaSmsBroadcastConfigInfo
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSmsBroadcastConfigInfo, Object, ICdmaSmsBroadcastConfigInfo);

CdmaSmsBroadcastConfigInfo::CdmaSmsBroadcastConfigInfo()
    : mFromServiceCategory(0)
    , mToServiceCategory(0)
    , mLanguage(0)
    , mSelected(FALSE)
{
}

ECode CdmaSmsBroadcastConfigInfo::constructor(
    /* [in] */ Int32 fromServiceCategory,
    /* [in] */ Int32 toServiceCategory,
    /* [in] */ Int32 language,
    /* [in] */ Boolean selected)
{
    mFromServiceCategory = fromServiceCategory;
    mToServiceCategory = toServiceCategory;
    mLanguage = language;
    mSelected = selected;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetFromServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFromServiceCategory;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetToServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mToServiceCategory;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetLanguage(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLanguage;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::IsSelected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSelected;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    *result = String("CdmaSmsBroadcastConfigInfo: Id [")
            + StringUtils::ToString(mFromServiceCategory) + ", "
            + StringUtils::ToString(mToServiceCategory) + "] "
            + ((IsSelected(&b), b) ? "ENABLED" : "DISABLED");
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


