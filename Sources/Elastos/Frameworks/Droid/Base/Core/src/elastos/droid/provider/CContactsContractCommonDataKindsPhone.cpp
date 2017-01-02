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
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/ContactsContractData.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsPhone.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Net::Uri;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsPhone)

CAR_INTERFACE_IMPL_3(CContactsContractCommonDataKindsPhone, Singleton
    , IContactsContractCommonDataKindsPhone
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns)

ECode CContactsContractCommonDataKindsPhone::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(ContactsContractData::GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, String("phones"), uri);
}

ECode CContactsContractCommonDataKindsPhone::GetCONTENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, String("filter"), uri);
}

ECode CContactsContractCommonDataKindsPhone::GetDisplayLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [in] */ const ArrayOf<ICharSequence*>& labelArray,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res))
    return GetTypeLabel(res, type, label, lb);
}

ECode CContactsContractCommonDataKindsPhone::GetDisplayLabel(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res))
    return GetTypeLabel(res, type, label, lb);
}

ECode CContactsContractCommonDataKindsPhone::GetTypeLabelResource(
    /* [in] */ Int32 type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    switch (type) {
        case TYPE_HOME:
            *resource = Elastos::Droid::R::string::phoneTypeHome;
            break;
        case TYPE_MOBILE:
            *resource = Elastos::Droid::R::string::phoneTypeMobile;
            break;
        case TYPE_WORK:
            *resource = Elastos::Droid::R::string::phoneTypeWork;
            break;
        case TYPE_FAX_WORK:
            *resource = Elastos::Droid::R::string::phoneTypeFaxWork;
            break;
        case TYPE_FAX_HOME:
            *resource = Elastos::Droid::R::string::phoneTypeFaxHome;
            break;
        case TYPE_PAGER:
            *resource = Elastos::Droid::R::string::phoneTypePager;
            break;
        case TYPE_OTHER:
            *resource = Elastos::Droid::R::string::phoneTypeOther;
            break;
        case TYPE_CALLBACK:
            *resource = Elastos::Droid::R::string::phoneTypeCallback;
            break;
        case TYPE_CAR:
            *resource = Elastos::Droid::R::string::phoneTypeCar;
            break;
        case TYPE_COMPANY_MAIN:
            *resource = Elastos::Droid::R::string::phoneTypeCompanyMain;
            break;
        case TYPE_ISDN:
            *resource = Elastos::Droid::R::string::phoneTypeIsdn;
            break;
        case TYPE_MAIN:
            *resource = Elastos::Droid::R::string::phoneTypeMain;
            break;
        case TYPE_OTHER_FAX:
            *resource = Elastos::Droid::R::string::phoneTypeOtherFax;
            break;
        case TYPE_RADIO:
            *resource = Elastos::Droid::R::string::phoneTypeRadio;
            break;
        case TYPE_TELEX:
            *resource = Elastos::Droid::R::string::phoneTypeTelex;
            break;
        case TYPE_TTY_TDD:
            *resource = Elastos::Droid::R::string::phoneTypeTtyTdd;
            break;
        case TYPE_WORK_MOBILE:
            *resource = Elastos::Droid::R::string::phoneTypeWorkMobile;
            break;
        case TYPE_WORK_PAGER:
            *resource = Elastos::Droid::R::string::phoneTypeWorkPager;
            break;
        case TYPE_ASSISTANT:
            *resource = Elastos::Droid::R::string::phoneTypeAssistant;
            break;
        case TYPE_MMS:
            *resource = Elastos::Droid::R::string::phoneTypeMms;
            break;
        default:
            *resource = Elastos::Droid::R::string::phoneTypeCustom;
            break;
    }
     return NOERROR;
}

/**
 * Return a {@link CharSequence} that best describes the given type,
 * possibly substituting the given {@link #LABEL} value
 * for {@link #TYPE_CUSTOM}.
 */
ECode CContactsContractCommonDataKindsPhone::GetTypeLabel(
    /* [in] */ IResources* res,
    /* [in] */ Int32 type,
    /* [in] */ ICharSequence* label,
    /* [out] */ ICharSequence** lb)
{
    VALIDATE_NOT_NULL(lb);

    if ((type == IContactsContractCommonDataKindsBaseTypes::TYPE_CUSTOM || type == TYPE_ASSISTANT) && !TextUtils::IsEmpty(label)) {
        *lb = label;
        REFCOUNT_ADD(*lb);
        return NOERROR;
    } else {
        Int32 labelRes;
        FAIL_RETURN(GetTypeLabelResource(type, &labelRes))
        return res->GetText(labelRes, lb);
    }
}

} //Provider
} //Droid
} //Elastos
