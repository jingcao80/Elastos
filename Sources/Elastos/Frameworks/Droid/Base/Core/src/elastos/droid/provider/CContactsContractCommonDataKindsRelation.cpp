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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsRelation.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/coredef.h>
#include "elastos/droid/R.h"

using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsRelation)

CAR_INTERFACE_IMPL_3(CContactsContractCommonDataKindsRelation, Singleton
    , IContactsContractCommonDataKindsRelation
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns)

ECode CContactsContractCommonDataKindsRelation::GetTypeLabelResource(
    /* [in] */ Int32 type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    switch (type) {
        case TYPE_ASSISTANT:
            *resource = Elastos::Droid::R::string::relationTypeAssistant;
            break;
        case TYPE_BROTHER:
            *resource = Elastos::Droid::R::string::relationTypeBrother;
            break;
        case TYPE_CHILD:
            *resource = Elastos::Droid::R::string::relationTypeChild;
            break;
        case TYPE_DOMESTIC_PARTNER:
            *resource = Elastos::Droid::R::string::relationTypeDomesticPartner;
            break;
        case TYPE_FATHER:
            *resource = Elastos::Droid::R::string::relationTypeFather;
            break;
        case TYPE_FRIEND:
            *resource = Elastos::Droid::R::string::relationTypeFriend;
            break;
        case TYPE_MANAGER:
            *resource = Elastos::Droid::R::string::relationTypeManager;
            break;
        case TYPE_MOTHER:
            *resource = Elastos::Droid::R::string::relationTypeMother;
            break;
        case TYPE_PARENT:
            *resource = Elastos::Droid::R::string::relationTypeParent;
            break;
        case TYPE_PARTNER:
            *resource = Elastos::Droid::R::string::relationTypePartner;
            break;
        case TYPE_REFERRED_BY:
            *resource = Elastos::Droid::R::string::relationTypeReferredBy;
            break;
        case TYPE_RELATIVE:
            *resource = Elastos::Droid::R::string::relationTypeRelative;
            break;
        case TYPE_SISTER:
            *resource = Elastos::Droid::R::string::relationTypeSister;
            break;
        case TYPE_SPOUSE:
            *resource = Elastos::Droid::R::string::relationTypeSpouse;
            break;
        default:
            *resource = Elastos::Droid::R::string::orgTypeCustom;
            break;
    }
     return NOERROR;
}

ECode CContactsContractCommonDataKindsRelation::GetTypeLabel(
    /* [in] */ IResources* res,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    if (type == IContactsContractCommonDataKindsBaseTypes::TYPE_CUSTOM && !TextUtils::IsEmpty(label)) {
        *lb = label;
        REFCOUNT_ADD(*lb);
        return NOERROR;
    } else {
        Int32 labelRes;
        FAIL_RETURN(GetTypeLabelResource(type, &labelRes))
        return res->GetText(labelRes, lb);
    }
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos