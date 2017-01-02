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

#ifndef __ELASTOS_DROID_ACCOUNTS_ABSTRACTACCOUNTAUTHENTICATOR_H__
#define __ELASTOS_DROID_ACCOUNTS_ABSTRACTACCOUNTAUTHENTICATOR_H__

#include "Elastos.Droid.Core.Server.h"
#include "elastos/droid/accounts/CAccountAuthenticatorTransport.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Accounts {

/**
 * Abstract base class for creating AccountAuthenticators.
 * In order to be an authenticator one must extend this class, provider implementations for the
 * abstract methods and write a service that returns the result of {@link #getIBinder()}
 * in the service's {@link android.app.Service#onBind(android.content.Intent)} when invoked
 * with an intent with action {@link AccountManager#ACTION_AUTHENTICATOR_INTENT}. This service
 * must specify the following intent filter and metadata tags in its AndroidManifest.xml file
 * <pre>
 *   &lt;intent-filter&gt;
 *     &lt;action android:name="android.accounts.AccountAuthenticator" /&gt;
 *   &lt;/intent-filter&gt;
 *   &lt;meta-data android:name="android.accounts.AccountAuthenticator"
 *             android:resource="@xml/authenticator" /&gt;
 * </pre>
 * The <code>android:resource</code> attribute must point to a resource that looks like:
 * <pre>
 * &lt;account-authenticator xmlns:android="http://schemas.android.com/apk/res/android"
 *    android:accountType="typeOfAuthenticator"
 *    android:icon="@drawable/icon"
 *    android:smallIcon="@drawable/miniIcon"
 *    android:label="@string/label"
 *    android:accountPreferences="@xml/account_preferences"
 * /&gt;
 * </pre>
 * Replace the icons and labels with your own resources. The <code>android:accountType</code>
 * attribute must be a string that uniquely identifies your authenticator and will be the same
 * string that user will use when making calls on the {@link AccountManager} and it also
 * corresponds to {@link Account#type} for your accounts. One user of the android:icon is the
 * "Account & Sync" settings page and one user of the android:smallIcon is the Contact Application's
 * tab panels.
 * <p>
 * The preferences attribute points to a PreferenceScreen xml hierarchy that contains
 * a list of PreferenceScreens that can be invoked to manage the authenticator. An example is:
 * <pre>
 * &lt;PreferenceScreen xmlns:android="http://schemas.android.com/apk/res/android"&gt;
 *    &lt;PreferenceCategory android:title="@string/title_fmt" /&gt;
 *    &lt;PreferenceScreen
 *         android:key="key1"
 *         android:title="@string/key1_action"
 *         android:summary="@string/key1_summary"&gt;
 *         &lt;intent
 *             android:action="key1.ACTION"
 *             android:targetPackage="key1.package"
 *             android:targetClass="key1.class" /&gt;
 *     &lt;/PreferenceScreen&gt;
 * &lt;/PreferenceScreen&gt;
 * </pre>
 *
 * <p>
 * The standard pattern for implementing any of the abstract methods is the following:
 * <ul>
 * <li> If the supplied arguments are enough for the authenticator to fully satisfy the request
 * then it will do so and return a {@link Bundle} that contains the results.
 * <li> If the authenticator needs information from the user to satisfy the request then it
 * will create an {@link Intent} to an activity that will prompt the user for the information
 * and then carry out the request. This intent must be returned in a Bundle as key
 * {@link AccountManager#KEY_INTENT}.
 * <p>
 * The activity needs to return the final result when it is complete so the Intent should contain
 * the {@link AccountAuthenticatorResponse} as {@link AccountManager#KEY_ACCOUNT_MANAGER_RESPONSE}.
 * The activity must then call {@link AccountAuthenticatorResponse#onResult} or
 * {@link AccountAuthenticatorResponse#onError} when it is complete.
 * <li> If the authenticator cannot synchronously process the request and return a result then it
 * may choose to return null and then use the AccountManagerResponse to send the result
 * when it has completed the request.
 * </ul>
 * <p>
 * The following descriptions of each of the abstract authenticator methods will not describe the
 * possible asynchronous nature of the request handling and will instead just describe the input
 * parameters and the expected result.
 * <p>
 * When writing an activity to satisfy these requests one must pass in the AccountManagerResponse
 * and return the result via that response when the activity finishes (or whenever else  the
 * activity author deems it is the correct time to respond).
 * The {@link AccountAuthenticatorActivity} handles this, so one may wish to extend that when
 * writing activities to handle these requests.
 */
class AbstractAccountAuthenticator
    : public Object
    , public IAbstractAccountAuthenticator
{
public:
    class MyRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        MyRunnable(
            /* [in] */ IAccountAuthenticatorResponse* response);

        CARAPI Run();

    public:
        AutoPtr<IAccountAuthenticatorResponse> mResponse;
    };

public:
    CAR_INTERFACE_DECL()

    AbstractAccountAuthenticator();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * @return the IBinder for the AccountAuthenticator
     */
    CARAPI GetIBinder(
        /* [out] */ IBinder** result);

    /**
     * Returns a Bundle that contains whatever is required to clone the account on a different
     * user. The Bundle is passed to the authenticator instance in the target user via
     * {@link #addAccountFromCredentials(AccountAuthenticatorResponse, Account, Bundle)}.
     * The default implementation returns null, indicating that cloning is not supported.
     * @param response to send the result back to the AccountManager, will never be null
     * @param account the account to clone, will never be null
     * @return a Bundle result or null if the result is to be returned via the response.
     * @throws NetworkErrorException
     * @see {@link #addAccountFromCredentials(AccountAuthenticatorResponse, Account, Bundle)}
     */
    CARAPI GetAccountCredentialsForCloning(
        /* [in] */ IAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [out] */ IBundle** result);

    /**
     * Creates an account based on credentials provided by the authenticator instance of another
     * user on the device, who has chosen to share the account with this user.
     * @param response to send the result back to the AccountManager, will never be null
     * @param account the account to clone, will never be null
     * @param accountCredentials the Bundle containing the required credentials to create the
     * account. Contents of the Bundle are only meaningful to the authenticator. This Bundle is
     * provided by {@link #getAccountCredentialsForCloning(AccountAuthenticatorResponse, Account)}.
     * @return a Bundle result or null if the result is to be returned via the response.
     * @throws NetworkErrorException
     * @see {@link #getAccountCredentialsForCloning(AccountAuthenticatorResponse, Account)}
     */
    CARAPI AddAccountFromCredentials(
        /* [in] */ IAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* accountCredentials,
        /* [out] */ IBundle** result);

private:
    CARAPI HandleException(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ const String& method,
        /* [in] */ const String& data,
        /* [in] */ UInt32 e);

    CARAPI CheckBinderPermission();

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<CAccountAuthenticatorTransport> mTransport;

    friend class CAccountAuthenticatorTransport;
};

} // Accounts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_ABSTRACTACCOUNTAUTHENTICATOR_H__
