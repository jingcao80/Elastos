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

#include "elastos/droid/internal/telephony/utility/CBlacklistUtils.h"
#include "elastos/droid/internal/telephony/utility/BlacklistUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Utility {

CAR_SINGLETON_IMPL(CBlacklistUtils)
CAR_INTERFACE_IMPL(CBlacklistUtils, Singleton, IBlacklistUtils)

ECode CBlacklistUtils::AddOrUpdate(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 valid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::AddOrUpdate(context, number, flags, valid);
    return NOERROR;
}

ECode CBlacklistUtils::IsListed(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int32 mode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsListed(context, number, mode);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistEnabled(context);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistNotifyEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistNotifyEnabled(context);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistPrivateNumberEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistPrivateNumberEnabled(context, mode);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistUnknownNumberEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistUnknownNumberEnabled(context, mode);
    return NOERROR;
}

ECode CBlacklistUtils::IsBlacklistRegexEnabled(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BlacklistUtils::IsBlacklistRegexEnabled(context);
    return NOERROR;
}


} // namespace Utility
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
