#ifndef __ELASTOS_DROID_DIALER_WIDGET_SEARCHEDITTEXTLAYOUT_H__
#define __ELASTOS_DROID_DIALER_WIDGET_SEARCHEDITTEXTLAYOUT_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include <elastos/droid/widget/FrameLayout.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Widget {

class SearchEditTextLayout
    : public FrameLayout
    , public ISearchEditTextLayout
{
private:
    class SearchViewFocusChangeListener
        : public Object
        , public IViewOnFocusChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        SearchViewFocusChangeListener(
            /* [in] */ SearchEditTextLayout* host);

        // @Override
        CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);

    private:
        SearchEditTextLayout* mHost;
    };

    class SearchCloseButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        SearchCloseButtonClickListener(
            /* [in] */ SearchEditTextLayout* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        SearchEditTextLayout* mHost;
    };

    class SearchBackButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        SearchBackButtonClickListener(
            /* [in] */ SearchEditTextLayout* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        SearchEditTextLayout* mHost;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        AnimatorUpdateListener(
            /* [in] */ SearchEditTextLayout* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        SearchEditTextLayout* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    SearchEditTextLayout();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetPreImeKeyListener(
        /* [in] */ IViewOnKeyListener* listener);

    CARAPI SetOnBackButtonClickedListener(
        /* [in] */ IOnBackButtonClickedListener* listener);

    // @Override
    CARAPI DispatchKeyEventPreIme(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    CARAPI FadeOut();

    // TODO:
    // CARAPI FadeOut(
    //     /* [in] */ AnimUtils.AnimationCallback callback);

    CARAPI FadeIn();

    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    CARAPI Expand(
        /* [in] */ Boolean animate,
        /* [in] */ Boolean requestFocus);

    CARAPI Collapse(
        /* [in] */ Boolean animate);

    CARAPI IsExpanded(
        /* [out] */ Boolean* isExpanded);

    CARAPI IsFadedOut(
        /* [out] */ Boolean* isFadedOut);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    /**
     * Updates the visibility of views depending on whether we will show the expanded or collapsed
     * search view. This helps prevent some jank with the crossfading if we are animating.
     *
     * @param isExpand Whether we are about to show the expanded search box.
     */
    CARAPI_(void) UpdateVisibility(
        /* [in] */ Boolean isExpand);

    CARAPI_(void) PrepareAnimator(
        /* [in] */ Boolean expand);

    /**
     * Assigns margins to the search box as a fraction of its maximum margin size
     *
     * @param fraction How large the margins should be as a fraction of their full size
     */
    CARAPI_(void) SetMargins(
        /* [in] */ Float fraction);

protected:
    /* Subclass-visible for testing */
    Boolean mIsExpanded;
    Boolean mIsFadedOut;

private:
    static const Float EXPAND_MARGIN_FRACTION_START;
    static const Int32 ANIMATION_DURATION;

    AutoPtr<IViewOnKeyListener> mPreImeKeyListener;
    Int32 mTopMargin;
    Int32 mBottomMargin;
    Int32 mLeftMargin;
    Int32 mRightMargin;

    Float mCollapsedElevation;

    AutoPtr<IView> mCollapsed;
    AutoPtr<IView> mExpanded;
    AutoPtr<IEditText> mSearchView;
    AutoPtr<IView> mSearchIcon;
    AutoPtr<IView> mCollapsedSearchBox;
    AutoPtr<IView> mVoiceSearchButtonView;
    AutoPtr<IView> mOverflowButtonView;
    AutoPtr<IView> mBackButtonView;
    AutoPtr<IView> mExpandedSearchBox;
    AutoPtr<IView> mClearButtonView;

    AutoPtr<IValueAnimator> mAnimator;

    AutoPtr<IOnBackButtonClickedListener> mOnBackButtonClickedListener;
};

} // Widget
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_WIDGET_SEARCHEDITTEXTLAYOUT_H__
