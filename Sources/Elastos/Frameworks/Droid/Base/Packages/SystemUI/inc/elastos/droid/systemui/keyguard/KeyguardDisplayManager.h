
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDDISPLAYMANAGER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDDISPLAYMANAGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardDisplayManager
    : public Object
{
private:
    class MyMediaRouterSimpleCallback
        : public MediaRouterSimpleCallback
    {
    public:
        TO_STRING_IMPL("KeyguardDisplayManager::MyMediaRouterSimpleCallback")

        MyMediaRouterSimpleCallback(
            /* [in] */ KeyguardDisplayManager* host)
            : mHost(host)
        {}

        CARAPI OnRouteSelected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IRouteInfo* info);

        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IRouteInfo* info);

        CARAPI OnRoutePresentationDisplayChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IRouteInfo* info);

        private:
            KeyguardDisplayManager* mHost;
    };

    class MyDialogInterfaceOnDismissListener
        : public Object
        , public IDialogInterfaceOnDismissListener
    {
    public:
        TO_STRING_IMPL("KeyguardDisplayManager::MyDialogInterfaceOnDismissListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnDismissListener(
            /* [in] */ KeyguardDisplayManager* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
            KeyguardDisplayManager* mHost;
    };

    class KeyguardPresentation
        : public Presentation
    {
    private:
        class MyRunnable
            : public Runnable
        {
        public:
            TO_STRING_IMPL("KeyguardDisplayManager::KeyguardPresentation::MyRunnable")

            MyRunnable(
                /* [in] */ KeyguardPresentation* host)
                : mHost(host)
            {}

            //@Override
            CARAPI Run();

        private:
            KeyguardPresentation* mHost;
        };

    public:
        TO_STRING_IMPL("KeyguardDisplayManager::KeyguardPresentation")

        KeyguardPresentation(
            /* [in] */ IContext* context,
            /* [in] */ IDisplay* display);

        CARAPI OnDetachedFromWindow();

    protected:
        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState)

    public:
        AutoPtr<Runnable> mMoveTextRunnable = new Runnable()

    private:
        static const Int32 VIDEO_SAFE_REGION; // Percentage of display width & height
        static const Int32 MOVE_CLOCK_TIMEOUT; // 10s
        AutoPtr<IView> mClock;
        Int32 mUsableWidth;
        Int32 mUsableHeight;
        Int32 mMarginTop;
        Int32 mMarginLeft;
    };

public:
    KeyguardDisplayManager(
        /* [in] */ IContext* context);

    CARAPI Show();

    CARAPI Hide();

protected:
    CARAPI UpdateDisplays(
        /* [in] */ Boolean showing);

protected:
    static const String TAG;

    AutoPtr<IPresentation> mPresentation;

private:
    static Boolean DEBUG;

    AutoPtr<IMediaRouter> mMediaRouter;
    AutoPtr<IContext> mContext;
    Boolean mShowing;

    AutoPtr<IMediaRouterSimpleCallback> mMediaRouterCallback;

    AutoPtr<IDialogInterfaceOnDismissListener> mOnDismissListener;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDDISPLAYMANAGER_H__