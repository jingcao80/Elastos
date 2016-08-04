
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/CCallCardFragment.h"
#include "elastos/droid/incallui/CallCardPresenter.h"
#include "elastos/droid/incallui/CInCallActivity.h"
#include "elastos/droid/incallui/InCallAnimationUtils.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
#include "R.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Telecom::IVideoProfileVideoStateHelper;
using Elastos::Droid::Telecom::CVideoProfileVideoStateHelper;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewAnimationUtilsHelper;
using Elastos::Droid::View::CViewAnimationUtilsHelper;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::Phone::Common::Animation::AnimUtils;
using Elastos::Droid::Dialer::R;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//==========================================================================
// CCallCardFragment::LayoutIgnoringListener
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::LayoutIgnoringListener, Object, IViewOnLayoutChangeListener);

ECode CCallCardFragment::LayoutIgnoringListener::OnLayoutChange(
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
    v->SetLeft(oldLeft);
    v->SetRight(oldRight);
    v->SetTop(oldTop);
    v->SetBottom(oldBottom);
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::ActionButtonClickListener
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::ActionButtonClickListener, Object, IViewOnClickListener);

ECode CCallCardFragment::ActionButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    CallCardPresenter* presenter = (CallCardPresenter*)mHost->GetPresenter().Get();
    presenter->EndCallClicked();
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::CallInfoClickListener
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::CallInfoClickListener, Object, IViewOnClickListener);

ECode CCallCardFragment::CallInfoClickListener::OnClick(
    /* [in] */ IView* v)
{
    CallCardPresenter* presenter = (CallCardPresenter*)mHost->GetPresenter().Get();
    presenter->SecondaryInfoClicked();
    mHost->UpdateFabPositionForSecondaryCallInfo();
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::CallStateButtonClickListener
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::CallStateButtonClickListener, Object, IViewOnClickListener);

ECode CCallCardFragment::CallStateButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    CallCardPresenter* presenter = (CallCardPresenter*)mHost->GetPresenter().Get();
    presenter->OnCallStateButtonTouched();
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::ManageConferenceCallButtonClickListener
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::ManageConferenceCallButtonClickListener, Object, IViewOnClickListener);

ECode CCallCardFragment::ManageConferenceCallButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    CInCallActivity* inCallActivity = (CInCallActivity*)activity.Get();
    inCallActivity->ShowConferenceCallManager();
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::AnimatorListener
//==========================================================================
ECode CCallCardFragment::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    FAIL_RETURN(AnimatorListenerAdapter::OnAnimationEnd(animation));
    if (!mVisible) {
        mHost->mPrimaryCallCardContainer->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode CCallCardFragment::AnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    FAIL_RETURN(AnimatorListenerAdapter::OnAnimationStart(animation));
    if (mVisible) {
        mHost->mPrimaryCallCardContainer->SetVisibility(IView::VISIBLE);
    }
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::ViewTreePreDrawListener
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::ViewTreePreDrawListener, Object, IOnPreDrawListener);

ECode CCallCardFragment::ViewTreePreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // We don't want to continue getting called.
    Boolean isAlive;
    if (mObserver->IsAlive(&isAlive), isAlive) {
        mObserver->RemoveOnPreDrawListener(this);
    }

    Float videoViewTranslation = 0;

    // Translate the call card to its pre-animation state.
    if (mHost->mIsLandscape) {
        Int32 width;
        mHost->mPrimaryCallCardContainer->GetWidth(&width);
        Float translationX = width;
        translationX *= mIsLayoutRtl ? 1 : -1;

        mHost->mPrimaryCallCardContainer->SetTranslationX(mVisible ? translationX : 0);

        if (mVisible) {
            mVideoView->GetWidth(&width);
            videoViewTranslation = width / 2 - mSpaceBesideCallCard / 2;
            videoViewTranslation *= mIsLayoutRtl ? -1 : 1;
        }
    }
    else {
        Int32 height;
        mHost->mPrimaryCallCardContainer->GetHeight(&height);
        mHost->mPrimaryCallCardContainer->SetTranslationY(mVisible ? -height : 0);

        if (mVisible) {
            mVideoView->GetHeight(&height);
            videoViewTranslation = height / 2 - mSpaceBesideCallCard / 2;
        }
    }

    // Perform animation of video view.
    AutoPtr<IViewPropertyAnimator> videoViewAnimator;
    mVideoView->Animate((IViewPropertyAnimator**)&videoViewAnimator);
    videoViewAnimator->SetInterpolator(ITimeInterpolator::Probe(AnimUtils::EASE_OUT_EASE_IN));
    videoViewAnimator->SetDuration(mHost->mVideoAnimationDuration);
    if (mHost->mIsLandscape) {
        videoViewAnimator->TranslationX(videoViewTranslation);
        videoViewAnimator->Start();
    }
    else {
        videoViewAnimator->TranslationY(videoViewTranslation);
        videoViewAnimator->Start();
    }

    // Animate the call card sliding.
    AutoPtr<IViewPropertyAnimator> callCardAnimator;
    mHost->mPrimaryCallCardContainer->Animate((IViewPropertyAnimator**)&callCardAnimator);
    callCardAnimator->SetInterpolator(ITimeInterpolator::Probe(AnimUtils::EASE_OUT_EASE_IN));
    callCardAnimator->SetDuration(mHost->mVideoAnimationDuration);
    callCardAnimator->SetListener(new AnimatorListener(mVisible, mHost));

    if (mHost->mIsLandscape) {
        Int32 width;
        mHost->mPrimaryCallCardContainer->GetWidth(&width);
        Float translationX = width;
        translationX *= mIsLayoutRtl ? 1 : -1;
        callCardAnimator->TranslationX(mVisible ? 0 : translationX);
        callCardAnimator->Start();
    }
    else {
        Int32 height;
        mHost->mPrimaryCallCardContainer->GetHeight(&height);
        callCardAnimator->TranslationY(mVisible ? 0 : -height);
        callCardAnimator->Start();
    }

    *result = TRUE;
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::AnimatorListener2
//==========================================================================
ECode CCallCardFragment::AnimatorListener2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->SetViewStatePostAnimation(mListener);
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::ViewTreeGlobalLayoutListener
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::ViewTreeGlobalLayoutListener, Object, IOnGlobalLayoutListener);

ECode CCallCardFragment::ViewTreeGlobalLayoutListener::OnGlobalLayout()
{
    AutoPtr<IView> v;
    mHost->GetView((IView**)&v);
    AutoPtr<IViewTreeObserver> observer;
    v->GetViewTreeObserver((IViewTreeObserver**)&observer);
    Boolean isAlive;
    if (observer->IsAlive(&isAlive), !isAlive) {
        return NOERROR;
    }
    observer->RemoveOnGlobalLayoutListener(this);

    AutoPtr<LayoutIgnoringListener> listener = new LayoutIgnoringListener();
    mHost->mPrimaryCallCardContainer->AddOnLayoutChangeListener(listener);

    // Prepare the state of views before the circular reveal animation
    Int32 originalHeight;
    mHost->mPrimaryCallCardContainer->GetHeight(&originalHeight);
    Int32 parentHeight;
    IView::Probe(mParent)->GetHeight(&parentHeight);
    mHost->mPrimaryCallCardContainer->SetBottom(parentHeight);

    // Set up FAB.
    mHost->mFloatingActionButtonContainer->SetVisibility(IView::GONE);
    Int32 parentWidth;
    IView::Probe(mParent)->GetWidth(&parentWidth);
    mHost->mFloatingActionButtonController->SetScreenWidth(parentWidth);
    IView::Probe(mHost->mCallButtonsContainer)->SetAlpha(0);
    IView::Probe(mHost->mCallStateLabel)->SetAlpha(0);
    IView::Probe(mHost->mPrimaryName)->SetAlpha(0);
    IView::Probe(mHost->mCallTypeLabel)->SetAlpha(0);
    IView::Probe(mHost->mCallNumberAndLabel)->SetAlpha(0);

    AutoPtr<IAnimator> revealAnimator = mHost->GetRevealAnimator(mStartPoint);
    AutoPtr<IAnimator> shrinkAnimator =
            mHost->GetShrinkAnimator(parentHeight, originalHeight);

    mHost->mAnimatorSet = NULL;
    CAnimatorSet::New((IAnimatorSet**)&mHost->mAnimatorSet);
    AutoPtr< ArrayOf<IAnimator*> > items = ArrayOf<IAnimator*>::Alloc(2);
    items->Set(0, revealAnimator);
    items->Set(1, shrinkAnimator);
    mHost->mAnimatorSet->PlaySequentially(items);
    IAnimator::Probe(mHost->mAnimatorSet)->AddListener(new AnimatorListener2(listener, mHost));
    IAnimator::Probe(mHost->mAnimatorSet)->Start();
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::ViewTreeGlobalLayoutListener2
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::ViewTreeGlobalLayoutListener2, Object, IOnGlobalLayoutListener);

ECode CCallCardFragment::ViewTreeGlobalLayoutListener2::OnGlobalLayout()
{
    AutoPtr<IViewTreeObserver> viewTreeObserver = mObserver;
    Boolean isAlive;
    if (viewTreeObserver->IsAlive(&isAlive), !isAlive) {
        viewTreeObserver = NULL;
        IView::Probe(mParent)->GetViewTreeObserver((IViewTreeObserver**)&viewTreeObserver);
    }
    viewTreeObserver->RemoveOnGlobalLayoutListener(this);
    Int32 width;
    IView::Probe(mParent)->GetWidth(&width);
    mHost->mFloatingActionButtonController->SetScreenWidth(width);
    mHost->UpdateFabPosition();
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::ViewTreeGlobalLayoutListener3
//==========================================================================
CAR_INTERFACE_IMPL(CCallCardFragment::ViewTreeGlobalLayoutListener3, Object, IOnGlobalLayoutListener);

ECode CCallCardFragment::ViewTreeGlobalLayoutListener3::OnGlobalLayout()
{
    AutoPtr<IViewTreeObserver> observer;
    mHost->mSecondaryCallInfo->GetViewTreeObserver((IViewTreeObserver**)&observer);
    Boolean isAlive;
    if (observer->IsAlive(&isAlive), !isAlive) {
        return NOERROR;
    }
    observer->RemoveOnGlobalLayoutListener(this);

    mHost->OnDialpadVisiblityChange(mHost->mIsDialpadShowing);
    return NOERROR;
}


//==========================================================================
// CCallCardFragment::AnimatorListener3
//==========================================================================
ECode CCallCardFragment::AnimatorListener3::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->AssignTranslateAnimation(IView::Probe(mHost->mCallStateLabel), 1);
    mHost->AssignTranslateAnimation(IView::Probe(mHost->mCallStateIcon), 1);
    mHost->AssignTranslateAnimation(IView::Probe(mHost->mPrimaryName), 2);
    mHost->AssignTranslateAnimation(mHost->mCallNumberAndLabel, 3);
    mHost->AssignTranslateAnimation(IView::Probe(mHost->mCallTypeLabel), 4);
    mHost->AssignTranslateAnimation(mHost->mCallButtonsContainer, 5);

    IView::Probe(mHost->mFloatingActionButton)->SetEnabled(TRUE);
    return NOERROR;
}


//==========================================================================
// CCallCardFragment
//==========================================================================
CAR_INTERFACE_IMPL_3(CCallCardFragment, BaseFragment, ICallCardFragment, ICallCardUi, IUi);

CAR_OBJECT_IMPL(CCallCardFragment);

CCallCardFragment::CCallCardFragment()
    : mRevealAnimationDuration(0)
    , mShrinkAnimationDuration(0)
    , mFabNormalDiameter(0)
    , mFabSmallDiameter(0)
    , mIsLandscape(FALSE)
    , mIsDialpadShowing(FALSE)
    , mFloatingActionButtonVerticalOffset(0)
    , mDensity(0)
    , mTranslationOffset(0)
    , mVideoAnimationDuration(0)
{}

ECode CCallCardFragment::constructor()
{
    return BaseFragment::constructor();
}

AutoPtr<IUi> CCallCardFragment::GetUi()
{
    return this;
}

AutoPtr<IPresenter> CCallCardFragment::CreatePresenter()
{
    AutoPtr<IPresenter> presenter = new CallCardPresenter();
    return presenter;
}

ECode CCallCardFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(BaseFragment::OnCreate(savedInstanceState));

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetInteger(R::integer::reveal_animation_duration, &mRevealAnimationDuration);
    res->GetInteger(R::integer::shrink_animation_duration, &mShrinkAnimationDuration);
    res->GetInteger(R::integer::video_animation_duration, &mVideoAnimationDuration);
    res->GetDimensionPixelOffset(
            R::dimen::floating_action_bar_vertical_offset, &mFloatingActionButtonVerticalOffset);
    res->GetDimensionPixelOffset(
            R::dimen::end_call_floating_action_button_diameter, &mFabNormalDiameter);
    res->GetDimensionPixelOffset(
            R::dimen::end_call_floating_action_button_small_diameter, &mFabSmallDiameter);
    return NOERROR;
}

ECode CCallCardFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(BaseFragment::OnActivityCreated(savedInstanceState));

    AutoPtr<CallList> calls = CallList::GetInstance();
    AutoPtr<Call> call = calls->GetFirstCall();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CallCardPresenter* presenter = (CallCardPresenter*)GetPresenter().Get();
    presenter->Init(IContext::Probe(activity), call);
    return NOERROR;
}

ECode CCallCardFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    FAIL_RETURN(BaseFragment::OnCreateView(inflater, container, savedInstanceState, view));

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    metrics->GetDensity(&mDensity);
    Int32 offset;
    res->GetDimensionPixelSize(R::dimen::call_card_anim_translate_y_offset, &offset);
    mTranslationOffset = offset;

    return inflater->Inflate(R::layout::call_card_content, container, FALSE, view);
}

ECode CCallCardFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(BaseFragment::OnViewCreated(view, savedInstanceState));

    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);
    AnimationUtils::LoadAnimation(context, R::anim::call_status_pulse, (IAnimation**)&mPulseAnimation);

    AutoPtr<IView> v;
    view->FindViewById(R::id::phoneNumber, (IView**)&v);
    mPhoneNumber = ITextView::Probe(v);
    v = NULL;
    view->FindViewById(R::id::name, (IView**)&v);
    mPrimaryName = ITextView::Probe(v);
    v = NULL;
    view->FindViewById(R::id::label, (IView**)&v);
    mNumberLabel = ITextView::Probe(v);
    view->FindViewById(R::id::secondary_call_info, (IView**)&mSecondaryCallInfo);
    view->FindViewById(R::id::secondary_call_provider_info, (IView**)&mSecondaryCallProviderInfo);
    v = NULL;
    view->FindViewById(R::id::photo, (IView**)&v);
    mPhoto = IImageView::Probe(v);
    v = NULL;
    view->FindViewById(R::id::callStateIcon, (IView**)&v);
    mCallStateIcon = IImageView::Probe(v);
    v = NULL;
    view->FindViewById(R::id::videoCallIcon, (IView**)&v);
    mCallStateVideoCallIcon = IImageView::Probe(v);
    v = NULL;
    view->FindViewById(R::id::callStateLabel, (IView**)&v);
    mCallStateLabel = ITextView::Probe(v);
    view->FindViewById(R::id::labelAndNumber, (IView**)&mCallNumberAndLabel);
    v = NULL;
    view->FindViewById(R::id::callTypeLabel, (IView**)&v);
    mCallTypeLabel = ITextView::Probe(v);
    v = NULL;
    view->FindViewById(R::id::elapsedTime, (IView**)&v);
    mElapsedTime = ITextView::Probe(v);
    view->FindViewById(R::id::primary_call_info_container, (IView**)&mPrimaryCallCardContainer);
    v = NULL;
    view->FindViewById(R::id::primary_call_banner, (IView**)&v);
    mPrimaryCallInfo = IViewGroup::Probe(v);
    view->FindViewById(R::id::callButtonFragment, (IView**)&mCallButtonsContainer);
    v = NULL;
    view->FindViewById(R::id::connectionServiceMessage, (IView**)&v);
    mInCallMessageLabel = ITextView::Probe(v);
    view->FindViewById(R::id::progressSpinner, (IView**)&mProgressSpinner);

    view->FindViewById(
            R::id::floating_end_call_action_button_container,
            (IView**)&mFloatingActionButtonContainer);
    v = NULL;
    view->FindViewById(
            R::id::floating_end_call_action_button, (IView**)&v);
    mFloatingActionButton = IImageButton::Probe(v);
    v->SetOnClickListener(new ActionButtonClickListener(this));
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mFloatingActionButtonController = new FloatingActionButtonController();
    mFloatingActionButtonController->constructor(activity,
            mFloatingActionButtonContainer, IView::Probe(mFloatingActionButton));

    mSecondaryCallInfo->SetOnClickListener(new CallInfoClickListener(this));

    view->FindViewById(R::id::callStateButton, (IView**)&mCallStateButton);
    mCallStateButton->SetOnClickListener(new CallStateButtonClickListener(this));

    view->FindViewById(R::id::manage_conference_call_button, (IView**)&mManageConferenceCallButton);
    mManageConferenceCallButton->SetOnClickListener(new ManageConferenceCallButtonClickListener(this));

    mPrimaryName->SetElegantTextHeight(FALSE);
    mCallStateLabel->SetElegantTextHeight(FALSE);
    return NOERROR;
}

ECode CCallCardFragment::SetVisible(
    /* [in] */ Boolean on)
{
    AutoPtr<IView> v;
    GetView((IView**)&v);
    if (on) {
        v->SetVisibility(IView::VISIBLE);
    }
    else {
        v->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

ECode CCallCardFragment::SetProgressSpinnerVisible(
    /* [in] */ Boolean visible)
{
    mProgressSpinner->SetVisibility(
            visible ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallCardFragment::SetCallCardVisible(
    /* [in] */ Boolean visible)
{
    // When animating the hide/show of the views in a landscape layout, we need to take into
    // account whether we are in a left-to-right locale or a right-to-left locale and adjust
    // the animations accordingly.
    Boolean isLayoutRtl = InCallPresenter::IsRtl();

    // Retrieve here since at fragment creation time the incoming video view is not inflated.
    AutoPtr<IView> v;
    GetView((IView**)&v);
    AutoPtr<IView> videoView;
    v->FindViewById(R::id::incomingVideo, (IView**)&videoView);

    // Determine how much space there is below or to the side of the call card.
    Float spaceBesideCallCard = GetSpaceBesideCallCard();

    // We need to translate the video surface, but we need to know its position after the layout
    // has occurred so use a {@code ViewTreeObserver}.
    AutoPtr<IViewTreeObserver> observer;
    v->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->AddOnPreDrawListener(new ViewTreePreDrawListener(
            observer, isLayoutRtl, visible, videoView, spaceBesideCallCard, this));
    return NOERROR;
}

Float CCallCardFragment::GetSpaceBesideCallCard()
{
    AutoPtr<IView> v;
    GetView((IView**)&v);
    if (mIsLandscape) {
        Int32 vWidth, cWidth;
        v->GetWidth(&vWidth);
        mPrimaryCallCardContainer->GetWidth(&cWidth);
        return vWidth - cWidth;
    }
    else {
        Int32 vHeight, cHeight;
        v->GetHeight(&vHeight);
        mPrimaryCallCardContainer->GetHeight(&cHeight);
        return vHeight - cHeight;
    }
}

ECode CCallCardFragment::SetPrimaryName(
    /* [in] */ const String& name,
    /* [in] */ Boolean nameIsNumber)
{
    if (TextUtils::IsEmpty(name)) {
        mPrimaryName->SetText((ICharSequence*)NULL);
    }
    else {
        mPrimaryName->SetText(CoreUtils::Convert(name));

        // Set direction of the name field
        Int32 nameDirection = IView::TEXT_DIRECTION_INHERIT;
        if (nameIsNumber) {
            nameDirection = IView::TEXT_DIRECTION_LTR;
        }
        IView::Probe(mPrimaryName)->SetTextDirection(nameDirection);
    }
    return NOERROR;
}

ECode CCallCardFragment::SetPrimaryImage(
    /* [in] */ IDrawable* image)
{
    if (image != NULL) {
        SetDrawableToImageView(mPhoto, image);
    }
    return NOERROR;
}

ECode CCallCardFragment::SetPrimaryPhoneNumber(
    /* [in] */ const String& number)
{
    // Set the number
    if (TextUtils::IsEmpty(number)) {
        mPhoneNumber->SetText((ICharSequence*)NULL);
        IView::Probe(mPhoneNumber)->SetVisibility(IView::GONE);
    }
    else {
        mPhoneNumber->SetText(CoreUtils::Convert(number));
        IView::Probe(mPhoneNumber)->SetVisibility(IView::VISIBLE);
        IView::Probe(mPhoneNumber)->SetTextDirection(IView::TEXT_DIRECTION_LTR);
    }
    return NOERROR;
}

ECode CCallCardFragment::SetPrimaryLabel(
    /* [in] */ const String& label)
{
    if (!TextUtils::IsEmpty(label)) {
        mNumberLabel->SetText(CoreUtils::Convert(label));
        IView::Probe(mNumberLabel)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mNumberLabel)->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode CCallCardFragment::SetPrimary(
    /* [in] */ const String& number,
    /* [in] */ const String& _name,
    /* [in] */ Boolean nameIsNumber,
    /* [in] */ const String& label,
    /* [in] */ IDrawable* _photo,
    /* [in] */ Boolean isConference,
    /* [in] */ Boolean canManageConference,
    /* [in] */ Boolean isSipCall)
{
    Logger::D("CCallCardFragment", "Setting primary call");

    String name = _name;
    AutoPtr<IDrawable> photo = _photo;
    if (isConference) {
        name = GetConferenceString(canManageConference);
        photo = GetConferencePhoto(canManageConference);
        photo->SetAutoMirrored(TRUE);
        nameIsNumber = FALSE;
    }

    // set the name field.
    SetPrimaryName(name, nameIsNumber);

    if (TextUtils::IsEmpty(number) && TextUtils::IsEmpty(label)) {
        mCallNumberAndLabel->SetVisibility(IView::GONE);
    }
    else {
        mCallNumberAndLabel->SetVisibility(IView::VISIBLE);
    }

    SetPrimaryPhoneNumber(number);

    // Set the label (Mobile, Work, etc)
    SetPrimaryLabel(label);

    ShowInternetCallLabel(isSipCall);

    SetDrawableToImageView(mPhoto, photo);
    return NOERROR;
}

ECode CCallCardFragment::SetSecondary(
    /* [in] */ Boolean show,
    /* [in] */ const String& _name,
    /* [in] */ Boolean nameIsNumber,
    /* [in] */ const String& label,
    /* [in] */ const String& providerLabel,
    /* [in] */ IDrawable* providerIcon,
    /* [in] */ Boolean isConference,
    /* [in] */ Boolean canManageConference)
{
    String name = _name;

    Boolean result;
    mSecondaryCallInfo->IsShown(&result);
    if (show != result) {
        UpdateFabPositionForSecondaryCallInfo();
    }

    if (show) {
        Boolean hasProvider = !TextUtils::IsEmpty(providerLabel);
        ShowAndInitializeSecondaryCallInfo(hasProvider);

        if (isConference) {
            name = GetConferenceString(canManageConference);
            nameIsNumber = FALSE;
            mSecondaryCallConferenceCallIcon->SetVisibility(IView::VISIBLE);
        }
        else {
            mSecondaryCallConferenceCallIcon->SetVisibility(IView::GONE);
        }

        mSecondaryCallName->SetText(CoreUtils::Convert(name));
        if (hasProvider) {
            mSecondaryCallProviderLabel->SetText(CoreUtils::Convert(providerLabel));
            mSecondaryCallProviderIcon->SetImageDrawable(providerIcon);
        }

        Int32 nameDirection = IView::TEXT_DIRECTION_INHERIT;
        if (nameIsNumber) {
            nameDirection = IView::TEXT_DIRECTION_LTR;
        }
        IView::Probe(mSecondaryCallName)->SetTextDirection(nameDirection);
    }
    else {
        mSecondaryCallInfo->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode CCallCardFragment::SetCallState(
    /* [in] */ Int32 state,
    /* [in] */ Int32 videoState,
    /* [in] */ Int32 sessionModificationState,
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [in] */ const String& connectionLabel,
    /* [in] */ IDrawable* connectionIcon,
    /* [in] */ const String& gatewayNumber)
{
    Boolean isGatewayCall = !TextUtils::IsEmpty(gatewayNumber);
    AutoPtr<ICharSequence> callStateLabel = GetCallStateLabelFromState(state, videoState,
            sessionModificationState, disconnectCause, connectionLabel, isGatewayCall);

    Logger::V("CCallCardFragment", "setCallState %s", TO_CSTR(callStateLabel));
    Logger::V("CCallCardFragment", "DisconnectCause %s", TO_CSTR(disconnectCause));
    Logger::V("CCallCardFragment", "gateway %s %s", connectionLabel.string(), gatewayNumber.string());

    AutoPtr<ICharSequence> label;
    mCallStateLabel->GetText((ICharSequence**)&label);
    if (TextUtils::Equals(callStateLabel, label)) {
        // Nothing to do if the labels are the same
        return NOERROR;
    }

    // Update the call state label and icon.
    if (!TextUtils::IsEmpty(callStateLabel)) {
        mCallStateLabel->SetText(callStateLabel);
        IView::Probe(mCallStateLabel)->SetAlpha(1);
        IView::Probe(mCallStateLabel)->SetVisibility(IView::VISIBLE);

        if (connectionIcon == NULL) {
            IView::Probe(mCallStateIcon)->SetVisibility(IView::GONE);
        }
        else {
            IView::Probe(mCallStateIcon)->SetVisibility(IView::VISIBLE);
            // Invoke setAlpha(float) instead of setAlpha(int) to set the view's alpha. This is
            // needed because the pulse animation operates on the view alpha.
            IView::Probe(mCallStateIcon)->SetAlpha(1.0f);
            mCallStateIcon->SetImageDrawable(connectionIcon);
        }

        AutoPtr<IVideoProfileVideoStateHelper> helper;
        CVideoProfileVideoStateHelper::AcquireSingleton((IVideoProfileVideoStateHelper**)&helper);
        Boolean isBidirectional;
        if ((helper->IsBidirectional(videoState, &isBidirectional), isBidirectional)
                || (state == Call::State::ACTIVE && sessionModificationState
                        == Call::SessionModificationState::WAITING_FOR_RESPONSE)) {
            IView::Probe(mCallStateVideoCallIcon)->SetVisibility(IView::VISIBLE);
        }
        else {
            IView::Probe(mCallStateVideoCallIcon)->SetVisibility(IView::GONE);
        }

        if (state == Call::State::ACTIVE || state == Call::State::CONFERENCED) {
            IView::Probe(mCallStateLabel)->ClearAnimation();
            IView::Probe(mCallStateIcon)->ClearAnimation();
        }
        else {
            IView::Probe(mCallStateLabel)->StartAnimation(mPulseAnimation);
            IView::Probe(mCallStateIcon)->StartAnimation(mPulseAnimation);
        }
    }
    else {
        AutoPtr<IAnimation> callStateAnimation;
        IView::Probe(mCallStateLabel)->GetAnimation((IAnimation**)&callStateAnimation);
        if (callStateAnimation != NULL) {
            callStateAnimation->Cancel();
        }
        mCallStateLabel->SetText((ICharSequence*)NULL);
        IView::Probe(mCallStateLabel)->SetAlpha(0);
        IView::Probe(mCallStateLabel)->SetVisibility(IView::GONE);
        // Invoke setAlpha(float) instead of setAlpha(int) to set the view's alpha. This is
        // needed because the pulse animation operates on the view alpha.
        IView::Probe(mCallStateIcon)->SetAlpha(0.0f);
        IView::Probe(mCallStateIcon)->SetVisibility(IView::GONE);

        IView::Probe(mCallStateVideoCallIcon)->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode CCallCardFragment::SetCallbackNumber(
    /* [in] */ const String& callbackNumber,
    /* [in] */ Boolean isEmergencyCall)
{
    if (mInCallMessageLabel == NULL) {
        return NOERROR;
    }

    if (TextUtils::IsEmpty(callbackNumber)) {
        IView::Probe(mInCallMessageLabel)->SetVisibility(IView::GONE);
        return NOERROR;
    }

    // TODO: The new Locale-specific methods don't seem to be working. Revisit this.
    AutoPtr<IPhoneNumberUtils> pnUtils;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&pnUtils);
    String formatedCallbackNumber;
    pnUtils->FormatNumber(callbackNumber, &formatedCallbackNumber);

    Int32 stringResourceId = isEmergencyCall ? R::string::card_title_callback_number_emergency
            : R::string::card_title_callback_number;

    AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, CoreUtils::Convert(formatedCallbackNumber));
    String text;
    GetString(stringResourceId, formatArgs, &text);
    mInCallMessageLabel->SetText(CoreUtils::Convert(text));

    IView::Probe(mInCallMessageLabel)->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

void CCallCardFragment::ShowInternetCallLabel(
    /* [in] */ Boolean show)
{
    if (show) {
        AutoPtr<IView> v;
        GetView((IView**)&v);
        AutoPtr<IContext> ctx;
        v->GetContext((IContext**)&ctx);
        String label;
        ctx->GetString(R::string::incall_call_type_label_sip, &label);
        IView::Probe(mCallTypeLabel)->SetVisibility(IView::VISIBLE);
        mCallTypeLabel->SetText(CoreUtils::Convert(label));
    }
    else {
        IView::Probe(mCallTypeLabel)->SetVisibility(IView::GONE);
    }
}

ECode CCallCardFragment::SetPrimaryCallElapsedTime(
    /* [in] */ Boolean show,
    /* [in] */ const String& callTimeElapsed)
{
    if (show) {
        Int32 visible;
        if (IView::Probe(mElapsedTime)->GetVisibility(&visible), visible != IView::VISIBLE) {
            AnimUtils::FadeIn(IView::Probe(mElapsedTime), AnimUtils::DEFAULT_DURATION);
        }
        mElapsedTime->SetText(CoreUtils::Convert(callTimeElapsed));
    }
    else {
        // hide() animation has no effect if it is already hidden.
        AnimUtils::FadeOut(IView::Probe(mElapsedTime), AnimUtils::DEFAULT_DURATION);
    }
    return NOERROR;
}

void CCallCardFragment::SetDrawableToImageView(
    /* [in] */ IImageView* view,
    /* [in] */ IDrawable* _photo)
{
    AutoPtr<IDrawable> photo = _photo;
    if (photo == NULL) {
        AutoPtr<IResources> res;
        IView::Probe(view)->GetResources((IResources**)&res);
        res->GetDrawable(R::drawable::img_no_image, (IDrawable**)&photo);
        photo->SetAutoMirrored(TRUE);
    }

    AutoPtr<IDrawable> current;
    view->GetDrawable((IDrawable**)&current);
    if (current == NULL) {
        view->SetImageDrawable(photo);
        AnimUtils::FadeIn(IView::Probe(mElapsedTime), AnimUtils::DEFAULT_DURATION);
    }
    else {
        InCallAnimationUtils::StartCrossFade(view, current, photo);
        IView::Probe(view)->SetVisibility(IView::VISIBLE);
    }
}

String CCallCardFragment::GetConferenceString(
    /* [in] */ Boolean canManageConference)
{
    Logger::V("CCallCardFragment", "canManageConferenceString: %d", canManageConference);
    Int32 resId = canManageConference
            ? R::string::card_title_conf_call : R::string::card_title_in_call;
    AutoPtr<IView> v;
    GetView((IView**)&v);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    String str;
    res->GetString(resId, &str);
    return str;
}

AutoPtr<IDrawable> CCallCardFragment::GetConferencePhoto(
    /* [in] */ Boolean canManageConference)
{
    Logger::V("CCallCardFragment", "canManageConferencePhoto: %d", canManageConference);
    Int32 resId = canManageConference ? R::drawable::img_conference : R::drawable::img_phone;
    AutoPtr<IView> v;
    GetView((IView**)&v);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDrawable> d;
    res->GetDrawable(resId, (IDrawable**)&d);
    return d;
}

AutoPtr<ICharSequence> CCallCardFragment::GetCallStateLabelFromState(
    /* [in] */ Int32 state,
    /* [in] */ Int32 videoState,
    /* [in] */ Int32 sessionModificationState,
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [in] */ const String& label,
    /* [in] */ Boolean isGatewayCall)
{
    AutoPtr<IView> v;
    GetView((IView**)&v);
    AutoPtr<IContext> context;
    v->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> callStateLabel;  // Label to display as part of the call banner

    Boolean isSpecialCall = !label.IsNull();
    Boolean isAccount = isSpecialCall && !isGatewayCall;

    switch  (state) {
        case Call::State::IDLE:
            // "Call state" is meaningless in this state.
            break;
        case Call::State::ACTIVE: {
            // We normally don't show a "call state label" at all in this state
            // (but we can use the call state label to display the provider name).
            AutoPtr<IVideoProfileVideoStateHelper> helper;
            CVideoProfileVideoStateHelper::AcquireSingleton((IVideoProfileVideoStateHelper**)&helper);
            Boolean isBidirectional;
            if (isAccount) {
                callStateLabel = CoreUtils::Convert(label);
            }
            else if (sessionModificationState
                    == Call::SessionModificationState::REQUEST_FAILED) {
                String str;
                context->GetString(R::string::card_title_video_call_error, &str);
                callStateLabel = CoreUtils::Convert(str);
            }
            else if (sessionModificationState
                    == Call::SessionModificationState::WAITING_FOR_RESPONSE) {
                String str;
                context->GetString(R::string::card_title_video_call_requesting, &str);
                callStateLabel = CoreUtils::Convert(str);
            }
            else if (helper->IsBidirectional(videoState, &isBidirectional), isBidirectional) {
                String str;
                context->GetString(R::string::card_title_video_call, &str);
                callStateLabel = CoreUtils::Convert(str);
            }
            break;
        }
        case Call::State::ONHOLD: {
            String str;
            context->GetString(R::string::card_title_on_hold, &str);
            callStateLabel = CoreUtils::Convert(str);
            break;
        }
        case Call::State::CONNECTING:
        case Call::State::DIALING:
            if (isSpecialCall) {
                AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
                formatArgs->Set(0, CoreUtils::Convert(label));
                String str;
                context->GetString(R::string::calling_via_template, formatArgs, &str);
                callStateLabel = CoreUtils::Convert(str);
            }
            else {
                String str;
                context->GetString(R::string::card_title_dialing, &str);
                callStateLabel = CoreUtils::Convert(str);
            }
            break;
        case Call::State::REDIALING: {
            String str;
            context->GetString(R::string::card_title_redialing, &str);
            callStateLabel = CoreUtils::Convert(str);
            break;
        }
        case Call::State::INCOMING:
        case Call::State::CALL_WAITING: {
            String str;
            AutoPtr<IVideoProfileVideoStateHelper> helper;
            CVideoProfileVideoStateHelper::AcquireSingleton((IVideoProfileVideoStateHelper**)&helper);
            Boolean isBidirectional;
            if (isAccount) {
                AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
                formatArgs->Set(0, CoreUtils::Convert(label));
                context->GetString(R::string::incoming_via_template, formatArgs, &str);
            }
            else if (helper->IsBidirectional(videoState, &isBidirectional), isBidirectional) {
                context->GetString(R::string::notification_incoming_video_call, &str);
            }
            else {
                context->GetString(R::string::card_title_incoming_call, &str);
            }
            callStateLabel = CoreUtils::Convert(str);
            break;
        }
        case Call::State::DISCONNECTING: {
            // While in the DISCONNECTING state we display a "Hanging up"
            // message in order to make the UI feel more responsive.  (In
            // GSM it's normal to see a delay of a couple of seconds while
            // negotiating the disconnect with the network, so the "Hanging
            // up" state at least lets the user know that we're doing
            // something.  This state is currently not used with CDMA.)
            String str;
            context->GetString(R::string::card_title_hanging_up, &str);
            callStateLabel = CoreUtils::Convert(str);
            break;
        }
        case Call::State::DISCONNECTED: {
            disconnectCause->GetLabel((ICharSequence**)&callStateLabel);
            if (TextUtils::IsEmpty(callStateLabel)) {
                String str;
                context->GetString(R::string::card_title_call_ended, &str);
                callStateLabel = CoreUtils::Convert(str);
            }
            break;
        }
        case Call::State::CONFERENCED: {
            String str;
            context->GetString(R::string::card_title_conf_call, &str);
            callStateLabel = CoreUtils::Convert(str);
            break;
        }
        default:
            Logger::E("CCallCardFragment", "updateCallStateWidgets: unexpected call: %d", state);
    }
    return callStateLabel;
}

void CCallCardFragment::ShowAndInitializeSecondaryCallInfo(
    /* [in] */ Boolean hasProvider)
{
    mSecondaryCallInfo->SetVisibility(IView::VISIBLE);

    // mSecondaryCallName is initialized here (vs. onViewCreated) because it is inaccessible
    // until mSecondaryCallInfo is inflated in the call above.
    if (mSecondaryCallName == NULL) {
        AutoPtr<IView> v, view;
        GetView((IView**)&v);
        v->FindViewById(R::id::secondaryCallName, (IView**)&view);
        mSecondaryCallName = ITextView::Probe(view);
        view = NULL;
        v->FindViewById(R::id::secondaryCallConferenceCallIcon, (IView**)&view);
        mSecondaryCallConferenceCallIcon = view;
        if (hasProvider) {
            mSecondaryCallProviderInfo->SetVisibility(IView::VISIBLE);
            view = NULL;
            v->FindViewById(R::id::secondaryCallProviderLabel, (IView**)&view);
            mSecondaryCallProviderLabel = ITextView::Probe(view);
            view = NULL;
            v->FindViewById(R::id::secondaryCallProviderIcon, (IView**)&view);
            mSecondaryCallProviderIcon = IImageView::Probe(view);
        }
    }
}

ECode CCallCardFragment::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    Int32 type;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED) {
        DispatchPopulateAccessibilityEvent(event, IView::Probe(mCallStateLabel));
        DispatchPopulateAccessibilityEvent(event, IView::Probe(mPrimaryName));
        DispatchPopulateAccessibilityEvent(event, IView::Probe(mPhoneNumber));
        return NOERROR;
    }
    DispatchPopulateAccessibilityEvent(event, IView::Probe(mCallStateLabel));
    DispatchPopulateAccessibilityEvent(event, IView::Probe(mPrimaryName));
    DispatchPopulateAccessibilityEvent(event, IView::Probe(mPhoneNumber));
    DispatchPopulateAccessibilityEvent(event, IView::Probe(mCallTypeLabel));
    DispatchPopulateAccessibilityEvent(event, IView::Probe(mSecondaryCallName));
    DispatchPopulateAccessibilityEvent(event, IView::Probe(mSecondaryCallProviderLabel));

    return NOERROR;
}

ECode CCallCardFragment::SetEndCallButtonEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean animate)
{
    Boolean isEnabled;
    IView::Probe(mFloatingActionButton)->IsEnabled(&isEnabled);
    if (enabled != isEnabled) {
        if (animate) {
            if (enabled) {
                mFloatingActionButtonController->ScaleIn(AnimUtils::NO_DELAY);
            }
            else {
                mFloatingActionButtonController->ScaleOut();
            }
        }
        else {
            if (enabled) {
                mFloatingActionButtonContainer->SetScaleX(1);
                mFloatingActionButtonContainer->SetScaleY(1);
                mFloatingActionButtonContainer->SetVisibility(IView::VISIBLE);
            }
            else {
                mFloatingActionButtonContainer->SetVisibility(IView::GONE);
            }
        }
        IView::Probe(mFloatingActionButton)->SetEnabled(enabled);
        UpdateFabPosition();
    }
    return NOERROR;
}

ECode CCallCardFragment::SetPhotoVisible(
    /* [in] */ Boolean isVisible)
{
    IView::Probe(mPhoto)->SetVisibility(isVisible ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode CCallCardFragment::ShowManageConferenceCallButton(
    /* [in] */ Boolean visible)
{
    mManageConferenceCallButton->SetVisibility(visible ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

void CCallCardFragment::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ IView* view)
{
    if (view == NULL) return;
    AutoPtr<IList> eventText;
    IAccessibilityRecord::Probe(event)->GetText((IList**)&eventText);
    Int32 size;
    eventText->GetSize(&size);
    Boolean result;
    view->DispatchPopulateAccessibilityEvent(event, &result);
    // if no text added write null to keep relative position
    Int32 size2;
    if (eventText->GetSize(&size2), size == size2) {
        eventText->Add(NULL);
    }
}

void CCallCardFragment::AnimateForNewOutgoingCall(
    /* [in] */ IPoint* touchPoint)
{
    AutoPtr<IViewParent> vp;
    IView::Probe(mPrimaryCallCardContainer)->GetParent((IViewParent**)&vp);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(vp);

    AutoPtr<IView> v;
    GetView((IView**)&v);
    AutoPtr<IViewTreeObserver> observer;
    v->GetViewTreeObserver((IViewTreeObserver**)&observer);

    AutoPtr<ILayoutTransition> lt;
    mPrimaryCallInfo->GetLayoutTransition((ILayoutTransition**)&lt);
    lt->DisableTransitionType(ILayoutTransition::CHANGING);

    observer->AddOnGlobalLayoutListener(new ViewTreeGlobalLayoutListener(parent, touchPoint, this));
}

void CCallCardFragment::OnDialpadVisiblityChange(
    /* [in] */ Boolean isShown)
{
    mIsDialpadShowing = isShown;
    UpdateFabPosition();
}

void CCallCardFragment::UpdateFabPosition()
{
    Int32 offsetY = 0;
    if (!mIsDialpadShowing) {
        offsetY = mFloatingActionButtonVerticalOffset;
        Boolean isShown;
        if (mSecondaryCallInfo->IsShown(&isShown), isShown) {
            Int32 height;
            mSecondaryCallInfo->GetHeight(&height);
            offsetY -= height;
        }
    }

    mFloatingActionButtonController->Align(
            mIsLandscape ? FloatingActionButtonController::ALIGN_QUARTER_END
                    : FloatingActionButtonController::ALIGN_MIDDLE,
            0 /* offsetX */,
            offsetY,
            TRUE);

    mFloatingActionButtonController->Resize(
            mIsDialpadShowing ? mFabSmallDiameter : mFabNormalDiameter, TRUE);
}

ECode CCallCardFragment::OnResume()
{
    FAIL_RETURN(BaseFragment::OnResume());
    // If the previous launch animation is still running, cancel it so that we don't get
    // stuck in an intermediate animation state.
    Boolean isRunning;
    if (mAnimatorSet != NULL && (IAnimator::Probe(mAnimatorSet)->IsRunning(&isRunning), isRunning)) {
        IAnimator::Probe(mAnimatorSet)->Cancel();
    }

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    mIsLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;

    AutoPtr<IViewParent> vp;
    IView::Probe(mPrimaryCallCardContainer)->GetParent((IViewParent**)&vp);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(vp);
    AutoPtr<IViewTreeObserver> observer;
    IView::Probe(parent)->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->AddOnGlobalLayoutListener(new ViewTreeGlobalLayoutListener2(parent, observer, this));
    return NOERROR;
}

void CCallCardFragment::UpdateFabPositionForSecondaryCallInfo()
{
    AutoPtr<IViewTreeObserver> observer;
    mSecondaryCallInfo->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->AddOnGlobalLayoutListener(new ViewTreeGlobalLayoutListener3(this));
}

AutoPtr<IAnimator> CCallCardFragment::GetShrinkAnimator(
    /* [in] */ Int32 startHeight,
    /* [in] */ Int32 endHeight)
{
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr< ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(2);
    (*values)[0] = startHeight;
    (*values)[1] = endHeight;
    AutoPtr<IObjectAnimator> shrinkAnimator;
    helper->OfInt32(mPrimaryCallCardContainer, String("bottom"), values, (IObjectAnimator**)&shrinkAnimator);
    IAnimator* animator = IAnimator::Probe(shrinkAnimator);
    animator->SetDuration(mShrinkAnimationDuration);
    animator->AddListener(new AnimatorListener3(this));
    animator->SetInterpolator(ITimeInterpolator::Probe(AnimUtils::EASE_IN));
    return animator;
}

AutoPtr<IAnimator> CCallCardFragment::GetRevealAnimator(
    /* [in] */ IPoint* touchPoint)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IWindow> window;
    activity->GetWindow((IWindow**)&window);
    AutoPtr<IView> view;
    window->GetDecorView((IView**)&view);
    AutoPtr<IWindowManager> wm;
    activity->GetWindowManager((IWindowManager**)&wm);
    AutoPtr<IDisplay> display;
    wm->GetDefaultDisplay((IDisplay**)&display);
    AutoPtr<IPoint> size;
    CPoint::New((IPoint**)&size);
    display->GetSize(size);

    Int32 sx, sy;
    size->Get(&sx, &sy);
    Int32 startX = sx / 2;
    Int32 startY = sy / 2;
    if (touchPoint != NULL) {
        touchPoint->Get(&startX, &startY);
    }

    AutoPtr<IViewAnimationUtilsHelper> helper;
    CViewAnimationUtilsHelper::AcquireSingleton((IViewAnimationUtilsHelper**)&helper);
    AutoPtr<IAnimator> valueAnimator;
    helper->CreateCircularReveal(view,
            startX, startY, 0, Elastos::Core::Math::Max(sx, sy), (IAnimator**)&valueAnimator);
    valueAnimator->SetDuration(mRevealAnimationDuration);
    return valueAnimator;
}

void CCallCardFragment::AssignTranslateAnimation(
    /* [in] */ IView* view,
    /* [in] */ Int32 offset)
{
    view->SetTranslationY(mTranslationOffset * offset);
    AutoPtr<IViewPropertyAnimator> vpAnimator;
    view->Animate((IViewPropertyAnimator**)&vpAnimator);
    vpAnimator->TranslationY(0);
    vpAnimator->Alpha(1);
    vpAnimator->WithLayer();
    vpAnimator->SetDuration(mShrinkAnimationDuration);
    vpAnimator->SetInterpolator(ITimeInterpolator::Probe(AnimUtils::EASE_IN));
}

void CCallCardFragment::SetViewStatePostAnimation(
    /* [in] */ IView* view)
{
    view->SetTranslationY(0);
    view->SetAlpha(1);
}

void CCallCardFragment::SetViewStatePostAnimation(
    /* [in] */ IViewOnLayoutChangeListener* layoutChangeListener)
{
    SetViewStatePostAnimation(mCallButtonsContainer);
    SetViewStatePostAnimation(IView::Probe(mCallStateLabel));
    SetViewStatePostAnimation(IView::Probe(mPrimaryName));
    SetViewStatePostAnimation(IView::Probe(mCallTypeLabel));
    SetViewStatePostAnimation(mCallNumberAndLabel);
    SetViewStatePostAnimation(IView::Probe(mCallStateIcon));

    mPrimaryCallCardContainer->RemoveOnLayoutChangeListener(layoutChangeListener);
    AutoPtr<ILayoutTransition> lt;
    mPrimaryCallInfo->GetLayoutTransition((ILayoutTransition**)&lt);
    lt->EnableTransitionType(ILayoutTransition::CHANGING);
    mFloatingActionButtonController->ScaleIn(AnimUtils::NO_DELAY);
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
