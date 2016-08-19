#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_RUNNINGPROCESSESVIEW_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_RUNNINGPROCESSESVIEW_H__

#include "elastos/droid/settings/applications/RunningState.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Internal::Utility::IMemInfoReader;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::AbsListView::IRecyclerListener;
using Elastos::Droid::Settings::ISettingsActivity;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

class RunningProcessesView
    : public FrameLayout
    , public IAdapterViewOnItemClickListener
    , public IRecyclerListener
    , public IRunningStateOnRefreshUiListener
{
public:
    class ActiveItem
        : public Object
    {
    protected:
        ActiveItem();

        ~ActiveItem();

        CARAPI_(void) UpdateTime(
            /* [in] */ IContext* context,
            /* [in] */ const String& builder);

    protected:
        AutoPtr<IView> mRootView;
        AutoPtr<IRunningStateBaseItem> mItem;
        AutoPtr<IActivityManagerRunningServiceInfo> mService;
        AutoPtr<ViewHolder> mHolder;
        Int64 mFirstRunTime;
        Boolean mSetBackground;
    };

    class ViewHolder
        : public Object
    {
    public:
        ViewHolder(
            /* [in] */ IView* v)

        CARAPI_(AutoPtr<ActiveItem>) Bind(
            /* [in] */ RunningState* state,
            /* [in] */ RunningState::BaseItem* item,
            /* [in] */ const String& builder)

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
        TimeTicker(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);
    };

    class ServiceListAdapter
        : public BaseAdapter
    {
    public:
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

    protected:
        ServiceListAdapter(
            /* [in] */ RunningState* state,
            /* [in] */ RunningProcessesView* host);

        virtual CARAPI_(void) SetShowBackground(
            /* [in] */ Boolean showBackground);

        virtual CARAPI_(Boolean) GetShowBackground();

        virtual CARAPI_(void) RefreshItems();

    protected:
        AutoPtr<RunningState> mState;
        AutoPtr<ILayoutInflater> mInflater;
        Boolean mShowBackground;
        // ArrayList<RunningState.MergedItem> mOrigItems;
        AutoPtr<IArrayList> mOrigItems;
        // final ArrayList<RunningState.MergedItem> mItems
        //         = new ArrayList<RunningState.MergedItem>();
        AutoPtr<IArrayList> mItems;

    private:
        RunningProcessesView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RunningProcessesView(
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

protected:
    Int32 mMyUserId;

    Int64 SECONDARY_SERVER_MEM;

    // final HashMap<View, ActiveItem> mActiveItems = new HashMap<View, ActiveItem>();
    AutoPtr<IHashMap> mActiveItems;

    AutoPtr<IActivityManager> mAm;

    AutoPtr<RunningState> mState;

    AutoPtr<IFragment> mOwner;

    AutoPtr<IRunnable> mDataAvail;

    StringBuilder mBuilder;// = new StringBuilder(128);

    AutoPtr<RunningState::BaseItem> mCurSelected;

    AutoPtr<IListView> mListView;
    AutoPtr<IView> mHeader;
    AutoPtr<ServiceListAdapter> mAdapter;
    AutoPtr<LinearColorBar> mColorBar;
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
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_RUNNINGPROCESSESVIEW_H__