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

#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_GRANTCREDENTIALSPERMISSIONACTIVITY_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_GRANTCREDENTIALSPERMISSIONACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Accounts.h>
#include <Elastos.Droid.Widget.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManagerCallback;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

class GrantCredentialsPermissionActivity
    : public Activity
{
private:
    class InnerListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ GrantCredentialsPermissionActivity* host);

        CARAPI OnClick(
            /* [in] */ IView* vs);
    private:
        GrantCredentialsPermissionActivity* mHost;
    };

    class OnCreateAccountManagerCallback
        : public Object
        , public IAccountManagerCallback
    {
    private:
        class  OnCreateAccountManagerCallbackRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            OnCreateAccountManagerCallbackRunnable(
                /* [in] */ ICharSequence* authTokenLabel,
                /* [in] */ OnCreateAccountManagerCallback* host);

            CARAPI Run();

        private:
            AutoPtr<ICharSequence> mAuthTokenLabel;
            OnCreateAccountManagerCallback* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        OnCreateAccountManagerCallback(
            /* [in] */ ITextView* textView,
            /* [in] */ GrantCredentialsPermissionActivity* host);

        CARAPI Run(
            /* [in] */ IAccountManagerFuture* accountManagerFuture);

    private:
        AutoPtr<ITextView> mAuthTokenTypeView;
        GrantCredentialsPermissionActivity* mHost;
    };

public:
    GrantCredentialsPermissionActivity();

    virtual CARAPI OnClick(
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
