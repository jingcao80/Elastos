
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
    ITelecomManager* telecomManager = ITelecomManager::Probe(service);
    AutoPtr<IPhoneAccount> account;
    telecomManager->GetPhoneAccount(phoneAccount, (IPhoneAccount**)&account);
    Boolean result;
    if (account == NULL || telecomManager->HasMultipleCallCapableAccounts(&result), !result) {
        return NULL;
    }
    return account;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
