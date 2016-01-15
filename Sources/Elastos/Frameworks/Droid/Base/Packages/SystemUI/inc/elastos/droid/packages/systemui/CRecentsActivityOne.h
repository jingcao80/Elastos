
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CRecentsActivityOne.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::SystemUI::Recent::IRecentsPanelView;
using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::SystemUI::StatusBar::Tablet::IStatusBarPanel;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

class CRecentsActivityOne
    : public Activity
    , public IRecentsActivity
{
public:
    class TouchOutsideListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    private:
        CRecentsActivityOne* mHost;
        IStatusBarPanel* mPanel;

    public:
        CAR_INTERFACE_DECL()

        TouchOutsideListener(
            /* [in] */ CRecentsActivityOne* host,
            /* [in] */ IStatusBarPanel* panel);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);
    };

private:
    class IntentReceiver
        : public BroadcastReceiver
    {
    public:
        IntentReceiver(
            /* [in] */ CRecentsActivityOne* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CRecentsActivityOne::IntentReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CRecentsActivityOne* mHost;
    };

public:
    CRecentsActivityOne();

    CAR_INTERFACE_DECL();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnBackPressed();

    CARAPI DismissAndGoHome();

    CARAPI DismissAndGoBack();

    CARAPI IsActivityShowing(
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) IsForeground();

    // CARAPI_(Boolean) IsActivityShowing();

    static CARAPI_(Boolean) ForceOpaqueBackground(
        /* [in] */ IContext* context);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI OnDestroy();

    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) UpdateWallpaperVisibility(
        /* [in] */ Boolean visible);

    CARAPI_(void) HandleIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean checkWaitingForAnimationParam);

private:
    static const String WAS_SHOWING; // = "was_showing";

    AutoPtr<IRecentsPanelView> mRecentsPanel;
    AutoPtr<IIntentFilter> mIntentFilter;
    Boolean mShowing;
    Boolean mForeground;
    AutoPtr<IntentReceiver> mIntentReceiver;
};


} // namespace ImageViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
