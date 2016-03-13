#ifndef __ELASTOS_DROID_ACCOUNTS_ACCOUNTAUTHENTICATORACTIVITY_H__
#define __ELASTOS_DROID_ACCOUNTS_ACCOUNTAUTHENTICATORACTIVITY_H__

//#include "Elastos.Droid.Core.Server.h"
#include "_Elastos.Droid.Accounts.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Accounts {

/**
 * Base class for implementing an Activity that is used to help implement an
 * AbstractAccountAuthenticator. If the AbstractAccountAuthenticator needs to use an activity
 * to handle the request then it can have the activity extend AccountAuthenticatorActivity.
 * The AbstractAccountAuthenticator passes in the response to the intent using the following:
 * <pre>
 *      intent.putExtra({@link AccountManager#KEY_ACCOUNT_AUTHENTICATOR_RESPONSE}, response);
 * </pre>
 * The activity then sets the result that is to be handed to the response via
 * {@link #setAccountAuthenticatorResult(android.os.Bundle)}.
 * This result will be sent as the result of the request when the activity finishes. If this
 * is never set or if it is set to null then error {@link AccountManager#ERROR_CODE_CANCELED}
 * will be called on the response.
 */
class AccountAuthenticatorActivity
    : public Activity
    , public IAccountAuthenticatorActivity
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Set the result that is to be sent as the result of the request that caused this
     * Activity to be launched. If result is null or this method is never called then
     * the request will be canceled.
     * @param result this is returned as the result of the AbstractAccountAuthenticator request
     */
    CARAPI SetAccountAuthenticatorResult(
        /* [in] */ IBundle* result);

    /**
     * Sends the result or a Constants.ERROR_CODE_CANCELED error if a result isn't present.
     */
    CARAPI Finish();

protected:
    /**
     * Retreives the AccountAuthenticatorResponse from either the intent of the icicle, if the
     * icicle is non-zero.
     * @param icicle the save instance data of this Activity, may be null
     */
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    AutoPtr<IAccountAuthenticatorResponse> mAccountAuthenticatorResponse;
    AutoPtr<IBundle> mResultBundle;
};

} // Accounts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_ACCOUNTAUTHENTICATORACTIVITY_H__
