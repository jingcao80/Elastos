#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_SIGNALCLUSTERVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_SIGNALCLUSTERVIEW_H__

#include "_SystemUI.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Packages::SystemUI::StatusBar::Policy::INetworkControllerImpl;
using Elastos::Droid::Packages::SystemUI::StatusBar::Policy::INetworkControllerImplSignalCluster;
using Elastos::Droid::Packages::SystemUI::StatusBar::Policy::ISecurityController;
using Elastos::Droid::Packages::SystemUI::StatusBar::Policy::ISecurityControllerCallback;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

class SignalClusterView
    : public LinearLayout
    , public ISignalClusterView
    , public INetworkControllerImplSignalCluster
    , public ISecurityControllerCallback
{
private:
    class ChangedRunnable : public Runnable
    {
    public:
        ChangedRunnable(
            /* [in] */ SignalClusterView* host);

        // @Override
        CARAPI Run();

    private:
        SignalClusterView* mHost;
    };

    class TmpCallback
        : public Object
        , public ISecurityControllerCallback
    {
    public:
        CAR_INTERFACE_DECL();

        TmpCallback(
            /* [in] */ SignalClusterView* host);

        CARAPI OnStateChanged();

    private:
        SignalClusterView* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    SignalClusterView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetNetworkController(
        /* [in] */ INetworkControllerImpl* nc);

    CARAPI SetSecurityController(
        /* [in] */ ISecurityController* sc);

    // From SecurityController.
    // @Override
    CARAPI OnStateChanged();

    //@Override
    virtual CARAPI SetWifiIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 strengthIcon,
        /* [in] */ const String& contentDescription);

    //@Override
    virtual CARAPI SetMobileDataIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 strengthIcon,
        /* [in] */ Int32 typeIcon,
        /* [in] */ const String& contentDescription,
        /* [in] */ const String& typeContentDescription,
        /* [in] */ Boolean roaming,
        /* [in] */ Boolean isTypeIconWide);

    //@Override
    virtual CARAPI SetIsAirplaneMode(
        /* [in] */ Boolean is,
        /* [in] */ Int32 airplaneIcon);

    //@Override
    virtual CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI OnFinishInflate();

private:
    // Run after each indicator change.
    CARAPI_(void) Apply();

private:
    static const String TAG;
    static Boolean DEBUG;

    AutoPtr<INetworkControllerImpl> mNC;
    AutoPtr<ISecurityController> mSC;

    Boolean mVpnVisible;
    Boolean mWifiVisible;
    Int32 mWifiStrengthId;
    Boolean mMobileVisible;
    Int32 mMobileStrengthId;
    Int32 mMobileTypeId;
    Boolean mIsAirplaneMode;
    Int32 mAirplaneIconId;
    String mWifiDescription;
    String mMobileDescription;
    String mMobileTypeDescription;
    Boolean mRoaming;
    Boolean mIsMobileTypeIconWide;

    AutoPtr<IViewGroup> mWifiGroup;
    AutoPtr<IViewGroup> mMobileGroup;
    AutoPtr<IImageView> mVpn;
    AutoPtr<IImageView> mWifi;
    AutoPtr<IImageView> mMobile;
    AutoPtr<IImageView> mMobileType;
    AutoPtr<IImageView> mAirplane;
    AutoPtr<IView> mWifiAirplaneSpacer;
    AutoPtr<IView> mWifiSignalSpacer;

    Int32 mWideTypeIconStartPadding;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_SIGNALCLUSTERVIEW_H__
