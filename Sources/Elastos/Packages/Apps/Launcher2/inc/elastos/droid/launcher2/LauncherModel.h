#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERMODEL_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERMODEL_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/DeferredHandler.h"
#include "elastos/droid/launcher2/AllAppsList.h"
#include "elastos/droid/launcher2/IconCache.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/FolderInfo.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::Pm::ILauncherApps;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::ILauncherAppsCallback;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IUserManager;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Core::IComparator;
using Elastos::Text::ICollator;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Maintains in-memory state of the Launcher. It is expected that there should be only one
 * LauncherModel object held in a static. Also provide APIs for updating the database state
 * for the Launcher.
 */
class LauncherModel
    : public BroadcastReceiver
    , public ILauncherModel
{
public:
    class ShortcutNameComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        ShortcutNameComparator();

        ShortcutNameComparator(
            /* [in] */ IHashMap* labelCache);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> mCollator;
        AutoPtr<IHashMap> mLabelCache;
    };

    class WidgetAndShortcutNameComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        WidgetAndShortcutNameComparator(
            /* [in] */ IPackageManager* pm);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> mCollator;
        AutoPtr<IPackageManager> mPackageManager;
        AutoPtr<IHashMap> mLabelCache;
    };

private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IArrayList* tmpWorkspaceItems,
            /* [in] */ IArrayList* tmpAppWidgets);

        CARAPI Run();

    private:
        AutoPtr<IArrayList> mTmpWorkspaceItems;
        AutoPtr<IArrayList> mTmpAppWidgets;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        MyRunnable2(
            /* [in] */ Int64 itemId,
            /* [in] */ ItemInfo* item);

        CARAPI Run();

    private:
        Int64 mItemId;
        AutoPtr<ItemInfo> mItem;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        MyRunnable3(
            /* [in] */ IContentResolver* cr,
            /* [in] */ IUri* uri,
            /* [in] */ IContentValues* values,
            /* [in] */ Int64 itemId,
            /* [in] */ ItemInfo* item);

        CARAPI Run();

    private:
        AutoPtr<IContentResolver> mCr;
        AutoPtr<IUri> mUri;
        AutoPtr<IContentValues> mValues;
        Int64 mItemId;
        AutoPtr<ItemInfo> mItem;
    };

    class MyRunnable4
        : public Runnable
    {
    public:
        MyRunnable4(
            /* [in] */ LauncherModel* host);

        CARAPI Run();

    private:
        AutoPtr<LauncherModel> mHost;
    };

    class MyRunnable5
        : public Runnable
    {
    public:
        MyRunnable5(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Boolean notify,
            /* [in] */ IContentValues* values,
            /* [in] */ ItemInfo* item);

        CARAPI Run();

    private:
        AutoPtr<IContentResolver> mCr;
        Boolean mNotify;
        AutoPtr<IContentValues> mValues;
        AutoPtr<ItemInfo> mItem;
    };

    class MyRunnable6
        : public Runnable
    {
    public:
        MyRunnable6(
            /* [in] */ IContentResolver* cr,
            /* [in] */ IUri* uriToDelete,
            /* [in] */ ItemInfo* item);

        CARAPI Run();

    private:
        AutoPtr<IContentResolver> mCr;
        AutoPtr<IUri> mUriToDelete;
        AutoPtr<ItemInfo> mItem;
    };

    class MyRunnable7
        : public Runnable
    {
    public:
        MyRunnable7(
            /* [in] */ IContentResolver* cr,
            /* [in] */ FolderInfo* info);

        CARAPI Run();

    private:
        AutoPtr<IContentResolver> mCr;
        AutoPtr<FolderInfo> mInfo;
    };

    class LauncherAppsCallback
        : public Object
        , public ILauncherAppsCallback
    {
    public:
        CAR_INTERFACE_DECL();

        LauncherAppsCallback(
            /* [in] */ LauncherModel* host);

        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ IUserHandle* user);

        //@Override
        CARAPI OnPackageAdded(
            /* [in] */ const String& packageName,
            /* [in] */ IUserHandle* user);

        //@Override
        CARAPI OnPackageChanged(
            /* [in] */ const String& packageName,
            /* [in] */ IUserHandle* user);

        //@Override
        CARAPI OnPackagesAvailable(
            /* [in] */ ArrayOf<String>* packageName,
            /* [in] */ IUserHandle* user,
            /* [in] */ Boolean replacing);

        //@Override
        CARAPI OnPackagesUnavailable(
            /* [in] */ ArrayOf<String>* packageName,
            /* [in] */ IUserHandle* user,
            /* [in] */ Boolean replacing);

    private:
        AutoPtr<LauncherModel> mHost;
    };

    class MyRunnable17
        : public Runnable
    {
    public:
        MyRunnable17(
            /* [in] */ IWeakReference* _callbacks,
            /* [in] */ ILauncherModelCallbacks* modelCallbacks,
            /* [in] */ IArrayList* addedFinal);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mCallbacks;
        AutoPtr<ILauncherModelCallbacks> mModelCallbacks;
        AutoPtr<IArrayList> mAddedFinal;
    };

    class MyRunnable18
        : public Runnable
    {
    public:
        MyRunnable18(
            /* [in] */ IWeakReference* _callbacks,
            /* [in] */ ILauncherModelCallbacks* modelCallbacks,
            /* [in] */ IArrayList* modifiedFinal);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mCallbacks;
        AutoPtr<ILauncherModelCallbacks> mModelCallbacks;
        AutoPtr<IArrayList> mModifiedFinal;
    };

    class MyRunnable19
        : public Runnable
    {
    public:
        MyRunnable19(
            /* [in] */ IWeakReference* _callbacks,
            /* [in] */ ILauncherModelCallbacks* modelCallbacks,
            /* [in] */ IArrayList* removedPackageNames,
            /* [in] */ IArrayList* removedApps,
            /* [in] */ Boolean permanent,
            /* [in] */ IUserHandle* user);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mCallbacks;
        AutoPtr<ILauncherModelCallbacks> mModelCallbacks;
        AutoPtr<IArrayList> mRemovedPackageNames;
        AutoPtr<IArrayList> mRemovedApps;
        Boolean mPermanent;
        AutoPtr<IUserHandle> mUser;
    };

    class MyRunnable20
        : public Runnable
    {
    public:
        MyRunnable20(
            /* [in] */ IWeakReference* _callbacks,
            /* [in] */ ILauncherModelCallbacks* modelCallbacks,
            /* [in] */ IArrayList* widgetsAndShortcuts);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mCallbacks;
        AutoPtr<ILauncherModelCallbacks> mModelCallbacks;
        AutoPtr<IArrayList> mWidgetsAndShortcuts;
    };

    /**
     * Runnable for the thread that loads the contents of the launcher:
     *   - workspace icons
     *   - widgets
     *   - all apps icons
     */
    class LoaderTask
        : public Object
        , public IRunnable
    {
    private:
        class MyRunnable8
            : public Runnable
        {
        public:
            MyRunnable8(
                /* [in] */ LoaderTask* host);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
        };

        class MyComparator
            : public Object
            , public IComparator
        {
        public:
            CAR_INTERFACE_DECL();

            //@Override
            CARAPI Compare(
                /* [in] */ IInterface* lhs,
                /* [in] */ IInterface* rhs,
                /* [out] */ Int32* result);
        };

        class MyComparator2
            : public Object
            , public IComparator
        {
        public:
            CAR_INTERFACE_DECL();

            //@Override
            CARAPI Compare(
                /* [in] */ IInterface* lhs,
                /* [in] */ IInterface* rhs,
                /* [out] */ Int32* result);
        };

        class MyRunnable9
            : public Runnable
        {
        public:
            MyRunnable9(
                /* [in] */ LoaderTask* host,
                /* [in] */ ILauncherModelCallbacks* oldCallbacks,
                /* [in] */ IArrayList* workspaceItems,
                /* [in] */ Int32 start,
                /* [in] */ Int32 chunkSize);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
            AutoPtr<ILauncherModelCallbacks> mOldCallbacks;
            AutoPtr<IArrayList> mWorkspaceItems;
            Int32 mStart;
            Int32 mChunkSize;
        };

        class MyRunnable10
            : public Runnable
        {
        public:
            MyRunnable10(
                /* [in] */ LoaderTask* host,
                /* [in] */ ILauncherModelCallbacks* oldCallbacks,
                /* [in] */ IHashMap* folders);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
            AutoPtr<ILauncherModelCallbacks> mOldCallbacks;
            AutoPtr<IHashMap> mFolders;
        };

        class MyRunnable11
            : public Runnable
        {
        public:
            MyRunnable11(
                /* [in] */ LoaderTask* host,
                /* [in] */ ILauncherModelCallbacks* oldCallbacks,
                /* [in] */ ILauncherAppWidgetInfo* widget);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
            AutoPtr<ILauncherModelCallbacks> mOldCallbacks;
            AutoPtr<ILauncherAppWidgetInfo> mWidget;
        };

        class MyRunnable12
            : public Runnable
        {
        public:
            MyRunnable12(
                /* [in] */ LoaderTask* host,
                /* [in] */ ILauncherModelCallbacks* oldCallbacks);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
            AutoPtr<ILauncherModelCallbacks> mOldCallbacks;
        };

        class MyRunnable13
            : public Runnable
        {
        public:
            MyRunnable13(
                /* [in] */ LoaderTask* host,
                /* [in] */ ILauncherModelCallbacks* oldCallbacks,
                /* [in] */ Int32 currentScreen);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
            AutoPtr<ILauncherModelCallbacks> mOldCallbacks;
            Int32 mCurrentScreen;
        };

        class MyRunnable14
            : public Runnable
        {
        public:
            MyRunnable14(
                /* [in] */ LoaderTask* host,
                /* [in] */ ILauncherModelCallbacks* oldCallbacks);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
            AutoPtr<ILauncherModelCallbacks> mOldCallbacks;
        };

        class MyRunnable15
            : public Runnable
        {
        public:
            MyRunnable15(
                /* [in] */ LoaderTask* host,
                /* [in] */ ILauncherModelCallbacks* oldCallbacks,
                /* [in] */ IArrayList* list);

            CARAPI Run();

        private:
            AutoPtr<LoaderTask> mHost;
            AutoPtr<ILauncherModelCallbacks> mOldCallbacks;
            AutoPtr<IArrayList> mList;
        };

        class MyRunnable16
            : public Runnable
        {
        public:
            MyRunnable16(
                /* [in] */ ILauncherModelCallbacks* _callbacks,
                /* [in] */ Boolean firstProfile,
                /* [in] */ IArrayList* added);

            CARAPI Run();

        private:
            AutoPtr<ILauncherModelCallbacks> mCallbacks;
            Boolean mFirstProfile;
            AutoPtr<IArrayList> mAdded;
        };

    public:
        CAR_INTERFACE_DECL();

        LoaderTask(
            /* [in] */ LauncherModel* host,
            /* [in] */ IContext* context,
            /* [in] */ Boolean isLaunching);

        CARAPI IsLaunching(
            /* [out] */ Boolean* result);

        CARAPI IsLoadingWorkspace(
            /* [out] */ Boolean* result);

        CARAPI RunBindSynchronousPage(
            /* [in] */ Int32 synchronousBindPage);

        CARAPI Run();

        CARAPI StopLocked();

        /**
         * Gets the callbacks object.  If we've been stopped, or if the launcher object
         * has somehow been garbage collected, return null instead.  Pass in the Callbacks
         * object that was around when the deferred message was scheduled, and if there's
         * a new Callbacks object around then also return null.  This will save us from
         * calling onto it with data that will be ignored.
         */
        CARAPI TryGetCallbacks(
            /* [in] */ ILauncherModelCallbacks* oldCallbacks,
            /* [out] */ ILauncherModelCallbacks** newCallbacks);

        CARAPI DumpState();

    private:
        CARAPI_(void) LoadAndBindWorkspace();

        CARAPI_(void) WaitForIdle();

        // check & update map of what's occupied; used to discard overlapping/invalid items
        CARAPI_(Boolean) CheckItemPlacement(
            /* [in] */ ArrayOf<ArrayOf<ArrayOf<ItemInfo*> > >* occupied,
            /* [in] */ ItemInfo* item);

        CARAPI_(void) LoadWorkspace();

        /** Filters the set of items who are directly or indirectly (via another container) on the
         * specified screen. */
        CARAPI_(void) FilterCurrentWorkspaceItems(
            /* [in] */ Int32 currentScreen,
            /* [in] */ IArrayList* allWorkspaceItems,
            /* [in] */ IArrayList* currentScreenItems,
            /* [in] */ IArrayList* otherScreenItems);

        /** Filters the set of widgets which are on the specified screen. */
        CARAPI_(void) FilterCurrentAppWidgets(
            /* [in] */ Int32 currentScreen,
            /* [in] */ IArrayList* appWidgets,
            /* [in] */ IArrayList* currentScreenWidgets,
            /* [in] */ IArrayList* otherScreenWidgets);

        /** Filters the set of folders which are on the specified screen. */
        CARAPI_(void) FilterCurrentFolders(
            /* [in] */ Int32 currentScreen,
            /* [in] */ IHashMap* itemsIdMap,
            /* [in] */ IHashMap* folders,
            /* [in] */ IHashMap* currentScreenFolders,
            /* [in] */ IHashMap* otherScreenFolders);

        /** Sorts the set of items by hotseat, workspace (spatially from top to bottom, left to
         * right) */
        CARAPI_(void) SortWorkspaceItemsSpatially(
            /* [in] */ IArrayList* workspaceItems);

        CARAPI_(void) BindWorkspaceItems(
            /* [in] */ ILauncherModelCallbacks* oldCallbacks,
            /* [in] */ IArrayList* workspaceItems,
            /* [in] */ IArrayList* appWidgets,
            /* [in] */ IHashMap* folders,
            /* [in] */ IArrayList* deferredBindRunnables);

        /**
         * Binds all loaded data to actual views on the main thread.
         */
        CARAPI_(void) BindWorkspace(
            /* [in] */ Int32 synchronizeBindPage);

        CARAPI_(void) LoadAndBindAllApps();

        CARAPI_(void) OnlyBindAllApps();

        CARAPI_(void) LoadAllAppsByBatch();

    private:
        AutoPtr<LauncherModel> mHost;
        AutoPtr<IContext> mContext;
        Boolean mIsLaunching;
        Boolean mIsLoadingAndBindingWorkspace;
        Boolean mStopped;
        Boolean mLoadAndBindStepFinished;

        AutoPtr<IHashMap> mLabelCache;
    };

    class PackageUpdatedTask
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();

        PackageUpdatedTask(
            /* [in] */ LauncherModel* host,
            /* [in] */ Int32 op,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ IUserHandle* user);

        CARAPI Run();

    public:
        AutoPtr<LauncherModel> mHost;
        Int32 mOp;
        AutoPtr<ArrayOf<String> > mPackages;
        AutoPtr<IUserHandle> mUser;

        static const Int32 OP_NONE;
        static const Int32 OP_ADD;
        static const Int32 OP_UPDATE;
        static const Int32 OP_REMOVE; // uninstlled
        static const Int32 OP_UNAVAILABLE; // external media unmounted
    };

    class MyComparator3
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        MyComparator3(
            /* [in] */ ICollator* collator);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> mCollator;
    };

    class MyComparator4
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        MyComparator4(
            /* [in] */ ICollator* collator);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> mCollator;
    };

public:
    class MyComparator5
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL();

    LauncherModel();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILauncherApplication* app,
        /* [in] */ IconCache* iconCache);

    CARAPI GetFallbackIcon(
        /* [out] */ IBitmap** bitmap);

    CARAPI UnbindItemInfosAndClearQueuedBindRunnables();

    /** Unbinds all the sBgWorkspaceItems and sBgAppWidgets on the main thread */
    CARAPI UnbindWorkspaceItemsOnMainThread();

    /**
     * Adds an item to the DB if it was not created previously, or move it to a new
     * <container, screen, cellX, cellY>
     */
    static CARAPI AddOrMoveItemInDatabase(
        /* [in] */ IContext* context,
        /* [in] */ IItemInfo* item,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY);


    static CARAPI CheckItemInfoLocked(
        /* [in] */ Int64 itemId,
        /* [in] */ IItemInfo* item);

    static CARAPI CheckItemInfo(
        /* [in] */ ItemInfo* item);

    static CARAPI UpdateItemInDatabaseHelper(
        /* [in] */ IContext* context,
        /* [in] */ IContentValues* values,
        /* [in] */ ItemInfo* item,
        /* [in] */ const String& callingFunction);

    CARAPI FlushWorkerThread();

    /**
     * Move an item in the DB to a new <container, screen, cellX, cellY>
     */
    static CARAPI MoveItemInDatabase(
        /* [in] */ IContext* context,
        /* [in] */ ItemInfo* item,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY);

    /**
     * Move and/or resize item in the DB to a new <container, screen, cellX, cellY, spanX, spanY>
     */
    static CARAPI ModifyItemInDatabase(
        /* [in] */ IContext* context,
        /* [in] */ ItemInfo* item,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY);

    /**
     * Update an item to the database in a specified container.
     */
    static CARAPI UpdateItemInDatabase(
        /* [in] */ IContext* context,
        /* [in] */ ItemInfo* item);

    /**
     * Returns true if the shortcuts already exists in the database.
     * we identify a shortcut by its title and intent.
     */
    static CARAPI ShortcutExists(
        /* [in] */ IContext* context,
        /* [in] */ const String& title,
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    /**
     * Returns an ItemInfo array containing all the items in the LauncherModel.
     * The ItemInfo.id is not set through this function.
     */
    static CARAPI GetItemsInLocalCoordinates(
        /* [in] */ IContext* context,
        /* [out] */ IArrayList** list);

    /**
     * Find a folder in the db, creating the FolderInfo if necessary, and adding it to folderList.
     */
    CARAPI GetFolderById(
        /* [in] */ IContext* context,
        /* [in] */ IHashMap* folderList,
        /* [in] */ Int64 id,
        /* [out] */ IFolderInfo** info);

    /**
     * Add an item to the database in a specified container. Sets the container, screen, cellX and
     * cellY fields of the item. Also assigns an ID to the item.
     */
    static CARAPI AddItemToDatabase(
        /* [in] */ IContext* context,
        /* [in] */ ItemInfo* item,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Boolean notify);

    /**
     * Creates a new unique child id, for a given cell span across all layouts.
     */
    static CARAPI GetCellLayoutChildId(
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 localCellX,
        /* [in] */ Int32 localCellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [out] */ Int32* id);

    static CARAPI GetCellCountX(
        /* [out] */ Int32* x);

    static CARAPI GetCellCountY(
        /* [out] */ Int32* y);
    /**
     * Updates the model orientation helper to take into account the current layout dimensions
     * when performing local/canonical coordinate transformations.
     */
    static CARAPI UpdateWorkspaceLayoutCells(
        /* [in] */ Int32 shortAxisCellCount,
        /* [in] */ Int32 longAxisCellCount);

    /**
     * Removes the specified item from the database
     * @param context
     * @param item
     */
    static CARAPI DeleteItemFromDatabase(
        /* [in] */ IContext* context,
        /* [in] */ ItemInfo* item);

    /**
     * Remove the contents of the specified folder from the database
     */
    static CARAPI DeleteFolderContentsFromDatabase(
        /* [in] */ IContext* context,
        /* [in] */ FolderInfo* info);

    /**
     * Set this as the current Launcher activity object for the loader.
     */
    // CARAPI Initialize(
    //     /* [in] */ ILauncherModelCallbacks* _callbacks);

    CARAPI GetLauncherAppsCallback(
        /* [out] */ ILauncherAppsCallback** _callback);

    /**
     * Call from the handler for ACTION_PACKAGE_ADDED, ACTION_PACKAGE_REMOVED and
     * ACTION_PACKAGE_CHANGED.
     */
    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI ForceReload();

    CARAPI ResetLoadedState(
        /* [in] */ Boolean resetAllAppsLoaded,
        /* [in] */ Boolean resetWorkspaceLoaded);

    /**
     * When the launcher is in the background, it's possible for it to miss paired
     * configuration changes.  So whenever we trigger the loader from the background
     * tell the launcher that it needs to re-run the loader when it comes back instead
     * of doing it now.
     */
    CARAPI StartLoaderFromBackground();

    CARAPI StartLoader(
        /* [in] */ Boolean isLaunching,
        /* [in] */ Int32 synchronousBindPage);

    CARAPI BindRemainingSynchronousPages();

    CARAPI StopLoader();

    CARAPI IsAllAppsLoaded(
        /* [out] */ Boolean* result);

    CARAPI IsLoadingWorkspace(
        /* [out] */ Boolean* result);

    CARAPI EnqueuePackageUpdated(
        /* [in] */ PackageUpdatedTask* task);

    // Returns a list of ResolveInfos/AppWindowInfos in sorted order
    static CARAPI GetSortedWidgetsAndShortcuts(
        /* [in] */ IContext* context,
        /* [out] */ IArrayList** list);

    /**
     * This is called from the code that adds shortcuts from the intent receiver.  This
     * doesn't have a Cursor, but
     */
    CARAPI GetShortcutInfo(
        /* [in] */ IPackageManager* manager,
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ IContext* context,
        /* [out] */ ShortcutInfo** sinfo);

    /**
     * Make an ShortcutInfo object for a shortcut that is an application.
     *
     * If c is not null, then it will be used to fill in missing data like the title and icon.
     */
    CARAPI GetShortcutInfo(
        /* [in] */ IPackageManager* manager,
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* c,
        /* [in] */ Int32 iconIndex,
        /* [in] */ Int32 titleIndex,
        /* [in] */ IHashMap* labelCache,
        /* [out] */ ShortcutInfo** sinfo);

    /**
     * Returns the set of workspace ShortcutInfos with the specified intent.
     */
    static CARAPI GetWorkspaceShortcutItemInfosWithIntent(
        /* [in] */ IIntent* intent,
        /* [out] */ IArrayList** list);

    CARAPI GetIconFromCursor(
        /* [in] */ ICursor* c,
        /* [in] */ Int32 iconIndex,
        /* [in] */ IContext* context,
        /* [out] */ IBitmap** map);

    CARAPI AddShortcut(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* data,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Boolean notify,
        /* [out] */ ShortcutInfo** sinfo);

    /**
     * Attempts to find an AppWidgetProviderInfo that matches the given component.
     */
    CARAPI FindAppWidgetProviderInfoWithComponent(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* component,
        /* [out] */ IAppWidgetProviderInfo** pinfo);

    /**
     * Returns a list of all the widgets that can handle configuration with a particular mimeType.
     */
    CARAPI ResolveWidgetsForMimeType(
        /* [in] */ IContext* context,
        /* [in] */ const String& mimeType,
        /* [out] */ IList** list);

    CARAPI InfoFromShortcutIntent(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* data,
        /* [in] */ IBitmap* fallbackIcon,
        /* [out] */ ShortcutInfo** sinfo);

    CARAPI QueueIconToBeChecked(
        /* [in] */ IHashMap* cache,
        /* [in] */ ShortcutInfo* info,
        /* [in] */ ICursor* c,
        /* [in] */ Int32 iconIndex,
        /* [out] */ Boolean* result);

    CARAPI UpdateSavedIcon(
        /* [in] */ IContext* context,
        /* [in] */ ShortcutInfo* info,
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI GetAppNameComparator(
        /* [out] */ IComparator** comparator);

    static CARAPI GetWidgetNameComparator(
        /* [out] */ IComparator** comparator);

    static CARAPI GetComponentNameFromResolveInfo(
        /* [in] */ IResolveInfo* info,
        /* [out] */ IComponentName** name);

    CARAPI DumpState();

private:
    static CARAPI_(Boolean) InitStaticBlock();

    /** Runs the specified runnable immediately if called from the main thread, otherwise it is
     * posted on the main thread handler. */
    CARAPI_(void) RunOnMainThread(
        /* [in] */ IRunnable* r);

    CARAPI_(void) RunOnMainThread(
        /* [in] */ IRunnable* r,
        /* [in] */ Int32 type);

    /** Runs the specified runnable immediately if called from the worker thread, otherwise it is
     * posted on the worker thread handler. */
    static CARAPI_(void) RunOnWorkerThread(
        /* [in] */ IRunnable* r);

    // If there is already a loader task running, tell it to stop.
    // returns true if isLaunching() was true on the old task
    CARAPI_(Boolean) StopLoaderLocked();

    /**
     * Make an ShortcutInfo object for a shortcut that isn't an application.
     */
    CARAPI_(AutoPtr<ShortcutInfo>) GetShortcutInfo(
        /* [in] */ ICursor* c,
        /* [in] */ IContext* context,
        /* [in] */ Int32 iconTypeIndex,
        /* [in] */ Int32 iconPackageIndex,
        /* [in] */ Int32 iconResourceIndex,
        /* [in] */ Int32 iconIndex,
        /* [in] */ Int32 titleIndex);

    /**
     * Return an existing FolderInfo object if we have encountered this ID previously,
     * or make a new one.
     */
    static CARAPI_(AutoPtr<FolderInfo>) FindOrMakeFolder(
        /* [in] */ IHashMap* folders,
        /* [in] */ Int64 id);

public:
    static AutoPtr<IComparator> APP_INSTALL_TIME_COMPARATOR;

protected:
    Int32 mPreviousConfigMcc;

private:
    static Boolean initStaticBlock;
    static const Boolean DEBUG_LOADERS;
    static const String TAG;

    static const Int32 ITEMS_CHUNK; // batch size for the workspace icons
    Int32 mBatchSize; // 0 is all apps at once
    Int32 mAllAppsLoadDelay; // milliseconds between batches

    Boolean mAppsCanBeOnRemoveableStorage;

    AutoPtr<ILauncherApplication> mApp;
    Object mLock;
    AutoPtr<DeferredHandler> mHandler;
    AutoPtr<LoaderTask> mLoaderTask;
    Boolean mIsLoaderTaskRunning;
    /* volatile */ Boolean mFlushingWorkerThread;

    // Specific runnable types that are run on the main thread deferred handler, this allows us to
    // clear all queued binding runnables when the Launcher activity is destroyed.
    static const Int32 MAIN_THREAD_NORMAL_RUNNABLE;
    static const Int32 MAIN_THREAD_BINDING_RUNNABLE;


    static AutoPtr<IHandlerThread> sWorkerThread;
    static AutoPtr<IHandler> sWorker;

    // We start off with everything not loaded.  After that, we assume that
    // our monitoring of the package manager provides all updates and we never
    // need to do a requery.  These are only ever touched from the loader thread.
    Boolean mWorkspaceLoaded;
    Boolean mAllAppsLoaded;

    // When we are loading pages synchronously, we can't just post the binding of items on the side
    // pages as this delays the rotation process.  Instead, we wait for a callback from the first
    // draw (in Workspace) to initiate the binding of the remaining side pages.  Any time we start
    // a normal load, we also clear this set of Runnables.
    static AutoPtr<IArrayList> mDeferredBindRunnables;

    AutoPtr<IWeakReference> mCallbacks;

    // < only access in worker thread >
    AutoPtr<AllAppsList> mBgAllAppsList;

    // The lock that must be acquired before referencing any static bg data structures.  Unlike
    // other locks, this one can generally be held long-term because we never expect any of these
    // static data structures to be referenced outside of the worker thread except on the first
    // load after configuration change.
    static Object sBgLock;

    // sBgItemsIdMap maps *all* the ItemInfos (shortcuts, folders, and widgets) created by
    // LauncherModel to their ids
    static AutoPtr<IHashMap> sBgItemsIdMap;

    // sBgWorkspaceItems is passed to bindItems, which expects a list of all folders and shortcuts
    //       created by LauncherModel that are directly on the home screen (however, no widgets or
    //       shortcuts within folders).
    static AutoPtr<IArrayList> sBgWorkspaceItems;

    // sBgAppWidgets is all LauncherAppWidgetInfo created by LauncherModel. Passed to bindAppWidget()
    static AutoPtr<IArrayList> sBgAppWidgets;

    // sBgFolders is all FolderInfos created by LauncherModel. Passed to bindFolders()
    static AutoPtr<IHashMap> sBgFolders;

    // sBgDbIconCache is the set of ItemInfos that need to have their icons updated in the database
    static AutoPtr<IHashMap> sBgDbIconCache;
    // </ only access in worker thread >

    AutoPtr<IconCache> mIconCache;
    AutoPtr<IBitmap> mDefaultIcon;

    static Int32 mCellCountX;
    static Int32 mCellCountY;

    AutoPtr<ILauncherApps> mLauncherApps;
    AutoPtr<IUserManager> mUserManager;
    AutoPtr<ILauncherAppsCallback> mLauncherAppsCallback;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERMODEL_H__