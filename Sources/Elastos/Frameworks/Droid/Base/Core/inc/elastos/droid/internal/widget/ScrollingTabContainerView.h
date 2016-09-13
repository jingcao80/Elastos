
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_SCROLLINGTABCONTAINERVIEW_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_SCROLLINGTABCONTAINERVIEW_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/widget/HorizontalScrollView.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::IActionBarTab;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::HorizontalScrollView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISpinner;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * This widget implements the dynamic action bar tab behavior that can change
 * across different configurations or circumstances.
 */
class ScrollingTabContainerView
    : public HorizontalScrollView
    , public IScrollingTabContainerView
{
public:
    class TabView
        : public LinearLayout
        , public ITabView
    {
    private:
        class LongClickListener
            : public Object
            , public IViewOnLongClickListener
        {
        public:
            CAR_INTERFACE_DECL()

            LongClickListener(
                /* [in] */ TabView* host);

            CARAPI OnLongClick(
                /* [in] */ IView* v,
                /* [out] */ Boolean* rst);

        private:
            TabView* mHost;
        };
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IActionBarTab* tab,
            /* [in] */ Boolean forList,
            /* [in] */ IScrollingTabContainerView* view);

        CARAPI BindTab(
            /* [in] */ IActionBarTab* tab);

        // @Override
        CARAPI SetSelected(
            /* [in] */ Boolean selected);

        // @Override
        CARAPI OnInitializeAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event);

        // @Override
        CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IAccessibilityNodeInfo* info);

        CARAPI Update();

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* rst);

        CARAPI GetTab(
            /* [out] */ IActionBarTab** tab);

        virtual CARAPI OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

    private:
        AutoPtr<IActionBarTab> mTab;
        AutoPtr<ITextView> mTextView;
        AutoPtr<IImageView> mIconView;
        AutoPtr<IView> mCustomView;
        ScrollingTabContainerView* mHost;
    };

private:
    class AnimateToTabRunnable : public Runnable
    {
    public:
        AnimateToTabRunnable(
            /* [in] */ IView* tabView,
            /* [in] */ ScrollingTabContainerView* host);
        CARAPI Run();

    private:
        AutoPtr<IView> mTabView;
        ScrollingTabContainerView* mHost;
    };

    class TabAdapter
        : public BaseAdapter
    {
    public:
        TabAdapter(
            /* [in] */ ScrollingTabContainerView* host);

        virtual CARAPI GetCount(
            /* [out] */ Int32* count);

        virtual CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        virtual CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* id);

        virtual CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        ScrollingTabContainerView* mHost;
    };

    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ ScrollingTabContainerView* host);

        CARAPI OnClick(
            /* [in] */ IView* view);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        ScrollingTabContainerView* mHost;
    };

protected:
    class VisibilityAnimListener
        : public Object
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        VisibilityAnimListener(
            /* [in] */ ScrollingTabContainerView* host);

        virtual CARAPI_(AutoPtr<VisibilityAnimListener>) WithFinalVisibility(
            /* [in] */ Int32 visibility);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animation);

    private:
        ScrollingTabContainerView* mHost;
        Boolean mCanceled;
        Int32 mFinalVisibility;
    };

public:
    CAR_INTERFACE_DECL()

    ScrollingTabContainerView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI SetAllowCollapse(
        /* [in] */ Boolean allowCollapse);

    virtual CARAPI SetTabSelected(
        /* [in] */ Int32 position);

    virtual CARAPI SetContentHeight(
        /* [in] */ Int32 contentHeight);

    virtual CARAPI AnimateToVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI AnimateToTab(
        /* [in] */ Int32 position);

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Boolean setSelected);

    virtual CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Int32 position,
        /* [in] */ Boolean setSelected);

    virtual CARAPI UpdateTab(
        /* [in] */ Int32 position);

    virtual CARAPI RemoveTabAt(
        /* [in] */ Int32 position);

    virtual CARAPI RemoveAllTabs();

    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

protected:
    virtual CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    /**
     * Indicates whether this view is collapsed into a dropdown menu instead
     * of traditional tabs.
     * @return true if showing as a spinner
     */
    CARAPI_(Boolean) IsCollapsed();

    CARAPI_(void) PerformCollapse();

    CARAPI_(Boolean) PerformExpand();

    CARAPI_(AutoPtr<ILinearLayout>) CreateTabLayout();

    CARAPI_(AutoPtr<ISpinner>) CreateSpinner();

    AutoPtr<ITabView> CreateTabView(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Boolean forAdapter);

protected:
    AutoPtr<IAnimator> mVisibilityAnim;
    AutoPtr<VisibilityAnimListener> mVisAnimListener;

private:
    static const String TAG;
    static AutoPtr<ITimeInterpolator> sAlphaInterpolator;
    static const Int32 FADE_DURATION;

    AutoPtr<IRunnable> mTabSelector;
    AutoPtr<InnerListener> mInnerListener;

    AutoPtr<ILinearLayout> mTabLayout;
    AutoPtr<ISpinner> mTabSpinner;
    Boolean mAllowCollapse;

    Int32 mMaxTabWidth;
    Int32 mStackedTabMaxWidth;
    Int32 mContentHeight;
    Int32 mSelectedTabIndex;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_INTERNAL_SCROLLINGTABCONTAINERVIEW_H__
