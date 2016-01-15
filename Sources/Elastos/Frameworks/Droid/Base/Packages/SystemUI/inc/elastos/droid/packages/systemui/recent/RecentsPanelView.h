#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSPANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSPANELVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/widget/BaseAdapter.h"


using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IPopupMenuOnDismissListener;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::SystemUI::Recent::IStatusBarTouchProxy;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

extern const InterfaceID EIID_ViewHolder;

class RecentsPanelView : public FrameLayout
{
public:
    class ViewHolder
        : public ElLightRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

    public:
        AutoPtr<IView> mThumbnailView;
        AutoPtr<IImageView> mThumbnailViewImage;
        AutoPtr<IBitmap> mThumbnailViewImageBitmap;
        AutoPtr<IImageView> mIconView;
        AutoPtr<ITextView> mLabelView;
        AutoPtr<ITextView> mDescriptionView;
        AutoPtr<IView> mCalloutLine;
        AutoPtr<ITaskDescription> mTaskDescription;
        Boolean mLoadedThumbnailAndIcon;
    };

    class TaskDescriptionAdapter
        : public ElRefBase
        , public BaseAdapter
        , public ITaskDescriptionAdapter
    {
    public:
        CAR_INTERFACE_DECL()

        TaskDescriptionAdapter(
            /* [in] */ IContext* context,
            /* [in] */ RecentsPanelView* host);

        ~TaskDescriptionAdapter();

        CARAPI_(Int32) GetCount();

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI_(AutoPtr<IInterface>) GetItem(
            /* [in] */ Int32 position);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI_(Int64) GetItemId(
            /* [in] */ Int32 position);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI CreateView(
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI_(AutoPtr<IView>) GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI RecycleView(
            /* [in] */ IView* v);

        CARAPI RegisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI UnregisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI HasStableIds(
            /* [out] */ Boolean* hasStableIds);

        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* viewType);

        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        CARAPI IsEmpty(
            /* [out] */ Boolean* isEmpty);

        CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        CARAPI NotifyDataSetChanged();

        CARAPI NotifyDataSetInvalidated();

    private:
        AutoPtr<ILayoutInflater> mInflater;
        RecentsPanelView* mHost;
    };

private:
    class OnLongClickDelegate
        : public ElRefBase
        , public IViewOnLongClickListener
    {
    public:
        OnLongClickDelegate(
            /* [in] */ IView* other) : mOtherView(other)
        {}

        CAR_INTERFACE_DECL()

        OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    public:
        IView* mOtherView;
    };

    class OnMenuItemClickListener
        : public ElRefBase
        , public IPopupMenuOnMenuItemClickListener
    {
    public:
        OnMenuItemClickListener(
            /* [in] */ RecentsPanelView* host,
            /* [in] */ IView* selectedView)
            : mHost(host)
            , mSelectedView(selectedView)
        {}

        CAR_INTERFACE_DECL()

        OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

    public:
        RecentsPanelView* mHost;
        IView* mSelectedView;
    };

    class OnDismissListener
        : public ElRefBase
        , public IPopupMenuOnDismissListener
    {
    public:
        OnDismissListener(
            /* [in] */ RecentsPanelView* host,
            /* [in] */ IView* thumbnailView)
            : mHost(host)
            , mThumbnailView(thumbnailView)
        {}

        CAR_INTERFACE_DECL()

        OnDismiss(
            /* [in] */ IPopupMenu* item);

    public:
        RecentsPanelView* mHost;
        IView* mThumbnailView;
    };

public:
    RecentsPanelView();

    ~RecentsPanelView();

    CARAPI NumItemsInOneScreenful(
        /* [out] */ Int32* count);

    CARAPI_(Boolean) IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI Show(
        /* [in] */ Boolean show);

    CARAPI Show(
        /* [in] */ Boolean show,
        /* [in] */ IObjectContainer* recentTaskDescriptions,
        /* [in] */ Boolean firstScreenful,
        /* [in] */ Boolean waitingForWindowAnimation);

    static CARAPI SendCloseSystemWindows(
        /* [in] */ IContext* context,
        /* [in] */ const String& reason);

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

    CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Boolean) IsShowing();

    CARAPI SetStatusBarView(
        /* [in] */ IView* statusBarView);

    CARAPI SetRecentTasksLoader(
        /* [in] */ IRecentTasksLoader* loader);

    CARAPI UpdateValuesFromResources();

    CARAPI SetMinSwipeAlpha(
        /* [in] */ Float minAlpha);

    CARAPI OnTaskThumbnailLoaded(
        /* [in] */ ITaskDescription* td);

    CARAPI OnWindowAnimationStart();

    CARAPI ClearRecentTasksList();

    CARAPI OnTaskLoadingCancelled();

    CARAPI RefreshViews();

    CARAPI RefreshRecentTasksList();

    CARAPI OnTasksLoaded(
        /* [in] */ IObjectContainer* tasks,
        /* [in] */ Boolean firstScreenful);

    CARAPI_(Boolean) SimulateClick(
        /* [in] */ Int32 persistentTaskId);

    CARAPI HandleOnClick(
        /* [in] */ IView* view);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI HandleSwipe(
        /* [in] */ IView* view);

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI HandleLongPress(
        /* [in] */ IView* selectedView,
        /* [in] */ IView* anchorView,
        /* [in] */ IView* thumbnailView);

protected:
    CARAPI Init(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI OnFinishInflate();

private:
    CARAPI_(Boolean) PointInside(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IView* v);

    CARAPI_(void) ShowIfReady();

    CARAPI_(void) ShowImpl(
        /* [in] */ Boolean show);

    CARAPI_(void) CreateCustomAnimations(
        /* [in] */ ILayoutTransition* transitioner);

    CARAPI_(void) UpdateIcon(
        /* [in] */ ViewHolder* h,
        /* [in] */ IDrawable* icon,
        /* [in] */ Boolean show,
        /* [in] */ Boolean anim);

    CARAPI_(void) UpdateThumbnail(
        /* [in] */ ViewHolder* h,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Boolean show,
        /* [in] */ Boolean anim);

    CARAPI_(void) AnimateInIconOfFirstTask();

    CARAPI_(void) RefreshRecentTasksList(
        /* [in] */ IObjectContainer* recentTasksList,
        /* [in] */ Boolean firstScreenful);

    CARAPI_(void) UpdateUiElements();

    CARAPI_(void) StartApplicationDetailsActivity(
        /* [in] */ const String& packageName);

public:
    static const String TAG;
    static const Boolean DEBUG;
    AutoPtr<IPopupMenu> mPopup;
    AutoPtr<IView> mRecentsScrim;
    AutoPtr<IView> mRecentsNoApps;
    AutoPtr<IViewGroup> mRecentsContainer;
    AutoPtr<IStatusBarTouchProxy> mStatusBarTouchProxy;

    Boolean mShowing;
    Boolean mWaitingToShow;
    AutoPtr<ViewHolder> mItemToAnimateInWhenWindowAnimationIsFinished;
    Boolean mAnimateIconOfFirstTask;
    Boolean mWaitingForWindowAnimation;
    Int64 mWindowAnimationStartTime;

    AutoPtr<IRecentTasksLoader> mRecentTasksLoader;
    AutoPtr< List<AutoPtr<ITaskDescription> > > mRecentTaskDescriptions;
    AutoPtr<ITaskDescriptionAdapter> mListAdapter;
    Int32 mThumbnailWidth;
    Boolean mFitThumbnailToXY;
    Int32 mRecentItemLayoutId;
    Boolean mHighEndGfx;
};

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSPANELVIEW_H__
