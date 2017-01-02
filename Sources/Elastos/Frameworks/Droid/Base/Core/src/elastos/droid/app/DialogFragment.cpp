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

#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/app/CDialog.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Os.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

static const String TAG("DialogFragment");

const String DialogFragment::SAVED_DIALOG_STATE_TAG("android:savedDialogState");
const String DialogFragment::SAVED_STYLE("android:style");
const String DialogFragment::SAVED_THEME("android:theme");
const String DialogFragment::SAVED_CANCELABLE("android:cancelable");
const String DialogFragment::SAVED_SHOWS_DIALOG("android:showsDialog");
const String DialogFragment::SAVED_BACK_STACK_ID("android:backStackId");

CAR_INTERFACE_IMPL_3(DialogFragment, Fragment, IDialogFragment, \
    IDialogInterfaceOnCancelListener, IDialogInterfaceOnDismissListener)

DialogFragment::DialogFragment()
    : mStyle(IDialogFragment::STYLE_NORMAL)
    , mTheme(0)
    , mCancelable(TRUE)
    , mShowsDialog(TRUE)
    , mBackStackId(-1)
    , mDialog(NULL)
    , mViewDestroyed(FALSE)
    , mDismissed(FALSE)
    , mShownByMe(FALSE)
{}

DialogFragment::~DialogFragment()
{}

ECode DialogFragment::constructor()
{
    return Fragment::constructor();
}

ECode DialogFragment::SetStyle(
    /* [in] */ Int32 style,
    /* [in] */ Int32 theme)
{
    mStyle = style;
    if (mStyle == IDialogFragment::STYLE_NO_FRAME || mStyle == IDialogFragment::STYLE_NO_INPUT) {
        mTheme = R::style::Theme_DeviceDefault_Dialog_NoFrame;
    }
    if (theme != 0) {
        mTheme = theme;
    }
    return NOERROR;
}

ECode DialogFragment::Show(
    /* [in] */ IFragmentManager* manager,
    /* [in] */ const String& tag)
{
    mDismissed = FALSE;
    mShownByMe = TRUE;
    AutoPtr<IFragmentTransaction> ft;
    manager->BeginTransaction((IFragmentTransaction**)&ft);
    ft->Add(this, tag);
    Int32 id;
    ft->Commit(&id);
    return NOERROR;
}

ECode DialogFragment::Show(
    /* [in] */ IFragmentTransaction* transaction,
    /* [in] */ const String& tag,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    mDismissed = FALSE;
    mShownByMe = TRUE;
    transaction->Add(this, tag);
    mViewDestroyed = FALSE;
    transaction->Commit(&mBackStackId);
    *id = mBackStackId;
    return NOERROR;
}

ECode DialogFragment::Dismiss()
{
    DismissInternal(FALSE);
    return NOERROR;
}

ECode DialogFragment::DismissAllowingStateLoss()
{
    DismissInternal(TRUE);
    return NOERROR;
}

void DialogFragment::DismissInternal(
    /* [in] */ Boolean allowStateLoss)
{
    if (mDismissed) {
        return;
    }
    mDismissed = TRUE;
    mShownByMe = FALSE;
    if (mDialog != NULL) {
        IDialogInterface::Probe(mDialog)->Dismiss();
        mDialog = NULL;
    }
    mViewDestroyed = TRUE;
    if (mBackStackId >= 0) {
        AutoPtr<IFragmentManager> frMgr;
        GetFragmentManager((IFragmentManager**)&frMgr);
        frMgr->PopBackStack(mBackStackId,
                IFragmentManager::POP_BACK_STACK_INCLUSIVE);
        mBackStackId = -1;
    } else {
        AutoPtr<IFragmentTransaction> ft;
        AutoPtr<IFragmentManager> frMgr;
        GetFragmentManager((IFragmentManager**)&frMgr);
        frMgr->BeginTransaction((IFragmentTransaction**)&ft);
        ft->Remove(this);
        if (allowStateLoss) {
            Int32 result;
            ft->CommitAllowingStateLoss(&result);
        } else {
            Int32 id;
            ft->Commit(&id);
        }
    }
}

ECode DialogFragment::GetDialog(
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);
    *dialog = mDialog;
    REFCOUNT_ADD(*dialog);
    return NOERROR;
}

ECode DialogFragment::GetTheme(
    /* [out] */ Int32* theme)
{
    VALIDATE_NOT_NULL(theme)
    *theme = mTheme;
    return NOERROR;
}

ECode DialogFragment::SetCancelable(
    /* [in] */ Boolean cancelable)
{
    mCancelable = cancelable;
    if (mDialog != NULL) mDialog->SetCancelable(cancelable);
    return NOERROR;
}

ECode DialogFragment::IsCancelable(
    /* [out] */ Boolean* cancelable)
{
    VALIDATE_NOT_NULL(cancelable);
    *cancelable = mCancelable;
    return NOERROR;
}

ECode DialogFragment::SetShowsDialog(
    /* [in] */ Boolean showsDialog)
{
    mShowsDialog = showsDialog;
    return NOERROR;
}

ECode DialogFragment::GetShowsDialog(
    /* [out] */ Boolean* showsDialog)
{
    VALIDATE_NOT_NULL(showsDialog);
    *showsDialog = mShowsDialog;
    return NOERROR;
}

ECode DialogFragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    Fragment::OnAttach(activity);
    if (!mShownByMe) {
        // If not explicitly shown through our API, take this as an
        // indication that the dialog is no longer dismissed.
        mDismissed = FALSE;
    }
    return NOERROR;
}

ECode DialogFragment::OnDetach()
{
    Fragment::OnDetach();
    if (!mShownByMe && !mDismissed) {
        // The fragment was not shown by a direct call here, it is not
        // dismissed, and now it is being detached...  well, okay, thou
        // art now dismissed.  Have fun.
        mDismissed = TRUE;
    }
    return NOERROR;
}

ECode DialogFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);

    mShowsDialog = mContainerId == 0;

    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(SAVED_STYLE, IDialogFragment::STYLE_NORMAL, &mStyle);
        savedInstanceState->GetInt32(SAVED_THEME, 0, &mTheme);
        savedInstanceState->GetBoolean(SAVED_CANCELABLE, TRUE, &mCancelable);
        savedInstanceState->GetBoolean(SAVED_SHOWS_DIALOG, mShowsDialog, &mShowsDialog);
        savedInstanceState->GetInt32(SAVED_BACK_STACK_ID, -1, &mBackStackId);
    }
    return NOERROR;
}

ECode DialogFragment::GetLayoutInflater(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater)
    *inflater = NULL;

    if (!mShowsDialog) {
        return Fragment::GetLayoutInflater(savedInstanceState, inflater);
    }

    mDialog = NULL;
    OnCreateDialog(savedInstanceState, (IDialog**)&mDialog);
    switch (mStyle) {
    case IDialogFragment::STYLE_NO_INPUT:
        {
            AutoPtr<IWindow> window;
            mDialog->GetWindow((IWindow**)&window);
            window->AddFlags(
                    IWindowManagerLayoutParams::FLAG_NOT_FOCUSABLE |
                    IWindowManagerLayoutParams::FLAG_NOT_TOUCHABLE);
            // fall through...
        }
    case IDialogFragment::STYLE_NO_FRAME:
    case IDialogFragment::STYLE_NO_TITLE:
        {
            Boolean support;
            mDialog->RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &support);
        }
    }
    AutoPtr<IContext> ctx;
    if (mDialog != NULL) {
        mDialog->GetContext((IContext**)&ctx);
    }
    else {
        ctx = IContext::Probe(mActivity);
    }

    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    *inflater = ILayoutInflater::Probe(obj);
    REFCOUNT_ADD(*inflater)
    return NOERROR;
}

ECode DialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Int32 theme;
    GetTheme(&theme);
    CDialog::New(IContext::Probe(activity), theme, dialog);
    return NOERROR;
}

ECode DialogFragment::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return NOERROR;
}

ECode DialogFragment::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (!mViewDestroyed) {
        // Note: we need to use allowStateLoss, because the dialog
        // dispatches this asynchronously so we can receive the call
        // after the activity is paused.  Worst case, when the user comes
        // back to the activity they see the dialog again.
        DismissInternal(TRUE);
    }
    return NOERROR;
}

ECode DialogFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnActivityCreated(savedInstanceState);

    if (!mShowsDialog) {
        return NOERROR;
    }

    AutoPtr<IView> view;
    GetView((IView**)&view);
    if (view != NULL) {
        AutoPtr<IViewParent> parent;
        view->GetParent((IViewParent**)&parent);
        if (parent != NULL) {
            Logger::E(TAG, "DialogFragment can not be attached to a container view");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mDialog->SetContentView(view);
    }
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mDialog->SetOwnerActivity(activity);
    mDialog->SetCancelable(mCancelable);
    Boolean result;
    mDialog->TakeCancelAndDismissListeners(String("DialogFragment"),
        this, this, &result);
    if (!result) {
        Logger::E(TAG, "You can not set Dialog's OnCancelListener or OnDismissListener");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (savedInstanceState != NULL) {
        AutoPtr<IBundle> dialogState;
        savedInstanceState->GetBundle(SAVED_DIALOG_STATE_TAG, (IBundle**)&dialogState);
        if (dialogState != NULL) {
            mDialog->OnRestoreInstanceState(dialogState);
        }
    }
    return NOERROR;
}

ECode DialogFragment::OnStart()
{
    Fragment::OnStart();
    if (mDialog != NULL) {
        mViewDestroyed = FALSE;
        mDialog->Show();
    }
    return NOERROR;
}

ECode DialogFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Fragment::OnSaveInstanceState(outState);
    if (mDialog != NULL) {
        AutoPtr<IBundle> dialogState;
        mDialog->OnSaveInstanceState((IBundle**)&dialogState);
        if (dialogState != NULL) {
            outState->PutBundle(SAVED_DIALOG_STATE_TAG, dialogState);
        }
    }
    if (mStyle != IDialogFragment::STYLE_NORMAL) {
        outState->PutInt32(SAVED_STYLE, mStyle);
    }
    if (mTheme != 0) {
        outState->PutInt32(SAVED_THEME, mTheme);
    }
    if (!mCancelable) {
        outState->PutBoolean(SAVED_CANCELABLE, mCancelable);
    }
    if (!mShowsDialog) {
        outState->PutBoolean(SAVED_SHOWS_DIALOG, mShowsDialog);
    }
    if (mBackStackId != -1) {
        outState->PutInt32(SAVED_BACK_STACK_ID, mBackStackId);
    }
    return NOERROR;
}

ECode DialogFragment::OnStop()
{
    Fragment::OnStop();
    if (mDialog != NULL) {
        mDialog->Hide();
    }
    return NOERROR;
}

ECode DialogFragment::OnDestroyView()
{
    Fragment::OnDestroyView();
    if (mDialog != NULL) {
        // Set removed here because this dismissal is just to hide
        // the dialog -- we don't want this to cause the fragment to
        // actually be removed.
        mViewDestroyed = TRUE;
        IDialogInterface::Probe(mDialog)->Dismiss();
        mDialog = NULL;
    }
    return NOERROR;
}

ECode DialogFragment::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    Fragment::Dump(prefix, fd, writer, args);
    writer->Print(prefix); writer->Println(String("DialogFragment:"));
    writer->Print(prefix); writer->Print(String("  mStyle="));
    writer->Print(mStyle);
    writer->Print(String(" mTheme=0x"));
//    writer->Println(Integer.toHexString(mTheme));
    writer->Print(prefix); writer->Print(String("  mCancelable="));
    writer->Print(mCancelable);
    writer->Print(String(" mShowsDialog=")); writer->Print(mShowsDialog);
    writer->Print(String(" mBackStackId=")); writer->Print(mBackStackId);
    writer->Print(prefix); writer->Print(String("  mDialog="));
    writer->Print((IInterface*)mDialog.Get());
    writer->Print(prefix); writer->Print(String("  mViewDestroyed="));
    writer->Print(mViewDestroyed);
    writer->Print(String(" mDismissed=")); writer->Print(mDismissed);
    writer->Print(String(" mShownByMe=")); writer->Println(mShownByMe);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
