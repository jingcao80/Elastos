
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/notification/CNotificationStation.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/ServiceManager.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Utils;

using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IDateTimeView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CNotificationStation::NotificationHistoryAdapter
//===============================================================================

CNotificationStation::NotificationHistoryAdapter::NotificationHistoryAdapter(
    /* [in] */ CNotificationStation* host)
    : mHost(host)
{}

ECode CNotificationStation::NotificationHistoryAdapter::constructor(
    /* [in] */ IContext* context)
{
    ArrayAdapter::constructor(context, 0);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    return NOERROR;
}

ECode CNotificationStation::NotificationHistoryAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    HistoricalNotificationInfo* info = (HistoricalNotificationInfo*)IObject::Probe(obj);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, CoreUtils::Convert(info->mPkg));
    args->Set(1, info->mTitle);
    Logd(String("GetView(%s/%s)"), args);

    AutoPtr<IView> row;
    if (convertView != NULL) {
        row = convertView;
    }
    else {
        row = CreateRow(parent);
    }
    row->SetTag((IObject*)info);

    // bind mIcon
    if (info->mIcon != NULL) {
        AutoPtr<IView> tmp;
        row->FindViewById(Elastos::Droid::R::id::icon, (IView**)&tmp);
        IImageView::Probe(tmp)->SetImageDrawable(info->mIcon);
    }
    if (info->mPkgicon != NULL) {
        AutoPtr<IView> tmp;
        row->FindViewById(R::id::pkgicon, (IView**)&tmp);
        IImageView::Probe(tmp)->SetImageDrawable(info->mPkgicon);
    }

    AutoPtr<IView> tmp;
    row->FindViewById(R::id::timestamp, (IView**)&tmp);
    IDateTimeView::Probe(tmp)->SetTime(info->mTimestamp);
    tmp = NULL;
    row->FindViewById(Elastos::Droid::R::id::title, (IView**)&tmp);
    ITextView::Probe(tmp)->SetText(info->mTitle);
    tmp = NULL;
    row->FindViewById(R::id::pkgname, (IView**)&tmp);
    ITextView::Probe(tmp)->SetText(info->mPkgname);

    tmp = NULL;
    row->FindViewById(R::id::extra, (IView**)&tmp);
    tmp->SetVisibility(IView::GONE);
    row->SetAlpha(info->mActive ? 1.0f : 0.5f);

    // set up click handler
    AutoPtr<MyOnClickListener> listener = new MyOnClickListener(mHost, info);
    row->SetOnClickListener(listener);

    *result = row;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IView> CNotificationStation::NotificationHistoryAdapter::CreateRow(
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    mInflater->Inflate(R::layout::notification_log_row, parent, FALSE, (IView**)&view);
    return view;
}

//===============================================================================
//                  CNotificationStation::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationStation::MyOnClickListener, Object, IViewOnClickListener)

CNotificationStation::MyOnClickListener::MyOnClickListener(
    /* [in] */ CNotificationStation* host,
    /* [in] */ HistoricalNotificationInfo* info)
    : mHost(host)
    , mInfo(info)
{}

ECode CNotificationStation::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    v->SetPressed(TRUE);
    mHost->StartApplicationDetailsActivity(mInfo->mPkg);
    return NOERROR;
}

//===============================================================================
//                  CNotificationStation::MyRunnable
//===============================================================================

CNotificationStation::MyRunnable::MyRunnable(
    /* [in] */ CNotificationStation* host)
    : mHost(host)
{}

ECode CNotificationStation::MyRunnable::Run()
{
    mHost->RefreshList();
    return NOERROR;
}

//===============================================================================
//                  CNotificationStation::MyNotificationListenerService
//===============================================================================

CNotificationStation::MyNotificationListenerService::MyNotificationListenerService()
{}

ECode CNotificationStation::MyNotificationListenerService::constructor(
    /* [in] */ CNotificationStation* host)
{
    mHost = host;
    return NotificationListenerService::constructor();
}

ECode CNotificationStation::MyNotificationListenerService::OnNotificationPosted(
    /* [in] */ IStatusBarNotification* notification)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, notification);

    Logd(String("onNotificationPosted: %s"), args);

    AutoPtr<IListView> listView;
    mHost->GetListView((IListView**)&listView);
    AutoPtr<IHandler> h;
    IView::Probe(listView)->GetHandler((IHandler**)&h);

    h->RemoveCallbacks(mHost->mRefreshListRunnable);
    Boolean res;
    h->PostDelayed(mHost->mRefreshListRunnable, 100, &res);
    return NOERROR;
}

ECode CNotificationStation::MyNotificationListenerService::OnNotificationRemoved(
    /* [in] */ IStatusBarNotification* notification)
{
    AutoPtr<IListView> listView;
    mHost->GetListView((IListView**)&listView);
    AutoPtr<IHandler> h;
    IView::Probe(listView)->GetHandler((IHandler**)&h);

    h->RemoveCallbacks(mHost->mRefreshListRunnable);
    Boolean res;
    h->PostDelayed(mHost->mRefreshListRunnable, 100, &res);
    return NOERROR;
}

//===============================================================================
//                  CNotificationStation::MyComparator
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationStation::MyComparator, Object, IComparator)

ECode CNotificationStation::MyComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    HistoricalNotificationInfo* lhs = (HistoricalNotificationInfo*) IObject::Probe(_lhs);
    HistoricalNotificationInfo* rhs = (HistoricalNotificationInfo*) IObject::Probe(_rhs);
    *result = (Int32)(rhs->mTimestamp - lhs->mTimestamp);
    return NOERROR;
}

//===============================================================================
//                  CNotificationStation
//===============================================================================

const String CNotificationStation::TAG("CNotificationStation");

const Boolean CNotificationStation::DEBUG = FALSE;

CAR_OBJECT_IMPL(CNotificationStation)

CNotificationStation::CNotificationStation()
    : mContext(NULL)
{}

CNotificationStation::~CNotificationStation()
{}

ECode CNotificationStation::constructor()
{
    mRefreshListRunnable = new MyRunnable(this);

    mListener = new MyNotificationListenerService();
    mListener->constructor(this);

    mNotificationSorter = new MyComparator();

    return SettingsPreferenceFragment::constructor();
}

ECode CNotificationStation::OnAttach(
    /* [in] */ IActivity* activity)
{
    AutoPtr<IClassInfo> klass;
    CObject::ReflectClassInfo(activity, (IClassInfo**)&klass);
    if (klass == NULL) {
        Logger::E("CNotificationStation", "can not find activity's class infomations");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String className;
    klass->GetName(&className);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(className));

    Logd(String("OnAttach(%s)"), args);

    SettingsPreferenceFragment::OnAttach(activity);
    mContext = IContext::Probe(activity);
    mContext->GetPackageManager((IPackageManager**)&mPm);
    mNoMan = IINotificationManager::Probe(
            ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));

    String packageName;
    mContext->GetPackageName(&packageName);
    AutoPtr<IComponentName> comp;
    CComponentName::New(packageName,
            String("Elastos.Droid.Settings.Notification.CNotificationStation")/*this->GetClass()->GetCanonicalName()*/,
            (IComponentName**)&comp);
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 currentUserId;
    helper->GetCurrentUser(&currentUserId);
    ECode ec = mListener->RegisterAsSystemService(mContext, comp, currentUserId);
    if (ec == (ECode) E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Cannot register listener 0x%08x", ec);
    }
    return NOERROR;
}

ECode CNotificationStation::OnDetach()
{
    ECode ec = mListener->UnregisterAsSystemService();
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Cannot unregister listener 0x%08x", ec);
    }

    return SettingsPreferenceFragment::OnDetach();
}

ECode CNotificationStation::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, savedInstanceState);
    Logd(String("OnActivityCreated(%s)"), args);
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    Utils::ForceCustomPadding(IView::Probe(listView), FALSE /* non additive padding */);

    mAdapter = new NotificationHistoryAdapter(this);
    mAdapter->constructor(mContext);
    IAdapterView::Probe(listView)->SetAdapter(mAdapter);
    return NOERROR;
}

ECode CNotificationStation::OnResume()
{
    Logd(String("OnResume()"), NULL);
    SettingsPreferenceFragment::OnResume();
    RefreshList();
    return NOERROR;
}

void CNotificationStation::RefreshList()
{
    AutoPtr<IList> infos = LoadNotifications();
    if (infos != NULL) {
        Int32 size;
        infos->GetSize(&size);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(size));
        Logd(String("adding %d infos"), args);
        mAdapter->Clear();
        mAdapter->AddAll(ICollection::Probe(infos));
        mAdapter->Sort(mNotificationSorter);
    }
}

void CNotificationStation::Logd(
    /* [in] */ const String& msg,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    if (DEBUG) {
        Logger::D(TAG, args == NULL || args->GetLength() == 0 ? msg : StringUtils::Format(msg, args));
    }
}

AutoPtr<IList> CNotificationStation::LoadNotifications()
{
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 currentUserId;
    helper->GetCurrentUser(&currentUserId);
    // try {
    String packageName;
    mContext->GetPackageName(&packageName);
    AutoPtr< ArrayOf<IStatusBarNotification*> > active;
    ECode ec = mNoMan->GetActiveNotifications(
            packageName, (ArrayOf<IStatusBarNotification*>**)&active);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Cannot load Notifications: 0x%08x", ec);
    }
    else {
        AutoPtr< ArrayOf<IStatusBarNotification*> > dismissed;
        ec = mNoMan->GetHistoricalNotifications(
                packageName, 50, (ArrayOf<IStatusBarNotification*>**)&dismissed);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::E(TAG, "Cannot load Notifications: 0x%08x", ec);
        }
        else {
            AutoPtr<IList> list; // List<HistoricalNotificationInfo
            CArrayList::New(active->GetLength() + dismissed->GetLength(), (IList**)&list);

            AutoPtr< ArrayOf< ArrayOf<IStatusBarNotification*>* > > argsUnion =
                    ArrayOf< ArrayOf<IStatusBarNotification*>* >::Alloc(2);
            argsUnion->Set(0, active);
            argsUnion->Set(1, dismissed);
            for (Int32 i = 0; i < 2; ++i) {
                AutoPtr< ArrayOf<IStatusBarNotification*> > resultset = (*argsUnion)[i];

                for (Int32 j = 0; j < resultset->GetLength(); ++j) {
                    AutoPtr<IStatusBarNotification> sbn = (*resultset)[j];

                    AutoPtr<HistoricalNotificationInfo> info = new HistoricalNotificationInfo();
                    sbn->GetPackageName(&info->mPkg);
                    sbn->GetUserId(&info->mUser);
                    AutoPtr<INotification> sbnNotification;
                    sbn->GetNotification((INotification**)&sbnNotification);
                    Int32 icon;
                    sbnNotification->GetIcon(&icon);
                    info->mIcon = LoadIconDrawable(info->mPkg, info->mUser, icon);
                    info->mPkgicon = LoadPackageIconDrawable(info->mPkg, info->mUser);
                    info->mPkgname = LoadPackageName(info->mPkg);
                    AutoPtr<IBundle> extras;
                    sbnNotification->GetExtras((IBundle**)&extras);
                    if (extras != NULL) {
                        String str;
                        extras->GetString(
                                INotification::EXTRA_TITLE, &str);
                        info->mTitle = CoreUtils::Convert(str);
                        if (info->mTitle == NULL || String("").Equals(str)) {
                            extras->GetString(
                                    INotification::EXTRA_TEXT, &str);
                            info->mTitle = CoreUtils::Convert(str);
                        }
                    }
                    if (info->mTitle == NULL || String("").Equals(CoreUtils::Unbox(info->mTitle))) {
                        sbnNotification->GetTickerText((ICharSequence**)&info->mTitle);
                    }
                    // still nothing? come on, give us something!
                    if (info->mTitle == NULL || String("").Equals(CoreUtils::Unbox(info->mTitle))) {
                        info->mTitle = info->mPkgname;
                    }
                    sbn->GetPostTime(&info->mTimestamp);
                    sbnNotification->GetPriority(&info->mPriority);

                    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
                    args->Set(0, CoreUtils::Convert(info->mTimestamp));
                    args->Set(1, CoreUtils::Convert(info->mPkg));
                    args->Set(2, info->mTitle);
                    Logd(String("   [%d] %s: %s"), args);

                    info->mActive = (resultset == active);

                    if (info->mUser == IUserHandle::USER_ALL
                            || info->mUser == currentUserId) {
                        list->Add((IObject*)info);
                    }
                }
            }

            return list;
        }
    }
    // } catch (RemoteException e) {
    //     Logger::E(TAG, "Cannot load Notifications: ", e);
    // }
    return NULL;
}

AutoPtr<IResources> CNotificationStation::GetResourcesForUserPackage(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    AutoPtr<IResources> r;

    if (!pkg.IsNull()) {
        if (userId == IUserHandle::USER_ALL) {
            userId = IUserHandle::USER_OWNER;
        }
        ECode ec = mPm->GetResourcesForApplicationAsUser(pkg, userId, (IResources**)&r);
        if (FAILED(ec)) { //PackageManager.NameNotFoundException
            Logger::E(TAG, "Icon package not found: %s 0x%08x", pkg.string(), ec);
            return NULL;
        }
    }
    else {
        mContext->GetResources((IResources**)&r);
    }
    return r;
}

AutoPtr<IDrawable> CNotificationStation::LoadPackageIconDrawable(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    AutoPtr<IDrawable> icon;
    ECode ec = mPm->GetApplicationIcon(pkg, (IDrawable**)&icon);
    if (FAILED(ec)) { //PackageManager.NameNotFoundException
        Logger::E(TAG, "Cannot get application icon 0x%08x", ec);
    }

    return icon;
}

AutoPtr<ICharSequence> CNotificationStation::LoadPackageName(
    /* [in] */ const String& pkg)
{
    AutoPtr<IApplicationInfo> info;
    ECode ec = mPm->GetApplicationInfo(pkg,
            IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&info);
    if (FAILED(ec)) { // PackageManager.NameNotFoundException
        Logger::E(TAG, "Cannot load package name 0x%08x", ec);
    }
    else {
        if (info != NULL) {
            AutoPtr<ICharSequence> cs;
            ECode ec = mPm->GetApplicationLabel(info, (ICharSequence**)&cs);
            if (FAILED(ec)) { // PackageManager.NameNotFoundException
                Logger::E(TAG, "Cannot load package name 0x%08x", ec);
            }
            else {
                return cs;
            }
        }

    }
    return CoreUtils::Convert(pkg);
}

AutoPtr<IDrawable> CNotificationStation::LoadIconDrawable(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 resId)
{
    AutoPtr<IResources> r = GetResourcesForUserPackage(pkg, userId);

    if (resId == 0) {
        return NULL;
    }

    AutoPtr<IDrawable> drawable;
    ECode ec = r->GetDrawable(resId, (IDrawable**)&drawable);
    if (SUCCEEDED(ec)) {
        return drawable;
    }
    else { //RuntimeException
        StringBuilder builder;
        builder += "Icon not found in ";
        if (!pkg.IsNull()) {
            builder += resId;
        }
        else {
            builder += "<system>";
        }
        builder += ": ";
        builder += StringUtils::ToHexString(resId);
        Logger::W(TAG, builder.ToString());
    }

    return NULL;
}

void CNotificationStation::StartApplicationDetailsActivity(
    /* [in] */ const String& packageName)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettings::ACTION_APPLICATION_DETAILS_SETTINGS, uri, (IIntent**)&intent);
    AutoPtr<IComponentName> comp;
    intent->ResolveActivity(mPm, (IComponentName**)&comp);
    intent->SetComponent(comp);
    StartActivity(intent);
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos