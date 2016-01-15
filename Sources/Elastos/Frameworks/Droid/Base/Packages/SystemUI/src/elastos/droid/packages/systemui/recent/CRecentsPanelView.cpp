#include "elastos/droid/systemui/recent/CRecentsPanelView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

IVIEW_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)
IVIEWGROUP_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)
IVIEWPARENT_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)
IVIEWMANAGER_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)
IDRAWABLECALLBACK_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)
IKEYEVENTCALLBACK_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)
IFRAMELAYOUT_METHODS_IMPL(CRecentsPanelView, RecentsPanelView)

ECode CRecentsPanelView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return RecentsPanelView::Init(ctx, attrs);
}

ECode CRecentsPanelView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return RecentsPanelView::Init(ctx, attrs, defStyle);
}

PInterface CRecentsPanelView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CRecentsPanelView::Probe(riid);
}

ECode CRecentsPanelView::NumItemsInOneScreenful(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    return RecentsPanelView::NumItemsInOneScreenful(num);
}

ECode CRecentsPanelView::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RecentsPanelView::IsInContentArea(x, y);
    return NOERROR;
}

ECode CRecentsPanelView::Show(
    /* [in] */ Boolean show)
{
    return RecentsPanelView::Show(show);
}

ECode CRecentsPanelView::Show(
    /* [in] */ Boolean show,
    /* [in] */ IObjectContainer* recentTaskDescriptions,
    /* [in] */ Boolean firstScreenful,
    /* [in] */ Boolean waitingForWindowAnimation)
{
    return RecentsPanelView::Show(show, recentTaskDescriptions, firstScreenful, waitingForWindowAnimation);
}

ECode CRecentsPanelView::OnUiHidden()
{
    return RecentsPanelView::OnUiHidden();
}

ECode CRecentsPanelView::Dismiss()
{
    return RecentsPanelView::Dismiss();
}

ECode CRecentsPanelView::DismissAndGoBack()
{
    return RecentsPanelView::DismissAndGoBack();
}

ECode CRecentsPanelView::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return RecentsPanelView::OnAnimationCancel(animation);
}

ECode CRecentsPanelView::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return RecentsPanelView::OnAnimationEnd(animation);
}

ECode CRecentsPanelView::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return RecentsPanelView::OnAnimationRepeat(animation);
}

ECode CRecentsPanelView::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return RecentsPanelView::OnAnimationStart(animation);
}

ECode CRecentsPanelView::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RecentsPanelView::IsShowing();
    return NOERROR;
}

ECode CRecentsPanelView::SetStatusBarView(
    /* [in] */ IView* statusBarView)
{
    return RecentsPanelView::SetStatusBarView(statusBarView);
}

ECode CRecentsPanelView::SetRecentTasksLoader(
    /* [in] */ IRecentTasksLoader* loader)
{
    return RecentsPanelView::SetRecentTasksLoader(loader);
}

ECode CRecentsPanelView::UpdateValuesFromResources()
{
    return RecentsPanelView::UpdateValuesFromResources();
}

ECode CRecentsPanelView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    return RecentsPanelView::SetMinSwipeAlpha(minAlpha);
}

ECode CRecentsPanelView::OnWindowAnimationStart()
{
    return RecentsPanelView::OnWindowAnimationStart();
}

ECode CRecentsPanelView::ClearRecentTasksList()
{
    return RecentsPanelView::ClearRecentTasksList();
}

ECode CRecentsPanelView::OnTaskLoadingCancelled()
{
    return RecentsPanelView::OnTaskLoadingCancelled();
}

ECode CRecentsPanelView::RefreshViews()
{
    return RecentsPanelView::RefreshViews();
}

ECode CRecentsPanelView::RefreshRecentTasksList()
{
    return RecentsPanelView::RefreshRecentTasksList();
}

ECode CRecentsPanelView::OnTasksLoaded(
    /* [in] */ IObjectContainer* tasks,
    /* [in] */ Boolean firstScreenful)
{
    return RecentsPanelView::OnTasksLoaded(tasks, firstScreenful);
}

ECode CRecentsPanelView::SimulateClick(
    /* [in] */ Int32 persistentTaskId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RecentsPanelView::SimulateClick(persistentTaskId);
    return NOERROR;
}

ECode CRecentsPanelView::HandleOnClick(
    /* [in] */ IView* view)
{
    return RecentsPanelView::HandleOnClick(view);
}

ECode CRecentsPanelView::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return RecentsPanelView::OnItemClick(parent, view, position, id);
}

ECode CRecentsPanelView::HandleSwipe(
    /* [in] */ IView* view)
{
    return RecentsPanelView::HandleSwipe(view);
}

ECode CRecentsPanelView::HandleLongPress(
    /* [in] */ IView* selectedView,
    /* [in] */ IView* anchorView,
    /* [in] */ IView* thumbnailView)
{
    return RecentsPanelView::HandleLongPress(selectedView, anchorView, thumbnailView);
}

ECode CRecentsPanelView::OnTaskThumbnailLoaded(
    /* [in] */ ITaskDescription* td)
{
    return RecentsPanelView::OnTaskThumbnailLoaded(td);
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
