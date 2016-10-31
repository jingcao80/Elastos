
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/settings/notification/CNotificationAppList.h"
#include "elastos/droid/settings/notification/CAppNotificationSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/widget/ListView.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Utils;

using Elastos::Droid::Animation::CLayoutTransition;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::ILauncherActivityInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Notification::INotificationListenerServiceRanking;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Widget::ListView;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::EIID_ISectionIndexer;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Core::Character;
using Elastos::Core::CoreUtils;
using Elastos::Core::IChar32;
using Elastos::Core::CChar32;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::StringBuilder;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::CCollatorHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CNotificationAppList::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationAppList::InnerListener, Object, IAdapterViewOnItemSelectedListener)

CNotificationAppList::InnerListener::InnerListener(
    /* [in] */ CNotificationAppList* host)
    : mHost(host)
{}

ECode CNotificationAppList::InnerListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemSelected(parent, view, position, id);
}

ECode CNotificationAppList::InnerListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return mHost->OnNothingSelected(parent);
}

//===============================================================================
//                  CNotificationAppList::Row
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationAppList::Row, Object, IRow)

//===============================================================================
//                  CNotificationAppList::AppRow
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationAppList::AppRow, Row, IAppRow)

CNotificationAppList::AppRow::AppRow()
    : mUid(0)
    , mBanned(FALSE)
    , mPriority(FALSE)
    , mSensitive(FALSE)
    , mFirst(FALSE)
{}

//===============================================================================
//                  CNotificationAppList::Backend
//===============================================================================

AutoPtr<IINotificationManager> CNotificationAppList::Backend::sINM = IINotificationManager::Probe(
        ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));

Boolean CNotificationAppList::Backend::SetNotificationsBanned(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean banned)
{
    ECode ec = sINM->SetNotificationsEnabledForPackage(pkg, uid, !banned);
    if (SUCCEEDED(ec)) {
        return TRUE;
    }
    else {
        Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
        return FALSE;
    }
}

Boolean CNotificationAppList::Backend::GetNotificationsBanned(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
{
    Boolean enabled;
    ECode ec = sINM->AreNotificationsEnabledForPackage(pkg, uid, &enabled);
    if (SUCCEEDED(ec)) {
        return !enabled;
    }
    else {
        Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
        return FALSE;
    }
}

Boolean CNotificationAppList::Backend::GetHighPriority(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
{
    Int32 data;
    ECode ec = sINM->GetPackagePriority(pkg, uid, &data);
    if (SUCCEEDED(ec)) {
        return data == INotification::PRIORITY_MAX;
    }
    else {
        Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
        return FALSE;
    }
}

Boolean CNotificationAppList::Backend::SetHighPriority(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean highPriority)
{
    ECode ec = sINM->SetPackagePriority(pkg, uid,
                highPriority ? INotification::PRIORITY_MAX : INotification::PRIORITY_DEFAULT);
    if (SUCCEEDED(ec)) {
        return TRUE;
    }
    else {
        Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
        return FALSE;
    }
}

Boolean CNotificationAppList::Backend::GetSensitive(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
{
    Int32 data;
    ECode ec = sINM->GetPackageVisibilityOverride(pkg, uid, &data);
    if (SUCCEEDED(ec)) {
        return data == INotification::VISIBILITY_PRIVATE;
    }
    else {
        Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
        return FALSE;
    }
}

Boolean CNotificationAppList::Backend::SetSensitive(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean sensitive)
{
    ECode ec = sINM->SetPackageVisibilityOverride(pkg, uid,
                sensitive ? INotification::VISIBILITY_PRIVATE
                        : INotificationListenerServiceRanking::VISIBILITY_NO_OVERRIDE);
    if (SUCCEEDED(ec)) {
        return TRUE;
    }
    else {
        Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
        return FALSE;
    }
}

//===============================================================================
//                  CNotificationAppList::NotificationAppAdapter
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationAppList::NotificationAppAdapter, ArrayAdapter, ISectionIndexer)

ECode CNotificationAppList::NotificationAppAdapter::constructor(
    /* [in] */ CNotificationAppList* host,
    /* [in] */ IContext* context)
{
    mHost = host;
    return ArrayAdapter::constructor(context, 0, 0);
}

ECode CNotificationAppList::NotificationAppAdapter::HasStableIds(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    *has = TRUE;
    return NOERROR;
}

ECode CNotificationAppList::NotificationAppAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId)
    *itemId = position;
    return NOERROR;
}

ECode CNotificationAppList::NotificationAppAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 2;
    return NOERROR;
}

ECode CNotificationAppList::NotificationAppAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    IRow* r = IRow::Probe(obj);
    *type = IAppRow::Probe(r) != NULL ? 1 : 0;
    return NOERROR;
}

ECode CNotificationAppList::NotificationAppAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<IRow> r = IRow::Probe(obj);
    AutoPtr<IView> v;
    if (convertView == NULL) {
        v = NewView(parent, r);
    }
    else {
        v = convertView;
    }
    BindView(v, r, FALSE /*animate*/);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AutoPtr<IView> CNotificationAppList::NotificationAppAdapter::NewView(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IRow* r)
{
    if (IAppRow::Probe(r) == NULL) {
        AutoPtr<IView> result;
        mHost->mInflater->Inflate(R::layout::notification_app_section, parent, FALSE, (IView**)&result);
        return result;
    }

    AutoPtr<IView> v;
    mHost->mInflater->Inflate(R::layout::notification_app, parent, FALSE, (IView**)&v);
    AutoPtr<ViewHolder> vh = new ViewHolder();
    vh->mRow = IViewGroup::Probe(v);
    AutoPtr<ILayoutTransition> layoutTransition;
    CLayoutTransition::New((ILayoutTransition**)&layoutTransition);
    vh->mRow->SetLayoutTransition(layoutTransition);
    layoutTransition = NULL;
    CLayoutTransition::New((ILayoutTransition**)&layoutTransition);
    vh->mRow->SetLayoutTransition(layoutTransition);
    AutoPtr<IView> tmp;
    v->FindViewById(Elastos::Droid::R::id::icon, (IView**)&tmp);
    vh->mIcon = IImageView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
    vh->mTitle = ITextView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(Elastos::Droid::R::id::text1, (IView**)&tmp);
    vh->mSubtitle = ITextView::Probe(tmp);
    vh->mRowDivider = NULL;
    v->FindViewById(R::id::row_divider, (IView**)&(vh->mRowDivider));
    v->SetTag((IObject*)vh);
    return v;
}

void CNotificationAppList::NotificationAppAdapter::EnableLayoutTransitions(
    /* [in] */ IViewGroup* vg,
    /* [in] */ Boolean enabled)
{
    if (enabled) {
        AutoPtr<ILayoutTransition> transition;
        vg->GetLayoutTransition((ILayoutTransition**)&transition);
        transition->EnableTransitionType(ILayoutTransition::APPEARING);
        transition->EnableTransitionType(ILayoutTransition::DISAPPEARING);
    }
    else {
        AutoPtr<ILayoutTransition> transition;
        vg->GetLayoutTransition((ILayoutTransition**)&transition);
        transition->DisableTransitionType(ILayoutTransition::APPEARING);
        transition->DisableTransitionType(ILayoutTransition::DISAPPEARING);
    }
}

ECode CNotificationAppList::NotificationAppAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ IRow* r,
    /* [in] */ Boolean animate)
{
    if (IAppRow::Probe(r) == NULL) {
        // it's a section row
        AutoPtr<IView> tmp;
        view->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
        ITextView* tv = ITextView::Probe(tmp);
        tv->SetText(CoreUtils::Convert(((Row*)r)->mSection));
        return NOERROR;
    }

    AutoPtr<AppRow> row = (AppRow*)IAppRow::Probe(r);
    AutoPtr<IInterface> obj;
    view->GetTag((IInterface**)&obj);
    AutoPtr<ViewHolder> vh = (ViewHolder*)IObject::Probe(obj);
    EnableLayoutTransitions(vh->mRow, animate);
    vh->mRowDivider->SetVisibility(row->mFirst ? IView::GONE : IView::VISIBLE);
    AutoPtr<MyOnClickListener> listener = new MyOnClickListener(mHost, row);
    IView::Probe(vh->mRow)->SetOnClickListener(listener);
    EnableLayoutTransitions(vh->mRow, animate);
    vh->mIcon->SetImageDrawable(row->mIcon);
    vh->mTitle->SetText(row->mLabel);
    String sub = GetSubtitle(row);
    vh->mSubtitle->SetText(CoreUtils::Convert(sub));
    IView::Probe(vh->mSubtitle)->SetVisibility(!sub.IsEmpty() ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

String CNotificationAppList::NotificationAppAdapter::GetSubtitle(
    /* [in] */ AppRow* row)
{
    if (row->mBanned) {
        String str;
        mHost->mContext->GetString(R::string::app_notification_row_banned, &str);
        return str;
    }
    if (!row->mPriority && !row->mSensitive) {
        return EMPTY_SUBTITLE;
    }
    String priString;
    mHost->mContext->GetString(R::string::app_notification_row_priority, &priString);
    String senString;
    mHost->mContext->GetString(R::string::app_notification_row_sensitive, &senString);
    if (row->mPriority != row->mSensitive) {
        return row->mPriority ? priString : senString;
    }

    String str;
    mHost->mContext->GetString(R::string::summary_divider_text, &str);
    StringBuilder builder;
    builder += priString;
    builder += str;
    builder += senString;
    return  builder.ToString();
}

ECode CNotificationAppList::NotificationAppAdapter::GetSections(
    /* [out] */ ArrayOf<IInterface*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 size;
    mHost->mSections->GetSize(&size);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(size);
    return mHost->mSections->ToArray(args, result);
}

ECode CNotificationAppList::NotificationAppAdapter::GetPositionForSection(
    /* [in] */ Int32 sectionIndex,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mHost->mSections->Get(sectionIndex, (IInterface**)&obj);
    String section = Object::ToString(obj);
    Int32 n;
    GetCount(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> tmp;
        GetItem(i, (IInterface**)&tmp);
        Row* r = (Row*)IRow::Probe(tmp);
        if (r->mSection.Equals(section)) {
            *result = i;
            return NOERROR;
        }
    }
    *result = 0;
    return NOERROR;
}

ECode CNotificationAppList::NotificationAppAdapter::GetSectionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    Row* row = (Row*)IRow::Probe(obj);
    return mHost->mSections->IndexOf(CoreUtils::Convert(row->mSection), result);
}

//===============================================================================
//                  CNotificationAppList::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationAppList::MyOnClickListener, Object, IViewOnClickListener)

CNotificationAppList::MyOnClickListener::MyOnClickListener(
    /* [in] */ CNotificationAppList* host,
    /* [in] */ AppRow* row)
    : mHost(host)
    , mRow(row)
{}

ECode CNotificationAppList::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APP_NOTIFICATION_SETTINGS, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
    intent->PutExtra(ISettings::EXTRA_APP_PACKAGE, mRow->mPkg);
    intent->PutExtra(ISettings::EXTRA_APP_UID, mRow->mUid);
    intent->PutBooleanExtra(CAppNotificationSettings::EXTRA_HAS_SETTINGS_INTENT, mRow->mSettingsIntent != NULL);
    intent->PutExtra(CAppNotificationSettings::EXTRA_SETTINGS_INTENT, IParcelable::Probe(mRow->mSettingsIntent));
    mHost->mContext->StartActivity(intent);
    return NOERROR;
}

//===============================================================================
//                  CNotificationAppList::MyComparator
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationAppList::MyComparator, Object, IComparator)

CNotificationAppList::MyComparator::MyComparator()
{
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&mCollator);
}

ECode CNotificationAppList::MyComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AppRow* lhs = (AppRow*)IAppRow::Probe(_lhs);
    AppRow* rhs = (AppRow*)IAppRow::Probe(_rhs);
    return mCollator->Compare(lhs->mLabel, rhs->mLabel, result);
}

//===============================================================================
//                  CNotificationAppList::MyRunnable
//===============================================================================

CNotificationAppList::MyRunnable::MyRunnable(
    /* [in] */ Int32 id,
    /* [in] */ CNotificationAppList* host)
    : mId(id)
    , mHost(host)
{}

ECode CNotificationAppList::MyRunnable::Run()
{
    switch (mId) {
        case 0: {
            AutoLock syncLock(mHost->mRows);
            Int64 start = SystemClock::GetUptimeMillis();
            if (DEBUG) Logger::D(TAG, "Collecting apps...");
            mHost->mRows->Clear();
            mHost->mSortedRows->Clear();

            // collect all launchable apps, plus any packages that have notification settings
            AutoPtr<IList> appInfos; // List<ApplicationInfo>
            CArrayList::New((IList**)&appInfos);

            AutoPtr<IList> lais; // List<LauncherActivityInfo>
            mHost->mLauncherApps->GetActivityList(String(NULL) /* all */,
                        UserHandle::GetCallingUserHandle(), (IList**)&lais);
            if (DEBUG) Logger::D(TAG, "  launchable activities:");
            Int32 size;
            lais->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                lais->Get(i, (IInterface**)&obj);
                ILauncherActivityInfo* lai = ILauncherActivityInfo::Probe(obj);
                AutoPtr<IComponentName> comp;
                lai->GetComponentName((IComponentName**)&comp);
                if (DEBUG) Logger::D(TAG, "    %s", Object::ToString(comp).string());
                AutoPtr<IApplicationInfo> info;
                lai->GetApplicationInfo((IApplicationInfo**)&info);
                appInfos->Add(info);
            }

            AutoPtr<IList> /*List<ResolveInfo>*/ resolvedConfigActivities
                    = mHost->QueryNotificationConfigActivities(mHost->mPM);
            if (DEBUG) Logger::D(TAG, "  config activities:");
            resolvedConfigActivities->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                resolvedConfigActivities->Get(i, (IInterface**)&obj);
                IResolveInfo* ri = IResolveInfo::Probe(obj);
                AutoPtr<IActivityInfo> info;
                ri->GetActivityInfo((IActivityInfo**)&info);
                IPackageItemInfo* _info = IPackageItemInfo::Probe(info);
                String packageName, name;
                _info->GetPackageName(&packageName);
                _info->GetName(&name);
                if (DEBUG) Logger::D(TAG, "    %s/%s", packageName.string(), name.string());
                AutoPtr<IApplicationInfo> applicationInfo;
                IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
                appInfos->Add(applicationInfo);
            }

            Boolean res;
            appInfos->GetSize(&size);
            for (Int32 i =0; i < size; i++) {
                AutoPtr<IInterface> obj;
                appInfos->Get(i, (IInterface**)&obj);
                IApplicationInfo* info = IApplicationInfo::Probe(obj);
                String key;
                IPackageItemInfo::Probe(info)->GetPackageName(&key);
                if (mHost->mRows->ContainsKey(CoreUtils::Convert(key), &res), res) {
                    // we already have this app, thanks
                    continue;
                }

                AutoPtr<AppRow> row = mHost->LoadAppRow(mHost->mPM, info, mHost->mBackend);
                mHost->mRows->Put(CoreUtils::Convert(key), (IAppRow*)row);
            }

            // add config activities to the list
            mHost->ApplyConfigActivities(mHost->mPM, mHost->mRows, resolvedConfigActivities);

            // sort rows
            AutoPtr<ICollection> coll;
            mHost->mRows->GetValues((ICollection**)&coll);
            mHost->mSortedRows->AddAll(coll);
            AutoPtr<ICollections> collections;
            CCollections::AcquireSingleton((ICollections**)&collections);
            collections->Sort(IList::Probe(mHost->mSortedRows), mHost->mRowComparator);
            // compute sections
            mHost->mSections->Clear();
            String section;
            mHost->mSortedRows->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mHost->mSortedRows->Get(i, (IInterface**)&obj);
                AppRow* r = (AppRow*)IAppRow::Probe(obj);
                r->mSection = mHost->GetSection(r->mLabel);
                if (!r->mSection.Equals(section)) {
                    section = r->mSection;
                    mHost->mSections->Add(CoreUtils::Convert(section));
                }
            }
            mHost->mHandler->Post(mHost->mRefreshAppsListRunnable, &res);
            Int64 elapsed = SystemClock::GetUptimeMillis() - start;
            mHost->mRows->GetSize(&size);
            if (DEBUG) Logger::D(TAG, "Collected %d apps in %dms", size, elapsed);
        }
            break;
        case 1:
            mHost->RefreshDisplayedItems();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CNotificationAppList
//===============================================================================

const String CNotificationAppList::TAG("CNotificationAppList");
const Boolean CNotificationAppList::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);

const String CNotificationAppList::EMPTY_SUBTITLE("");
const String CNotificationAppList::SECTION_BEFORE_A("*");
const String CNotificationAppList::SECTION_AFTER_Z("**");

static AutoPtr<IIntent> InitAPP_NOTIFICATION_PREFS_CATEGORY_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(INotification::INTENT_CATEGORY_NOTIFICATION_PREFERENCES);
    return intent;
}

const AutoPtr<IIntent> CNotificationAppList::APP_NOTIFICATION_PREFS_CATEGORY_INTENT = InitAPP_NOTIFICATION_PREFS_CATEGORY_INTENT();

const AutoPtr<IComparator> CNotificationAppList::mRowComparator = new MyComparator();

CAR_OBJECT_IMPL(CNotificationAppList);

CNotificationAppList::CNotificationAppList()
    : mContext(NULL)
{}

CNotificationAppList::~CNotificationAppList()
{}

ECode CNotificationAppList::constructor()
{
    CHandler::New((IHandler**)&mHandler);
    CArrayMap::New((IArrayMap**)&mRows);
    CArrayList::New((IArrayList**)&mSortedRows);
    CArrayList::New((IArrayList**)&mSections);
    mBackend = new Backend();
    mCollectAppsRunnable = new MyRunnable(0, this);
    mRefreshAppsListRunnable = new MyRunnable(1, this);

    return PinnedHeaderListFragment::constructor();
}

ECode CNotificationAppList::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    PinnedHeaderListFragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mContext = IContext::Probe(activity);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    mAdapter = new NotificationAppAdapter();
    mAdapter->constructor(this, mContext);
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    helper->Get(mContext, (IUserManager**)&mUM);
    mContext->GetPackageManager((IPackageManager**)&mPM);
    obj = NULL;
    mContext->GetSystemService(IContext::LAUNCHER_APPS_SERVICE, (IInterface**)&obj);
    mLauncherApps = ILauncherApps::Probe(obj);
    activity->SetTitle(R::string::app_notifications_title);
    return NOERROR;
}

ECode CNotificationAppList::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    return inflater->Inflate(R::layout::notification_app_list, container, FALSE, result);
}

ECode CNotificationAppList::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    PinnedHeaderListFragment::OnViewCreated(view, savedInstanceState);
    mProfileSpinnerAdapter = Utils::CreateUserSpinnerAdapter(mUM, mContext);
    if (mProfileSpinnerAdapter != NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<ILayoutInflater> inflater;
        activity->GetLayoutInflater((ILayoutInflater**)&inflater);
        AutoPtr<IView> tmp;
        inflater->Inflate(R::layout::spinner_view, NULL, (IView**)&tmp);
        // ISpinner* spinner = ISpinner::Probe(tmp);
        IAdapterView* spinner = IAdapterView::Probe(tmp);
        spinner->SetAdapter((IAdapter*)mProfileSpinnerAdapter);
        AutoPtr<InnerListener> listener = new InnerListener(this);
        spinner->SetOnItemSelectedListener(listener);
        SetPinnedHeaderView(tmp);
    }
    return NOERROR;
}

ECode CNotificationAppList::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    PinnedHeaderListFragment::OnActivityCreated(savedInstanceState);
    RepositionScrollbar();
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IAdapterView::Probe(listView)->SetAdapter((IAdapter*)mAdapter);
    return NOERROR;
}

ECode CNotificationAppList::OnPause()
{
    PinnedHeaderListFragment::OnPause();
    if (DEBUG) Logger::D(TAG, "Saving listView state");
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    mListViewState = ((ListView*)listView.Get())->OnSaveInstanceState();
    return NOERROR;
}

ECode CNotificationAppList::OnDestroyView()
{
    PinnedHeaderListFragment::OnDestroyView();
    mListViewState = NULL;  // you're dead to me
    return NOERROR;
}

ECode CNotificationAppList::OnResume()
{
    PinnedHeaderListFragment::OnResume();
    LoadAppsList();
    return NOERROR;
}

ECode CNotificationAppList::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IUserHandle> selectedUser = mProfileSpinnerAdapter->GetUserHandle(position);
    Int32 identifier;
    selectedUser->GetIdentifier(&identifier);
    if (identifier != UserHandle::GetMyUserId()) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IIntent> intent;
        CIntent::New(IContext::Probe(activity), ECLSID_CSettingsNotificationAppListActivity, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
        mContext->StartActivityAsUser(intent, selectedUser);
    }
    return NOERROR;
}

ECode CNotificationAppList::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return NOERROR;
}

ECode CNotificationAppList::SetBackend(
    /* [in] */ Backend* backend)
{
    mBackend = backend;
    return NOERROR;
}

void CNotificationAppList::LoadAppsList()
{
    AsyncTask::Execute(mCollectAppsRunnable);
}

String CNotificationAppList::GetSection(
    /* [in] */ ICharSequence* label)
{
    Int32 length;
    if (label == NULL || (label->GetLength(&length), length) == 0) return SECTION_BEFORE_A;
    Char32 ca;
    label->GetCharAt(0, &ca);
    Char32 c = Character::ToUpperCase(ca);
    if (c < 'A') return SECTION_BEFORE_A;
    if (c > 'Z') return SECTION_AFTER_Z;

    // return Character::ToString(c);
    AutoPtr<IChar32> ch;
    CChar32::New(c, (IChar32**)&ch);
    return Object::ToString(ch);
}

void CNotificationAppList::RepositionScrollbar()
{
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IView* _listView = IView::Probe(listView);
    Int32 size;
    _listView->GetScrollBarSize(&size);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    AutoPtr<ITypedValueHelper> helper;
    CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&helper);
    Float data;
    helper->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP, size, metrics, &data);
    Int32 sbWidthPx = (Int32) data;
    AutoPtr<IView> view;
    GetView((IView**)&view);
    AutoPtr<IViewParent> _parent;
    view->GetParent((IViewParent**)&_parent);
    IView* parent = IView::Probe(_parent);
    Int32 end;
    parent->GetPaddingEnd(&end);
    const Int32 eat = Elastos::Core::Math::Min(sbWidthPx, end);
    if (eat <= 0) return;
    Int32 direction;
    _listView->GetLayoutDirection(&direction);

    StringBuilder builder("");
    builder += "Eating ";
    builder += eat;
    builder += "px into ";
    builder += end;
    builder += "px padding for ";
    builder += sbWidthPx;
    builder += "px scroll, ld=";
    builder += direction;
    if (DEBUG) Logger::D(TAG, builder.ToString());

    Int32 top, bottom, start;
    parent->GetPaddingStart(&start);
    parent->GetPaddingTop(&top);
    parent->GetPaddingBottom(&bottom);
    parent->SetPaddingRelative(start, top, end - eat, bottom);
}

AutoPtr<CNotificationAppList::AppRow> CNotificationAppList::LoadAppRow(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IApplicationInfo* _app,
    /* [in] */ Backend* backend)
{
    AutoPtr<AppRow> row = new AppRow();
    IPackageItemInfo* app = IPackageItemInfo::Probe(_app);
    app->GetPackageName(&row->mPkg);
    _app->GetUid(&row->mUid);
    ECode ec = app->LoadLabel(pm, (ICharSequence**)&row->mLabel);
    if (FAILED(ec)) {
        Logger::E(TAG, "Error loading application label for %s , 0x%08x", row->mPkg.string(), ec);
        row->mLabel = CoreUtils::Convert(row->mPkg);
    }

    app->LoadIcon(pm, (IDrawable**)&row->mIcon);
    row->mBanned = backend->GetNotificationsBanned(row->mPkg, row->mUid);
    row->mPriority = backend->GetHighPriority(row->mPkg, row->mUid);
    row->mSensitive = backend->GetSensitive(row->mPkg, row->mUid);
    return row;
}

AutoPtr<IList> CNotificationAppList::QueryNotificationConfigActivities(
    /* [in] */ IPackageManager* pm)
{
    if (DEBUG) Logger::D(TAG, "APP_NOTIFICATION_PREFS_CATEGORY_INTENT is %s",
            TO_CSTR(APP_NOTIFICATION_PREFS_CATEGORY_INTENT));
    AutoPtr<IList> resolveInfos;
    pm->QueryIntentActivities(
            APP_NOTIFICATION_PREFS_CATEGORY_INTENT,
            0 /*PackageManager.MATCH_DEFAULT_ONLY*/, (IList**)&resolveInfos);
    return resolveInfos;
}

void CNotificationAppList::CollectConfigActivities(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IArrayMap* rows)
{
    AutoPtr<IList> resolveInfos = QueryNotificationConfigActivities(pm);
    ApplyConfigActivities(pm, rows, resolveInfos);
}

void CNotificationAppList::ApplyConfigActivities(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IArrayMap* rows,
    /* [in] */ IList* resolveInfos)
{
    Int32 size;
    resolveInfos->GetSize(&size);
    if (DEBUG) {
        StringBuilder builder;
        builder += "Found ";
        builder += size;
        builder += " preference activities";
        builder += size == 0 ? " ;_;" : "";
        Logger::D(TAG, builder.ToString());
    }

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        resolveInfos->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(obj);

        AutoPtr<IActivityInfo> activityInfo;
        ri->GetActivityInfo((IActivityInfo**)&activityInfo);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String appInfoPkgName;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&appInfoPkgName);
        AutoPtr<IInterface> rowObj;
        rows->Get(CoreUtils::Convert(appInfoPkgName), (IInterface**)&rowObj);
        AppRow* row = (AppRow*)IAppRow::Probe(rowObj);

        String name, packageName;
        IPackageItemInfo::Probe(activityInfo)->GetName(&name);
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
        if (row == NULL) {
            Logger::V(TAG, "Ignoring notification preference activity (%s) for unknown package %s",
                    name.string(), packageName.string());
            continue;
        }
        if (row->mSettingsIntent != NULL) {
            Logger::V(TAG, "Ignoring duplicate notification preference activity (%s) for package %s",
                    name.string(), packageName.string());
            continue;
        }
        CIntent::New(APP_NOTIFICATION_PREFS_CATEGORY_INTENT, (IIntent**)&row->mSettingsIntent);
        row->mSettingsIntent->SetClassName(packageName, name);
    }
}

void CNotificationAppList::RefreshDisplayedItems()
{
    if (DEBUG) Logger::D(TAG, "Refreshing apps...");
    mAdapter->Clear();
    {
        AutoLock syncLock(mSortedRows);
        String section;
        Int32 N;
        mSortedRows->GetSize(&N);
        Boolean first = TRUE;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mSortedRows->Get(i, (IInterface**)&obj);
            IAppRow* rowObj = IAppRow::Probe(obj);
            AppRow* row = (AppRow*)rowObj;
            if (!row->mSection.Equals(section)) {
                section = row->mSection;
                AutoPtr<Row> r = new Row();
                r->mSection = section;
                mAdapter->Add((IRow*)r);
                first = TRUE;
            }
            row->mFirst = first;
            mAdapter->Add(rowObj);
            first = FALSE;
        }
    }
    if (mListViewState != NULL) {
        if (DEBUG) Logger::D(TAG, "Restoring listView state");
        AutoPtr<IListView> listView;
        GetListView((IListView**)&listView);
        ((ListView*)listView.Get())->OnRestoreInstanceState(mListViewState);
        mListViewState = NULL;
    }
    Int32 size;
    mSortedRows->GetSize(&size);
    if (DEBUG) Logger::D(TAG, "Refreshed %d displayed items", size);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos