#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_SIGNALCLUSTERVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_SIGNALCLUSTERVIEW_H__

#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::SystemUI::StatusBar::Policy::INetworkController;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


class SignalClusterView : public LinearLayout
{
public:
    SignalClusterView();

    SignalClusterView(
        /* [in] */ IContext* context);

    SignalClusterView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    SignalClusterView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetNetworkController(
        /* [in] */ INetworkController* nc);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    //@Override
    virtual CARAPI SetWifiIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 strengthIcon,
        /* [in] */ Int32 activityIcon,
        /* [in] */ const String& contentDescription);

    //@Override
    virtual CARAPI SetMobileDataIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 strengthIcon,
        /* [in] */ Int32 activityIcon,
        /* [in] */ Int32 typeIcon,
        /* [in] */ const String& contentDescription,
        /* [in] */ const String& typeContentDescription);

    //@Override
    virtual CARAPI SetEthernetIndicators(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 stateIcon,
        /* [in] */ Int32 activityIcon,
        /* [in] */ const String& contentDescription);

    //@Override
    virtual CARAPI SetIsAirplaneMode(
        /* [in] */ Boolean is,
        /* [in] */ Int32 airplaneIcon);

    //@Override
    virtual CARAPI_(Boolean) DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

private:
    // Run after each indicator change.
    CARAPI_(void) Apply();

private:
    // static final Boolean DEBUG = false;
    // static final String TAG = "SignalClusterView";

    AutoPtr<INetworkController> mNC;

    Boolean mWifiVisible;
    Int32 mWifiStrengthId;
    Int32 mWifiActivityId;
    Boolean mMobileVisible;
    Int32 mMobileStrengthId;
    Int32 mMobileActivityId;
    Int32 mMobileTypeId;
    Boolean mEthernetVisible;
    Int32 mEthernetStateId;
    Int32 mEthernetActivityId;
    Boolean mIsAirplaneMode;
    Int32 mAirplaneIconId;
    String mWifiDescription;
    String mMobileDescription;
    String mMobileTypeDescription;
    String mEthernetDescription;

    AutoPtr<IViewGroup> mWifiGroup;
    AutoPtr<IViewGroup> mMobileGroup;
    AutoPtr<IViewGroup> mEthernetGroup;

    AutoPtr<IImageView> mWifi;
    AutoPtr<IImageView> mMobile;
    AutoPtr<IImageView> mWifiActivity;
    AutoPtr<IImageView> mMobileActivity;
    AutoPtr<IImageView> mMobileType;
    AutoPtr<IImageView> mEthernet;
    AutoPtr<IImageView> mAirplane;

    AutoPtr<IView> mSpacer;
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_SIGNALCLUSTERVIEW_H__
