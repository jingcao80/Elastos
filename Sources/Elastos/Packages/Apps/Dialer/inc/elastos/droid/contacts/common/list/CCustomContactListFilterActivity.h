
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCUSTOMCONTACTLISTFILTERACTIVITY_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCUSTOMCONTACTLISTFILTERACTIVITY_H__

#include "_Elastos_Droid_Contacts_Common_List_CCustomContactListFilterActivity.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/contacts/common/util/WeakAsyncTask.h"
#include "elastos/droid/content/AsyncTaskLoader.h"
#include "elastos/droid/widget/BaseExpandableListAdapter.h"
#include <elastos/utility/ArrayList.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::ILoader;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::App::ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Contacts::Common::Model::IAccountTypeManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::IExpandableListViewOnChildClickListener;
using Elastos::Droid::Widget::IExpandableListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::BaseExpandableListAdapter;
using Elastos::Core::IComparator;
using Elastos::Core::ICharSequence;
using Elastos::Utility::ArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

CarClass(CCustomContactListFilterActivity)
    , public Activity
    , public ICustomContactListFilterActivity
    , public IViewOnClickListener
    , public IExpandableListViewOnChildClickListener
    , public ILoaderManagerLoaderCallbacks
{
protected:
    /**
     * Set of all {@link AccountDisplay} entries, one for each source.
     */
    class AccountSet
        : public ArrayList
    {
    public:
        CARAPI_(AutoPtr<IArrayList>) BuildDiff();
    };

    /**
     * {@link GroupDelta} details for a single {@link AccountWithDataSet}, usually shown as
     * children under a single expandable group.
     */
    class AccountDisplay
        : public Object
    {
    public:
        /**
         * Build an {@link AccountDisplay} covering all {@link Groups} under the
         * given {@link AccountWithDataSet}.
         */
        AccountDisplay(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ const String& accountName,
            /* [in] */ const String& accountType,
            /* [in] */ const String& dataSet);

        /**
         * Set the {@link GroupDelta#putShouldSync(boolean)} value for all
         * children {@link GroupDelta} rows.
         */
        CARAPI_(void) SetShouldSync(
            /* [in] */ Boolean shouldSync);

        CARAPI_(void) SetShouldSync(
            /* [in] */ IGroupDelta* child,
            /* [in] */ Boolean shouldSync);

        /**
         * Set {@link GroupDelta#putShouldSync(boolean)}, and file internally
         * based on updated state.
         */
        CARAPI_(void) SetShouldSync(
            /* [in] */ IGroupDelta* child,
            /* [in] */ Boolean shouldSync,
            /* [in] */ Boolean attemptRemove);

        /**
         * Build set of {@link ContentProviderOperation} to persist any user
         * changes to {@link GroupDelta} rows under this {@link AccountWithDataSet}.
         */
        CARAPI_(void) BuildDiff(
            /* [in] */ IArrayList* diff);

    private:
        /**
         * Add the given {@link GroupDelta} internally, filing based on its
         * {@link GroupDelta#getShouldSync()} status.
         */
        CARAPI_(void) AddGroup(
            /* [in] */ IGroupDelta* group);

    public:
        String mName;
        String mType;
        String mDataSet;

        AutoPtr<IGroupDelta> mUngrouped;
        List<AutoPtr<IGroupDelta> > mSyncedGroups;
        List<AutoPtr<IGroupDelta> > mUnsyncedGroups;
    };

    /**
     * {@link ExpandableListAdapter} that shows {@link GroupDelta} settings,
     * grouped by {@link AccountWithDataSet} type. Shows footer row when any groups are
     * unsynced, as determined through {@link AccountDisplay#mUnsyncedGroups}.
     */
    class DisplayAdapter
        : public BaseExpandableListAdapter
    {
    public:
        DisplayAdapter(
            /* [in] */ IContext* context);

        CARAPI_(void) SetAccounts(
            /* [in] */ AccountSet* accounts);

        /**
         * In group descriptions, show the number of contacts with phone
         * numbers, in addition to the total contacts.
         */
        CARAPI_(void) SetChildDescripWithPhones(
            /* [in] */ Boolean withPhones);

        // @Override
        CARAPI GetGroupView(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Boolean isExpanded,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        // @Override
        CARAPI GetChildView(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [in] */ Boolean isLastChild,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        // @Override
        CARAPI GetChild(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [out] */ IInterface** child);

        // @Override
        CARAPI GetChildId(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [out] */ Int64* id);

        // @Override
        CARAPI GetChildrenCount(
            /* [in] */ Int32 groupPosition,
            /* [out] */ Int32* count);

        // @Override
        CARAPI GetGroup(
            /* [in] */ Int32 groupPosition,
            /* [out] */ IInterface** child);

        // @Override
        CARAPI GetGroupCount(
            /* [out] */ Int32* count);

        // @Override
        CARAPI GetGroupId(
            /* [in] */ Int32 groupPosition,
            /* [out] */ Int64* id);

        // @Override
        CARAPI HasStableIds(
            /* [out] */ Boolean* id);

        // @Override
        CARAPI IsChildSelectable(
            /* [in] */ Int32 groupPosition,
            /* [in] */ Int32 childPosition,
            /* [out] */ Boolean* selectable);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<ILayoutInflater> mInflater;
        AutoPtr<IAccountTypeManager> mAccountTypes;
        AutoPtr<AccountSet> mAccounts;

        Boolean mChildWithPhones;
    };

public:
    class CustomFilterConfigurationLoader
        : public AsyncTaskLoader
    {
    public:
        CustomFilterConfigurationLoader(
            /* [in] */ IContext* context);

        // @Override
        CARAPI LoadInBackground(
            /* [out] */ IInterface** result);

        // @Override
        CARAPI DeliverResult(
            /* [in] */ IInterface* data);

    protected:
        // @Override
        CARAPI OnStartLoading();

        // @Override
        CARAPI OnStopLoading();

        // @Override
        CARAPI OnReset();

    private:
        AutoPtr<AccountSet> mAccountSet;
    };

    /**
     * Background task that persists changes to {@link Groups#GROUP_VISIBLE},
     * showing spinner dialog to user while updating.
     */
    class UpdateTask
        : public WeakAsyncTask
    {
    public:
        UpdateTask(
            /* [in] */ IActivity* target);

    protected:
        /** {@inheritDoc} */
        // @Override
        CARAPI OnPreExecute(
            /* [in] */ IInterface* target);

        /** {@inheritDoc} */
        // @Override
        CARAPI DoInBackground(
            /* [in] */ IInterface* target,
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        /** {@inheritDoc} */
        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* target,
            /* [in] */ IInterface* result);

    private:
        AutoPtr<IProgressDialog> mProgress;
    };

private:
    class IdComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */  IInterface* a,
            /* [in] */  IInterface* b,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnCreateLoader(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args,
        /* [out] */ ILoader** loader);

    // @Override
    CARAPI OnLoadFinished(
        /* [in] */ ILoader* loader,
        /* [in] */ IInterface* data);

    // @Override
    CARAPI OnLoaderReset(
        /* [in] */ ILoader* loader);

    CARAPI OnClick(
        /* [in] */ IView* view);

    // @Override
    CARAPI OnChildClick(
        /* [in] */ IExpandableListView* parent,
        /* [in] */ IView* v,
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [in] */ Int64 id,
        /* [out] */ Boolean* handle);

    // @Override
    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI_(Int32) GetSyncMode(
        /* [in] */ AccountDisplay* account);

    CARAPI_(void) SshowRemoveSync(
        /* [in] */ IContextMenu* menu,
        /* [in] */ AccountDisplay* account,
        /* [in] */ IGroupDelta* child,
        /* [in] */ Int32 syncMode);

    CARAPI_(void) HandleRemoveSync(
        /* [in] */ AccountDisplay* account,
        /* [in] */ IGroupDelta* child,
        /* [in] */ Int32 syncMode,
        /* [in] */ ICharSequence* title);

    CARAPI_(void) ShowAddSync(
        /* [in] */ IContextMenu* menu,
        /* [in] */ AccountDisplay* account,
        /* [in] */ Int32 syncMode);

private:
    static CARAPI_(AutoPtr<IUri>) AddCallerIsSyncAdapterParameter(
        /* [in] */ IUri* uri);

    CARAPI_(void) DoSaveAction();

private:
    static const String TAG;

    static const Int32 ACCOUNT_SET_LOADER_ID = 1;

    static const Int32 DEFAULT_SHOULD_SYNC = 1;
    static const Int32 DEFAULT_VISIBLE = 0;

    // TODO: move these definitions to framework constants when we begin
    // defining this mode through <sync-adapter> tags
    static const Int32 SYNC_MODE_UNSUPPORTED = 0;
    static const Int32 SYNC_MODE_UNGROUPED = 1;
    static const Int32 SYNC_MODE_EVERYTHING = 2;

    AutoPtr<IExpandableListView> mList;
    AutoPtr<DisplayAdapter> mAdapter;

    AutoPtr<ISharedPreferences> mPrefs;

    /**
     * {@link Comparator} to sort by {@link Groups#_ID}.
     */
    static AutoPtr<IComparator> sIdComparator;// = new Comparator<GroupDelta>()
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CCUSTOMCONTACTLISTFILTERACTIVITY_H__
