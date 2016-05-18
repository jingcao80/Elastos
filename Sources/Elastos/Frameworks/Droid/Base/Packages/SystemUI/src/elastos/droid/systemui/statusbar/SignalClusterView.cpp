
#include "elastos/droid/systemui/statusbar/SignalClusterView.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_INetworkControllerImplSignalCluster;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_ISecurityControllerCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

SignalClusterView::ChangedRunnable::ChangedRunnable(
    /* [in] */ SignalClusterView* host)
    : mHost(host)
{}

ECode SignalClusterView::ChangedRunnable::Run()
{
    mHost->mSC->IsVpnEnabled(&mHost->mVpnVisible);
    mHost->Apply();
    return NOERROR;
}

CAR_INTERFACE_IMPL(SignalClusterView::TmpCallback, Object, ISecurityControllerCallback)
SignalClusterView::TmpCallback::TmpCallback(
    /* [in] */ SignalClusterView* host)
    : mHost(host)
{}

ECode SignalClusterView::TmpCallback::OnStateChanged()
{
    return mHost->OnStateChanged();
}

const String SignalClusterView::TAG("SignalClusterView");
Boolean SignalClusterView::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
CAR_INTERFACE_IMPL_3(SignalClusterView, LinearLayout, ISignalClusterView \
        , INetworkControllerImplSignalCluster, ISecurityControllerCallback);
SignalClusterView::SignalClusterView()
    : mVpnVisible(FALSE)
    , mWifiVisible(FALSE)
    , mWifiStrengthId(0)
    , mMobileVisible(FALSE)
    , mMobileStrengthId(0)
    , mMobileTypeId(0)
    , mIsAirplaneMode(FALSE)
    , mAirplaneIconId(0)
    , mRoaming(FALSE)
    , mIsMobileTypeIconWide(FALSE)
    , mWideTypeIconStartPadding(0)
{}

ECode SignalClusterView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SignalClusterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode SignalClusterView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return LinearLayout::constructor(context, attrs, defStyle);
}

ECode SignalClusterView::SetNetworkController(
    /* [in] */ INetworkControllerImpl* nc)
{
    if (DEBUG) Logger::D(TAG, "NetworkController=%p", nc);
    mNC = nc;
    return NOERROR;
}

ECode SignalClusterView::SetSecurityController(
    /* [in] */ ISecurityController* sc)
{
    if (DEBUG) Logger::D(TAG, "SecurityController=%p", sc);
    mSC = sc;
    AutoPtr<TmpCallback> callback = new TmpCallback(this);
    mSC->AddCallback(callback);
    mSC->IsVpnEnabled(&mVpnVisible);
    return NOERROR;
}

ECode SignalClusterView::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::wide_type_icon_start_padding, &mWideTypeIconStartPadding);
    return NOERROR;
}

ECode SignalClusterView::OnAttachedToWindow()
{
    LinearLayout::OnAttachedToWindow();

    AutoPtr<IView> tmp;
    FindViewById(R::id::vpn, (IView**)&tmp);
    mVpn = IImageView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::wifi_combo, (IView**)&tmp);
    mWifiGroup = IViewGroup::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::wifi_signal, (IView**)&tmp);
    mWifi = IImageView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::mobile_combo, (IView**)&tmp);
    mMobileGroup = IViewGroup::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::mobile_signal, (IView**)&tmp);
    mMobile = IImageView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::mobile_type, (IView**)&tmp);
    mMobileType = IImageView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::airplane, (IView**)&tmp);
    mAirplane = IImageView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::wifi_airplane_spacer, (IView**)&tmp);
    mWifiAirplaneSpacer = IView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::wifi_signal_spacer, (IView**)&tmp);
    mWifiSignalSpacer = IView::Probe(tmp);

    Apply();
    return NOERROR;
}

ECode SignalClusterView::OnDetachedFromWindow()
{
    mVpn            = NULL;
    mWifiGroup      = NULL;
    mWifi           = NULL;
    mMobileGroup    = NULL;
    mMobile         = NULL;
    mMobileType     = NULL;
    mAirplane       = NULL;

    return LinearLayout::OnDetachedFromWindow();
}

ECode SignalClusterView::OnStateChanged()
{
    AutoPtr<ChangedRunnable> r = new ChangedRunnable(this);
    Boolean tmp = FALSE;
    Post(r, &tmp);
    return NOERROR;
}

ECode SignalClusterView::SetWifiIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 strengthIcon,
    /* [in] */ const String& contentDescription)
{
    mWifiVisible = visible;
    mWifiStrengthId = strengthIcon;
    mWifiDescription = contentDescription;

    Apply();
    return NOERROR;
}

ECode SignalClusterView::SetMobileDataIndicators(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 strengthIcon,
    /* [in] */ Int32 typeIcon,
    /* [in] */ const String& contentDescription,
    /* [in] */ const String& typeContentDescription,
    /* [in] */ Boolean roaming,
    /* [in] */ Boolean isTypeIconWide)
{
    mMobileVisible = visible;
    mMobileStrengthId = strengthIcon;
    mMobileTypeId = typeIcon;
    mMobileDescription = contentDescription;
    mMobileTypeDescription = typeContentDescription;
    mRoaming = roaming;
    mIsMobileTypeIconWide = isTypeIconWide;

    Apply();
    return NOERROR;
}

ECode SignalClusterView::SetIsAirplaneMode(
    /* [in] */ Boolean is,
    /* [in] */ Int32 airplaneIcon)
{
    mIsAirplaneMode = is;
    mAirplaneIconId = airplaneIcon;

    Apply();
    return NOERROR;
}

ECode SignalClusterView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    assert(event != NULL);
    AutoPtr<IList> container;
    IAccessibilityRecord::Probe(event)->GetText((IList**)&container);
    // Standard group layout onPopulateAccessibilityEvent() implementations
    // ignore content description, so populate manually
    if (mWifiVisible && mWifiGroup != NULL) {
        AutoPtr<ICharSequence> seq;
        IView::Probe(mWifiGroup)->GetContentDescription((ICharSequence**)&seq);
        if (seq != NULL) {
            container->Add(seq);
        }
    }

    if (mMobileVisible && mMobileGroup != NULL) {
        AutoPtr<ICharSequence> seq;
        IView::Probe(mMobileGroup)->GetContentDescription((ICharSequence**)&seq);
        if (seq != NULL) {
            container->Add(seq);
        }
    }

    return LinearLayout::DispatchPopulateAccessibilityEvent(event, result);
}

ECode SignalClusterView::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    LinearLayout::OnRtlPropertiesChanged(layoutDirection);

    if (mWifi != NULL) {
        mWifi->SetImageDrawable(NULL);
    }

    if (mMobile != NULL) {
        mMobile->SetImageDrawable(NULL);
    }

    if (mMobileType != NULL) {
        mMobileType->SetImageDrawable(NULL);
    }

    if(mAirplane != NULL) {
        mAirplane->SetImageDrawable(NULL);
    }

    Apply();
    return NOERROR;
}

ECode SignalClusterView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

void SignalClusterView::Apply()
{
    if (mWifiGroup == NULL) return;
    IView::Probe(mVpn)->SetVisibility(mVpnVisible ? IView::VISIBLE : IView::GONE);
    if (DEBUG) Logger::D(TAG, "vpn: %s", (mVpnVisible ? "VISIBLE" : "GONE"));
    if (mWifiVisible) {
        mWifi->SetImageResource(mWifiStrengthId);
        AutoPtr<ICharSequence> des;
        CString::New(mWifiDescription, (ICharSequence**)&des);
        IView::Probe(mWifiGroup)->SetContentDescription(des);
        IView::Probe(mWifiGroup)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mWifiGroup)->SetVisibility(IView::GONE);
    }

    if (DEBUG) Logger::D(TAG, "wifi: %s sig=%d", (mWifiVisible ? "VISIBLE" : "GONE"), mWifiStrengthId);

    if (mMobileVisible && !mIsAirplaneMode) {
        mMobile->SetImageResource(mMobileStrengthId);
        mMobileType->SetImageResource(mMobileTypeId);
        AutoPtr<ICharSequence> des;
        CString::New(mMobileTypeDescription + " " + mMobileDescription, (ICharSequence**)&des);
        IView::Probe(mMobileGroup)->SetContentDescription(des);
        IView::Probe(mMobileGroup)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mMobileGroup)->SetVisibility(IView::GONE);
    }

    if (mIsAirplaneMode) {
        mAirplane->SetImageResource(mAirplaneIconId);
        IView::Probe(mAirplane)->SetVisibility(IView::VISIBLE);
    } else {
        IView::Probe(mAirplane)->SetVisibility(IView::GONE);
    }

    if (mIsAirplaneMode && mWifiVisible) {
        mWifiAirplaneSpacer->SetVisibility(IView::VISIBLE);
    } else {
        mWifiAirplaneSpacer->SetVisibility(IView::GONE);
    }

    if (mRoaming && mMobileVisible && mWifiVisible) {
        mWifiSignalSpacer->SetVisibility(IView::VISIBLE);
    } else {
        mWifiSignalSpacer->SetVisibility(IView::GONE);
    }

    IView::Probe(mMobile)->SetPaddingRelative(mIsMobileTypeIconWide ? mWideTypeIconStartPadding : 0, 0, 0, 0);

    if (DEBUG) Logger::D(TAG, "mobile: %s sig=%d typ=%d", (mMobileVisible ? "VISIBLE" : "GONE"),
                mMobileStrengthId, mMobileTypeId);

    IView::Probe(mMobileType)->SetVisibility((mRoaming || mMobileTypeId != 0) ? IView::VISIBLE : IView::GONE);
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
