
#include "elastos/droid/launcher2/AppsCustomizeTabHost.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

AppsCustomizeTabHost::MyRunnable::MyRunnable(
    /* [in] */ AppsCustomizeTabHost* host)
    : mHost(host)
{
}

ECode AppsCustomizeTabHost::MyRunnable::Run()
{
    mHost->mTabs->RequestLayout();
    return mHost->mTabsContainer->SetAlpha(1f);
}

CAR_INTERFACE_IMPL(AppsCustomizeTabHost::MyTabHostTabContentFactory, Object,
        ITabHostTabContentFactory);

AppsCustomizeTabHost::MyTabHostTabContentFactory::MyTabHostTabContentFactory(
    /* [in] */ IAppsCustomizePagedView* appsCustomizePane)
    : mAppsCustomizePane(appsCustomizePane)
{
}

ECode AppsCustomizeTabHost::MyTabHostTabContentFactory::CreateTabContent(
    /* [in] */ const String& tag,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = IView::Probe(mAppsCustomizePane);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AppsCustomizeTabHost::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ AppsCustomizeTabHost* host)
    : mHost(host)
{
}

ECode AppsCustomizeTabHost::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return ClearAnimationBuffer();
}

ECode AppsCustomizeTabHost::MyAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return ClearAnimationBuffer();
}

ECode AppsCustomizeTabHost::MyAnimatorListenerAdapter::ClearAnimationBuffer()
{
    mHost->mAnimationBuffer->SetVisibility(IView::GONE);
    PagedViewWidget::SetRecyclePreviewsWhenDetachedFromWindow(FALSE);
    mHost->mAnimationBuffer->RemoveAllViews();
    return PagedViewWidget::SetRecyclePreviewsWhenDetachedFromWindow(TRUE);
}

AppsCustomizeTabHost::MyAnimatorListenerAdapter2::MyAnimatorListenerAdapter2(
    /* [in] */ AppsCustomizeTabHost* host)
    : mHost(host)
{
}

ECode AppsCustomizeTabHost::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mHost->ReloadCurrentPage();
}

AppsCustomizeTabHost::MyRunnable2::MyRunnable2(
    /* [in] */ AppsCustomizeTabHost* host)
    : mHost(host)
{
}

ECode AppsCustomizeTabHost::MyRunnable2::Run()
{
    Int32 width;
    mHost->mAppsCustomizePane->GetMeasuredWidth(&width);
    Int32 height;
    mHost->mAppsCustomizePane->GetMeasuredHeight(&height);
    if (width <= 0 || height <= 0) {
        mHost->ReloadCurrentPage();
        return NOERROR;
    }

    // Take the visible pages and re-parent them temporarily to mAnimatorBuffer
    // and then cross fade to the new pages
    AutoPtr<ArrayOf<Int32> > visiblePageRange = ArrayOf<Int32>::Alloc(2);
    mHost->mAppsCustomizePane->GetVisiblePages(visiblePageRange);
    if ((*visiblePageRange)[0] == -1 && (*visiblePageRange)[1] == -1) {
        // If we can't get the visible page ranges, then just skip the animation
        mHost->ReloadCurrentPage();
        return NOERROR;
    }
    AutoPtr<IArrayList> visiblePages;
    CArrayList::New((IArrayList**)&visiblePages);
    for (Int32 i = (*visiblePageRange)[0]; i <= (*visiblePageRange)[1]; i++) {
        AutoPtr<IView> view;
        mHost->mAppsCustomizePane->GetPageAt(i, (IView**)&view);
        visiblePages->Add(TO_IINTERFACE(view));
    }

    // We want the pages to be rendered in exactly the same way as they were when
    // their parent was mAppsCustomizePane -- so set the scroll on mAnimationBuffer
    // to be exactly the same as mAppsCustomizePane, and below, set the left/top
    // parameters to be correct for each of the pages
    Int32 x;
    mAppsCustomizePane->GetScrollX(&x);
    mHost->mAnimationBuffer->ScrollTo(x, 0);

    // mAppsCustomizePane renders its children in reverse order, so
    // add the pages to mAnimationBuffer in reverse order to match that behavior
    Int32 size;
    visiblePages->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        visiblePages->Get(i, (IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);
        if (IPagedViewCellLayout::Probe(child) != NULL) {
            IPagedViewCellLayout::Probe(child)->RresetChildrenOnKeyListeners();
        }
        else if (IPagedViewGridLayout::Probe(child) != NULL) {
            IPagedViewGridLayout::Probe(child)->ResetChildrenOnKeyListeners();
        }
        PagedViewWidget::SetDeletePreviewsWhenDetachedFromWindow(FALSE);
        mHost->mAppsCustomizePane->RemoveView(child);
        PagedViewWidget::SetDeletePreviewsWhenDetachedFromWindow(TRUE);
        mHost->mAnimationBuffer->SetAlpha(1f);
        mHost->mAnimationBuffer->SetVisibility(IView::VISIBLE);
        Int32 width;
        child->GetMeasuredWidth(&width);
        Int32 height;
        child->GetMeasuredHeight(&height);
        AutoPtr<IFrameLayoutLayoutParams> p;
        CFrameLayoutLayoutParams::New(width, height, (IFrameLayoutLayoutParams**)&p);
        Int32 left;
        child->GetLeft(&left);
        Int32 top;
        child->GetTop(&top);
        p->SetMargins((Int32)left, (Int32)top, 0, 0);
        mHost->mAnimationBuffer->AddView(child, p);
    }

    // Toggle the new content
    mHost->OnTabChangedStart();
    mHost->OnTabChangedEnd(type);

    // Animate the transition
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = 0f;
    AutoPtr<IObjectAnimator> outAnim = LauncherAnimUtils::OfFloat(
            mHost->mAnimationBuffer, String("alpha"), array);
    AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter(mHost);
    outAnim->AddListener(listener);

    AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
    (*array2)[0] = 1f;
    AutoPtr<IObjectAnimator> inAnim = LauncherAnimUtils::OfFloat(
            mAppsCustomizePane, String("alpha"), array2);
    AutoPtr<IAnimatorListener> listener2 = MyAnimatorListenerAdapter2(mHost);
    inAnim->AddListener(listener2);

    AutoPtr<IAnimatorSet> animSet;
    LauncherAnimUtils::CreateAnimatorSet((IAnimatorSet**)&animSet);
    animSet->PlayTogether(outAnim, inAnim);
    animSet->SetDuration(duration);
    return animSet->Start();
}

const String AppsCustomizeTabHost::LOG_TAG("AppsCustomizeTabHost");

const String AppsCustomizeTabHost::APPS_TAB_TAG("APPS");
const String AppsCustomizeTabHost::WIDGETS_TAB_TAG("WIDGETS");

CAR_INTERFACE_IMPL_3(AppsCustomizeTabHost, TabHost, IAppsCustomizeTabHost,
        ILauncherTransitionable, ITabHostOnTabChangeListener);

AppsCustomizeTabHost::AppsCustomizeTabHost(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : TabHost(context, attrs)
    , mInTransition(FALSE)
    , mTransitioningToWorkspace(FALSE)
    , mResetAfterTransition(FALSE)
{
    LayoutInflater::From(context, (ILayoutInflater**)&mLayoutInflater);
    mRelayoutAndMakeVisible = new MyRunnable(this);
}

ECode AppsCustomizeTabHost::SetContentTypeImmediate(
    /* [in] */ AppsCustomizePagedView::ContentType type)
{
    SetOnTabChangedListener(NULL);
    OnTabChangedStart();
    OnTabChangedEnd(type);
    String str;
    GetTabTagForContentType(type, &str);
    SetCurrentTabByTag(str);
    return SetOnTabChangedListener(this);
}

ECode AppsCustomizeTabHost::SelectAppsTab()
{
    return SetContentTypeImmediate(AppsCustomizePagedView::ContentType::Applications);
}

ECode AppsCustomizeTabHost::SelectWidgetsTab()
{
    return SetContentTypeImmediate(AppsCustomizePagedView::ContentType::Widgets);
}

ECode AppsCustomizeTabHost::OnFinishInflate()
{
    // Setup the tab host
    Setup();

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::tabs_container,
            (IView**)&view);
    AutoPtr<IViewGroup> tabsContainer = IViewGroup::Probe(view);
    AutoPtr<ITabWidget> tabs;
    GetTabWidget((ITabWidget**)&tabs);

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::Launcher2::R::id::apps_customize_pane_content,
            (IView**)&view2);
    AutoPtr<IAppsCustomizePagedView> appsCustomizePane =
            IAppsCustomizePagedView::Probe(view2);

    mTabs = IViewGroup::Probe(tabs);
    mTabsContainer = tabsContainer;
    mAppsCustomizePane = appsCustomizePane;
    AutoPtr<IView> view3;
    FindViewById(Elastos::Droid::Launcher2::R::id::animation_buffer,
            (IView**)&view3);
    mAnimationBuffer = IFrameLayout::Probe(view3);

    AutoPtr<IView> view4;
    FindViewById(Elastos::Droid::Launcher2::R::id::apps_customize_content,
            (IView**)&view4);
    mContent = ILinearLayout::Probe(view4);

    if (tabs == NULL || mAppsCustomizePane == NULL) {
        //throw new Resources.NotFoundException();
        Slogger::E(LOG_TAG, "Resources.NotFoundException");
        return E_RESOURCES_NOT_FOUND_EXCEPTION;
    }

    // Configure the tabs content factory to return the same paged view (that we change the
    // content filter on)
    AutoPtr<ITabHostTabContentFactory> contentFactory =
            new MyTabHostTabContentFactory(appsCustomizePane);

    // Create the tabs
    AutoPtr<ITextView> tabView;
    String label;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    IContext->GetString(Elastos::Droid::Launcher2::R::
            string::all_apps_button_label, &label);
    AutoPtr<IView> view2;
    mLayoutInflater->Inflate(Elastos::Droid::Launcher2::R::
            layout::tab_widget_indicator,
            IViewGroup::Probe(tabs), FALSE, (IView**)&view2);
    tabView = ITextView::Probe(view2);
    tabView->SetText(label);
    tabView->SetContentDescription(label);
    AutoPtr<ITabSpec> spec;
    NewTabSpec(APPS_TAB_TAG, (ITabSpec**)&spec);
    spec->SetIndicator(IView::Probe(tabView));
    spec->SetContent(contentFactory);
    AddTab(spec);

    AutoPtr<IContext> context2;
    GetContext((IContext**)&context2);
    context2->getString(Elastos::Droid::Launcher2::R::
            string::widgets_tab_label, &label);
    AutoPtr<IView> view3;
    mLayoutInflater->Inflate(Elastos::Droid::Launcher2::R::
            layout::tab_widget_indicator,
            IViewGroup::Probe(tabs), FALSE, (IView**)&view3);
    tabView = NULL;
    tabView = ITextView::Probe(view3);
    tabView->SetText(label);
    tabView->SetContentDescription(label);
    AutoPtr<ITabSpec> spec2;
    NewTabSpec(WIDGETS_TAB_TAG, (ITabSpec**)&spec2);
    spec2->SetIndicator(IView::Probe(tabView));
    spec2->SetContent(contentFactory);
    AddTab(spec2);
    SetOnTabChangedListener(this);

    // Setup the key listener to jump between the last tab view and the market icon
    AutoPtr<AppsCustomizeTabKeyEventListener> keyListener =
            new AppsCustomizeTabKeyEventListener();
    Int32 count;
    tabs->GetTabCount(&count);
    AutoPtr<IView> lastTab;
    tabs->GetChildTabViewAt(count - 1, (IView**)&lastTab);
    lastTab->SetOnKeyListener(keyListener);
    AutoPtr<IView> shopButton;
    FindViewById(Elastos::Droid::Launcher2::R::id::market_button,
            (IView**)&shopButton);
    shopButton->SetOnKeyListener(keyListener);

    // Hide the tab bar until we measure
    return mTabsContainer->SetAlpha(0f);
}

ECode AppsCustomizeTabHost::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<IViewGroupLayoutParams> params;
    IView::Probe(mTabs)->GetLayoutParams((IViewGroupLayoutParams**)&params);
    Int32 width;
    params->GetWidth(&width);
    Boolean remeasureTabWidth = (width <= 0);
    TabHost::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    // Set the width of the tab list to the content width
    if (remeasureTabWidth) {
        Int32 contentWidth;
        mAppsCustomizePane->GetPageContentWidth(&contentWidth);
        if (contentWidth > 0 && width != contentWidth) {
            // Set the width and show the tab bar
            params->SetWidth(contentWidth);
            mRelayoutAndMakeVisible->Run();
        }

        TabHost::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // If we are mid transitioning to the workspace, then intercept touch events here so we
    // can ignore them, otherwise we just let all apps handle the touch events.
    if (mInTransition && mTransitioningToWorkspace) {
        *result = TRUE;
        return NOERROR;
    }
    return TabHost::OnInterceptTouchEvent(ev, result);
}

ECode AppsCustomizeTabHost::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Allow touch events to fall through to the workspace if we are transitioning there
    if (mInTransition && mTransitioningToWorkspace) {
        return TabHost::OnTouchEvent(event, result);
    }

    // Intercept all touch events up to the bottom of the AppsCustomizePane so they do not fall
    // through to the workspace and trigger showWorkspace()
    Int32 y;
    event->GetY(&y);
    Int32 bottom;
    mAppsCustomizePane->GetBottom(&bottom);
    if (y < bottom) {
        *result = TRUE;
        return NOERROR;
    }
    return TabHost::OnTouchEvent(event, result);
}

ECode AppsCustomizeTabHost::OnTabChangedStart()
{
    return mAppsCustomizePane->HideScrollingIndicator(FALSE);
}

ECode AppsCustomizeTabHost::ReloadCurrentPage()
{
    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (!res) {
        mAppsCustomizePane->FlashScrollingIndicator(TRUE);
    }

    Int32 page;
    mAppsCustomizePane->GetCurrentPage(&page);
    mAppsCustomizePane->LoadAssociatedPages(page);
    return mAppsCustomizePane->RequestFocus();
}

ECode AppsCustomizeTabHost::OnTabChangedEnd(
    /* [in] */ AppsCustomizePagedView::ContentType type)
{
    return mAppsCustomizePane->SetContentType(type);
}

ECode AppsCustomizeTabHost::OnTabChanged(
    /* [in] */ const String& tabId)
{
    AppsCustomizePagedView::ContentType type;
    GetContentTypeForTabTag(tabId, &type);

    // Animate the changing of the tab content by fading pages in and out
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 duration;
    res->GetInteger(Elastos::Droid::Launcher2::R::
            integer::config_tabTransitionDuration, &duration);

    // We post a runnable here because there is a delay while the first page is loading and
    // the feedback from having changed the tab almost feels better than having it stick
    AutoPtr<IRunnable> run = new MyRunnable2(this);
    Boolean res;
    return Post(run, &res);
}

ECode AppsCustomizeTabHost::SetCurrentTabFromContent(
    /* [in] */ AppsCustomizePagedView::ContentType type)
{
    SetOnTabChangedListener(NULL);
    String str;
    GetTabTagForContentType(type, &str);
    SetCurrentTabByTag(str);
    return SetOnTabChangedListener(this);
}

ECode AppsCustomizeTabHost::GetContentTypeForTabTag(
    /* [in] */ const String& tag,
    /* [out] */ AppsCustomizePagedView::ContentType* type)
{
    VALIDATE_NOT_NULL(type);

    Boolean res;
    tag.Equals(APPS_TAB_TAG, &res);
    if (res) {
        *type = AppsCustomizePagedView::ContentType::Applications;
        return NOERROR;
    }
    else {
        Boolean res;
        tag.Equals(WIDGETS_TAB_TAG, &res);
        if (res) {
            *type = AppsCustomizePagedView::ContentType::Widgets;
            return NOERROR;
        }
    }
    *type = AppsCustomizePagedView::ContentType::Applications;
    return NOERROR;
}

ECode AppsCustomizeTabHost::GetTabTagForContentType(
    /* [in] */ AppsCustomizePagedView::ContentType type,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (type == AppsCustomizePagedView::ContentType::Applications) {
        *str = APPS_TAB_TAG;
        return NOERROR;
    } else if (type == AppsCustomizePagedView::ContentType::Widgets) {
        *str = WIDGETS_TAB_TAG;
        return NOERROR;
    }
    *str = APPS_TAB_TAG;
    return NOERROR;
}

ECode AppsCustomizeTabHost::GetDescendantFocusability(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 res;
    GetVisibility(&res);
    if (res != IView::VISIBLE) {
        *result = IViewGroup::FOCUS_BLOCK_DESCENDANTS;
        return NOERROR;
    }
    return TabHost::GetDescendantFocusability(result);
}

ECode AppsCustomizeTabHost::Reset()
{
    if (mInTransition) {
        // Defer to after the transition to reset
        mResetAfterTransition = TRUE;
    }
    else {
        // Reset immediately
        mAppsCustomizePane->Reset();
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::EnableAndBuildHardwareLayer()
{
    // isHardwareAccelerated() checks if we're attached to a window and if that
    // window is HW accelerated-- we were sometimes not attached to a window
    // and buildLayer was throwing an IllegalStateException
    Boolean res;
    isHardwareAccelerated(&res);
    if (res) {
        // Turn on hardware layers for performance
        SetLayerType(LAYER_TYPE_HARDWARE, NULL);

        // force building the layer, so you don't get a blip early in an animation
        // when the layer is created layer
        BuildLayer();
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::GetContent(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = mContent;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode AppsCustomizeTabHost::OnLauncherTransitionPrepare(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    mAppsCustomizePane->OnLauncherTransitionPrepare(l, animated, toWorkspace);
    mInTransition = TRUE;
    mTransitioningToWorkspace = toWorkspace;

    if (toWorkspace) {
        // Going from All Apps -> Workspace
        SetVisibilityOfSiblingsWithLowerZOrder(VISIBLE);
        // Stop the scrolling indicator - we don't want All Apps to be invalidating itself
        // during the transition, especially since it has a hardware layer set on it
        mAppsCustomizePane->CancelScrollingIndicatorAnimations();
    }
    else {
        // Going from Workspace -> All Apps
        mContent->SetVisibility(VISIBLE);

        // Make sure the current page is loaded (we start loading the side pages after the
        // transition to prevent slowing down the animation)
        Int32 page;
        mAppsCustomizePane->GetCurrentPage(&page);
        mAppsCustomizePane->LoadAssociatedPages(page, TRUE);

        Boolean res;
        LauncherApplication::IsScreenLarge(&res);
        if (!res) {
            mAppsCustomizePane->ShowScrollingIndicator(TRUE);
        }
    }

    if (mResetAfterTransition) {
        mAppsCustomizePane->Reset();
        mResetAfterTransition = FALSE;
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::OnLauncherTransitionStart(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    if (animated) {
        return EnableAndBuildHardwareLayer();
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::OnLauncherTransitionStep(
    /* [in] */ ILauncher* l,
    /* [in] */ Float t)
{
    // Do nothing
    return NOERROR;
}

ECode AppsCustomizeTabHost::OnLauncherTransitionEnd(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    mAppsCustomizePane->OnLauncherTransitionEnd(l, animated, toWorkspace);
    mInTransition = FALSE;
    if (animated) {
        SetLayerType(LAYER_TYPE_NONE, NULL);
    }

    if (!toWorkspace) {
        // Dismiss the workspace cling
        l->DismissWorkspaceCling(NULL);
        // Show the all apps cling (if not already shown)
        mAppsCustomizePane->ShowAllAppsCling();
        // Make sure adjacent pages are loaded (we wait until after the transition to
        // prevent slowing down the animation)
        Int32 page;
        mAppsCustomizePane->GetCurrentPage(&page);
        mAppsCustomizePane->LoadAssociatedPages(page);

        Boolean res;
        LauncherApplication->IsScreenLarge(&res);
        if (!res) {
            mAppsCustomizePane->HideScrollingIndicator(FALSE);
        }

        // Going from Workspace -> All Apps
        // NOTE: We should do this at the end since we check visibility state in some of the
        // cling initialization/dismiss code above.
        SetVisibilityOfSiblingsWithLowerZOrder(INVISIBLE);
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::SetVisibilityOfSiblingsWithLowerZOrder(
    /* [in] */ Int32 visibility)
{
    AutoPtr<IViewParent> p;
    GetParent((IViewParent**)&p);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(p);
    if (parent == NULL) return NOERROR;

    Int32 count;
    parent->GetChildCount(&count);
    Boolean res;
    IsChildrenDrawingOrderEnabled(&res);
    if (!res) {
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            parent->GetChildAt(i, (IView**)&child);
            if (TO_IINTERFACE(this) == TO_IINTERFACE(child)) {
                break;
            }
            else {
                Int32 tmp;
                child->GetVisibility(&tmp);
                if (tmp == GONE) {
                    continue;
                }
                child->SetVisibility(visibility);
            }
        }
    }
    else {
        //throw new RuntimeException("Failed; can't get z-order of views");
        Slogger::E("AppsCustomizeTabHost", "Failed; can't get z-order of views");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::OnWindowVisible()
{
    Int32 tmp;
    GetVisibility(&tmp);
    if (tmp == VISIBLE) {
        mContent->SetVisibility(VISIBLE);
        // We unload the widget previews when the UI is hidden, so need to reload pages
        // Load the current page synchronously, and the neighboring pages asynchronously
        Int32 page;
        mAppsCustomizePane->GetCurrentPage(&page);
        mAppsCustomizePane->LoadAssociatedPages(page, TRUE);
        return mAppsCustomizePane->LoadAssociatedPages(page);
    }
    return NOERROR;
}

ECode AppsCustomizeTabHost::OnTrimMemory()
{
    mContent->SetVisibility(GONE);
    // Clear the widget pages of all their subviews - this will trigger the widget previews
    // to delete their bitmaps
    return mAppsCustomizePane->ClearAllWidgetPages();
}

ECode AppsCustomizeTabHost::IsTransitioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mInTransition;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos