#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_MANAGEAPPLICATIONS_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_MANAGEAPPLICATIONS_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/settings/CUserSpinnerAdapter.h"
#include "elastos/droid/settings/applications/ApplicationsState.h"
// #include "elastos/droid/settings/applications/RunningProcessesView.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/os/AsyncTask.h"
// #include "elastos/droid/support/v4/view/PagerAdapter.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/Filter.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Internal::App::IIMediaContainerService;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
// using Elastos::Droid::Support::V4::View::PagerAdapter;
// using Elastos::Droid::Support::V4::View::IPagerTabStrip;
// using Elastos::Droid::Support::V4::View::IViewPager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IFilterResults;
using Elastos::Droid::Widget::IRecyclerListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::Filter;
using Elastos::Droid::Widget::IFilter;
using Elastos::Droid::Widget::IFilterable;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

/**
 * Activity to pick an application that will be used to display installation information and
 * options to uninstall/delete user data for system applications. This activity
 * can be launched through Settings or via the ACTION_MANAGE_PACKAGE_STORAGE
 * intent.
 */
class ManageApplications
    : public Fragment
    , public IManageApplicationsAppClickListener
    , public IDialogInterfaceOnClickListener
    , public IDialogInterfaceOnDismissListener
    , public IAdapterViewOnItemSelectedListener
{
    friend class AppViewHolder;
public:
    class ApplicationsAdapter;

    class TabInfo
        : public Object
        , public IManageApplicationsTabInfo
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ManageApplications::TabInfo")

        TabInfo(
            /* [in] */ ManageApplications* owner,
            /* [in] */ ApplicationsState* apps,
            /* [in] */ ICharSequence* label,
            /* [in] */ Int32 listType,
            /* [in] */ IManageApplicationsAppClickListener* clickListener,
            /* [in] */ IBundle* savedInstanceState);

        virtual CARAPI SetContainerService(
            /* [in] */ IIMediaContainerService* containerService);

        virtual CARAPI_(AutoPtr<IView>) Build(
            /* [in] */ ILayoutInflater* inflater,
            /* [in] */ IViewGroup* contentParent,
            /* [in] */ IView* contentChild);

        virtual CARAPI DetachView();

        virtual CARAPI Resume(
            /* [in] */ Int32 sortOrder);

        virtual CARAPI Pause();

        virtual CARAPI ReleaseItem();

        virtual CARAPI_(void) UpdateStorageUsage();

        //@Override
        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        virtual CARAPI_(void) HandleRunningProcessesAvail();

    protected:
        virtual CARAPI_(void) ApplyCurrentStorage();

    public:
        ManageApplications* mOwner;
        AutoPtr<ApplicationsState> mApplicationsState;
        AutoPtr<ICharSequence> mLabel;
        Int32 mListType;
        Int32 mFilter;
        AutoPtr<IManageApplicationsAppClickListener> mClickListener;
        AutoPtr<ICharSequence> mInvalidSizeStr;
        AutoPtr<ICharSequence> mComputingSizeStr;
        AutoPtr<ApplicationsAdapter> mApplications;
        AutoPtr<ILayoutInflater> mInflater;
        AutoPtr<IView> mRootView;

        AutoPtr<IView> mLoadingContainer;

        AutoPtr<IView> mListContainer;

    protected:
        AutoPtr<Runnable> mRunningProcessesAvail;

    private:
        AutoPtr<IBundle> mSavedInstanceState;
        AutoPtr<IIMediaContainerService> mContainerService;

        AutoPtr<IViewGroup> mPinnedHeader;

        // ListView used to display list
        AutoPtr<IListView> mListView;
        // Custom view used to display running processes
        // TODO
        // AutoPtr<RunningProcessesView> mRunningProcessesView;

        //private LinearColorBar mColorBar;
        //private TextView mStorageChartLabel;
        //private TextView mUsedStorageText;
        //private TextView mFreeStorageText;
        Int64 mFreeStorage, mAppStorage, mTotalStorage;
        Int64 mLastUsedStorage, mLastAppStorage, mLastFreeStorage;
    };

    /*
     * Custom adapter implementation for the ListView
     * This adapter maintains a map for each displayed application and its properties
     * An index value on each AppInfo object indicates the correct position or index
     * in the list. If the list gets updated dynamically when the user is viewing the list of
     * applications, we need to return the correct index of position. This is done by mapping
     * the getId methods via the package name into the internal maps and indices.
     * The order of applications in the list is mirrored in mAppLocalList
     */
    class ApplicationsAdapter
        : public BaseAdapter
        , public IFilterable
        , public IApplicationsStateCallbacks
        , public IRecyclerListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ManageApplications::ApplicationsAdapter")

        ApplicationsAdapter(
            /* [in] */ ApplicationsState* state,
            /* [in] */ TabInfo* tab,
            /* [in] */ Int32 filterMode);

        virtual CARAPI Resume(
            /* [in] */ Int32 sort);

        virtual CARAPI Pause();

        virtual CARAPI ReleaseItem();

        virtual CARAPI Rebuild(
            /* [in] */ Int32 sort);

        virtual CARAPI Rebuild(
            /* [in] */ Boolean eraseold);

        virtual CARAPI_(AutoPtr<IArrayList>) ApplyPrefixFilter( /*ArrayList<ApplicationsState.AppEntry>*/
            /* [in] */ ICharSequence* prefix,
            /* [in] */ IArrayList* origEntries);//ArrayList<ApplicationsState.AppEntry>

        //@Override
        CARAPI OnRunningStateChanged(
            /* [in] */ Boolean running);

        //@Override
        CARAPI OnRebuildComplete(
            /* [in] */ IArrayList* apps);

        //@Override
        CARAPI OnPackageListChanged();

        //@Override
        CARAPI OnPackageIconChanged();

        //@Override
        CARAPI OnPackageSizeChanged(
            /* [in] */ const String& packageName);

        //@Override
        CARAPI OnAllSizesComputed();

        //@Override
        CARAPI GetCount(
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** result);

        virtual CARAPI_(AutoPtr<ApplicationsState::AppEntry>) GetAppEntry(
            /* [in] */ Int32 position);

        //@Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        //@Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

        //@Override
        CARAPI GetFilter(
            /* [out] */ IFilter** filter);

        //@Override
        CARAPI OnMovedToScrapHeap(
            /* [in] */ IView* view);

    public:
        AutoPtr<ICharSequence> mCurFilterPrefix;
        AutoPtr<TabInfo> mTab;
        AutoPtr<IArrayList> mBaseEntries; //ArrayList<ApplicationsState::AppEntry>
        AutoPtr<IArrayList> mEntries; //ArrayList<ApplicationsState::AppEntry>

    private:
        AutoPtr<ApplicationsState> mState;
        AutoPtr<ApplicationsState::Session> mSession;
        AutoPtr<IContext> mContext;
        AutoPtr<IArrayList> mActive; // ArrayList<View>
        Int32 mFilterMode;
        Boolean mResumed;
        Int32 mLastSortMode;
        Boolean mWaitingForData;
        Int32 mWhichSize;

        AutoPtr<IFilter> mFilter;
    };

protected:
    class TabInfoRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("ManageApplications::TabInfoRunnable")

        TabInfoRunnable(
            /* [in] */ TabInfo* host);

        ~TabInfoRunnable();

        //@Override
        CARAPI Run();

    private:
        TabInfo* mHost;
    };

    // class MyPagerAdapter
    //     : public PagerAdapter
    //     , public IViewPagerOnPageChangeListener
    // {
    // public:
    //     CAR_INTERFACE_DECL()

    //     TO_STRING_IMPL("ManageApplications::MyPagerAdapter")

    //     MyPagerAdapter(
    //         /* [in] */ ManageApplications* host);

    //     ~MyPagerAdapter();

    //     //@Override
    //     CARAPI GetCount(
    //         /* [out] */ Int32* count);

    //     //@Override
    //     CARAPI InstantiateItem(
    //         /* [in] */ IViewGroup* container,
    //         /* [in] */ Int32 position,
    //         /* [out] */ IInterface** item);

    //     //@Override
    //     CARAPI DestroyItem(
    //         /* [in] */ IViewGroup* container,
    //         /* [in] */ Int32 position,
    //         /* [in] */ IInterface* object);

    //     //@Override
    //     CARAPI IsViewFromObject(
    //         /* [in] */ IView* view,
    //         /* [in] */ IInterface* object,
    //         /* [out] */ Boolean* result);

    //     //@Override
    //     CARAPI GetItemPosition(
    //         /* [in] */ IInterface* object,
    //         /* [out] */ Int32* pos);

    //     //@Override
    //     CARAPI GetPageTitle(
    //         /* [in] */ Int32 position,
    //         /* [out] */ ICharSequence** title);

    //     //@Override
    //     CARAPI OnPageScrolled(
    //         /* [in] */ Int32 position,
    //         /* [in] */ Float positionOffset,
    //         /* [in] */ Int32 positionOffsetPixels);

    //     //@Override
    //     CARAPI OnPageSelected(
    //         /* [in] */ Int32 position);

    //     //@Override
    //     CARAPI OnPageScrollStateChanged(
    //         /* [in] */ Int32 state);

    // protected:
    //     Int32 mCurPos;

    // private:
    //     ManageApplications* mHost;
    // };

    class ApplicationsAdapterFilter
        : public Filter
    {
        friend class ManageApplications;
    protected:
        TO_STRING_IMPL("ManageApplications::ApplicationsAdapterFilter")

        ApplicationsAdapterFilter(
            /* [in] */ ApplicationsAdapter* host);

        ~ApplicationsAdapterFilter();

        //@Override
        CARAPI PerformFiltering(
            /* [in] */ ICharSequence* constraint,
            /* [out] */ IFilterResults** filterResults);

        //@Override
        CARAPI PublishResults(
            /* [in] */ ICharSequence* constraint,
            /* [in] */ IFilterResults* results);

    private:
        ApplicationsAdapter* mHost;
    };

private:
    class OnClickAsyncTask
        //<Void, Void, Void>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("ManageApplications::OnClickAsyncTask")

        OnClickAsyncTask(
            /* [in] */ ManageApplications* host,
            /* [in] */ IPackageManager* pm,
            /* [in] */ IINotificationManager* nm,
            /* [in] */ IIPackageManager* ipm,
            /* [in] */ IAppOpsManager* aom,
            /* [in] */ INetworkPolicyManager* npm,
            /* [in] */ IHandler* handler);

        ~OnClickAsyncTask();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        ManageApplications* mHost;
        AutoPtr<IPackageManager> mPm;
        AutoPtr<IINotificationManager> mNm;
        AutoPtr<IIPackageManager> mIPm;
        AutoPtr<IAppOpsManager> mAom;
        AutoPtr<INetworkPolicyManager> mNpm;
        AutoPtr<IHandler> mHandler;
    };

    class OnClickRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("ManageApplications::OnClickRunnable")

        OnClickRunnable(
            /* [in] */ ManageApplications* host);

        ~OnClickRunnable();

        //@Override
        CARAPI Run();

    private:
        ManageApplications* mHost;
    };

    class InitServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ManageApplications::InitServiceConnection")

        InitServiceConnection(
            /* [in] */ ManageApplications* host);

        ~InitServiceConnection();

        //@Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        //@Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        ManageApplications* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("ManageApplications")

    ManageApplications();

    ~ManageApplications();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //@Override
    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu);

    //@Override
    CARAPI OnDestroyOptionsMenu();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    virtual CARAPI OnItemClick(
        /* [in] */ IManageApplicationsTabInfo* tab,
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    virtual CARAPI UpdateCurrentTab(
        /* [in] */ Int32 position);

private:
    CARAPI_(void) UpdateNumTabs();

    // utility method used to start sub activity
    CARAPI_(void) StartApplicationDetailsActivity();

protected:
    virtual CARAPI_(AutoPtr<TabInfo>) TabForType(
        /* [in] */ Int32 type);

    virtual CARAPI_(void) UpdateOptionsMenu();

    virtual CARAPI_(void) BuildResetDialog();

public:
    // attributes used as keys when passing values to InstalledAppDetails activity
    static const String APP_CHG;

    static const Int32 SIZE_TOTAL;
    static const Int32 SIZE_INTERNAL = 1;
    static const Int32 SIZE_EXTERNAL = 2;

    // Filter options used for displayed list of applications
    static const Int32 FILTER_APPS_ALL;
    static const Int32 FILTER_APPS_THIRD_PARTY = 1;//MENU_OPTIONS_BASE + 1;
    static const Int32 FILTER_APPS_SDCARD = 2;//MENU_OPTIONS_BASE + 2;
    static const Int32 FILTER_APPS_DISABLED = 3;//MENU_OPTIONS_BASE + 3;

    static const Int32 SORT_ORDER_ALPHA;
    static const Int32 SORT_ORDER_SIZE = 5;//MENU_OPTIONS_BASE + 5;
    static const Int32 SHOW_RUNNING_SERVICES;
    static const Int32 SHOW_BACKGROUND_PROCESSES;
    static const Int32 RESET_APP_PREFERENCES;

protected:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<TabInfo> mCurTab;

    // Size resource used for packages whose size computation failed for some reason
    AutoPtr<ICharSequence> mInvalidSizeStr;
    static const Int32 LIST_TYPE_DOWNLOADED;
    static const Int32 LIST_TYPE_RUNNING;
    static const Int32 LIST_TYPE_SDCARD;
    static const Int32 LIST_TYPE_ALL;
    static const Int32 LIST_TYPE_DISABLED;

    AutoPtr<IAlertDialog> mResetDialog;

private:
    static const String EXTRA_LIST_TYPE;
    static const String EXTRA_SORT_ORDER;
    static const String EXTRA_SHOW_BACKGROUND;
    static const String EXTRA_DEFAULT_LIST_TYPE;
    static const String EXTRA_RESET_DIALOG;

    // constant value that can be used to check return code from sub activity.
    static const Int32 INSTALLED_APP_DETAILS;

    // sort order that can be changed through the menu can be sorted alphabetically
    // or Size(descending)
    static const Int32 MENU_OPTIONS_BASE = 0;
    // sort order
    Int32 mSortOrder;

    AutoPtr<ApplicationsState> mApplicationsState;

    AutoPtr<IArrayList> /*ArrayList<TabInfo>*/ mTabs;
    Int32 mNumTabs;

    AutoPtr<ICharSequence> mComputingSizeStr;

    // layout inflater object used to inflate views
    AutoPtr<ILayoutInflater> mInflater;

    String mCurrentPkgName;

    AutoPtr<IMenu> mOptionsMenu;

    // These are for keeping track of activity and spinner switch state.
    Boolean mActivityResumed;

    static const Int32 LIST_TYPE_MISSING;

    Boolean mShowBackground;

    Int32 mDefaultListType;

    AutoPtr<IViewGroup> mContentContainer;
    AutoPtr<IView> mRootView;
    // TODO
    // AutoPtr<IViewPager> mViewPager;
    AutoPtr<CUserSpinnerAdapter> mProfileSpinnerAdapter;
    AutoPtr<IContext> mContext;

    /*volatile*/ AutoPtr<IIMediaContainerService> mContainerService;

    AutoPtr<IServiceConnection> mContainerConnection;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_MANAGEAPPLICATIONS_H__