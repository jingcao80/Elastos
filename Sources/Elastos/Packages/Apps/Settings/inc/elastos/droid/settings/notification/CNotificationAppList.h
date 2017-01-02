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

#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONAPPLIST_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONAPPLIST_H__

#include "Elastos.CoreLibrary.Text.h"
#include "_Elastos_Droid_Settings_Notification_CNotificationAppList.h"
#include "elastos/droid/settings/CUserSpinnerAdapter.h"
#include "elastos/droid/settings/PinnedHeaderListFragment.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::Settings::PinnedHeaderListFragment;
using Elastos::Droid::Settings::CUserSpinnerAdapter;

using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::ILauncherApps;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISectionIndexer;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::IComparator;
using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

/** Just a sectioned list of installed applications, nothing else to index **/
CarClass(CNotificationAppList)
    , public PinnedHeaderListFragment
{
public:
    class InnerListener
        : public Object
        , public IAdapterViewOnItemSelectedListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CNotificationAppList::InnerListener")

        InnerListener(
            /* [in] */ CNotificationAppList* host);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        //@Override
        CARAPI OnNothingSelected(
            /* [in] */ IAdapterView* parent);

    private:
        CNotificationAppList* mHost;
    };

    class Row
        : public Object
        , public IRow
    {
    public:
        TO_STRING_IMPL("CNotificationAppList::Row")

        CAR_INTERFACE_DECL()

    public:
        String mSection;
    };

    class AppRow
        : public Row
        , public IAppRow
    {
    public:
        TO_STRING_IMPL("CNotificationAppList::AppRow")

        CAR_INTERFACE_DECL()

        AppRow();

    public:
        String mPkg;
        Int32 mUid;
        AutoPtr<IDrawable> mIcon;
        AutoPtr<ICharSequence> mLabel;
        AutoPtr<IIntent> mSettingsIntent;
        Boolean mBanned;
        Boolean mPriority;
        Boolean mSensitive;
        Boolean mFirst;  // first app in section
    };

    class Backend
        : public Object
    {
    public:
        TO_STRING_IMPL("CNotificationAppList::Backend")

        virtual CARAPI_(Boolean) SetNotificationsBanned(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean banned);

        virtual CARAPI_(Boolean) GetNotificationsBanned(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid);

        virtual CARAPI_(Boolean) GetHighPriority(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid);

        virtual CARAPI_(Boolean) SetHighPriority(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean highPriority);

        virtual CARAPI_(Boolean) GetSensitive(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid);

        virtual CARAPI_(Boolean) SetSensitive(
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean sensitive);

    public:
        static AutoPtr<IINotificationManager> sINM;
    };

private:
    class ViewHolder
        : public Object
    {
    public:
        AutoPtr<IViewGroup> mRow;
        AutoPtr<IImageView> mIcon;
        AutoPtr<ITextView> mTitle;
        AutoPtr<ITextView> mSubtitle;
        AutoPtr<IView> mRowDivider;
    };

    class NotificationAppAdapter
        : public ArrayAdapter
        , public ISectionIndexer
    {
    public:
        TO_STRING_IMPL("CNotificationAppList::NotificationAppAdapter")

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ CNotificationAppList* host,
            /* [in] */ IContext* context);

        //@Override
        CARAPI HasStableIds(
            /* [out] */ Boolean* has);

        //@Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        //@Override
        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        //@Override
        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* type);

        virtual CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        virtual CARAPI_(AutoPtr<IView>) NewView(
            /* [in] */ IViewGroup* parent,
            /* [in] */ IRow* r);

        virtual CARAPI BindView(
            /* [in] */ IView* view,
            /* [in] */ IRow* r,
            /* [in] */ Boolean animate);

        //@Override
        CARAPI GetSections(
            /* [out] */ ArrayOf<IInterface*>** result);

        //@Override
        CARAPI GetPositionForSection(
            /* [in] */ Int32 sectionIndex,
            /* [out] */ Int32* result);

        //@Override
        CARAPI GetSectionForPosition(
            /* [in] */ Int32 position,
            /* [out] */ Int32* result);

    private:
        CARAPI_(void) EnableLayoutTransitions(
            /* [in] */ IViewGroup* vg,
            /* [in] */ Boolean enabled);

        CARAPI_(String) GetSubtitle(
            /* [in] */ AppRow* row);

    private:
        CNotificationAppList* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CNotificationAppList::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CNotificationAppList* host,
            /* [in] */ AppRow* row);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CNotificationAppList* mHost;
        AutoPtr<AppRow> mRow;
    };

    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        TO_STRING_IMPL("CNotificationAppList::MyComparator")

        CAR_INTERFACE_DECL()

        MyComparator();

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> mCollator;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CNotificationAppList::MyRunnable")

        MyRunnable(
            /* [in] */ Int32 id,
            /* [in] */ CNotificationAppList* host);

        //@Override
        CARAPI Run();

    private:
        Int32 mId;
        CNotificationAppList* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CNotificationAppList();

    ~CNotificationAppList();

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
    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //@Override
    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

    virtual CARAPI SetBackend(
        /* [in] */ Backend* backend);

    static CARAPI_(AutoPtr<AppRow>) LoadAppRow(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IApplicationInfo* app,
        /* [in] */ Backend* backend);

    static CARAPI_(AutoPtr<IList>) QueryNotificationConfigActivities(
        /* [in] */ IPackageManager* pm);

    static CARAPI_(void) CollectConfigActivities(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IArrayMap* rows); //ArrayMap<String, AppRow>

    static CARAPI_(void) ApplyConfigActivities(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IArrayMap* rows, //ArrayMap<String, AppRow>
        /* [in] */ IList* resolveInfos); //List<ResolveInfo>

private:
    CARAPI_(void) LoadAppsList();

    CARAPI_(String) GetSection(
        /* [in] */ ICharSequence* label);

    CARAPI_(void) RepositionScrollbar();

    CARAPI_(void) RefreshDisplayedItems();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String EMPTY_SUBTITLE;
    static const String SECTION_BEFORE_A;
    static const String SECTION_AFTER_Z;
    static const AutoPtr<IIntent> APP_NOTIFICATION_PREFS_CATEGORY_INTENT;

    static const AutoPtr<IComparator> mRowComparator;

    AutoPtr<IHandler> mHandler;
    // ArrayMap<String, AppRow> mRows = new ArrayMap<String, AppRow>();
    AutoPtr<IArrayMap> mRows;
    // ArrayList<AppRow> mSortedRows = new ArrayList<AppRow>();
    AutoPtr<IArrayList> mSortedRows;
    // ArrayList<String> mSections = new ArrayList<String>();
    AutoPtr<IArrayList> mSections;

    IContext* mContext;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<NotificationAppAdapter> mAdapter;
    AutoPtr< ArrayOf<ISignature*> > mSystemSignature;
    AutoPtr<IParcelable> mListViewState;
    AutoPtr<Backend> mBackend;
    AutoPtr<CUserSpinnerAdapter> mProfileSpinnerAdapter;

    AutoPtr<IPackageManager> mPM;
    AutoPtr<IUserManager> mUM;
    AutoPtr<ILauncherApps> mLauncherApps;

    AutoPtr<Runnable> mCollectAppsRunnable;
    AutoPtr<Runnable> mRefreshAppsListRunnable;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONAPPLIST_H__