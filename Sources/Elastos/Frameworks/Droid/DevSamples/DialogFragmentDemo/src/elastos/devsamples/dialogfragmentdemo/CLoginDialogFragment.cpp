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

#include "CLoginDialogFragment.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

static const String TAG("DialogFragmentDemo::CLoginDialogFragment");

//=======================================================================
// CLoginDialogFragment::LoginButtonOnClickListener
//=======================================================================
CAR_INTERFACE_IMPL(CLoginDialogFragment::LoginButtonOnClickListener, Object, IDialogInterfaceOnClickListener)

CLoginDialogFragment::LoginButtonOnClickListener::LoginButtonOnClickListener(
    /* [in] */ CLoginDialogFragment* host)
    : mHost(host)
{
}

ECode CLoginDialogFragment::LoginButtonOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 id)
{
    Logger::I(TAG, "LoginButtonOnClickListener::OnClick");

    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    ILoginInputListenerHolder* holder = ILoginInputListenerHolder::Probe(activity);
    if (holder) {
        AutoPtr<ILoginInputListener> listener;
        holder->GetLoginInputListener((ILoginInputListener**)&listener);

        AutoPtr<ICharSequence> csq;
        mHost->mUsernameExitText->GetText((ICharSequence**)&csq);
        String username;
        csq->ToString(&username);

        csq = NULL;
        mHost->mPasswordExitText->GetText((ICharSequence**)&csq);
        String password;
        csq->ToString(&password);

        Logger::I(TAG, "notify OnLoginInputComplete(%s, %s)", username.string(), password.string());
        listener->OnLoginInputComplete(username, password);
    }
    else {
        Logger::I(TAG, "%s not implements ILoginInputListenerHolder.", TO_CSTR(activity));
    }

    return NOERROR;
}

//=======================================================================
// CLoginDialogFragment
//=======================================================================
CAR_OBJECT_IMPL(CLoginDialogFragment)

ECode CLoginDialogFragment::constructor()
{
    return DialogFragment::constructor();
}

ECode CLoginDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    Logger::I(TAG, "LoginButtonOnClickListener::OnCreateDialog");

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);

    AutoPtr<IView> contentView;
    inflater->Inflate(R::layout::fragment_login_dialog, NULL, (IView**)&contentView);

    AutoPtr<IView> view;
    contentView->FindViewById(R::id::id_txt_username, (IView**)&view);
    mUsernameExitText = ITextView::Probe(view);
    AutoPtr<ICharSequence> username;
    mUsernameExitText->GetText((ICharSequence**)&username);

    view = NULL;
    contentView->FindViewById(R::id::id_txt_password, (IView**)&view);
    mPasswordExitText = ITextView::Probe(view);
    AutoPtr<ICharSequence> password;
    mPasswordExitText->GetText((ICharSequence**)&password);

    AutoPtr<IDialogInterfaceOnClickListener> listener = new LoginButtonOnClickListener(this);

    builder->SetView(contentView);
    builder->SetPositiveButton(username, listener);
    builder->SetNegativeButton(password, NULL);

    AutoPtr<IAlertDialog> ad;
    ECode ec = builder->Create((IAlertDialog**)&ad);
    assert(ad != NULL);
    if (FAILED(ec)) return ec;

    *dialog = IDialog::Probe(ad);
    REFCOUNT_ADD(*dialog);
    return NOERROR;
}

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos