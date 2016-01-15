#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSDIALOG_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::Dialog;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class RecentApplicationsDialog: public Dialog
{
public:
    /**
     * This is the listener for the ACTION_CLOSE_SYSTEM_DIALOGS intent.  It's an indication that
     * we should close ourselves immediately, in order to allow a higher-priority UI to take over
     * (e.g. phone call received).
     */
    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ RecentApplicationsDialog* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("RecentApplicationsDialog::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        RecentApplicationsDialog* mHost;
    };

    class RecentTag
        : public IInterface
        , public ElLightRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        AutoPtr<IActivityManagerRecentTaskInfo> mInfo;
        AutoPtr<IIntent> mIntent;
    };

    class CleanupRunnable
        : public Runnable
    {
    public:
        CleanupRunnable(
           /* [in] */ RecentApplicationsDialog* host);

        virtual CARAPI Run();

    private:
        RecentApplicationsDialog* mHost;
    };

public:
    RecentApplicationsDialog();

    /**
     * Dismiss the dialog and switch to the selected application.
     */
    CARAPI DismissAndSwitch();

    // @Override
    virtual CARAPI_(Boolean) OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    virtual CARAPI_(void) OnStart();

    virtual CARAPI_(void) OnStop();

protected:
    CARAPI Init(
        /* [in] */ IContext* context);

    /**
     * We create the recent applications dialog just once, and it stays around (hidden)
     * until activated by the user.
     *
     * @see PhoneWindowManager#showRecentAppsDialog
     */
    virtual CARAPI_(void) OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    virtual CARAPI OnClick(
        /* [in] */ IView* view);

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
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_RECENTAPPLICATIONSDIALOG_H__
