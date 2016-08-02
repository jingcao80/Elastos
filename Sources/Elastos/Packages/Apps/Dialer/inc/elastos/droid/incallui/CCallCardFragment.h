
#ifndef __ELASTOS_DROID_INCALLUI_CCALLCARDFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_CCALLCARDFRAGMENT_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_InCallUI_CCallCardFragment.h"
#include "elastos/droid/contacts/common/widget/FloatingActionButtonController.h"
#include "elastos/droid/incallui/BaseFragment.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Contacts::Common::Widget::FloatingActionButtonController;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CCallCardFragment)
    , public BaseFragment
    , public ICallCardFragment
    , public ICallCardUi
    , public IUi
{
private:
    class LayoutIgnoringListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);
    };

    class ActionButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        ActionButtonClickListener(
            /* [in] */ CCallCardFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCallCardFragment* mHost;
    };

    class CallInfoClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        CallInfoClickListener(
            /* [in] */ CCallCardFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCallCardFragment* mHost;
    };

    class CallStateButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        CallStateButtonClickListener(
            /* [in] */ CCallCardFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCallCardFragment* mHost;
    };

    class ManageConferenceCallButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        ManageConferenceCallButtonClickListener(
            /* [in] */ CCallCardFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCallCardFragment* mHost;
    };

    class AnimatorListener
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListener(
            /* [in] */ Boolean visible,
            /* [in] */ CCallCardFragment* host)
            : mVisible(visible)
            , mHost(host)
        {}

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        Boolean mVisible;
        CCallCardFragment* mHost;
    };

    class ViewTreePreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL();

        ViewTreePreDrawListener(
            /* [in] */ IViewTreeObserver* observer,
            /* [in] */ Boolean isLayoutRtl,
            /* [in] */ Boolean visible,
            /* [in] */ IView* videoView,
            /* [in] */ Float spaceBesideCallCard,
            /* [in] */ CCallCardFragment* host)
            : mObserver(observer)
            , mIsLayoutRtl(isLayoutRtl)
            , mVisible(visible)
            , mVideoView(videoView)
            , mSpaceBesideCallCard(spaceBesideCallCard)
            , mHost(host)
        {}

        // @Override
        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        IViewTreeObserver* mObserver;
        Boolean mIsLayoutRtl;
        Boolean mVisible;
        AutoPtr<IView> mVideoView;
        Boolean mSpaceBesideCallCard;
        CCallCardFragment* mHost;
    };

    class AnimatorListener2
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListener2(
            /* [in] */ LayoutIgnoringListener* listener,
            /* [in] */ CCallCardFragment* host)
            : mListener(listener)
            , mHost(host)
        {}

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<LayoutIgnoringListener> mListener;
        CCallCardFragment* mHost;
    };

    class ViewTreeGlobalLayoutListener
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL();

        ViewTreeGlobalLayoutListener(
            /* [in] */ IViewGroup* parent,
            /* [in] */ IPoint* startPoint,
            /* [in] */ CCallCardFragment* host)
            : mParent(parent)
            , mStartPoint(startPoint)
            , mHost(host)
        {}

        // @Override
        CARAPI OnGlobalLayout();

    private:
        AutoPtr<IViewGroup> mParent;
        AutoPtr<IPoint> mStartPoint;
        CCallCardFragment* mHost;
    };

    class ViewTreeGlobalLayoutListener2
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL();

        ViewTreeGlobalLayoutListener2(
            /* [in] */ IViewGroup* parent,
            /* [in] */ IViewTreeObserver* observer,
            /* [in] */ CCallCardFragment* host)
            : mParent(parent)
            , mObserver(observer)
            , mHost(host)
        {}

        // @Override
        CARAPI OnGlobalLayout();

    private:
        AutoPtr<IViewGroup> mParent;
        IViewTreeObserver* mObserver;
        CCallCardFragment* mHost;
    };

    class ViewTreeGlobalLayoutListener3
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL();

        ViewTreeGlobalLayoutListener3(
            /* [in] */ CCallCardFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnGlobalLayout();

    private:
        CCallCardFragment* mHost;
    };

    class AnimatorListener3
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListener3(
            /* [in] */ CCallCardFragment* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        CCallCardFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CCallCardFragment();

    CARAPI constructor();

    // @Override
    CARAPI_(AutoPtr<IUi>) GetUi();

    // @Override
    CARAPI_(AutoPtr<IPresenter>) CreatePresenter();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI SetVisible(
        /* [in] */ Boolean on);

    /**
     * Hides or shows the progress spinner.
     *
     * @param visible {@code True} if the progress spinner should be visible.
     */
    // @Override
    CARAPI SetProgressSpinnerVisible(
        /* [in] */ Boolean visible);

    /**
     * Sets the visibility of the primary call card.
     * Ensures that when the primary call card is hidden, the video surface slides over to fill the
     * entire screen.
     *
     * @param visible {@code True} if the primary call card should be visible.
     */
    // @Override
    CARAPI SetCallCardVisible(
        /* [in] */ Boolean visible);

    /**
     * Determines the amount of space below the call card for portrait layouts), or beside the
     * call card for landscape layouts.
     *
     * @return The amount of space below or beside the call card.
     */
    CARAPI_(Float) GetSpaceBesideCallCard();

    // @Override
    CARAPI SetPrimaryName(
        /* [in] */ const String& name,
        /* [in] */ Boolean nameIsNumber);

    // @Override
    CARAPI SetPrimaryImage(
        /* [in] */ IDrawable* image);

    // @Override
    CARAPI SetPrimaryPhoneNumber(
        /* [in] */ const String& phoneNumber);

    // @Override
    CARAPI SetPrimaryLabel(
        /* [in] */ const String& label);

    // @Override
    CARAPI SetPrimary(
        /* [in] */ const String& number,
        /* [in] */ const String& name,
        /* [in] */ Boolean nameIsNumber,
        /* [in] */ const String& label,
        /* [in] */ IDrawable* photo,
        /* [in] */ Boolean isConference,
        /* [in] */ Boolean canManageConference,
        /* [in] */ Boolean isSipCall);

    // @Override
    CARAPI SetSecondary(
        /* [in] */ Boolean show,
        /* [in] */ const String& name,
        /* [in] */ Boolean nameIsNumber,
        /* [in] */ const String& label,
        /* [in] */ const String& providerLabel,
        /* [in] */ IDrawable* providerIcon,
        /* [in] */ Boolean isConference,
        /* [in] */ Boolean canManageConference);

    // @Override
    CARAPI SetCallState(
        /* [in] */ Int32 state,
        /* [in] */ Int32 videoState,
        /* [in] */ Int32 sessionModificationState,
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [in] */ const String& connectionLabel,
        /* [in] */ IDrawable* connectionIcon,
        /* [in] */ const String& gatewayNumber);

    // @Override
    CARAPI SetCallbackNumber(
        /* [in] */ const String& number,
        /* [in] */ Boolean isEmergencyCall);

    // @Override
    CARAPI SetPrimaryCallElapsedTime(
        /* [in] */ Boolean show,
        /* [in] */ const String& duration);

    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI SetEndCallButtonEnabled(
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean animate);

    CARAPI SetPhotoVisible(
        /* [in] */ Boolean isVisible);

    CARAPI ShowManageConferenceCallButton(
        /* [in] */ Boolean visible);

    CARAPI_(void) AnimateForNewOutgoingCall(
        /* [in] */ IPoint* touchPoint);

    CARAPI_(void) OnDialpadVisiblityChange(
        /* [in] */ Boolean isShown);

    // @Override
    CARAPI OnResume();

private:
    CARAPI_(void) ShowInternetCallLabel(
        /* [in] */ Boolean show);

    CARAPI_(void) SetDrawableToImageView(
        /* [in] */ IImageView* view,
        /* [in] */ IDrawable* photo);

    CARAPI_(String) GetConferenceString(
        /* [in] */ Boolean canManageConference);

    CARAPI_(AutoPtr<IDrawable>) GetConferencePhoto(
        /* [in] */ Boolean canManageConference);

    /**
     * Gets the call state label based on the state of the call or cause of disconnect.
     *
     * Additional labels are applied as follows:
     *         1. All outgoing calls with display "Calling via [Provider]".
     *         2. Ongoing calls will display the name of the provider.
     *         3. Incoming calls will only display "Incoming via..." for accounts.
     *         4. Video calls, and session modification states (eg. requesting video).
     */
    CARAPI_(AutoPtr<ICharSequence>) GetCallStateLabelFromState(
        /* [in] */ Int32 state,
        /* [in] */ Int32 videoState,
        /* [in] */ Int32 sessionModificationState,
        /* [in] */ IDisconnectCause* disconnectCause,
        /* [in] */ const String& label,
        /* [in] */ Boolean isGatewayCall);

    CARAPI_(void) ShowAndInitializeSecondaryCallInfo(
        /* [in] */ Boolean hasProvider);

    CARAPI_(void) DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ IView* view);

    CARAPI_(void) UpdateFabPosition();

    /**
     * Adds a global layout listener to update the FAB's positioning on the next layout. This allows
     * us to position the FAB after the secondary call info's height has been calculated.
     */
    CARAPI_(void) UpdateFabPositionForSecondaryCallInfo();

    /**
     * Animator that performs the upwards shrinking animation of the blue call card scrim.
     * At the start of the animation, each child view is moved downwards by a pre-specified amount
     * and then translated upwards together with the scrim.
     */
    CARAPI_(AutoPtr<IAnimator>) GetShrinkAnimator(
        /* [in] */ Int32 startHeight,
        /* [in] */ Int32 endHeight);

    CARAPI_(AutoPtr<IAnimator>) GetRevealAnimator(
        /* [in] */ IPoint* touchPoint);

    CARAPI_(void) AssignTranslateAnimation(
        /* [in] */ IView* view,
        /* [in] */ Int32 offset);

    CARAPI_(void) SetViewStatePostAnimation(
        /* [in] */ IView* view);

    CARAPI_(void) SetViewStatePostAnimation(
        /* [in] */ IViewOnLayoutChangeListener* layoutChangeListener);

private:
    AutoPtr<IAnimatorSet> mAnimatorSet;
    Int32 mRevealAnimationDuration;
    Int32 mShrinkAnimationDuration;
    Int32 mFabNormalDiameter;
    Int32 mFabSmallDiameter;
    Boolean mIsLandscape;
    Boolean mIsDialpadShowing;

    // Primary caller info
    AutoPtr<ITextView> mPhoneNumber;
    AutoPtr<ITextView> mNumberLabel;
    AutoPtr<ITextView> mPrimaryName;
    AutoPtr<IView> mCallStateButton;
    AutoPtr<IImageView> mCallStateIcon;
    AutoPtr<IImageView> mCallStateVideoCallIcon;
    AutoPtr<ITextView> mCallStateLabel;
    AutoPtr<ITextView> mCallTypeLabel;
    AutoPtr<IView> mCallNumberAndLabel;
    AutoPtr<IImageView> mPhoto;
    AutoPtr<ITextView> mElapsedTime;

    // Container view that houses the entire primary call card, including the call buttons
    AutoPtr<IView> mPrimaryCallCardContainer;
    // Container view that houses the primary call information
    AutoPtr<IViewGroup> mPrimaryCallInfo;
    AutoPtr<IView> mCallButtonsContainer;

    // Secondary caller info
    AutoPtr<IView> mSecondaryCallInfo;
    AutoPtr<ITextView> mSecondaryCallName;
    AutoPtr<IView> mSecondaryCallProviderInfo;
    AutoPtr<ITextView> mSecondaryCallProviderLabel;
    AutoPtr<IImageView> mSecondaryCallProviderIcon;
    AutoPtr<IView> mSecondaryCallConferenceCallIcon;
    AutoPtr<IView> mProgressSpinner;

    AutoPtr<IView> mManageConferenceCallButton;

    // Dark number info bar
    AutoPtr<ITextView> mInCallMessageLabel;

    AutoPtr<FloatingActionButtonController> mFloatingActionButtonController;
    AutoPtr<IView> mFloatingActionButtonContainer;
    AutoPtr<IImageButton> mFloatingActionButton;
    Int32 mFloatingActionButtonVerticalOffset;

    // Cached DisplayMetrics density.
    Float mDensity;

    Float mTranslationOffset;
    AutoPtr<IAnimation> mPulseAnimation;

    Int32 mVideoAnimationDuration;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CCALLCARDFRAGMENT_H__
