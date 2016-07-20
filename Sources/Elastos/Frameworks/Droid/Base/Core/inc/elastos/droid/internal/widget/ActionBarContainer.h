
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_ACTIONBARCONTAINER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_ACTIONBARCONTAINER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Internal::Widget::IActionBarContainer;
using Elastos::Droid::Internal::Widget::IScrollingTabContainerView;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
  * This class acts as a container for the action bar view and action mode context views.
  * It applies special styles as needed to help handle animated transitions between them.
  * @hide
  */
class ActionBarContainer
    : public FrameLayout
    , public IActionBarContainer
{
public:
    /**
      * Dummy drawable so that we don't break background display lists and
      * projection surfaces.
      */
    class ActionBarBackgroundDrawable
        : public Drawable
    {
    public:
        CARAPI constructor(
            /* [in] */ IActionBarContainer* owner);

        // @Override
        CARAPI Draw(
            /* [in] */ ICanvas* canvas);

        // @Override
        CARAPI GetOutline(
            /* [in] */ IOutline* outline);

        // @Override
        CARAPI SetAlpha(
            /* [in] */ Int32 alpha);

        // @Override
        CARAPI SetColorFilter(
            /* [in] */ IColorFilter* cf);

        // @Override
        CARAPI GetOpacity(
            /* [out] */ Int32* result);

    private:
        ActionBarContainer* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    ActionBarContainer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI OnFinishInflate();

    virtual CARAPI SetPrimaryBackground(
        /* [in] */ IDrawable* bg);

    virtual CARAPI SetStackedBackground(
        /* [in] */ IDrawable* bg);

    virtual CARAPI SetSplitBackground(
        /* [in] */ IDrawable* bg);

    // @Override
    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI JumpDrawablesToCurrentState();

    /**
      * @hide
      */
    // @Override
    CARAPI OnResolveDrawables(
        /* [in] */ Int32 layoutDirection);

    /**
      * Set the action bar into a "transitioning" state. While transitioning
      * the bar will block focus and touch from all of its descendants. This
      * prevents the user from interacting with the bar while it is animating
      * in or out.
      *
      * @param isTransitioning true if the bar is currently transitioning, false otherwise.
      */
    virtual CARAPI SetTransitioning(
        /* [in] */ Boolean isTransitioning);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    virtual CARAPI SetTabContainer(
        /* [in] */ IScrollingTabContainerView* tabView);

    virtual CARAPI GetTabContainer(
        /* [out] */ IView** result);

    // @Override
    CARAPI StartActionModeForChild(
        /* [in] */ IView* child,
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** result);

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

protected:
    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    // @Override
    CARAPI DrawableStateChanged();

private:
    static CARAPI_(Boolean) IsCollapsed(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetMeasuredHeightWithMargins(
        /* [in] */ IView* view);

private:
    Boolean mIsTransitioning;
    AutoPtr<IView> mTabContainer;
    AutoPtr<IView> mActionBarView;
    AutoPtr<IView> mActionContextView;
    AutoPtr<IDrawable> mBackground;
    AutoPtr<IDrawable> mStackedBackground;
    AutoPtr<IDrawable> mSplitBackground;
    Boolean mIsSplit;
    Boolean mIsStacked;
    Int32 mHeight;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_ACTIONBARCONTAINER_H__

