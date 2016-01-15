#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSVERTICALSCROLLVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSVERTICALSCROLLVIEW_H__

#include "elastos/droid/widget/ScrollView.h"
#include "elastos/droid/systemui/recent/RecentsScrollViewPerformanceHelper.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::HashSet;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Widget::ScrollView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::SystemUI::ISwipeHelper;
using Elastos::Droid::SystemUI::Recent::IRecentsCallback;
using Elastos::Droid::SystemUI::Recent::ITaskDescriptionAdapter;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

class RecentsVerticalScrollView : public ScrollView
{
private:
    class NoOpListener
        : public IViewOnTouchListener
        , public ElRefBase
    {
    public:
        NoOpListener(
           /* [in] */  RecentsVerticalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        RecentsVerticalScrollView* mHost;
    };

    class ClickListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        ClickListener(
           /* [in] */  RecentsVerticalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        RecentsVerticalScrollView* mHost;
    };

    class LaunchAppListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        LaunchAppListener(
           /* [in] */  RecentsVerticalScrollView* host,
            /* [in] */  IView* view)
            : mHost(host)
            , mView(view)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        RecentsVerticalScrollView* mHost;
        IView* mView;
    };

    class LongClickListener
        : public IViewOnLongClickListener
        , public ElRefBase
    {
    public:
        LongClickListener(
           /* [in] */  RecentsVerticalScrollView* host,
            /* [in] */  IView* view,
            /* [in] */  IView* thumbnailView)
            : mHost(host)
            , mView(view)
            , mThumbnailView(thumbnailView)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        RecentsVerticalScrollView* mHost;
        IView* mView;
        IView* mThumbnailView;
    };

    class UpdateScroll
        : public IOnGlobalLayoutListener
        , public ElRefBase
    {
    public:
        UpdateScroll(
           /* [in] */  RecentsVerticalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnGlobalLayout();

    private:
        RecentsVerticalScrollView* mHost;
    };

    class SizeChangedRunnable
        : public IRunnable
        , public ElRefBase
    {
    public:
        SizeChangedRunnable(
           /* [in] */  RecentsVerticalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        RecentsVerticalScrollView* mHost;
    };


    class AdapterDataSetObserver
        : public IDataSetObserver
        , public ElRefBase
    {
    public:
        AdapterDataSetObserver(
           /* [in] */  RecentsVerticalScrollView* host) : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnChanged();

        CARAPI OnInvalidated();

    private:
        RecentsVerticalScrollView* mHost;
    };

public:
    RecentsVerticalScrollView();

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

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_RECENTSVERTICALSCROLLVIEW_H__
