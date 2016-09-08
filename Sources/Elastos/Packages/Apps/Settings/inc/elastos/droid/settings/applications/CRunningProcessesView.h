#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGPROCESSESVIEW_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGPROCESSESVIEW_H__

#include "_Elastos_Droid_Settings_Applications_CRunningProcessesView.h"
#include "elastos/droid/settings/applications/CLinearColorBar.h"
#include "elastos/droid/settings/applications/RunningState.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/widget/TextView.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IMemInfoReader;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::TextView;
using Elastos::Droid::Widget::IRecyclerListener;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CRunningProcessesView)
    , public FrameLayout
{
public:
    class InnerListener
        : public Object
        , public IAdapterViewOnItemClickListener
        , public IRecyclerListener
        , public IRunningStateOnRefreshUiListener
    {
    public:
        CAR_INTERFACE_DECL();

        TO_STRING_IMPL("CRunningProcessesView::InnerListener")

        InnerListener(
            /* [in] */ CRunningProcessesView* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnMovedToScrapHeap(
            /* [in] */ IView* view);

        CARAPI OnRefreshUi(
            /* [in] */ Int32 what);

    private:
        CRunningProcessesView* mHost;
    };

    class ViewHolder;

    class ActiveItem
        : public Object
    {
    public:
        TO_STRING_IMPL("CRunningProcessesView::ActiveItem")

        ActiveItem();

        CARAPI_(void) UpdateTime(
            /* [in] */ IContext* context,
            /* [in] */ const String& builder);

    public:
        AutoPtr<IView> mRootView;
        AutoPtr<RunningState::BaseItem> mItem;
        AutoPtr<IActivityManagerRunningServiceInfo> mService;
        AutoPtr<ViewHolder> mHolder;
        Int64 mFirstRunTime;
        Boolean mSetBackground;
    };

    class ViewHolder
        : public Object
    {
    public:
        TO_STRING_IMPL("CRunningProcessesView::ViewHolder")

        ViewHolder(
            /* [in] */ IView* v);

        CARAPI_(AutoPtr<ActiveItem>) Bind(
            /* [in] */ RunningState* state,
            /* [in] */ RunningState::BaseItem* item,
            /* [in] */ const String& builder);

    public:
        AutoPtr<IView> mRootView;
        AutoPtr<IImageView> mIcon;
        AutoPtr<ITextView> mName;
        AutoPtr<ITextView> mDescription;
        AutoPtr<ITextView> mSize;
        AutoPtr<ITextView> mUptime;
    };

protected:
    class TimeTicker
        : public TextView
    {
    public:
        TO_STRING_IMPL("CRunningProcessesView::TimeTicker")

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);
    };

    class ServiceListAdapter
        : public BaseAdapter
    {
    public:
        TO_STRING_IMPL("CRunningProcessesView::ServiceListAdapter")

        ServiceListAdapter(
            /* [in] */ RunningState* state,
            /* [in] */ CRunningProcessesView* host);

        virtual CARAPI_(void) SetShowBackground(
            /* [in] */ Boolean showBackground);

        virtual CARAPI_(Boolean) GetShowBackground();

        virtual CARAPI_(void) RefreshItems();

        CARAPI HasStableIds(
            /* [out] */ Boolean* has);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        //@Override
        CARAPI IsEmpty(
            /* [out] */ Boolean* res);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** result);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI NewView(
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI BindView(
            /* [in] */ IView* view,
            /* [in] */ Int32 position);

    public:
        AutoPtr<RunningState> mState;
        AutoPtr<ILayoutInflater> mInflater;
        Boolean mShowBackground;
        // ArrayList<RunningState.MergedItem> mOrigItems;
        AutoPtr<IArrayList> mOrigItems;
        // final ArrayList<RunningState.MergedItem> mItems
        //         = new ArrayList<RunningState.MergedItem>();
        AutoPtr<IArrayList> mItems;

    private:
        CRunningProcessesView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CRunningProcessesView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    virtual CARAPI OnMovedToScrapHeap(
        /* [in] */ IView* view);

    virtual CARAPI DoCreate(
        /* [in] */ IBundle* savedInstanceState);

    virtual CARAPI DoPause();

    virtual CARAPI_(Boolean) DoResume(
        /* [in] */ IFragment* owner,
        /* [in] */ IRunnable* dataAvail);

    //@Override
    CARAPI OnRefreshUi(
        /* [in] */ Int32 what);

protected:
    virtual CARAPI_(void) RefreshUi(
        /* [in] */ Boolean dataChanged);

    virtual CARAPI_(void) UpdateTimes();

private:
    // utility method used to start sub activity
    CARAPI_(void) StartServiceDetailsActivity(
        /* [in] */ RunningState::MergedItem* mi);

public:
    Int32 mMyUserId;

    Int64 SECONDARY_SERVER_MEM;

    // final HashMap<View, ActiveItem> mActiveItems = new HashMap<View, ActiveItem>();
    AutoPtr<IHashMap> mActiveItems;

    AutoPtr<IActivityManager> mAm;

    AutoPtr<RunningState> mState;

    AutoPtr<IFragment> mOwner;

    AutoPtr<IRunnable> mDataAvail;

    AutoPtr<StringBuilder> mBuilder;// = new StringBuilder(128);

    AutoPtr<RunningState::BaseItem> mCurSelected;

    AutoPtr<IListView> mListView;
    AutoPtr<IView> mHeader;
    AutoPtr<ServiceListAdapter> mAdapter;
    AutoPtr<CLinearColorBar> mColorBar;
    AutoPtr<ITextView> mBackgroundProcessPrefix;
    AutoPtr<ITextView> mAppsProcessPrefix;
    AutoPtr<ITextView> mForegroundProcessPrefix;
    AutoPtr<ITextView> mBackgroundProcessText;
    AutoPtr<ITextView> mAppsProcessText;
    AutoPtr<ITextView> mForegroundProcessText;

    Int64 mCurTotalRam;
    Int64 mCurHighRam;      // "System" or "Used"
    Int64 mCurMedRam;       // "Apps" or "Cached"
    Int64 mCurLowRam;       // "Free"
    Boolean mCurShowCached;

    AutoPtr<IDialog> mCurDialog;

    AutoPtr<IMemInfoReader> mMemInfoReader;

    AutoPtr<InnerListener> mListener;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGPROCESSESVIEW_H__