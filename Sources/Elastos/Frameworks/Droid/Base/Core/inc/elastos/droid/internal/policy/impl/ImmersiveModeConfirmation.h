#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_IMMERSIVEMODECONFIRMATION_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_IMMERSIVEMODECONFIRMATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
  *  Helper to manage showing/hiding a confirmation prompt when the navigation bar is hidden
  *  entering immersive mode.
  */
class ImmersiveModeConfirmation
    : public Object
    , public IImmersiveModeConfirmation
{
public:
    class H
        : public Handler
    {
    public:
        H(
            /* [in] */ ImmersiveModeConfirmation* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 SHOW = 1;
        static const Int32 HIDE = 2;

    private:
        ImmersiveModeConfirmation* mOwner;
    };

private:
    class ClingWindowView
        : public FrameLayout
    {
    private:
        class InnerOnClickListener1
            : public Object
            , public IViewOnClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            InnerOnClickListener1(
                /* [in] */ ClingWindowView* owner);

            // @Override
            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            ClingWindowView* mOwner;
        };

        class InnerValueAnimatorAnimatorUpdateListener1
            : public Object
            , public IAnimatorUpdateListener
        {
        public:
            CAR_INTERFACE_DECL()

            InnerValueAnimatorAnimatorUpdateListener1(
                /* [in] */ ClingWindowView* owner);

            // @Override
            CARAPI OnAnimationUpdate(
                /* [in] */ IValueAnimator* animation);

        private:
            ClingWindowView* mOwner;
        };

        class InnerRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            InnerRunnable(
                /* [in] */ ClingWindowView* owner);

            // @Override
            CARAPI Run();

        private:
            ClingWindowView* mOwner;
        };

        class InnerBroadcastReceiver
            : public BroadcastReceiver
        {
        public:
            InnerBroadcastReceiver(
                /* [in] */ ClingWindowView* owner);

            // @Override
            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

        private:
            ClingWindowView* mOwner;
        };

    public:
        ClingWindowView(
            /* [in] */ ImmersiveModeConfirmation* owner,
            /* [in] */ IContext* context,
            /* [in] */ IRunnable* confirm);

        // @Override
        CARAPI OnAttachedToWindow();

        // @Override
        CARAPI OnDetachedFromWindow();

        // @Override
        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* motion,
            /* [out] */ Boolean* res);

    private:
        static const Int32 BGCOLOR = 0x80000000;
        static const Int32 OFFSET_DP = 48;
        ImmersiveModeConfirmation* mOwner;
        AutoPtr<IRunnable> mConfirm;
        AutoPtr<IColorDrawable> mColor;
        AutoPtr<IValueAnimator> mColorAnim;
        AutoPtr<IViewGroup> mClingLayout;
        AutoPtr<IRunnable> mUpdateLayoutRunnable;
        AutoPtr<IBroadcastReceiver> mReceiver;
    };

    class InnerRunnable1
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable1(
            /* [in] */ ImmersiveModeConfirmation* owner);

        // @Override
        CARAPI Run();

    private:
        ImmersiveModeConfirmation* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    ImmersiveModeConfirmation();

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI LoadSetting(
        /* [in] */ Int32 currentUserId);

    virtual CARAPI ImmersiveModeChanged(
        /* [in] */ const String& pkg,
        /* [in] */ Boolean isImmersiveMode,
        /* [in] */ Boolean userSetupComplete);

    virtual CARAPI ConfirmCurrentPrompt();

    virtual CARAPI GetClingWindowLayoutParams(
        /* [out] */ IWindowManagerLayoutParams** result);

    virtual CARAPI GetBubbleLayoutParams(
        /* [out] */ IFrameLayoutLayoutParams** result);

private:
    CARAPI_(Int64) GetNavBarExitDuration();

    CARAPI_(void) SaveSetting();

    CARAPI_(void) HandleHide();

    CARAPI_(void) HandleShow();

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean DEBUG_SHOW_EVERY_TIME;
    // super annoying, use with caution
    static const String CONFIRMED;
    AutoPtr<IContext> mContext;
    AutoPtr<H> mHandler;
    Int64 mShowDelayMs;

    Boolean mConfirmed;
    AutoPtr<ClingWindowView> mClingWindow;
    AutoPtr<IWindowManager> mWindowManager;
    Int32 mCurrentUserId;
    AutoPtr<IRunnable> mConfirm;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_IMMERSIVEMODECONFIRMATION_H__

