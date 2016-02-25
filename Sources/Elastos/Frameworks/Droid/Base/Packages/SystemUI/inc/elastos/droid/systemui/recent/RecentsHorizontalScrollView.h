#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSHORIZONTALSCROLLVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSHORIZONTALSCROLLVIEW_H__

#include "elastos/droid/widget/HorizontalScrollView.h"
#include "elastos/droid/systemui/recent/RecentsScrollViewPerformanceHelper.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::HashSet;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::HorizontalScrollView;
using Elastos::Droid::SystemUI::ISwipeHelper;
using Elastos::Droid::SystemUI::Recent::IRecentsCallback;
using Elastos::Droid::SystemUI::Recent::ITaskDescriptionAdapter;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

class RecentsHorizontalScrollView : public HorizontalScrollView
{
private:
    class NoOpListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        NoOpListener(
           /* [in] */  RecentsHorizontalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        RecentsHorizontalScrollView* mHost;
    };

    class ClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        ClickListener(
           /* [in] */  RecentsHorizontalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        RecentsHorizontalScrollView* mHost;
    };

    class LaunchAppListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        LaunchAppListener(
            /* [in] */  IView* view,
            /* [in] */  RecentsHorizontalScrollView* host)
            : mView(view)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        IView* mView;
        RecentsHorizontalScrollView* mHost;
    };

    class LongClickListener
        : public ElRefBase
        , public IViewOnLongClickListener
    {
    public:
        LongClickListener(
            /* [in] */  IView* view,
            /* [in] */  IView* thumbnailView,
            /* [in] */  RecentsHorizontalScrollView* host)
            : mView(view)
            , mThumbnailView(thumbnailView)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        IView* mView;
        IView* mThumbnailView;
        RecentsHorizontalScrollView* mHost;
    };

    class UpdateScroll
        : public ElRefBase
        , public IOnGlobalLayoutListener
    {
    public:
        UpdateScroll(
           /* [in] */  RecentsHorizontalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnGlobalLayout();

    private:
        RecentsHorizontalScrollView* mHost;
    };

    class SizeChangedRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        SizeChangedRunnable(
           /* [in] */  RecentsHorizontalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        RecentsHorizontalScrollView* mHost;
    };

    class AdapterDataSetObserver
        : public ElRefBase
        , public IDataSetObserver
    {
    public:
        AdapterDataSetObserver(
           /* [in] */  RecentsHorizontalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnChanged();

        CARAPI OnInvalidated();

    private:
        RecentsHorizontalScrollView* mHost;
    };

public:
    RecentsHorizontalScrollView();

    CARAPI SetMinSwipeAlpha(
        /* [in] */ Float minAlpha);

    CARAPI FindViewForTask(
        /* [in] */ Int32 persistentTaskId,
        /* [out] */ IView** view);

    CARAPI RemoveViewInLayout(
        /* [in] */ IView* view);

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Boolean) CanChildBeDismissed(
        /* [in] */ IView* v);

    CARAPI DismissChild(
        /* [in] */ IView* v);

    CARAPI OnChildDismissed(
        /* [in] */ IView* v);

    CARAPI OnBeginDrag(
        /* [in] */ IView* v);

    CARAPI OnDragCancelled(
        /* [in] */ IView* v);

    CARAPI GetChildAtPosition(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ IView** view);

    CARAPI GetChildContentView(
        /* [in] */ IView* v,
        /* [out] */ IView** view);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Int32) GetVerticalFadingEdgeLength();

    CARAPI_(Int32) GetHorizontalFadingEdgeLength();

    CARAPI OnAttachedToWindow();

    CARAPI SetAdapter(
        /* [in] */ ITaskDescriptionAdapter* adapter);

    CARAPI_(Int32) NumItemsInOneScreenful();

    CARAPI SetLayoutTransition(
        /* [in] */ ILayoutTransition* transition);

    CARAPI SetCallback(
        /* [in] */ IRecentsCallback* callback);

protected:
    CARAPI Init(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnFinishInflate();

    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    CARAPI_(Int32) ScrollPositionOfMostRecent();

    CARAPI_(void) AddToRecycledViews(
        /* [in] */ IView* v);

    CARAPI_(void) Update();

    CARAPI_(void) SetOverScrollEffectPadding(
        /* [in] */ Int32 leftPadding,
        /* [in] */ Int32 i);

protected:
    Int32 mLastScrollPosition;

private:
    static const String TAG;
    static const Boolean DEBUG;
    AutoPtr<ILinearLayout> mLinearLayout;
    AutoPtr<ITaskDescriptionAdapter> mAdapter;
    // IRecentsCallback is it's parent's parent
    IRecentsCallback* mCallback;
    AutoPtr<ISwipeHelper> mSwipeHelper;
    AutoPtr<RecentsScrollViewPerformanceHelper> mPerformanceHelper;
    HashSet<AutoPtr<IView> > mRecycledViews;
    Int32 mNumItemsInOneScreenful;
};

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSHORIZONTALSCROLLVIEW_H__
