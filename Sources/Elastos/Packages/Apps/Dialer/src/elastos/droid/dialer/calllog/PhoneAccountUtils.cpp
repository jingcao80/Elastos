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

#include "elastos/droid/dialer/calllog/PhoneAccountUtils.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telecom::CPhoneAccountHandle;
using Elastos::Droid::Telecom::ITelecomManager;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

AutoPtr<IPhoneAccountHandle> PhoneAccountUtils::GetAccount(
    /* [in] */ const String& componentString,
    /* [in] */ const String& accountId)
{
    if (TextUtils::IsEmpty(componentString) || TextUtils::IsEmpty(accountId)) {
        return NULL;
    }

    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    AutoPtr<IComponentName> componentName;
    helper->UnflattenFromString(componentString, (IComponentName**)&componentName);

    AutoPtr<IPhoneAccountHandle> handle;
    CPhoneAccountHandle::New(componentName, accountId, (IPhoneAccountHandle**)&handle);
    return handle;
}

AutoPtr<IDrawable> PhoneAccountUtils::GetAccountIcon(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneAccountHandle* phoneAccount)
{
    AutoPtr<IPhoneAccount> account = GetAccountOrNull(context, phoneAccount);
    if (account == NULL) {
        return NULL;
    }
    AutoPtr<IDrawable> drawable;
    account->GetIcon(context, (IDrawable**)&drawable);
    return drawable;
}

String PhoneAccountUtils::GetAccountLabel(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneAccountHandle* phoneAccount)
{
    AutoPtr<IPhoneAccount> account = GetAccountOrNull(context, phoneAccount);
    if (account == NULL) {
        return String(NULL);
    }

    AutoPtr<ICharSequence> lable;
    account->GetLabel((ICharSequence**)&lable);
    String str;
    lable->ToString(&str);
    return str;
}

AutoPtr<IPhoneAccount> PhoneAccountUtils::GetAccountOrNull(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneAccountHandle* phoneAccount)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    AutoPtr<ITelecomManager> telecomManager = ITelecomManager::Probe(service);
    AutoPtr<IPhoneAccount> account;
    telecomManager->GetPhoneAccount(phoneAccount, (IPhoneAccount**)&account);
    Boolean result;
    if (account == NULL || (telecomManager->HasMultipleCallCapableAccounts(&result), !result)) {
        return NULL;
    }
    return account;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
