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

#ifndef __ELASTOS_DROID_APP_DIALOGFRAGMENT_H__
#define __ELASTOS_DROID_APP_DIALOGFRAGMENT_H__

#include "elastos/droid/app/Fragment.h"

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC DialogFragment
    : public Fragment
    , public IDialogFragment
    , public IDialogInterfaceOnCancelListener
    , public IDialogInterfaceOnDismissListener
{
public:
    CAR_INTERFACE_DECL()

    DialogFragment();

    virtual ~DialogFragment();

    CARAPI constructor();

    CARAPI SetStyle(
        /* [in] */ Int32 style,
        /* [in] */ Int32 theme);

    CARAPI Show(
        /* [in] */ IFragmentManager* manager,
        /* [in] */ const String& tag);

    CARAPI Show(
        /* [in] */ IFragmentTransaction* transaction,
        /* [in] */ const String& tag,
        /* [out] */ Int32* id);

    CARAPI Dismiss();

    CARAPI DismissAllowingStateLoss();

    CARAPI_(void) DismissInternal(
        /* [in] */ Boolean allowStateLoss);

    CARAPI GetDialog(
        /* [out] */ IDialog** dialog);

    CARAPI GetTheme(
        /* [out] */ Int32* theme);

    CARAPI SetCancelable(
        /* [in] */ Boolean cancelable);

    CARAPI IsCancelable(
        /* [out] */ Boolean* cancelable);

    CARAPI SetShowsDialog(
        /* [in] */ Boolean showsDialog);

    CARAPI GetShowsDialog(
        /* [out] */ Boolean* showsDialog);

    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    CARAPI OnDetach();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI GetLayoutInflater(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ ILayoutInflater** inflater);

    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI OnStop();

    CARAPI OnDestroyView();

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

public:
    Int32 mStyle;
    Int32 mTheme;
    Boolean mCancelable;
    Boolean mShowsDialog;
    Int32 mBackStackId;

    AutoPtr<IDialog> mDialog;
    Boolean mViewDestroyed;
    Boolean mDismissed;
    Boolean mShownByMe;

private:
    ECO_LOCAL static const String SAVED_DIALOG_STATE_TAG;
    ECO_LOCAL static const String SAVED_STYLE;
    ECO_LOCAL static const String SAVED_THEME;
    ECO_LOCAL static const String SAVED_CANCELABLE;
    ECO_LOCAL static const String SAVED_SHOWS_DIALOG;
    ECO_LOCAL static const String SAVED_BACK_STACK_ID;

};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_DIALOGFRAGMENT_H__
