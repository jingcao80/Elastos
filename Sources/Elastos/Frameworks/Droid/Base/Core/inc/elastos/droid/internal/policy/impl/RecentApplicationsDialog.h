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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSDIALOG_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSDIALOG_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::Dialog;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class RecentApplicationsDialog
    : public Dialog
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ RecentApplicationsDialog* host);

        virtual CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        RecentApplicationsDialog* mHost;
    };

    /**
     * This is the listener for the ACTION_CLOSE_SYSTEM_DIALOGS intent.  It's an indication that
     * we should close ourselves immediately, in order to allow a higher-priority UI to take over
     * (e.g. phone call received).
     */
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ RecentApplicationsDialog* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("RecentApplicationsDialog::MyBroadcastReceiver: ")
    private:
        RecentApplicationsDialog* mHost;
    };

    class RecentTag
        : public Object
    {
    public:
        AutoPtr<IActivityManagerRecentTaskInfo> mInfo;
        AutoPtr<IIntent> mIntent;
    };

    class CleanupRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        CleanupRunnable(
           /* [in] */ RecentApplicationsDialog* host);

        virtual CARAPI Run();

    private:
        RecentApplicationsDialog* mHost;
    };

public:
    RecentApplicationsDialog();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    /**
     * Dismiss the dialog and switch to the selected application.
     */
    CARAPI DismissAndSwitch();

    // @Override
    virtual CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI OnStart();

    virtual CARAPI OnStop();

    virtual CARAPI OnClick(
        /* [in] */ IView* view);

protected:
    CARAPI Init(
        /* [in] */ IContext* context);

    /**
     * We create the recent applications dialog just once, and it stays around (hidden)
     * until activated by the user.
     *
     * @see PhoneWindowManager#showRecentAppsDialog
     */
    virtual CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) SwitchTo(
        /* [in] */ RecentTag* tag);

    /**
     * Reload the 6 buttons with recent activities
     */
    CARAPI_(void) ReloadButtons();

private:
    static const Boolean DBG_FORCE_EMPTY_LIST;
    static const String TAG;

    static AutoPtr<IStatusBarManager> sStatusBar;

    static const Int32 NUM_BUTTONS; // =  8
    static const Int32 MAX_RECENT_TASKS;// = NUM_BUTTONS * 2;    // allow for some discards

    AutoPtr<ArrayOf<ITextView*> > mIcons;
    AutoPtr<IView> mNoAppsText;
    AutoPtr<IIntentFilter> mBroadcastIntentFilter;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IRunnable> mCleanup;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSDIALOG_H__
