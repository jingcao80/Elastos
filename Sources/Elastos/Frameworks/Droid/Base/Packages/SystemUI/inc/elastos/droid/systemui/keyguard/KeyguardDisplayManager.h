
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDDISPLAYMANAGER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDDISPLAYMANAGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/app/Presentation.h>
#include <elastos/droid/media/MediaRouterSimpleCallback.h>

using Elastos::Droid::App::Presentation;
using Elastos::Droid::App::IPresentation;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::IMediaRouterCallback;
using Elastos::Droid::Media::MediaRouterSimpleCallback;
using Elastos::Droid::Media::IMediaRouterSimpleCallback;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IDisplay;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardDisplayManager
    : public Object
    , public IKeyguardDisplayManager
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
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouterRouteInfo* info);

        CARAPI OnRoutePresentationDisplayChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IMediaRouterRouteInfo* info);

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

        KeyguardPresentation();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IDisplay* display);

        CARAPI OnDetachedFromWindow();

    protected:
        //@Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

    public:
        AutoPtr<Runnable> mMoveTextRunnable;

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
    CAR_INTERFACE_DECL()

    KeyguardDisplayManager();

    CARAPI constructor(
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

    AutoPtr<IMediaRouterCallback> mMediaRouterCallback;
    AutoPtr<IDialogInterfaceOnDismissListener> mOnDismissListener;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDDISPLAYMANAGER_H__