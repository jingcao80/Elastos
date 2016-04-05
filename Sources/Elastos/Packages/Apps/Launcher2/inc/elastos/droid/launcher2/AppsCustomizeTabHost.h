#ifndef  __ELASTOS_DROID_LAUNCHER2_APPSCUSTOMIZETABHOST_H__
#define  __ELASTOS_DROID_LAUNCHER2_APPSCUSTOMIZETABHOST_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class AppsCustomizeTabHost
    : public TabHost
    , public IAppsCustomizeTabHost
    , public ILauncherTransitionable
    , public ITabHostOnTabChangeListener
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ AppsCustomizeTabHost* host);

        CARAPI Run();

    private:
        AppsCustomizeTabHost* mHost;
    };

    class MyTabHostTabContentFactory
        : public Object
        , public ITabHostTabContentFactory
    {
    public:
        CAR_INTERFACE_DECL();

        MyTabHostTabContentFactory(
            /* [in] */ IAppsCustomizePagedView* appsCustomizePane);

        CARAPI CreateTabContent(
            /* [in] */ const String& tag,
            /* [out] */ IView** view);

    private:
        AutoPtr<IAppsCustomizePagedView> mAppsCustomizePane;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        MyRunnable2(
            /* [in] */ AppsCustomizeTabHost* host);

        CARAPI Run();

    private:
        AppsCustomizeTabHost* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ AppsCustomizeTabHost* host);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        //@Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        CARAPI ClearAnimationBuffer();

    private:
        AppsCustomizeTabHost* mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ AppsCustomizeTabHost* host);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AppsCustomizeTabHost* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    AppsCustomizeTabHost(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Convenience methods to select specific tabs.  We want to set the content type immediately
     * in these cases, but we note that we still call setCurrentTabByTag() so that the tab view
     * reflects the new content (but doesn't do the animation and logic associated with changing
     * tabs manually).
     */
    CARAPI SetContentTypeImmediate(
        /* [in] */ AppsCustomizePagedView::ContentType type);

    CARAPI SelectAppsTab();

    CARAPI SelectWidgetsTab();

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTabChanged(
        /* [in] */ const String& tabId);

    CARAPI SetCurrentTabFromContent(
        /* [in] */ AppsCustomizePagedView::ContentType type);

    /**
     * Returns the content type for the specified tab tag.
     */
    CARAPI GetContentTypeForTabTag(
        /* [in] */ const String& tag,
        /* [out] */ AppsCustomizePagedView::ContentType* type);

    /**
     * Returns the tab tag for a given content type.
     */
    CARAPI GetTabTagForContentType(
        /* [in] */ AppsCustomizePagedView::ContentType type,
        /* [out] */ String* str);

    /**
     * Disable focus on anything under this view in the hierarchy if we are not visible.
     */
    //@Override
    CARAPI GetDescendantFocusability(
        /* [out] */ Int32* result);

    CARAPI Reset();

    //@Override
    CARAPI GetContent(
        /* [out] */ IView** view);

    /* LauncherTransitionable overrides */
    //@Override
    CARAPI OnLauncherTransitionPrepare(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    //@Override
    CARAPI OnLauncherTransitionStart(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    //@Override
    CARAPI OnLauncherTransitionStep(
        /* [in] */ ILauncher* l,
        /* [in] */ Float t);

    //@Override
    CARAPI OnLauncherTransitionEnd(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    CARAPI OnWindowVisible();

    CARAPI OnTrimMemory();

    CARAPI IsTransitioning(
        /* [out] */ Boolean* result);

protected:
    /**
     * Setup the tab host and create all necessary tabs.
     */
    //@Override
    CARAPI OnFinishInflate();

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI OnTabChangedStart();

    CARAPI ReloadCurrentPage();

    CARAPI OnTabChangedEnd(
        /* [in] */ AppsCustomizePagedView::ContentType type);

    CARAPI EnableAndBuildHardwareLayer();

    CARAPI SetVisibilityOfSiblingsWithLowerZOrder(
        /* [in] */ Int32 visibility);

public:
    static const String LOG_TAG;

private:
    static const String APPS_TAB_TAG;
    static const String WIDGETS_TAB_TAG;

    AutoPtr<ILayoutInflater> mLayoutInflater;
    AutoPtr<IViewGroup> mTabs;
    AutoPtr<IViewGroup> mTabsContainer;
    AutoPtr<IAppsCustomizePagedView> mAppsCustomizePane;
    AutoPtr<IFrameLayout> mAnimationBuffer;
    AutoPtr<ILinearLayout> mContent;

    Boolean mInTransition;
    Boolean mTransitioningToWorkspace;
    Boolean mResetAfterTransition;
    AutoPtr<IRunnable> mRelayoutAndMakeVisible;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_APPSCUSTOMIZETABHOST_H__