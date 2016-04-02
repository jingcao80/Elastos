
#include "elastos/droid/systemui/statusbar/phone/CStatusBarHeaderView.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "../../R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/R.h>
#include <elastos/droid/utility/MathUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::IAnimatable;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::SystemUI::FontSizeUtils;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IBatteryStateChangeCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_INextAlarmChangeCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IUserInfoControllerOnUserInfoChangedListener;
using Elastos::Droid::SystemUI::Qs::EIID_IQSPanelCallback;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IRelativeLayoutLayoutParams;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CStatusBarHeaderView::LayoutValues::LayoutValues()
    : mDateExpandedAlpha(0)
    , mDateCollapsedAlpha(0)
    , mEmergencyCallsOnlyAlpha(0)
    , mAlarmStatusAlpha(0)
    , mTimeScale(1.f)
    , mClockY(0)
    , mDateY(0)
    , mAvatarScale(0)
    , mAvatarX(0)
    , mAvatarY(0)
    , mBatteryX(0)
    , mBatteryY(0)
    , mBatteryLevelAlpha(0)
    , mSettingsAlpha(0)
    , mSettingsTranslation(0)
    , mSignalClusterAlpha(0)
    , mSettingsRotation(0)
{}

void CStatusBarHeaderView::LayoutValues::Interpoloate(
    /* [in] */ LayoutValues* v1,
    /* [in] */ LayoutValues* v2,
    /* [in] */ Float t)
{
    mTimeScale = v1->mTimeScale * (1 - t) + v2->mTimeScale * t;
    mClockY = v1->mClockY * (1 - t) + v2->mClockY * t;
    mDateY = v1->mDateY * (1 - t) + v2->mDateY * t;
    mAvatarScale = v1->mAvatarScale * (1 - t) + v2->mAvatarScale * t;
    mAvatarX = v1->mAvatarX * (1 - t) + v2->mAvatarX * t;
    mAvatarY = v1->mAvatarY * (1 - t) + v2->mAvatarY * t;
    mBatteryX = v1->mBatteryX * (1 - t) + v2->mBatteryX * t;
    mBatteryY = v1->mBatteryY * (1 - t) + v2->mBatteryY * t;
    mSettingsTranslation = v1->mSettingsTranslation * (1 - t) + v2->mSettingsTranslation * t;

    Float t1 = Elastos::Core::Math::Max((Float)0, t - 0.5f) * 2;
    mSettingsRotation = v1->mSettingsRotation * (1 - t1) + v2->mSettingsRotation * t1;
    mEmergencyCallsOnlyAlpha =
            v1->mEmergencyCallsOnlyAlpha * (1 - t1) + v2->mEmergencyCallsOnlyAlpha * t1;

    Float t2 = Elastos::Core::Math::Min((Float)1, 2 * t);
    mSignalClusterAlpha = v1->mSignalClusterAlpha * (1 - t2) + v2->mSignalClusterAlpha * t2;

    Float t3 = Elastos::Core::Math::Max((Float)0, t - 0.7f) / 0.3f;
    mBatteryLevelAlpha = v1->mBatteryLevelAlpha * (1 - t3) + v2->mBatteryLevelAlpha * t3;
    mSettingsAlpha = v1->mSettingsAlpha * (1 - t3) + v2->mSettingsAlpha * t3;
    mDateExpandedAlpha = v1->mDateExpandedAlpha * (1 - t3) + v2->mDateExpandedAlpha * t3;
    mDateCollapsedAlpha = v1->mDateCollapsedAlpha * (1 - t3) + v2->mDateCollapsedAlpha * t3;
    mAlarmStatusAlpha = v1->mAlarmStatusAlpha * (1 - t3) + v2->mAlarmStatusAlpha * t3;
}

CStatusBarHeaderView::Runnable1::Runnable1(
    /* [in] */ QsPanelCallback* host,
    /* [in] */ Boolean state)
    : mHost(host)
    , mState(state)
{}

ECode CStatusBarHeaderView::Runnable1::Run()
{
    mHost->HandleToggleStateChanged(mState);
    return NOERROR;
}

CStatusBarHeaderView::Runnable2::Runnable2(
    /* [in] */ QsPanelCallback* host,
    /* [in] */ IQSTileDetailAdapter* detail)
    : mHost(host)
    , mDetail(detail)
{}

ECode CStatusBarHeaderView::Runnable2::Run()
{
    mHost->HandleShowingDetail(mDetail);
    return NOERROR;
}

CStatusBarHeaderView::Runnable3::Runnable3(
    /* [in] */ QsPanelCallback* host,
    /* [in] */ Boolean state)
    : mHost(host)
    , mState(state)
{}

ECode CStatusBarHeaderView::Runnable3::Run()
{
    mHost->HandleScanStateChanged(mState);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CStatusBarHeaderView::ClickListener, Object, IViewOnClickListener);
CStatusBarHeaderView::ClickListener::ClickListener(
    /* [in] */ CStatusBarHeaderView* host,
    /* [in] */ IQSTileDetailAdapter* detail)
    : mHost(host)
    , mDetail(detail)
{}

ECode CStatusBarHeaderView::ClickListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean tmp = FALSE;
    ICheckable::Probe(mHost->mQsDetailHeaderSwitch)->IsChecked(&tmp);
    AutoPtr<IBoolean> b;
    CBoolean::New(!tmp, (IBoolean**)&b);
    mDetail->SetToggleState(b);
    return NOERROR;
}

CStatusBarHeaderView::Runnable4::Runnable4(
    /* [in] */ IView* v,
    /* [in] */ Boolean in)
    : mView(v)
    , mIn(in)
{}

ECode CStatusBarHeaderView::Runnable4::Run()
{
    if (!mIn) {
        mView->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CStatusBarHeaderView::QsPanelCallback, Object, IQSPanelCallback);
CStatusBarHeaderView::QsPanelCallback::QsPanelCallback(
    /* [in] */ CStatusBarHeaderView* host)
    : mScanState(FALSE)
    , mHost(host)
{}

ECode CStatusBarHeaderView::QsPanelCallback::OnToggleStateChanged(
    /* [in] */ Boolean state)
{
    AutoPtr<Runnable1> run = new Runnable1(this, state);
    Boolean tmp = FALSE;
    mHost->Post(run, &tmp);
    return NOERROR;
}

ECode CStatusBarHeaderView::QsPanelCallback::OnShowingDetail(
    /* [in] */ IQSTileDetailAdapter* detail)
{
    AutoPtr<Runnable2> run = new Runnable2(this, detail);
    Boolean tmp = FALSE;
    mHost->Post(run, &tmp);
    return NOERROR;
}

ECode CStatusBarHeaderView::QsPanelCallback::OnScanStateChanged(
    /* [in] */ Boolean state)
{
    AutoPtr<Runnable3> run = new Runnable3(this, state);
    Boolean tmp = FALSE;
    mHost->Post(run, &tmp);
    return NOERROR;
}

void CStatusBarHeaderView::QsPanelCallback::HandleToggleStateChanged(
    /* [in] */ Boolean state)
{
    ICheckable::Probe(mHost->mQsDetailHeaderSwitch)->SetChecked(state);
}

void CStatusBarHeaderView::QsPanelCallback::HandleScanStateChanged(
    /* [in] */ Boolean state)
{
    if (mScanState == state) return;
    mScanState = state;
    AutoPtr<IDrawable> d;
    mHost->mQsDetailHeaderProgress->GetDrawable((IDrawable**)&d);
    AutoPtr<IAnimatable> anim = IAnimatable::Probe(d);
    AutoPtr<IViewPropertyAnimator> vpa;
    IView::Probe(mHost->mQsDetailHeaderProgress)->Animate((IViewPropertyAnimator**)&vpa);
    if (state) {
        vpa->Alpha(1.f);
        anim->Start();
    }
    else {
        vpa->Alpha(0.f);
        anim->Stop();
    }
}

void CStatusBarHeaderView::QsPanelCallback::HandleShowingDetail(
    /* [in] */ IQSTileDetailAdapter* detail)
{
    const Boolean showingDetail = detail != NULL;
    Transition(mHost->mClock, !showingDetail);
    Transition(mHost->mDateGroup, !showingDetail);
    if (mHost->mAlarmShowing) {
        Transition(IView::Probe(mHost->mAlarmStatus), !showingDetail);
    }
    Transition(mHost->mQsDetailHeader, showingDetail);
    mHost->mShowingDetail = showingDetail;
    if (showingDetail) {
        Int32 v = 0;
        detail->GetTitle(&v);
        mHost->mQsDetailHeaderTitle->SetText(v);
        AutoPtr<IBoolean> toggleState;
        detail->GetToggleState((IBoolean**)&toggleState);
        if (toggleState == NULL) {
            IView::Probe(mHost->mQsDetailHeaderSwitch)->SetVisibility(IView::INVISIBLE);
            mHost->mQsDetailHeader->SetClickable(FALSE);
        }
        else {
            IView::Probe(mHost->mQsDetailHeaderSwitch)->SetVisibility(IView::VISIBLE);
            Boolean tmp = FALSE;
            toggleState->GetValue(&tmp);
            ICheckable::Probe(mHost->mQsDetailHeaderSwitch)->SetChecked(tmp);
            mHost->mQsDetailHeader->SetClickable(TRUE);
            AutoPtr<ClickListener> listener = new ClickListener(mHost, detail);
            mHost->mQsDetailHeader->SetOnClickListener(listener);
        }
    }
    else {
        mHost->mQsDetailHeader->SetClickable(FALSE);
    }
}

void CStatusBarHeaderView::QsPanelCallback::Transition(
    /* [in] */ IView* v,
    /* [in] */ Boolean in)
{
    if (in) {
        v->BringToFront();
        v->SetVisibility(VISIBLE);
    }
    AutoPtr<IViewPropertyAnimator> vpa;
    v->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Alpha(in ? 1 : 0);
    vpa->WithLayer();
    AutoPtr<Runnable4> run = new Runnable4(v, in);
    vpa->WithEndAction(run);
    vpa->Start();
}

CAR_INTERFACE_IMPL(CStatusBarHeaderView::ClickListener2, Object, IViewOnClickListener);
CStatusBarHeaderView::ClickListener2::ClickListener2(
    /* [in] */ CStatusBarHeaderView* host)
    : mHost(host)
{}

ECode CStatusBarHeaderView::ClickListener2::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

CAR_INTERFACE_IMPL(CStatusBarHeaderView::OnLayoutChangeListener, Object, IViewOnLayoutChangeListener);
CStatusBarHeaderView::OnLayoutChangeListener::OnLayoutChangeListener(
    /* [in] */ CStatusBarHeaderView* host)
    : mHost(host)
{}

ECode CStatusBarHeaderView::OnLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    if ((right - left) != (oldRight - oldLeft)) {
        // width changed, update clipping
        Int32 h = 0;
        mHost->GetHeight(&h);
        mHost->SetClipping(h);
    }
    Int32 value = 0;
    Boolean rtl = (mHost->GetLayoutDirection(&value), value) == LAYOUT_DIRECTION_RTL;
    IView::Probe(mHost->mTime)->SetPivotX(rtl ? (IView::Probe(mHost->mTime)->GetWidth(&value), value) : 0);
    IView::Probe(mHost->mTime)->SetPivotY((IView::Probe(mHost->mTime)->GetBaseline(&value), value));
    mHost->UpdateAmPmTranslation();
    return NOERROR;
}

CStatusBarHeaderView::OutlineProvider::OutlineProvider(
    /* [in] */ CStatusBarHeaderView* host)
    : mHost(host)
{}

ECode CStatusBarHeaderView::OutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    outline->SetRect(mHost->mClipBounds);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CStatusBarHeaderView::OnUserInfoChangedListener, Object, IUserInfoControllerOnUserInfoChangedListener);
CStatusBarHeaderView::OnUserInfoChangedListener::OnUserInfoChangedListener(
    /* [in] */ CStatusBarHeaderView* host)
    : mHost(host)
{}

ECode CStatusBarHeaderView::OnUserInfoChangedListener::OnUserInfoChanged(
    /* [in] */ const String& name,
    /* [in] */ IDrawable* picture)
{
    mHost->mMultiUserAvatar->SetImageDrawable(picture);
    return NOERROR;
}

CAR_OBJECT_IMPL(CStatusBarHeaderView);
CAR_INTERFACE_IMPL_4(CStatusBarHeaderView, RelativeLayout, IStatusBarHeaderView, IViewOnClickListener \
        , IBatteryStateChangeCallback, INextAlarmChangeCallback);
CStatusBarHeaderView::CStatusBarHeaderView()
    : mExpanded(FALSE)
    , mListening(FALSE)
    , mShowEmergencyCallsOnly(FALSE)
    , mAlarmShowing(FALSE)
    , mCollapsedHeight(0)
    , mExpandedHeight(0)
    , mMultiUserExpandedMargin(0)
    , mMultiUserCollapsedMargin(0)
    , mClockMarginBottomExpanded(0)
    , mClockMarginBottomCollapsed(0)
    , mMultiUserSwitchWidthCollapsed(0)
    , mMultiUserSwitchWidthExpanded(0)
    , mClockCollapsedSize(0)
    , mClockExpandedSize(0)
    , mClockCollapsedScaleFactor(0)
    , mAvatarCollapsedScaleFactor(0)
    , mCaptureValues(FALSE)
    , mSignalClusterDetached(FALSE)
    , mCurrentT(0)
    , mShowingDetail(FALSE)
{
    CRect::New((IRect**)&mClipBounds);
    mCollapsedValues = new LayoutValues();
    mExpandedValues = new LayoutValues();
    mCurrentValues = new LayoutValues();
    mQsPanelCallback = new QsPanelCallback(this);
}

ECode CStatusBarHeaderView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

ECode CStatusBarHeaderView::OnFinishInflate()
{
    RelativeLayout::OnFinishInflate();
    FindViewById(R::id::system_icons_super_container, (IView**)&mSystemIconsSuperContainer);

    AutoPtr<IView> view;
    FindViewById(R::id::system_icons_container, (IView**)&view);
    mSystemIconsContainer = IViewGroup::Probe(view);

    AutoPtr<ClickListener2> cl = new ClickListener2(this);
    mSystemIconsSuperContainer->SetOnClickListener(cl);
    FindViewById(R::id::date_group, (IView**)&mDateGroup);
    FindViewById(R::id::clock, (IView**)&mClock);

    view = NULL;
    FindViewById(R::id::time_view, (IView**)&view);
    mTime = ITextView::Probe(view);

    view = NULL;
    FindViewById(R::id::am_pm_view, (IView**)&view);
    mAmPm = ITextView::Probe(view);

    view = NULL;
    FindViewById(R::id::multi_user_switch, (IView**)&view);
    mMultiUserSwitch = IMultiUserSwitch::Probe(view);

    view = NULL;
    FindViewById(R::id::multi_user_avatar, (IView**)&view);
    mMultiUserAvatar = IImageView::Probe(view);

    view = NULL;
    FindViewById(R::id::date_collapsed, (IView**)&view);
    mDateCollapsed = ITextView::Probe(view);

    view = NULL;
    FindViewById(R::id::date_expanded, (IView**)&view);
    mDateExpanded = ITextView::Probe(view);
    FindViewById(R::id::settings_button, (IView**)&mSettingsButton);
    mSettingsButton->SetOnClickListener(cl);
    FindViewById(R::id::qs_detail_header, (IView**)&mQsDetailHeader);
    mQsDetailHeader->SetAlpha(0);

    view = NULL;
    mQsDetailHeader->FindViewById(Elastos::Droid::R::id::title, (IView**)&view);
    mQsDetailHeaderTitle = ITextView::Probe(view);

    view = NULL;
    mQsDetailHeader->FindViewById(Elastos::Droid::R::id::toggle, (IView**)&view);
    mQsDetailHeaderSwitch = ISwitch::Probe(view);

    view = NULL;
    FindViewById(R::id::qs_detail_header_progress, (IView**)&view);
    mQsDetailHeaderProgress = IImageView::Probe(view);

    view = NULL;
    FindViewById(R::id::header_emergency_calls_only, (IView**)&view);
    mEmergencyCallsOnly = ITextView::Probe(view);

    view = NULL;
    FindViewById(R::id::battery_level, (IView**)&view);
    mBatteryLevel = ITextView::Probe(view);

    view = NULL;
    FindViewById(R::id::alarm_status, (IView**)&view);
    mAlarmStatus = ITextView::Probe(view);
    IView::Probe(mAlarmStatus)->SetOnClickListener(cl);
    FindViewById(R::id::signal_cluster, (IView**)&mSignalCluster);

    view = NULL;
    FindViewById(R::id::system_icons, (IView**)&view);
    mSystemIcons = ILinearLayout::Probe(view);
    LoadDimens();
    UpdateVisibilities();
    UpdateClockScale();
    UpdateAvatarScale();

    AutoPtr<OnLayoutChangeListener> ocl = new OnLayoutChangeListener(this);
    AddOnLayoutChangeListener(ocl);

    AutoPtr<OutlineProvider> op = new OutlineProvider(this);
    SetOutlineProvider(op);
    RequestCaptureValues();
    return NOERROR;
}

ECode CStatusBarHeaderView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    RelativeLayout::OnLayout(changed, l, t, r, b);
    if (mCaptureValues) {
        if (mExpanded) {
            CaptureLayoutValues(mExpandedValues);
        }
        else {
            CaptureLayoutValues(mCollapsedValues);
        }
        mCaptureValues = FALSE;
        UpdateLayoutValues(mCurrentT);
    }
    Int32 left = 0, right = 0;
    mDateGroup->GetLeft(&left);
    IView::Probe(mDateCollapsed)->GetRight(&right);
    IView::Probe(mAlarmStatus)->SetX(left + right);
    return NOERROR;
}

void CStatusBarHeaderView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    RelativeLayout::OnConfigurationChanged(newConfig);
    FontSizeUtils::UpdateFontSize(mBatteryLevel, R::dimen::battery_level_text_size);
    FontSizeUtils::UpdateFontSize(mEmergencyCallsOnly,
            R::dimen::qs_emergency_calls_only_text_size);
    FontSizeUtils::UpdateFontSize(mDateCollapsed, R::dimen::qs_date_collapsed_size);
    FontSizeUtils::UpdateFontSize(mDateExpanded, R::dimen::qs_date_collapsed_size);
    FontSizeUtils::UpdateFontSize(mAlarmStatus, R::dimen::qs_date_collapsed_size);
    FontSizeUtils::UpdateFontSize(this, Elastos::Droid::R::id::title, R::dimen::qs_detail_header_text_size);
    FontSizeUtils::UpdateFontSize(this, Elastos::Droid::R::id::toggle, R::dimen::qs_detail_header_text_size);
    FontSizeUtils::UpdateFontSize(mAmPm, R::dimen::qs_time_collapsed_size);
    FontSizeUtils::UpdateFontSize(this, R::id::empty_time_view, R::dimen::qs_time_expanded_size);

    mEmergencyCallsOnly->SetText(Elastos::Droid::R::string::emergency_calls_only);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::qs_time_collapsed_size, &mClockCollapsedSize);
    res->GetDimensionPixelSize(R::dimen::qs_time_expanded_size, &mClockExpandedSize);
    mClockCollapsedScaleFactor = (Float) mClockCollapsedSize / (Float) mClockExpandedSize;

    UpdateClockScale();
    UpdateClockCollapsedMargin();
}

void CStatusBarHeaderView::UpdateClockCollapsedMargin()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 padding = 0;
    res->GetDimensionPixelSize(R::dimen::clock_collapsed_bottom_margin, &padding);
    Int32 largePadding = 0;
    res->GetDimensionPixelSize(R::dimen::clock_collapsed_bottom_margin_large_text, &largePadding);

    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Float fontScale = 0;
    config->GetFontScale(&fontScale);
    Float largeFactor = (MathUtils::Constrain(fontScale, 1.0f,
            FontSizeUtils::LARGE_TEXT_SCALE) - 1.f) / (FontSizeUtils::LARGE_TEXT_SCALE - 1.f);
    mClockMarginBottomCollapsed = Elastos::Core::Math::Round((1 - largeFactor) * padding + largeFactor * largePadding);
    RequestLayout();
}

void CStatusBarHeaderView::RequestCaptureValues()
{
    mCaptureValues = TRUE;
    RequestLayout();
}

void CStatusBarHeaderView::LoadDimens()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::status_bar_header_height, &mCollapsedHeight);
    res->GetDimensionPixelSize(R::dimen::status_bar_header_height_expanded, &mExpandedHeight);
    res->GetDimensionPixelSize(R::dimen::multi_user_switch_expanded_margin, &mMultiUserExpandedMargin);
    res->GetDimensionPixelSize(R::dimen::multi_user_switch_collapsed_margin, &mMultiUserCollapsedMargin);
    res->GetDimensionPixelSize(R::dimen::clock_expanded_bottom_margin, &mClockMarginBottomExpanded);
    UpdateClockCollapsedMargin();
    res->GetDimensionPixelSize(R::dimen::multi_user_switch_width_collapsed, &mMultiUserSwitchWidthCollapsed);
    res->GetDimensionPixelSize(R::dimen::multi_user_switch_width_expanded, &mMultiUserSwitchWidthExpanded);

    AutoPtr<IViewGroupLayoutParams> vp;
    IView::Probe(mMultiUserAvatar)->GetLayoutParams((IViewGroupLayoutParams**)&vp);
    Int32 w = 0, v = 0;
    vp->GetWidth(&w);
    res->GetDimensionPixelSize(R::dimen::multi_user_avatar_collapsed_size, &v);
    mAvatarCollapsedScaleFactor = v / (Float) w;
    res->GetDimensionPixelSize(R::dimen::qs_time_collapsed_size, &mClockCollapsedSize);
    res->GetDimensionPixelSize(R::dimen::qs_time_expanded_size, &mClockExpandedSize);
    mClockCollapsedScaleFactor = (Float) mClockCollapsedSize / (Float) mClockExpandedSize;
}

ECode CStatusBarHeaderView::SetActivityStarter(
    /* [in] */ IActivityStarter* activityStarter)
{
    mActivityStarter = activityStarter;
    return NOERROR;
}

ECode CStatusBarHeaderView::SetBatteryController(
    /* [in] */ IBatteryController* batteryController)
{
    mBatteryController = batteryController;
    AutoPtr<IView> view;
    FindViewById(R::id::battery, (IView**)&view);
    IBatteryMeterView::Probe(view)->SetBatteryController(batteryController);
    return NOERROR;
}

ECode CStatusBarHeaderView::SetNextAlarmController(
    /* [in] */ INextAlarmController* nextAlarmController)
{
    mNextAlarmController = nextAlarmController;
    return NOERROR;
}

ECode CStatusBarHeaderView::GetCollapsedHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCollapsedHeight;
    return NOERROR;
}

ECode CStatusBarHeaderView::GetExpandedHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExpandedHeight;
    return NOERROR;
}

ECode CStatusBarHeaderView::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening == mListening) {
        return NOERROR;
    }
    mListening = listening;
    UpdateListeners();
    return NOERROR;
}

ECode CStatusBarHeaderView::SetExpanded(
    /* [in] */ Boolean expanded)
{
    Boolean changed = expanded != mExpanded;
    mExpanded = expanded;
    if (changed) {
        UpdateEverything();
    }
    return NOERROR;
}

ECode CStatusBarHeaderView::UpdateEverything()
{
    UpdateHeights();
    UpdateVisibilities();
    UpdateSystemIconsLayoutParams();
    UpdateClickTargets();
    UpdateMultiUserSwitch();
    if (mQSPanel != NULL) {
        mQSPanel->SetExpanded(mExpanded);
    }
    UpdateClockScale();
    UpdateAvatarScale();
    UpdateClockLp();
    RequestCaptureValues();
    return NOERROR;
}

void CStatusBarHeaderView::UpdateHeights()
{
    Int32 height = mExpanded ? mExpandedHeight : mCollapsedHeight;
    AutoPtr<IViewGroupLayoutParams> lp;
    GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 h = 0;
    lp->GetHeight(&h);
    if (h != height) {
        lp->SetHeight(height);
        SetLayoutParams(lp);
    }
}

void CStatusBarHeaderView::UpdateVisibilities()
{
    IView::Probe(mDateCollapsed)->SetVisibility(mExpanded && mAlarmShowing ? IView::VISIBLE : IView::INVISIBLE);
    IView::Probe(mDateExpanded)->SetVisibility(mExpanded && mAlarmShowing ? IView::INVISIBLE : IView::VISIBLE);
    IView::Probe(mAlarmStatus)->SetVisibility(mExpanded && mAlarmShowing ? IView::VISIBLE : IView::INVISIBLE);
    mSettingsButton->SetVisibility(mExpanded ? IView::VISIBLE : IView::INVISIBLE);
    mQsDetailHeader->SetVisibility(mExpanded && mShowingDetail? IView::VISIBLE : IView::INVISIBLE);
    if (mSignalCluster != NULL) {
        UpdateSignalClusterDetachment();
    }
    IView::Probe(mEmergencyCallsOnly)->SetVisibility(mExpanded && mShowEmergencyCallsOnly ? IView::VISIBLE : IView::GONE);
    IView::Probe(mBatteryLevel)->SetVisibility(mExpanded ? IView::VISIBLE : IView::GONE);
}

void CStatusBarHeaderView::UpdateSignalClusterDetachment()
{
    Boolean detached = mExpanded;
    if (detached != mSignalClusterDetached) {
        if (detached) {
            AutoPtr<IViewOverlay> vo;
            GetOverlay((IViewOverlay**)&vo);
            IViewGroupOverlay::Probe(vo)->Add(mSignalCluster);
        }
        else {
            ReattachSignalCluster();
        }
    }
    mSignalClusterDetached = detached;
}

void CStatusBarHeaderView::ReattachSignalCluster()
{
    AutoPtr<IViewOverlay> vo;
    GetOverlay((IViewOverlay**)&vo);
    IViewGroupOverlay::Probe(vo)->Remove(mSignalCluster);
    IViewGroup::Probe(mSystemIcons)->AddView(mSignalCluster, 1);
}

void CStatusBarHeaderView::UpdateSystemIconsLayoutParams()
{
    AutoPtr<IViewGroupLayoutParams> vp;
    mSystemIconsSuperContainer->GetLayoutParams((IViewGroupLayoutParams**)&vp);
    AutoPtr<IRelativeLayoutLayoutParams> lp = IRelativeLayoutLayoutParams::Probe(vp);

    Int32 i1 = 0, i2 = 0;
    Int32 rule = mExpanded
            ? (mSettingsButton->GetId(&i1), i1)
            : (IView::Probe(mMultiUserSwitch)->GetId(&i2), i2);

    AutoPtr<ArrayOf<Int32> > ivs;
    lp->GetRules((ArrayOf<Int32>**)&ivs);
    if (rule != (*ivs)[IRelativeLayout::START_OF]) {
        lp->AddRule(IRelativeLayout::START_OF, rule);
        mSystemIconsSuperContainer->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
}

void CStatusBarHeaderView::UpdateListeners()
{
    if (mListening) {
        mBatteryController->AddStateChangedCallback(this);
        mNextAlarmController->AddStateChangedCallback(this);
    }
    else {
        mBatteryController->RemoveStateChangedCallback(this);
        mNextAlarmController->RemoveStateChangedCallback(this);
    }
}

void CStatusBarHeaderView::UpdateAvatarScale()
{
    if (mExpanded) {
        IView::Probe(mMultiUserAvatar)->SetScaleX(1.f);
        IView::Probe(mMultiUserAvatar)->SetScaleY(1.f);
    }
    else {
        IView::Probe(mMultiUserAvatar)->SetScaleX(mAvatarCollapsedScaleFactor);
        IView::Probe(mMultiUserAvatar)->SetScaleY(mAvatarCollapsedScaleFactor);
    }
}

void CStatusBarHeaderView::UpdateClockScale()
{
    mTime->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mExpanded
            ? mClockExpandedSize
            : mClockCollapsedSize);
    IView::Probe(mTime)->SetScaleX(1.f);
    IView::Probe(mTime)->SetScaleY(1.f);
    UpdateAmPmTranslation();
}

void CStatusBarHeaderView::UpdateAmPmTranslation()
{
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    Boolean rtl = dir == LAYOUT_DIRECTION_RTL;
    Int32 w = 0;
    IView::Probe(mTime)->GetWidth(&w);
    Float x = 0;
    IView::Probe(mTime)->GetScaleX(&x);
    IView::Probe(mAmPm)->SetTranslationX((rtl ? 1 : -1) * w * (1 - x));
}

ECode CStatusBarHeaderView::OnBatteryLevelChanged(
    /* [in] */ Int32 level,
    /* [in] */ Boolean pluggedIn,
    /* [in] */ Boolean charging)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    AutoPtr<IInteger32> obj;
    CInteger32::New(level, (IInteger32**)&obj);
    AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
    objs->Set(0, obj);

    String value;
    res->GetString(R::string::battery_level_template, objs, &value);
    AutoPtr<ICharSequence> cs;
    CString::New(value, (ICharSequence**)&cs);
    mBatteryLevel->SetText(cs);
    return NOERROR;
}

ECode CStatusBarHeaderView::OnPowerSaveChanged()
{
    // could not care less
    return NOERROR;
}

ECode CStatusBarHeaderView::OnNextAlarmChanged(
    /* [in] */ IAlarmClockInfo* nextAlarm)
{
    mNextAlarm = nextAlarm;
    if (nextAlarm != NULL) {
        assert(0 && "TODO: Need the app Keyguard.");
        // mAlarmStatus->SetText(KeyguardStatusView.formatNextAlarm(getContext(), nextAlarm));
    }
    mAlarmShowing = nextAlarm != NULL;
    UpdateEverything();
    RequestCaptureValues();
    return NOERROR;
}

void CStatusBarHeaderView::UpdateClickTargets()
{
    IView::Probe(mMultiUserSwitch)->SetClickable(mExpanded);
    IView::Probe(mMultiUserSwitch)->SetFocusable(mExpanded);
    mSystemIconsSuperContainer->SetClickable(mExpanded);
    mSystemIconsSuperContainer->SetFocusable(mExpanded);
    AutoPtr<IPendingIntent> pi;
    IView::Probe(mAlarmStatus)->SetClickable(mNextAlarm != NULL && (mNextAlarm->GetShowIntent((IPendingIntent**)&pi), pi) != NULL);
}

void CStatusBarHeaderView::UpdateClockLp()
{
    Int32 marginBottom = mExpanded
            ? mClockMarginBottomExpanded
            : mClockMarginBottomCollapsed;

    AutoPtr<IViewGroupLayoutParams> lp;
    mDateGroup->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 bottomMargin = 0;
    IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottomMargin);
    if (marginBottom != bottomMargin) {
        IViewGroupMarginLayoutParams::Probe(lp)->SetBottomMargin(marginBottom);
        mDateGroup->SetLayoutParams(lp);
    }
}

void CStatusBarHeaderView::UpdateMultiUserSwitch()
{
    Int32 marginEnd;
    Int32 width;
    if (mExpanded) {
        marginEnd = mMultiUserExpandedMargin;
        width = mMultiUserSwitchWidthExpanded;
    }
    else {
        marginEnd = mMultiUserCollapsedMargin;
        width = mMultiUserSwitchWidthCollapsed;
    }

    AutoPtr<IViewGroupLayoutParams> vp;
    IView::Probe(mMultiUserSwitch)->GetLayoutParams((IViewGroupLayoutParams**)&vp);
    AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(vp);
    Int32 me = 0, w = 0;
    if (marginEnd != (lp->GetMarginEnd(&me), me) || (vp->GetWidth(&w), w) != width) {
        lp->SetMarginEnd(marginEnd);
        vp->SetWidth(width);
        IView::Probe(mMultiUserSwitch)->SetLayoutParams(vp);
    }
}

ECode CStatusBarHeaderView::SetExpansion(
    /* [in] */ Float t)
{
    if (!mExpanded) {
        t = 0.f;
    }
    mCurrentT = t;
    Float height = mCollapsedHeight + t * (mExpandedHeight - mCollapsedHeight);
    if (height < mCollapsedHeight) {
        height = mCollapsedHeight;
    }
    if (height > mExpandedHeight) {
        height = mExpandedHeight;
    }
    SetClipping(height);
    UpdateLayoutValues(t);
    return NOERROR;
}

void CStatusBarHeaderView::UpdateLayoutValues(
    /* [in] */ Float t)
{
    if (mCaptureValues) {
        return;
    }
    mCurrentValues->Interpoloate(mCollapsedValues, mExpandedValues, t);
    ApplyLayoutValues(mCurrentValues);
}

void CStatusBarHeaderView::SetClipping(
    /* [in] */ Float height)
{
    Int32 pl = 0, w = 0, pr = 0;
    GetPaddingLeft(&pl);
    GetWidth(&w);
    GetPaddingRight(&pr);
    mClipBounds->Set(pl, 0, w - pr, (Int32) height);
    SetClipBounds(mClipBounds);
    InvalidateOutline();
}

ECode CStatusBarHeaderView::SetUserInfoController(
    /* [in] */ IUserInfoController* userInfoController)
{
    AutoPtr<OnUserInfoChangedListener> cl = new OnUserInfoChangedListener(this);
    userInfoController->AddListener(cl);
    return NOERROR;
}

ECode CStatusBarHeaderView::OnClick(
    /* [in] */ IView* v)
{
    if (v == mSettingsButton) {
        StartSettingsActivity();
    }
    else if (v == mSystemIconsSuperContainer) {
        StartBatteryActivity();
    }
    else if (v == IView::Probe(mAlarmStatus) && mNextAlarm != NULL) {
        AutoPtr<IPendingIntent> showIntent;
        mNextAlarm->GetShowIntent((IPendingIntent**)&showIntent);
        Boolean tmp = FALSE;
        if (showIntent != NULL && (showIntent->IsActivity(&tmp), tmp)) {
            AutoPtr<IIntent> i;
            showIntent->GetIntent((IIntent**)&i);
            mActivityStarter->StartActivity(i, TRUE /* dismissShade */);
        }
    }
    return NOERROR;
}

void CStatusBarHeaderView::StartSettingsActivity()
{
    AutoPtr<IIntent> i;
    CIntent::New(ISettings::ACTION_SETTINGS, (IIntent**)&i);
    mActivityStarter->StartActivity(i, TRUE /* dismissShade */);
}

void CStatusBarHeaderView::StartBatteryActivity()
{
    AutoPtr<IIntent> i;
    CIntent::New(IIntent::ACTION_POWER_USAGE_SUMMARY, (IIntent**)&i);
    mActivityStarter->StartActivity(i, TRUE /* dismissShade */);
}

ECode CStatusBarHeaderView::SetQSPanel(
    /* [in] */ IQSPanel* qsp)
{
    mQSPanel = qsp;
    if (mQSPanel != NULL) {
        mQSPanel->SetCallback(mQsPanelCallback);
    }
    mMultiUserSwitch->SetQsPanel(qsp);
    return NOERROR;
}

ECode CStatusBarHeaderView::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CStatusBarHeaderView::SetShowEmergencyCallsOnly(
    /* [in] */ Boolean show)
{
    Boolean changed = show != mShowEmergencyCallsOnly;
    if (changed) {
        mShowEmergencyCallsOnly = show;
        if (mExpanded) {
            UpdateEverything();
            RequestCaptureValues();
        }
    }
    return NOERROR;
}

void CStatusBarHeaderView::DispatchSetPressed(
    /* [in] */ Boolean pressed)
{
    // We don't want that everything lights up when we click on the header, so block the request
    // here.
}

void CStatusBarHeaderView::CaptureLayoutValues(
    /* [in] */ LayoutValues* target)
{
    target->mTimeScale = mExpanded ? 1.f : mClockCollapsedScaleFactor;
    Int32 v = 0;
    target->mClockY = (mClock->GetBottom(&v), v);
    target->mDateY = (mDateGroup->GetTop(&v), v);
    target->mEmergencyCallsOnlyAlpha = GetAlphaForVisibility(IView::Probe(mEmergencyCallsOnly));
    target->mAlarmStatusAlpha = GetAlphaForVisibility(IView::Probe(mAlarmStatus));
    target->mDateCollapsedAlpha = GetAlphaForVisibility(IView::Probe(mDateCollapsed));
    target->mDateExpandedAlpha = GetAlphaForVisibility(IView::Probe(mDateExpanded));
    IView::Probe(mMultiUserAvatar)->GetScaleX(&target->mAvatarScale);
    Int32 v2 = 0;
    target->mAvatarX = (IView::Probe(mMultiUserSwitch)->GetLeft(&v), v) + (IView::Probe(mMultiUserAvatar)->GetLeft(&v2), v2);
    target->mAvatarY = (IView::Probe(mMultiUserSwitch)->GetTop(&v), v) + (IView::Probe(mMultiUserAvatar)->GetTop(&v2), v2);
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    if (dir == LAYOUT_DIRECTION_LTR) {
        target->mBatteryX = (mSystemIconsSuperContainer->GetLeft(&v), v)
                + (IView::Probe(mSystemIconsContainer)->GetRight(&v2), v2);
    }
    else {
        target->mBatteryX = (mSystemIconsSuperContainer->GetLeft(&v), v)
                + (IView::Probe(mSystemIconsContainer)->GetLeft(&v2), v2);
    }
    target->mBatteryY = (mSystemIconsSuperContainer->GetTop(&v), v) + (IView::Probe(mSystemIconsContainer)->GetTop(&v2), v2);
    target->mBatteryLevelAlpha = GetAlphaForVisibility(IView::Probe(mBatteryLevel));
    target->mSettingsAlpha = GetAlphaForVisibility(mSettingsButton);
    target->mSettingsTranslation = mExpanded
            ? 0
            : (IView::Probe(mMultiUserSwitch)->GetLeft(&v), v) - (mSettingsButton->GetLeft(&v2), v2);
    target->mSignalClusterAlpha = mSignalClusterDetached ? 0.f : 1.f;
    target->mSettingsRotation = !mExpanded ? 90.f : 0.f;
}

Float CStatusBarHeaderView::GetAlphaForVisibility(
    /* [in] */ IView* v)
{
    Int32 value = 0;
    return v == NULL || (v->GetVisibility(&value), value) == IView::VISIBLE ? 1.f : 0.f;
}

void CStatusBarHeaderView::ApplyAlpha(
    /* [in] */ IView* v,
    /* [in] */ Float alpha)
{
    Int32 value = 0;
    if (v == NULL || (v->GetVisibility(&value)) == IView::GONE) {
        return;
    }
    if (alpha == 0.f) {
        v->SetVisibility(IView::INVISIBLE);
    }
    else {
        v->SetVisibility(IView::VISIBLE);
        v->SetAlpha(alpha);
    }
}

void CStatusBarHeaderView::ApplyLayoutValues(
    /* [in] */ LayoutValues* values)
{
    IView::Probe(mTime)->SetScaleX(values->mTimeScale);
    IView::Probe(mTime)->SetScaleY(values->mTimeScale);
    Int32 v = 0;
    mClock->SetY(values->mClockY - (mClock->GetHeight(&v), v));
    mDateGroup->SetY(values->mDateY);
    IView::Probe(mAlarmStatus)->SetY(values->mDateY - (IView::Probe(mAlarmStatus)->GetPaddingTop(&v), v));
    IView::Probe(mMultiUserAvatar)->SetScaleX(values->mAvatarScale);
    IView::Probe(mMultiUserAvatar)->SetScaleY(values->mAvatarScale);
    IView::Probe(mMultiUserAvatar)->SetX(values->mAvatarX - (IView::Probe(mMultiUserSwitch)->GetLeft(&v), v));
    IView::Probe(mMultiUserAvatar)->SetY(values->mAvatarY - (IView::Probe(mMultiUserSwitch)->GetTop(&v), v));
    if ((GetLayoutDirection(&v), v) == LAYOUT_DIRECTION_LTR) {
        mSystemIconsSuperContainer->SetX(values->mBatteryX - (IView::Probe(mSystemIconsContainer)->GetRight(&v), v));
    }
    else {
        mSystemIconsSuperContainer->SetX(values->mBatteryX - (IView::Probe(mSystemIconsContainer)->GetLeft(&v), v));
    }
    mSystemIconsSuperContainer->SetY(values->mBatteryY - (IView::Probe(mSystemIconsContainer)->GetTop(&v), v));
    if (mSignalCluster != NULL && mExpanded) {
        Float fv = 0;
        if ((GetLayoutDirection(&v), v) == LAYOUT_DIRECTION_LTR) {
            mSignalCluster->SetX((mSystemIconsSuperContainer->GetX(&fv), fv)
                    - (mSignalCluster->GetWidth(&v), v));
        }
        else {
            mSignalCluster->SetX((mSystemIconsSuperContainer->GetX(&fv), fv)
                    + (mSystemIconsSuperContainer->GetWidth(&v), v));
        }
        Int32 v2 = 0;
        mSignalCluster->SetY(
                (mSystemIconsSuperContainer->GetY(&fv), fv) + (mSystemIconsSuperContainer->GetHeight(&v), v) / 2
                        - (mSignalCluster->GetHeight(&v2), v2) / 2);
    }
    else if (mSignalCluster != NULL) {
        mSignalCluster->SetTranslationX(0.f);
        mSignalCluster->SetTranslationY(0.f);
    }

    Float y = 0;
    mSettingsButton->SetTranslationY((mSystemIconsSuperContainer->GetTranslationY(&y), y));
    mSettingsButton->SetTranslationX(values->mSettingsTranslation);
    mSettingsButton->SetRotation(values->mSettingsRotation);
    ApplyAlpha(IView::Probe(mEmergencyCallsOnly), values->mEmergencyCallsOnlyAlpha);
    if (!mShowingDetail) {
        // Otherwise it needs to stay invisible
        ApplyAlpha(IView::Probe(mAlarmStatus), values->mAlarmStatusAlpha);
    }
    ApplyAlpha(IView::Probe(mDateCollapsed), values->mDateCollapsedAlpha);
    ApplyAlpha(IView::Probe(mDateExpanded), values->mDateExpandedAlpha);
    ApplyAlpha(IView::Probe(mBatteryLevel), values->mBatteryLevelAlpha);
    ApplyAlpha(mSettingsButton, values->mSettingsAlpha);
    ApplyAlpha(mSignalCluster, values->mSignalClusterAlpha);
    if (!mExpanded) {
        IView::Probe(mTime)->SetScaleX(1.f);
        IView::Probe(mTime)->SetScaleY(1.f);
    }
    UpdateAmPmTranslation();
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
