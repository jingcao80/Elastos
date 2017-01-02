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

#include "CContactDisplayUtils.h"
#include "ContactDisplayUtils.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CAR_INTERFACE_IMPL(CContactDisplayUtils, Singleton, IContactDisplayUtils)

CAR_SINGLETON_IMPL(CContactDisplayUtils)

ECode CContactDisplayUtils::IsCustomPhoneType(
    /* [in] */ IInteger32* type,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    *result = ContactDisplayUtils::IsCustomPhoneType(type);
    return NOERROR;
}

ECode CContactDisplayUtils::GetLabelForCallOrSms(
    /* [in] */ IInteger32* type,
    /* [in] */ ICharSequence* customLabel,
    /* [in] */ Int32 interactionType,
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    VALUE_NOT_NULL(result);

    AutoPtr<ICharSequence> cs = ContactDisplayUtils::GetLabelForCallOrSms(
            type, customLabel, interactionType, context);
    *result = cs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CContactDisplayUtils::GetPhoneLabelResourceId(
    /* [in] */ IInteger32* type,
    /* [out] */ Int32* id)
{
    VALUE_NOT_NULL(id);

    *id = ContactDisplayUtils::GetPhoneLabelResourceId(type);
    return NOERROR;
}

ECode CContactDisplayUtils::GetSmsLabelResourceId(
    /* [in] */ IInteger32* type,
    /* [out] */ Int32* id)
{
    VALUE_NOT_NULL(id);

    *id = ContactDisplayUtils::GetSmsLabelResourceId(type);
    return NOERROR;
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
