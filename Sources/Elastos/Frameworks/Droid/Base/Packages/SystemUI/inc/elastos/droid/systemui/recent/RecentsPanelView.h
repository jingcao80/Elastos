//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSPANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSPANELVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/systemui/recent/TaskDescription.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Runnable;
using Elastos::Droid::SystemUI::Recent::IRecentsCallback;
using Elastos::Droid::SystemUI::Recent::IRecentsScrollView;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;
using Elastos::Droid::SystemUI::StatusBar::IStatusBarPanel;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::IPopupMenuOnDismissListener;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

class RecentsPanelView
    : public FrameLayout
    , public IAdapterViewOnItemClickListener
    , public IStatusBarPanel
    , public IAnimatorListener
    , public IRecentsPanelView
{
public:
    class ViewHolder
        : public Object
    {
    public:
        ViewHolder();

        CARAPI ToString(
            /* [out] */ String* str);

        AutoPtr<IView> mThumbnailView;
        AutoPtr<IImageView> mThumbnailViewImage;
        AutoPtr<IDrawable> mThumbnailViewDrawable;
        AutoPtr<IImageView> mIconView;
        AutoPtr<ITextView> mLabelView;
        AutoPtr<ITextView> mDescriptionView;
        AutoPtr<IView> mCalloutLine;
        AutoPtr<ITaskDescription> mTaskDescription;
        Boolean mLoadedThumbnailAndIcon;
    };

    class TaskDescriptionAdapter
        : public BaseAdapter
        , public ITaskDescriptionAdapter
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RecentsPanelView::TaskDescriptionAdapter")

        TaskDescriptionAdapter(
            /* [in] */ IContext* context,
            /* [in] */ RecentsPanelView* host);

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

    private:
        AutoPtr<ILayoutInflater> mInflater;
        RecentsPanelView* mHost;
    };

private:
    class InnerListener
        : public Object
        , public IRecentsCallback
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RecentsPanelView::InnerListener")

        InnerListener(
            /* [in] */ RecentsPanelView* host);

        CARAPI HandleOnClick(
            /* [in] */ IView* view);

        CARAPI HandleSwipe(
            /* [in] */ IView* view);

        CARAPI HandleLongPress(
            /* [in] */ IView* selectedView,
            /* [in] */ IView* anchorView,
            /* [in] */ IView* thumbnailView);

        CARAPI Dismiss();

    public:
        RecentsPanelView* mHost;

    };

    class OnLongClickDelegate
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        TO_STRING_IMPL("RecentsPanelView::OnLongClickDelegate")

        OnLongClickDelegate(
            /* [in] */ IView* other);

        CAR_INTERFACE_DECL()

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    public:
        AutoPtr<IView> mOtherView;
    };

    class OnMenuItemClickListener
        : public Object
        , public IPopupMenuOnMenuItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RecentsPanelView::OnMenuItemClickListener")

        OnMenuItemClickListener(
            /* [in] */ RecentsPanelView* host,
            /* [in] */ IView* selectedView);

        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

    public:
        RecentsPanelView* mHost;
        IView* mSelectedView;
    };

    class OnDismissListener
        : public Object
        , public IPopupMenuOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RecentsPanelView::OnDismissListener")

        OnDismissListener(
            /* [in] */ RecentsPanelView* host,
            /* [in] */ IView* thumbnailView);

        CARAPI OnDismiss(
            /* [in] */ IPopupMenu* item);

    public:
        RecentsPanelView* mHost;
        IView* mThumbnailView;
    };

    class OnScrollListenerRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("RecentsPanelView::OnScrollListenerRunnable")

        OnScrollListenerRunnable(
            /* [in] */ RecentsPanelView* host);

        CARAPI Run();

    private:
        RecentsPanelView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RecentsPanelView();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI NumItemsInOneScreenful(
        /* [out] */ Int32* count);

    CARAPI IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    CARAPI Show(
        /* [in] */ Boolean show);

    CARAPI Show(
        /* [in] */ Boolean show,
        /* [in] */ IArrayList* recentTaskDescriptions,
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

    using FrameLayout::OnAnimationEnd;

    CARAPI OnAnimationEnd(
        /* [in] */ IAnimator* animation);

    CARAPI OnAnimationRepeat(
        /* [in] */ IAnimator* animation);

    using FrameLayout::OnAnimationStart;

    CARAPI OnAnimationStart(
        /* [in] */ IAnimator* animation);

    CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Whether the panel is showing, or, if it's animating, whether it will be
     * when the animation is done.
     */
    CARAPI IsShowing(
        /* [out] */ Boolean* result);

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
        /* [in] */ IArrayList* tasks,
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

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI HandleLongPress(
        /* [in] */ IView* selectedView,
        /* [in] */ IView* anchorView,
        /* [in] */ IView* thumbnailView);

protected:
    CARAPI OnFinishInflate();

    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

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
        /* [in] */ IDrawable* thumbnail,
        /* [in] */ Boolean show,
        /* [in] */ Boolean anim);

    CARAPI_(void) AnimateInIconOfFirstTask();

    CARAPI_(void) RefreshRecentTasksList(
        /* [in] */ IArrayList* recentTasksList,
        /* [in] */ Boolean firstScreenful);

    CARAPI_(void) UpdateUiElements();

    CARAPI_(void) StartApplicationDetailsActivity(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

public:
    static const String TAG;
    static const Boolean DEBUG;
    AutoPtr<IPopupMenu> mPopup;
    AutoPtr<IView> mRecentsScrim;
    AutoPtr<IView> mRecentsNoApps;
    AutoPtr<IRecentsScrollView> mRecentsContainer;

    Boolean mShowing;
    Boolean mWaitingToShow;
    AutoPtr<ViewHolder> mItemToAnimateInWhenWindowAnimationIsFinished;
    Boolean mAnimateIconOfFirstTask;
    Boolean mWaitingForWindowAnimation;
    Int64 mWindowAnimationStartTime;
    Boolean mCallUiHiddenBeforeNextReload;

    AutoPtr<IRecentTasksLoader> mRecentTasksLoader;
    AutoPtr<IList> mRecentTaskDescriptions;
    AutoPtr<TaskDescriptionAdapter> mListAdapter;
    Int32 mThumbnailWidth;
    Boolean mFitThumbnailToXY;
    Int32 mRecentItemLayoutId;
    Boolean mHighEndGfx;
};

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::SystemUI::Recent::RecentsPanelView::ViewHolder, IInterface)

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSPANELVIEW_H__
