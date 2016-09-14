#ifndef  __ELASTOS_DROID_LAUNCHER2_CAPPSCUSTOMIZETABHOST_H__
#define  __ELASTOS_DROID_LAUNCHER2_CAPPSCUSTOMIZETABHOST_H__

#include "_Elastos_Droid_Launcher2_CAppsCustomizeTabHost.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/TabHost.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Launcher2::AppsCustomizePagedViewContentType;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::TabHost;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITabHostTabContentFactory;
using Elastos::Droid::Widget::ITabHostOnTabChangeListener;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CAppsCustomizeTabHost)
    , public TabHost
    , public IAppsCustomizeTabHost
    , public ILauncherTransitionable
{
private:
    class TabChangeListener
        : public Object
        , public ITabHostOnTabChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TabChangeListener(
            /* [in] */ CAppsCustomizeTabHost* host);

        CARAPI OnTabChanged(
            /* [in] */ const String& tabId);

    private:
        CAppsCustomizeTabHost* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CAppsCustomizeTabHost* host);

        CARAPI Run();

    private:
        CAppsCustomizeTabHost* mHost;
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
        class MyAnimatorListenerAdapter
            : public AnimatorListenerAdapter
        {
        public:
            MyAnimatorListenerAdapter(
                /* [in] */ CAppsCustomizeTabHost* host);

            //@Override
            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animation);

            //@Override
            CARAPI OnAnimationCancel(
                /* [in] */ IAnimator* animation);

        private:
            CARAPI ClearAnimationBuffer();

        private:
            CAppsCustomizeTabHost* mHost;
        };

        class MyAnimatorListenerAdapter2
            : public AnimatorListenerAdapter
        {
        public:
            MyAnimatorListenerAdapter2(
                /* [in] */ CAppsCustomizeTabHost* host);

            //@Override
            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animation);

        private:
            CAppsCustomizeTabHost* mHost;
        };

    public:
        MyRunnable2(
            /* [in] */ CAppsCustomizeTabHost* host,
            /* [in] */ AppsCustomizePagedViewContentType type,
            /* [in] */ Int32 duration);

        CARAPI Run();

    private:
        CAppsCustomizeTabHost* mHost;
        AppsCustomizePagedViewContentType mType;
        Int32 mDuration;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CAppsCustomizeTabHost();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Convenience methods to select specific tabs.  We want to set the content type immediately
     * in these cases, but we note that we still call setCurrentTabByTag() so that the tab view
     * reflects the new content (but doesn't do the animation and logic associated with changing
     * tabs manually).
     */
    CARAPI SetContentTypeImmediate(
        /* [in] */ AppsCustomizePagedViewContentType type);

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
        /* [in] */ AppsCustomizePagedViewContentType type);

    /**
     * Returns the content type for the specified tab tag.
     */
    CARAPI GetContentTypeForTabTag(
        /* [in] */ const String& tag,
        /* [out] */ AppsCustomizePagedViewContentType* type);

    /**
     * Returns the tab tag for a given content type.
     */
    CARAPI GetTabTagForContentType(
        /* [in] */ AppsCustomizePagedViewContentType type,
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
        /* [in] */ AppsCustomizePagedViewContentType type);

    CARAPI EnableAndBuildHardwareLayer();

    CARAPI SetVisibilityOfSiblingsWithLowerZOrder(
        /* [in] */ Int32 visibility);

private:
    static const String TAG;
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
    AutoPtr<TabChangeListener> mTabChangeListener;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CAPPSCUSTOMIZETABHOST_H__