
#include "elastos/droid/accounts/AccountAuthenticatorActivity.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

void AccountAuthenticatorActivity::SetAccountAuthenticatorResult(
    /* [in] */ IBundle* result)
{
    mResultBundle = result;
}

ECode AccountAuthenticatorActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    FAIL_RETURN(Activity::OnCreate(icicle));

    AutoPtr<IParcelable> parcelable;
    AutoPtr<IIntent> intent;
    FAIL_RETURN(GetIntent((IIntent**)&intent));
    FAIL_RETURN(intent->GetParcelableExtra(
            IAccountManager::KEY_ACCOUNT_AUTHENTICATOR_RESPONSE, (IParcelable**)&parcelable));
    mAccountAuthenticatorResponse = (IAccountAuthenticatorResponse*)
            parcelable->Probe(EIID_IAccountAuthenticatorResponse);
    if (mAccountAuthenticatorResponse != NULL) {
        return mAccountAuthenticatorResponse->OnRequestContinued();
    }
    return NOERROR;
}

ECode AccountAuthenticatorActivity::Finish()
{
    if (mAccountAuthenticatorResponse != NULL) {
        // send the result bundle back if set, otherwise send an error.
        if (mResultBundle != NULL) {
            return mAccountAuthenticatorResponse->OnResult(mResultBundle);
        }
        else {
            return mAccountAuthenticatorResponse->OnError(IAccountManager::ERROR_CODE_CANCELED,
                    String("canceled"));
        }
        mAccountAuthenticatorResponse = NULL;
    }
    return Activity::Finish();
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
