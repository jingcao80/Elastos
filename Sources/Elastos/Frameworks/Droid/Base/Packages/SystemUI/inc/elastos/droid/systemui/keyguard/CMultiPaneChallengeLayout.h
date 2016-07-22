
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CMULTIPANECHALLENGELAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CMULTIPANECHALLENGELAYOUT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CMultiPaneChallengeLayout.h"
#include <elastos/droid/view/ViewGroup.h>

using Elastos::Droid::View::ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CMultiPaneChallengeLayout)
    , public ViewGroup
    , public IChallengeLayout
    , public IMultiPaneChallengeLayout
{
public:
    class MultiPaneChallengeLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IMultiPaneChallengeLayoutLayoutParams
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams")

        CAR_INTERFACE_DECL()

        MultiPaneChallengeLayoutLayoutParams();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ IMultiPaneChallengeLayout* parent);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IMultiPaneChallengeLayoutLayoutParams* source);

    public:
        Float mCenterWithinArea;

        Int32 mChildType;

        Int32 mGravity;

        Int32 mMaxWidth;
        Int32 mMaxHeight;
    };

private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CMultiPaneChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMultiPaneChallengeLayout* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ CMultiPaneChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        CMultiPaneChallengeLayout* mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MyAnimatorListenerAdapter2")

        MyAnimatorListenerAdapter2(
            /* [in] */ CMultiPaneChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI onAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CMultiPaneChallengeLayout* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CMultiPaneChallengeLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI SetInsets(
        /* [in] */ IRect* insets);

    //@Override
    CARAPI IsChallengeShowing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsChallengeOverlapping(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ShowChallenge(
        /* [in] */ Boolean b);

    //@Override
    CARAPI GetBouncerAnimationDuration(
        /* [out] */ Int32* duration);

    //@Override
    CARAPI ShowBouncer();

    //@Override
    CARAPI HideBouncer();

    //@Override
    CARAPI IsBouncing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetOnBouncerStateChangedListener(
        /* [in] */ IOnBouncerStateChangedListener* listener);

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    CARAPI SetScrimView(
        /* [in] */ IView* scrim);

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** p);

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ IViewGroupLayoutParams** outp);

    // @Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** outp);

    // @Override
    CARAPI CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p,
        /* [out] */ Boolean* result);

private:
    CARAPI_(Int32) GetVirtualHeight(
        /* [in] */ IMultiPaneChallengeLayoutLayoutParams* lp,
        /* [in] */ Int32 height,
        /* [in] */ Int32 heightUsed);

    CARAPI_(void) LayoutWithGravity(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IView* child,
        /* [in] */ IRect* padding,
        /* [in] */ Boolean adjustPadding);


private:
    friend class CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams;
    static const String TAG;

    Int32 mOrientation;
    Boolean mIsBouncing;

    AutoPtr<IKeyguardSecurityContainer> mChallengeView;
    AutoPtr<IView> mUserSwitcherView;
    AutoPtr<IView> mScrimView;
    AutoPtr<IOnBouncerStateChangedListener> mBouncerListener;

    AutoPtr<IRect> mTempRect;
    AutoPtr<IRect> mZeroPadding;
    AutoPtr<IRect> mInsets;

    AutoPtr<IDisplayMetrics> mDisplayMetrics;

    AutoPtr<IViewOnClickListener> mScrimClickListener;

};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CMULTIPANECHALLENGELAYOUT_H__
