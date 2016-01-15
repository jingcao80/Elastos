#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_GRANTCREDENTIALSPERMISSIONACTIVITY_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_GRANTCREDENTIALSPERMISSIONACTIVITY_H__

#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Accounts::IAccountManagerCallback;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::Accounts::IAccount;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

class GrantCredentialsPermissionActivity
    : public Activity
    , public IViewOnClickListener
{
private:
    class OnCreateAccountManagerCallback
        : public ElRefBase
        , public IAccountManagerCallback
    {
    private:
        class  OnCreateAccountManagerCallbackRunnable
            : public ElRefBase
            , public IRunnable
        {
        public:
            OnCreateAccountManagerCallbackRunnable(
                /* [in] */ ICharSequence* authTokenLabel,
                /* [in] */ OnCreateAccountManagerCallback* host);

            CAR_INTERFACE_DECL();

            CARAPI Run();

        private:
            AutoPtr<ICharSequence> mAuthTokenLabel;
            AutoPtr<OnCreateAccountManagerCallback> mHost;
        };

    public:
        OnCreateAccountManagerCallback(
            /* [in] */ ITextView* textView,
            /* [in] */ GrantCredentialsPermissionActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI Run(
            /* [in] */ IAccountManagerFuture* accountManagerFuture);

    private:
        AutoPtr<ITextView> mAuthTokenTypeView;
        AutoPtr<GrantCredentialsPermissionActivity> mHost;
    };

public:
    GrantCredentialsPermissionActivity();

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI OnClick(
        /* [in] */ IView* vs);

    CARAPI_(void) SetAccountAuthenticatorResult(
        /* [in] */ IBundle* result);

    /**
     * Sends the result or a {@link AccountManager#ERROR_CODE_CANCELED} error if a
     * result isn't present.
     */
    CARAPI Finish();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI GetAccountLabel(
        /* [in] */ IAccount* account,
        /* [out] */ String* label);

    CARAPI_(AutoPtr<IView>) NewPackageView(
        /* [in] */ const String& packageLabel);

public:
    static const String EXTRAS_ACCOUNT;
    static const String EXTRAS_AUTH_TOKEN_LABEL;
    static const String EXTRAS_AUTH_TOKEN_TYPE;
    static const String EXTRAS_RESPONSE;
    static const String EXTRAS_ACCOUNT_TYPE_LABEL;
    static const String EXTRAS_PACKAGES;
    static const String EXTRAS_REQUESTING_UID;

protected:
    AutoPtr<ILayoutInflater> mInflater;

private:
    AutoPtr<IAccount> mAccount;
    String mAuthTokenType;
    Int32 mUid;
    AutoPtr<IBundle> mResultBundle;
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_GRANTCREDENTIALSPERMISSIONACTIVITY_H__
