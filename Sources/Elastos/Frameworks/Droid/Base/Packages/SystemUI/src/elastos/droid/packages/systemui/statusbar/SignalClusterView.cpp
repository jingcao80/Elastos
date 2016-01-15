#include "elastos/droid/systemui/statusbar/SignalClusterView.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CString;
using Elastos::Droid::SystemUI::SystemUIR;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

SignalClusterView::SignalClusterView()
    : mWifiVisible(FALSE)
    , mWifiStrengthId(0)
    , mWifiActivityId(0)
    , mMobileVisible(FALSE)
    , mMobileStrengthId(0)
    , mMobileActivityId(0)
    , mMobileTypeId(0)
    , mEthernetVisible(FALSE)
    , mEthernetStateId(0)
    , mEthernetActivityId(0)
    , mIsAirplaneMode(FALSE)
    , mAirplaneIconId(0)
{}

SignalClusterView::SignalClusterView(
    /* [in] */ IContext* context)
    : LinearLayout(context, NULL)
    , mWifiVisible(FALSE)
    , mWifiStrengthId(0)
    , mWifiActivityId(0)
    , mMobileVisible(FALSE)
    , mMobileStrengthId(0)
    , mMobileActivityId(0)
    , mMobileTypeId(0)
    , mEthernetVisible(FALSE)
    , mEthernetStateId(0)
    , mEthernetActivityId(0)
    , mIsAirplaneMode(FALSE)
    , mAirplaneIconId(0)
{}

SignalClusterView::SignalClusterView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
    , mWifiVisible(FALSE)
    , mWifiStrengthId(0)
    , mWifiActivityId(0)
    , mMobileVisible(FALSE)
    , mMobileStrengthId(0)
    , mMobileActivityId(0)
    , mMobileTypeId(0)
    , mEthernetVisible(FALSE)
    , mEthernetStateId(0)
    , mEthernetActivityId(0)
    , mIsAirplaneMode(FALSE)
    , mAirplaneIconId(0)
{}

SignalClusterView::SignalClusterView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : LinearLayout(context, attrs, defStyle)
    , mWifiVisible(FALSE)
    , mWifiStrengthId(0)
    , mWifiActivityId(0)
    , mMobileVisible(FALSE)
    , mMobileStrengthId(0)
    , mMobileActivityId(0)
    , mMobileTypeId(0)
    , mEthernetVisible(FALSE)
    , mEthernetStateId(0)
    , mEthernetActivityId(0)
    , mIsAirplaneMode(FALSE)
    , mAirplaneIconId(0)
{}

ECode SignalClusterView::SetNetworkController(
    /* [in] */ INetworkController* nc)
{
    mNC = nc;
    return NOERROR;
}

//@Override
ECode SignalClusterView::OnAttachedToWindow()
{
    LinearLayout::OnAttachedToWindow();

    AutoPtr<IView> tmp;

    tmp = FindViewById(SystemUIR::id::wifi_combo);
    assert(tmp != NULL);
    mWifiGroup = IViewGroup::Probe(tmp.Get());
    assert(mWifiGroup != NULL);

    tmp = FindViewById(SystemUIR::id::wifi_signal);
    mWifi = IImageView::Probe(tmp.Get());
    assert(mWifi != NULL);

    tmp = FindViewById(SystemUIR::id::wifi_inout);
    mWifiActivity = IImageView::Probe(tmp.Get());
    assert(mWifiActivity != NULL);

    tmp = FindViewById(SystemUIR::id::mobile_combo);
    mMobileGroup = IViewGroup::Probe(tmp.Get());
    assert(mMobileGroup != NULL);

    tmp = FindViewById(SystemUIR::id::mobile_signal);
    mMobile = IImageView::Probe(tmp.Get());
    assert(mMobile != NULL);

    tmp = FindViewById(SystemUIR::id::mobile_inout);
    mMobileActivity = IImageView::Probe(tmp.Get());
    assert(mMobileActivity != NULL);

    tmp = FindViewById(SystemUIR::id::mobile_type);
    mMobileType = IImageView::Probe(tmp.Get());
    assert(mMobileType != NULL);

    tmp = FindViewById(SystemUIR::id::ethernet_combo);
    mEthernetGroup = IViewGroup::Probe(tmp.Get());
    assert(mEthernetGroup != NULL);

    tmp = FindViewById(SystemUIR::id::ethernet_state);
    mEthernet = IImageView::Probe(tmp.Get());
    assert(mEthernet != NULL);

    mSpacer = FindViewById(SystemUIR::id::spacer);
    assert(mSpacer != NULL);

    tmp = FindViewById(SystemUIR::id::airplane);
    // mAirplane = IImageView::Probe(tmp.Get());
    // assert(mAirplane != NULL);

    Apply();
    return NOERROR;
}

//@Override
ECode SignalClusterView::OnDetachedFromWindow()
{
    mWifiGroup      = NULL;
    mWifi           = NULL;
    mWifiActivity   = NULL;
    mMobileGroup    = NULL;
    mMobile         = NULL;
    mMobileActivity = NULL;
    mMobileType     = NULL;
    mSpacer         = NULL;
    mAirplane       = NULL;

    return LinearLayout::OnDetachedFromWindow();
}

//@Override
ECode SignalClusterView::SetWifiIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 strengthIcon,
    /* [in] */ Int32 activityIcon,
    /* [in] */ const String& contentDescription)
{
    mWifiVisible = visible;
    mWifiStrengthId = strengthIcon;
    mWifiActivityId = activityIcon;
    mWifiDescription = contentDescription;

    Apply();
    return NOERROR;
}

//@Override
ECode SignalClusterView::SetMobileDataIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 strengthIcon,
    /* [in] */ Int32 activityIcon,
    /* [in] */ Int32 typeIcon,
    /* [in] */ const String& contentDescription,
    /* [in] */ const String& typeContentDescription)
{
    mMobileVisible = visible;
    mMobileStrengthId = strengthIcon;
    mMobileActivityId = activityIcon;
    mMobileTypeId = typeIcon;
    mMobileDescription = contentDescription;
    mMobileTypeDescription = typeContentDescription;

    Apply();
    return NOERROR;
}

//@Override
ECode SignalClusterView::SetEthernetIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 strengthIcon,
    /* [in] */ Int32 activityIcon,
    /* [in] */ const String& contentDescription)
{
    mEthernetVisible = visible;
    mEthernetStateId = strengthIcon;
    mEthernetActivityId = activityIcon;
    mEthernetDescription = contentDescription;

    Apply();
    return NOERROR;
}

//@Override
ECode SignalClusterView::SetIsAirplaneMode(
    /* [in] */ Boolean is,
    /* [in] */ Int32 airplaneIcon)
{
    mIsAirplaneMode = is;
    mAirplaneIconId = airplaneIcon;

    Apply();
    return NOERROR;
}

//@Override
Boolean SignalClusterView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    assert(event != NULL);
    AutoPtr<IObjectContainer> container;
    event->GetText((IObjectContainer**)&container);
    // Standard group layout onPopulateAccessibilityEvent() implementations
    // ignore content description, so populate manually
    if (mWifiVisible) {
        AutoPtr<ICharSequence> seq;
        mWifiGroup->GetContentDescription((ICharSequence**)&seq);
        if (seq != NULL) {
            container->Add(seq->Probe(EIID_IInterface));
        }
    }

    if (mMobileVisible) {
        AutoPtr<ICharSequence> seq;
        mMobileGroup->GetContentDescription((ICharSequence**)&seq);
        if (seq != NULL) {
            container->Add(seq->Probe(EIID_IInterface));
        }
    }

    return LinearLayout::DispatchPopulateAccessibilityEvent(event);
}


// Run after each indicator change.
void SignalClusterView::Apply()
{
    if (mWifiGroup == NULL) return;

    if (mWifiVisible) {
        mWifiGroup->SetVisibility(IView::VISIBLE);
        mWifi->SetImageResource(mWifiStrengthId);
        mWifiActivity->SetImageResource(mWifiActivityId);

        // AutoPtr<ICharSequence> seq;
        // CString::New(mWifiDescription, (ICharSequence**)&seq);
        // mWifiGroup->SetContentDescription(seq);
    }
    else {
        mWifiGroup->SetVisibility(IView::GONE);
    }

    if (mEthernetVisible) {
        AutoPtr<ICharSequence> seq;
        CString::New(mEthernetDescription, (ICharSequence**)&seq);
        mEthernetGroup->SetVisibility(IView::VISIBLE);
        mEthernet->SetImageResource(mEthernetStateId);
        mEthernetGroup->SetContentDescription(seq);
    } else {
        mEthernetGroup->SetVisibility(IView::GONE);
    }

    // if (DEBUG) Slog.d(TAG,
    //         String.format("wifi: %s sig=%d act=%d",
    //             (mWifiVisible ? "VISIBLE" : "GONE"),
    //             mWifiStrengthId, mWifiActivityId));

    // if (mMobileVisible && !mIsAirplaneMode) {
    //     mMobileGroup->SetVisibility(IView::VISIBLE);
    //     mMobile->SetImageResource(mMobileStrengthId);
    //     mMobileActivity->SetImageResource(mMobileActivityId);
    //     mMobileType->SetImageResource(mMobileTypeId);
    //     StringBuilder sb(mMobileTypeDescription);
    //     sb += " ";
    //     sb += mMobileDescription;
    //     AutoPtr<ICharSequence> seq = sb.ToCharSequence();
    //     mMobileGroup->SetContentDescription(seq);
    // }
    // else {
    //     mMobileGroup->SetVisibility(IView::GONE);
    // }

    // if (mIsAirplaneMode) {
    //    // mAirplane->SetVisibility(IView::VISIBLE);
    //    // mAirplane->SetImageResource(mAirplaneIconId);
    // } else {
    //     // mAirplane->SetVisibility(IView::GONE);
    // }

    if (mMobileVisible && (mWifiVisible || mEthernetVisible) && mIsAirplaneMode) {
        mSpacer->SetVisibility(IView::INVISIBLE);
    } else {
        mSpacer->SetVisibility(IView::GONE);
    }

    // if (DEBUG) Slog.d(TAG,
    //         String.format("mobile: %s sig=%d act=%d typ=%d",
    //             (mMobileVisible ? "VISIBLE" : "GONE"),
    //             mMobileStrengthId, mMobileActivityId, mMobileTypeId));

    mMobileType->SetVisibility(
            !(mWifiVisible || mEthernetVisible) ? IView::VISIBLE : IView::GONE);
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
