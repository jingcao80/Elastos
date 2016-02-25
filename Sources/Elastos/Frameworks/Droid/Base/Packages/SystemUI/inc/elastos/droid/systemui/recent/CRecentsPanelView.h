#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSPANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSPANELVIEW_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecentsPanelView.h"



#include "elastos/droid/systemui/recent/RecentsPanelView.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecentsPanelView), public RecentsPanelView
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()
    IFRAMELAYOUT_METHODS_DECL()

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI NumItemsInOneScreenful(
        /* [out] */ Int32* num);

    CARAPI IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    CARAPI Show(
        /* [in] */ Boolean show);

    CARAPI Show(
        /* [in] */ Boolean show,
        /* [in] */ IObjectContainer* recentTaskDescriptions,
        /* [in] */ Boolean firstScreenful,
        /* [in] */ Boolean waitingForWindowAnimation);

    CARAPI OnUiHidden();

    CARAPI Dismiss();

    CARAPI DismissAndGoBack();

    CARAPI OnAnimationCancel(
        /* [in] */ IAnimator* animation);

    CARAPI OnAnimationEnd(
        /* [in] */ IAnimator* animation);

    CARAPI OnAnimationRepeat(
        /* [in] */ IAnimator* animation);

    CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    CARAPI SetStatusBarView(
        /* [in] */ IView* statusBarView);

    CARAPI SetRecentTasksLoader(
        /* [in] */ IRecentTasksLoader* loader);

    CARAPI UpdateValuesFromResources();

    CARAPI SetMinSwipeAlpha(
        /* [in] */ Float minAlpha);

    CARAPI OnWindowAnimationStart();

    CARAPI ClearRecentTasksList();

    CARAPI OnTaskLoadingCancelled();

    CARAPI RefreshViews();

    CARAPI RefreshRecentTasksList();

    CARAPI OnTasksLoaded(
        /* [in] */ IObjectContainer* tasks,
        /* [in] */ Boolean firstScreenful);

    CARAPI SimulateClick(
        /* [in] */ Int32 persistentTaskId,
        /* [out] */ Boolean* result);

    CARAPI HandleOnClick(
        /* [in] */ IView* view);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI HandleSwipe(
        /* [in] */ IView* view);

    CARAPI HandleLongPress(
        /* [in] */ IView* selectedView,
        /* [in] */ IView* anchorView,
        /* [in] */ IView* thumbnailView);

    CARAPI OnTaskThumbnailLoaded(
        /* [in] */ ITaskDescription* td);
};

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSPANELVIEW_H__
