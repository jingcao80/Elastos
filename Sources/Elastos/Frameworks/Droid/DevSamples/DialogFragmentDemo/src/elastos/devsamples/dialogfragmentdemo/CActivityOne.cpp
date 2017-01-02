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

#include "CActivityOne.h"
#include "CEditNameDialogFragment.h"
#include "CFireMissilesDialogFragment.h"
#include "CLoginDialogFragment.h"
#include "R.h"

#include <elastos/droid/R.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

static const String TAG("DialogFragmentDemo::CActivityOne");

//=======================================================================
// LoginButtonOnClickListener
//=======================================================================
CAR_INTERFACE_IMPL(CActivityOne::LoginButtonOnClickListener, Object, IDialogInterfaceOnClickListener)

CActivityOne::LoginButtonOnClickListener::LoginButtonOnClickListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::LoginButtonOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 id)
{
    // sign in the user ...
    Logger::I(TAG, "LoginButtonOnClickListener::OnClick");
    return NOERROR;
}

//=======================================================================
// ViewClickListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::LoginInputListener, Object, ILoginInputListener)

CActivityOne::LoginInputListener::LoginInputListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::LoginInputListener::OnLoginInputComplete(
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    Logger::I(TAG, "LoginInputListener::OnLoginInputComplete : [%s, %s]",
        username.string(), password.string());

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);

    StringBuilder sb("UserName:");
    sb += username;
    sb += ", Password:";
    sb += password;
    AutoPtr<ICharSequence> info = sb.ToCharSequence();

    AutoPtr<IToast> toast;
    helper->MakeText(mHost, info, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
    return NOERROR;
}


//=======================================================================
// ViewClickListener
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne::ViewClickListener, Object, IViewOnClickListener)

CActivityOne::ViewClickListener::ViewClickListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::ViewClickListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::I(TAG, "ViewClickListener::OnClick");
    Int32 id;
    v->GetId(&id);
    switch(id) {
        case R::id::ConfimDialog:
            mHost->ShowConfimDialog();
            break;
        case R::id::EditDialog:
            mHost->ShowEditDialog();
            break;
        case R::id::LoginDialog:
            mHost->ShowLoginDialog();
            break;
        case R::id::DialogInDifferentScreen:
            mHost->ShowDialogInDifferentScreen();
            break;
        case R::id::LoginDialogWithoutFragment:
            mHost->ShowLoginDialogWithoutFragment();
            break;
    }
    return NOERROR;
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_INTERFACE_IMPL(CActivityOne, Activity, ILoginInputListenerHolder)

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<ViewClickListener> l = new ViewClickListener(this);

    AutoPtr<IView> view = FindViewById(R::id::ConfimDialog);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::EditDialog);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::LoginDialog);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::DialogInDifferentScreen);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::LoginDialogWithoutFragment);
    view->SetOnClickListener(l.Get());

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

//=========================================================================

ECode CActivityOne::GetLoginInputListener(
    /* [out] */ ILoginInputListener** listener)
{
    VALIDATE_NOT_NULL(listener)
    if (mLoginInputListener == NULL) {
        mLoginInputListener = new LoginInputListener(this);
    }
    *listener = mLoginInputListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

void CActivityOne::ShowConfimDialog()
{
    Logger::I(TAG, " >> ShowConfimDialog");
    AutoPtr<IFragmentManager> mgr;
    GetFragmentManager((IFragmentManager**)&mgr);

    AutoPtr<IDialogFragment> dialogFragment;
    CFireMissilesDialogFragment::New((IDialogFragment**)&dialogFragment);
    dialogFragment->Show(mgr, String("ConfirmDialog"));
}

void CActivityOne::ShowEditDialog()
{
    Logger::I(TAG, " >> ShowEditDialog");
    AutoPtr<IFragmentManager> mgr;
    GetFragmentManager((IFragmentManager**)&mgr);

    AutoPtr<IDialogFragment> dialogFragment;
    CEditNameDialogFragment::New((IDialogFragment**)&dialogFragment);
    dialogFragment->Show(mgr, String("EditNameDialog"));
}

void CActivityOne::ShowLoginDialog()
{
    Logger::I(TAG, " >> ShowLoginDialog");
    AutoPtr<IFragmentManager> mgr;
    GetFragmentManager((IFragmentManager**)&mgr);

    AutoPtr<IDialogFragment> dialogFragment;
    CLoginDialogFragment::New((IDialogFragment**)&dialogFragment);
    dialogFragment->Show(mgr, String("LoginDialog"));
}

void CActivityOne::ShowDialogInDifferentScreen()
{
    Logger::I(TAG, " >> ShowDialogInDifferentScreen");
    AutoPtr<IFragmentManager> mgr;
    GetFragmentManager((IFragmentManager**)&mgr);

    AutoPtr<IDialogFragment> dialogFragment;
    CEditNameDialogFragment::New((IDialogFragment**)&dialogFragment);

    Boolean isLargeLayout = FALSE;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    // res->GetBoolean(R::bool_::large_layout, &isLargeLayout);
    Logger::I(TAG, " >> isLargeLayout : %d", isLargeLayout);

    if (isLargeLayout ) {
        // The device is using a large layout, so show the fragment as a
        // dialog
        dialogFragment->Show(mgr, String("Dialog"));
    }
    else {
        // The device is smaller, so show the fragment fullscreen
        AutoPtr<IFragmentTransaction> transaction;
        mgr->BeginTransaction((IFragmentTransaction**)&transaction);
        // For a little polish, specify a transition animation
        transaction->SetTransition(IFragmentTransaction::TRANSIT_FRAGMENT_OPEN);
        // To make it fullscreen, use the 'content' root view as the
        // container
        // for the fragment, which is always the root view for the activity
        transaction->Replace(R::id::id_ly, IFragment::Probe(dialogFragment));
        Int32 status;
        transaction->Commit(&status);
    }
}

void CActivityOne::ShowLoginDialogWithoutFragment()
{
    Logger::I(TAG, " >> ShowLoginDialogWithoutFragment");
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
    // Get the layout inflater
    AutoPtr<ILayoutInflater> inflater;
    GetLayoutInflater((ILayoutInflater**)&inflater);

    // Inflate and set the layout for the dialog
    // Pass null as the parent view because its going in the dialog layout
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::fragment_login_dialog, NULL, (IView**)&view);
    assert(view != NULL);

    builder->SetView(view);
    // Add action buttons
    AutoPtr<ICharSequence> csq = CoreUtils::Convert("Sign in");
    AutoPtr<IDialogInterfaceOnClickListener> listener = new LoginButtonOnClickListener(this);
    builder->SetPositiveButton(csq, listener);
    csq = CoreUtils::Convert("Cancel");
    builder->SetNegativeButton(csq, NULL);
    AutoPtr<IAlertDialog> dialog;
    builder->Show((IAlertDialog**)&dialog);
}

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos
