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

#include "ContactDisplayUtils.h"

using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

const String ContactDisplayUtils::TAG("ContactDisplayUtils");

Boolean ContactDisplayUtils::IsCustomPhoneType(
        /* [in] */ IInteger32* type)
{
    Int32 value;
    type->GetValue(&value);
    return value == Phone.TYPE_CUSTOM || value == Phone.TYPE_ASSISTANT;
}

AutoPtr<ICharSequence> ContactDisplayUtils::GetLabelForCallOrSms(
    /* [in] */ IInteger32* type,
    /* [in] */ ICharSequence* customLabel,
    /* [in] */ Int32 interactionType,
    /* [in] */ IContext* context)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    preconditions->CheckNotNull(context);

    if (IsCustomPhoneType(type)) {
        return (customLabel == NULL) ? CoreUtils::Convert(String("")) : customLabel;
    }
    else {
        Int32 resId;
        if (interactionType == INTERACTION_SMS) {
            resId = GetSmsLabelResourceId(type);
        }
        else {
            resId = GetPhoneLabelResourceId(type);
            if (interactionType != INTERACTION_CALL) {
                Logger::E(TAG,
                        "Un-recognized interaction type: %d. Defaulting to ContactDisplayUtils.INTERACTION_CALL.",
                        interactionType);
            }
        }

        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> result;
        resources->GetText(resId, (ICharSequence**)&result);
        return result;
    }
}

Int32 ContactDisplayUtils::GetPhoneLabelResourceId(
    /* [in] */ IInteger32* type)
{
    if (type == NULL) return R.string.call_other;

    Int32 value;
    type->GetValue(&value);
    switch (value) {
        case Phone.TYPE_HOME:
            return R.string.call_home;
        case Phone.TYPE_MOBILE:
            return R.string.call_mobile;
        case Phone.TYPE_WORK:
            return R.string.call_work;
        case Phone.TYPE_FAX_WORK:
            return R.string.call_fax_work;
        case Phone.TYPE_FAX_HOME:
            return R.string.call_fax_home;
        case Phone.TYPE_PAGER:
            return R.string.call_pager;
        case Phone.TYPE_OTHER:
            return R.string.call_other;
        case Phone.TYPE_CALLBACK:
            return R.string.call_callback;
        case Phone.TYPE_CAR:
            return R.string.call_car;
        case Phone.TYPE_COMPANY_MAIN:
            return R.string.call_company_main;
        case Phone.TYPE_ISDN:
            return R.string.call_isdn;
        case Phone.TYPE_MAIN:
            return R.string.call_main;
        case Phone.TYPE_OTHER_FAX:
            return R.string.call_other_fax;
        case Phone.TYPE_RADIO:
            return R.string.call_radio;
        case Phone.TYPE_TELEX:
            return R.string.call_telex;
        case Phone.TYPE_TTY_TDD:
            return R.string.call_tty_tdd;
        case Phone.TYPE_WORK_MOBILE:
            return R.string.call_work_mobile;
        case Phone.TYPE_WORK_PAGER:
            return R.string.call_work_pager;
        case Phone.TYPE_ASSISTANT:
            return R.string.call_assistant;
        case Phone.TYPE_MMS:
            return R.string.call_mms;
        default:
            return R.string.call_custom;
    }
}

Int32 ContactDisplayUtils::GetSmsLabelResourceId(
    /* [in] */ IInteger32* type)
{
    if (type == NULL) return R.string.sms_other;

    Int32 value;
    type->GetValue(&value);
    switch (value) {
        case Phone.TYPE_HOME:
            return R.string.sms_home;
        case Phone.TYPE_MOBILE:
            return R.string.sms_mobile;
        case Phone.TYPE_WORK:
            return R.string.sms_work;
        case Phone.TYPE_FAX_WORK:
            return R.string.sms_fax_work;
        case Phone.TYPE_FAX_HOME:
            return R.string.sms_fax_home;
        case Phone.TYPE_PAGER:
            return R.string.sms_pager;
        case Phone.TYPE_OTHER:
            return R.string.sms_other;
        case Phone.TYPE_CALLBACK:
            return R.string.sms_callback;
        case Phone.TYPE_CAR:
            return R.string.sms_car;
        case Phone.TYPE_COMPANY_MAIN:
            return R.string.sms_company_main;
        case Phone.TYPE_ISDN:
            return R.string.sms_isdn;
        case Phone.TYPE_MAIN:
            return R.string.sms_main;
        case Phone.TYPE_OTHER_FAX:
            return R.string.sms_other_fax;
        case Phone.TYPE_RADIO:
            return R.string.sms_radio;
        case Phone.TYPE_TELEX:
            return R.string.sms_telex;
        case Phone.TYPE_TTY_TDD:
            return R.string.sms_tty_tdd;
        case Phone.TYPE_WORK_MOBILE:
            return R.string.sms_work_mobile;
        case Phone.TYPE_WORK_PAGER:
            return R.string.sms_work_pager;
        case Phone.TYPE_ASSISTANT:
            return R.string.sms_assistant;
        case Phone.TYPE_MMS:
            return R.string.sms_mms;
        default:
            return R.string.sms_custom;
    }
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
