#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDBOTTOMAREAVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDBOTTOMAREAVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CKeyguardBottomAreaView.h"
#include "elastos/droid/systemui/statusbar/phone/UnlockMethodCache.h"
#include "elastos/droid/systemui/statusbar/policy/PreviewInflater.h"
#include "Elastos.Droid.Telecomm.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/graphics/drawable/InsetDrawable.h>
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Graphics::Drawable::InsetDrawable;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::SystemUI::StatusBar::Policy::IAccessibilityController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IAccessibilityStateChangedCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::IFlashlightController;
using Elastos::Droid::SystemUI::StatusBar::Policy::PreviewInflater;
using Elastos::Droid::View::IAccessibilityDelegate;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Implementation for the bottom area of the Keyguard, including camera/phone affordance and status
 * text.
 */
CarClass(CKeyguardBottomAreaView)
    , public FrameLayout
    , public IKeyguardBottomAreaView
    , public IViewOnClickListener
    , public IOnUnlockMethodChangedListener
    , public IAccessibilityStateChangedCallback
    , public IViewOnLongClickListener
{
private:
    /**
     * A wrapper around another Drawable that overrides the intrinsic size.
     */
    class IntrinsicSizeDrawable: public InsetDrawable
    {
    public:
        IntrinsicSizeDrawable(
            /* [in] */ IDrawable* drawable,
            /* [in] */ Int32 intrinsicWidth,
            /* [in] */ Int32 intrinsicHeight);

        // @Override
        CARAPI GetIntrinsicWidth(
            /* [out] */ Int32* width);

        // @Override
        CARAPI GetIntrinsicHeight(
            /* [out] */ Int32* height);

    private:
        Int32 mIntrinsicWidth;
        Int32 mIntrinsicHeight;
    };

    class KBAAccessibilityDelegate: public AccessibilityDelegate
    {
    public:
        KBAAccessibilityDelegate(
            /* [in] */ CKeyguardBottomAreaView* host);

        // @Override
        CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityNodeInfo* info);

        // @Override
        CARAPI PerformAccessibilityAction(
            /* [in] */ IView* host,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* args,
            /* [out] */ Boolean* result);

    private:
        CKeyguardBottomAreaView* mHost;
    };

    class DevicePolicyReceiver: public BroadcastReceiver
    {
    private:
        class DevicePolicyReceiverRun: public Runnable
        {
        public:
            DevicePolicyReceiverRun(
                /* [in] */ CKeyguardBottomAreaView* host);

            // @Override
            CARAPI Run();

        private:
            CKeyguardBottomAreaView* mHost;
        };

    public:
        DevicePolicyReceiver(
            /* [in] */ CKeyguardBottomAreaView* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("CKeyguardBottomAreaView.DevicePolicyReceiver");
            return NOERROR;
        }

    private:
        CKeyguardBottomAreaView* mHost;
    };

    // class UpdateMonitorCallback: public KeyguardUpdateMonitorCallback
    // {
    //     @Override
    //     public void onUserSwitchComplete(Int32 userId) {
    //         updateCameraVisibility();
    //     }

    //     @Override
    //     public void onScreenTurnedOn() {
    //         updateLockIcon();
    //     }

    //     @Override
    //     public void onScreenTurnedOff(Int32 why) {
    //         updateLockIcon();
    //     }
    // };

    class UnlockMethodChangedListener
        : public Object
        , public IOnUnlockMethodChangedListener
    {
    public:
        CAR_INTERFACE_DECL();

        UnlockMethodChangedListener(
            /* [in] */ CKeyguardBottomAreaView* host);

        CARAPI OnMethodSecureChanged(
            /* [in] */ Boolean methodSecure);

    private:
        CKeyguardBottomAreaView* mHost;
    };

    class OnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnClickListener(
            /* [in] */ CKeyguardBottomAreaView* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CKeyguardBottomAreaView* mHost;
    };

    class OnLongClickListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnLongClickListener(
            /* [in] */ CKeyguardBottomAreaView* host);

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        CKeyguardBottomAreaView* mHost;
    };

    class TaskRunnable: public Runnable
    {
    public:
        TaskRunnable(
            /* [in] */ ITelecomManager* tm);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<ITelecomManager> mTm;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CKeyguardBottomAreaView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetActivityStarter(
        /* [in] */ IActivityStarter* activityStarter);

    CARAPI SetFlashlightController(
        /* [in] */ IFlashlightController* flashlightController);

    CARAPI SetAccessibilityController(
        /* [in] */ IAccessibilityController* accessibilityController);

    CARAPI SetPhoneStatusBar(
        /* [in] */ IPhoneStatusBar* phoneStatusBar);

    // @Override
    CARAPI OnStateChanged(
        /* [in] */ Boolean accessibilityEnabled,
        /* [in] */ Boolean touchExplorationEnabled);

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnLongClick(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI LaunchCamera();

    CARAPI LaunchPhone();

    CARAPI GetPhoneView(
        /* [out] */ IKeyguardAffordanceView** view);

    CARAPI GetCameraView(
        /* [out] */ IKeyguardAffordanceView** view);

    CARAPI GetPhonePreview(
        /* [out] */ IView** view);

    CARAPI GetCameraPreview(
        /* [out] */ IView** view);

    CARAPI GetLockIcon(
        /* [out] */ IKeyguardAffordanceView** view);

    CARAPI GetIndicationView(
        /* [out] */ IView** view);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnMethodSecureChanged(
        /* [in] */ Boolean methodSecure);

    CARAPI SetKeyguardIndicationController(
        /* [in] */ IKeyguardIndicationController* keyguardIndicationController);

protected:
    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) InitAccessibility();

    CARAPI_(AutoPtr<IIntent>) GetCameraIntent();

    CARAPI_(void) UpdateCameraVisibility();

    CARAPI_(void) UpdatePhoneVisibility();

    CARAPI_(Boolean) IsPhoneVisible();

    CARAPI_(Boolean) IsCameraDisabledByDpm();

    CARAPI_(void) WatchForCameraPolicyChanges();

    CARAPI_(void) UpdateLockIconClickability();

    CARAPI_(void) HandleTrustCircleClick();

    CARAPI_(void) UpdateLockIcon();

    CARAPI_(void) InflatePreviews();

private:
    static const String TAG;

    static AutoPtr<IIntent> SECURE_CAMERA_INTENT;;
    static AutoPtr<IIntent> INSECURE_CAMERA_INTENT;
    static AutoPtr<IIntent> PHONE_INTENT;

    AutoPtr<IKeyguardAffordanceView> mCameraImageView;
    AutoPtr<IKeyguardAffordanceView> mPhoneImageView;
    AutoPtr<IKeyguardAffordanceView> mLockIcon;
    AutoPtr<ITextView> mIndicationText;
    AutoPtr<IViewGroup> mPreviewContainer;

    AutoPtr<IView> mPhonePreview;
    AutoPtr<IView> mCameraPreview;

    AutoPtr<IActivityStarter> mActivityStarter;
    AutoPtr<UnlockMethodCache> mUnlockMethodCache;
    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    AutoPtr<IFlashlightController> mFlashlightController;
    AutoPtr<PreviewInflater> mPreviewInflater;
    AutoPtr<IKeyguardIndicationController> mIndicationController;
    AutoPtr<IAccessibilityController> mAccessibilityController;
    AutoPtr<IPhoneStatusBar> mPhoneStatusBar;

    AutoPtr<IDrawable> mTrustDrawable;

    Int32 mLastUnlockIconRes;
    AutoPtr<IAccessibilityDelegate> mAccessibilityDelegate;
    AutoPtr<IBroadcastReceiver> mDevicePolicyReceiver;
    // AutoPtr<IKeyguardUpdateMonitorCallback> mUpdateMonitorCallback;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDBOTTOMAREAVIEW_H__
