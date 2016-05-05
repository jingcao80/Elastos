
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Service.h>
#include "elastos/droid/server/notification/ManagedServices.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String ManagedServices::ENABLED_SERVICES_SEPARATOR(":");

//===============================================================================
//                  DumpFilter
//===============================================================================

DumpFilter::DumpFilter()
{}

DumpFilter::~DumpFilter()
{}

AutoPtr<DumpFilter> DumpFilter::ParseFromArguments(
    /* [in] */ ArrayOf<String>* args)
{
    if (args != NULL && args->GetLength() == 2 && String("p").Equals((*args)[0])
            && !(*args)[1].IsNull() && !(*args)[1].Trim().IsEmpty()) {
        AutoPtr<DumpFilter> filter = new DumpFilter();
        filter->mPkgFilter = (*args)[1].Trim().ToLowerCase();
        return filter;
    }
    if (args != NULL && args->GetLength() == 1 && String("zen").Equals((*args)[0])) {
        AutoPtr<DumpFilter> filter = new DumpFilter();
        filter->mZen = TRUE;
        return filter;
    }
    return NULL;
}

Boolean DumpFilter::Matches(
    /* [in] */ IStatusBarNotification* sbn)
{
    String name, pkg;
    sbn->GetPackageName(&name);
    sbn->GetOpPkg(&pkg);
    return mZen ? TRUE : sbn != NULL && (Matches(name) || Matches(pkg));
}

Boolean DumpFilter::Matches(
    /* [in] */ IComponentName* component)
{
    String name;
    component->GetPackageName(&name);
    return mZen ? TRUE : component != NULL && Matches(name);
}

Boolean DumpFilter::Matches(
    /* [in] */ const String& pkg)
{
    return mZen ? TRUE : !pkg.IsNull() && pkg.ToLowerCase().Contains(mPkgFilter);
}

ECode DumpFilter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    if (mZen) {
        *str = "zen";
    }
    else {
        StringBuilder builder;
        builder += '\'';
        builder += mPkgFilter;
        builder += '\'';
        *str = builder.ToString();
    }
    return NOERROR;
}

//===============================================================================
//                  ManagedServices::ManagedServiceInfo
//===============================================================================

CAR_INTERFACE_IMPL(ManagedServices::ManagedServiceInfo, Object, IProxyDeathRecipient);

ManagedServices::ManagedServiceInfo::ManagedServiceInfo(
    /* [in] */ ManagedServices* host,
    /* [in] */ IInterface* service,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userid,
    /* [in] */ Boolean isSystem,
    /* [in] */ IServiceConnection* connection,
    /* [in] */ Int32 targetSdkVersion)
    : mService(service)
    , mComponent(component)
    , mUserid(userid)
    , mIsSystem(isSystem)
    , mConnection(connection)
    , mTargetSdkVersion(targetSdkVersion)
    , mHost(host)
{}

ManagedServices::ManagedServiceInfo::~ManagedServiceInfo()
{}

ECode ManagedServices::ManagedServiceInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder builder("ManagedServiceInfo[");
    builder.Append("component=");
    builder.Append(mComponent);
    builder.Append(",userid=");
    builder.Append(mUserid);
    builder.Append(",isSystem=");
    builder.Append(mIsSystem);
    builder.Append(",targetSdkVersion=");
    builder.Append(mTargetSdkVersion);
    builder.Append(",connection=");
    builder.Append(mConnection == NULL ? NULL : "<connection>");
    builder.Append(",service=");
    builder.Append(mService);
    builder.AppendChar(']');

    *str = builder.ToString();
    return NOERROR;
}

Boolean ManagedServices::ManagedServiceInfo::EnabledAndUserMatches(
    /* [in] */ Int32 nid)
{
    if (!IsEnabledForCurrentProfiles()) {
        return FALSE;
    }
    if (mUserid == IUserHandle::USER_ALL) {
        return TRUE;
    }
    if (nid == IUserHandle::USER_ALL || nid == mUserid) {
        return TRUE;
    }
    return SupportsProfiles() && mHost->mUserProfiles->IsCurrentProfile(nid);
}

Boolean ManagedServices::ManagedServiceInfo::SupportsProfiles()
{
    return mTargetSdkVersion >= Build::VERSION_CODES::LOLLIPOP;
}

ECode ManagedServices::ManagedServiceInfo::ProxyDied()
{
    if (mHost->DEBUG) Slogger::D(mHost->TAG, "ProxyDied");
    // Remove the service, but don't unbind from the service. The system will bring the
    // service back up, and the onServiceConnected handler will readd the service with the
    // new binding. If this isn't a bound service, and is just a registered
    // service, just removing it from the list is all we need to do anyway.

    AutoPtr<ManagedServiceInfo> info = mHost->RemoveServiceImpl(mService, mUserid);
    return NOERROR;
}

Boolean ManagedServices::ManagedServiceInfo::IsEnabledForCurrentProfiles()
{
    if (mIsSystem) {
        return TRUE;
    }
    if (mConnection == NULL) {
        return FALSE;
    }
    Boolean res;
    ISet::Probe(mHost->mEnabledServicesForCurrentProfiles)->Contains(mComponent, &res);
    return res;
}

//===============================================================================
//                  ManagedServices::UserProfiles
//===============================================================================

ManagedServices::UserProfiles::UserProfiles()
{
    CSparseArray::New((ISparseArray**)&mCurrentProfiles);
}

ManagedServices::UserProfiles::~UserProfiles()
{}

void ManagedServices::UserProfiles::UpdateCache(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
    if (userManager != NULL) {
        AutoPtr<IActivityManagerHelper> helper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
        Int32 currentUserId;
        helper->GetCurrentUser(&currentUserId);
        AutoPtr<IList> profiles;
        userManager->GetProfiles(currentUserId, (IList**)&profiles);
        synchronized(mCurrentProfiles) {
            mCurrentProfiles->Clear();
            Int32 size;
            profiles->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                profiles->Get(i, (IInterface**)&obj);
                AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);
                Int32 id;
                user->GetId(&id);
                mCurrentProfiles->Put(id, user);
            }
        }
    }
}

AutoPtr< ArrayOf<Int32> > ManagedServices::UserProfiles::GetCurrentProfileIds()
{
    synchronized(mCurrentProfiles) {
        Int32 N;
        mCurrentProfiles->GetSize(&N);
        AutoPtr< ArrayOf<Int32> > users = ArrayOf<Int32>::Alloc(N);
        for (Int32 i = 0; i < N; ++i) {
            Int32 key;
            mCurrentProfiles->KeyAt(i, &key);
            (*users)[i] = key;
        }
        return users;
    }
    return NULL;
}

Boolean ManagedServices::UserProfiles::IsCurrentProfile(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    synchronized(mCurrentProfiles) {
        mCurrentProfiles->Get(userId, (IInterface**)&obj);
    }
    return obj != NULL;
}

//===============================================================================
//                  ManagedServices::SettingsObserver
//===============================================================================

ManagedServices::SettingsObserver::SettingsObserver(
    /* [in] */ ManagedServices* host,
    /* [in] */ IHandler* handler)
    : mHost(host)
{
    ContentObserver::constructor(handler);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    secure->GetUriFor(mHost->mConfig->mSecureSettingName,
            (IUri**)&mSecureSettingsUri);
}

ManagedServices::SettingsObserver::~SettingsObserver()
{}

void ManagedServices::SettingsObserver::Observe()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    resolver->RegisterContentObserver(mSecureSettingsUri, FALSE,
            this, IUserHandle::USER_ALL);

    Update(NULL);
}

ECode ManagedServices::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Update(uri);
    return NOERROR;
}

void ManagedServices::SettingsObserver::Update(
    /* [in] */ IUri* uri)
{
    Boolean res;
    if (uri == NULL || (IObject::Probe(mSecureSettingsUri)->Equals(uri, &res), res)) {
        if (mHost->DEBUG) {
            Slogger::D(mHost->TAG, "Setting changed: mSecureSettingsUri=%p / uri=%p",
                    mSecureSettingsUri.Get(), uri);
        }
        mHost->RebindServices();
    }
}

//===============================================================================
//                  ManagedServices::MyServiceConnection
//===============================================================================

CAR_INTERFACE_IMPL(ManagedServices::MyServiceConnection, Object, IServiceConnection);

ManagedServices::MyServiceConnection::MyServiceConnection(
    /* [in] */ ManagedServices* host,
    /* [in] */ const String& servicesBindingTag,
    /* [in] */ Int32 userid,
    /* [in] */ Int32 targetSdkVersion)
    : mHost(host)
    , mServicesBindingTag(servicesBindingTag)
    , mUserid(userid)
    , mTargetSdkVersion(targetSdkVersion)
{}

ManagedServices::MyServiceConnection::~MyServiceConnection()
{}

ECode ManagedServices::MyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* binder)
{
    Boolean added = FALSE;
    AutoPtr<ManagedServiceInfo> info;
    synchronized(mHost->mMutex) {
        mHost->mServicesBinding->Remove(CoreUtils::Convert(mServicesBindingTag));
        // try {
        mService = TO_IINTERFACE(binder);
        info = mHost->NewServiceInfo(mService, name,
                mUserid, FALSE /*isSystem*/, this, mTargetSdkVersion);

        AutoPtr<IProxy> proxy = (IProxy*)binder->Probe(EIID_IProxy);
        if (proxy != NULL && FAILED(proxy->LinkToDeath(info, 0))) {
            // already dead
        }

        mHost->mServices->Add((IProxyDeathRecipient*)info, &added);
        // } catch (RemoteException e) {
        //     // already dead
        // }
    }
    if (added) {
        mHost->OnServiceAdded(info);
    }
    return NOERROR;
}

ECode ManagedServices::MyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    Slogger::V(mHost->TAG, "%s connection lost: %p", mHost->GetCaption().string(), name);
    return NOERROR;
}

//===============================================================================
//                  ManagedServices
//===============================================================================
CAR_INTERFACE_IMPL(ManagedServices, Object, IManagedServices)

ManagedServices::ManagedServices()
{
}

ManagedServices::~ManagedServices()
{}

ECode ManagedServices::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IInterface* mutex,
    /* [in] */ UserProfiles* userProfiles)
{
    AutoPtr<IClassInfo> classInfo;
    CObject::ReflectClassInfo(TO_IINTERFACE(this), (IClassInfo**)&classInfo);
    String TAG;
    classInfo->GetName(&TAG);

    DEBUG = Logger::IsLoggable(TAG, ILogHelper::DEBUG);

    mContext = context;
    mMutex = (Object*)IObject::Probe(mutex);
    mUserProfiles = userProfiles;
    mConfig = GetConfig();
    mSettingsObserver = new SettingsObserver(this, handler);
    CArrayList::New((IArrayList**)&mServices);
    CArrayList::New((IArrayList**)&mServicesBinding);
    CArraySet::New((IArraySet**)&mEnabledServicesForCurrentProfiles);
    CArraySet::New((IArraySet**)&mEnabledServicesPackageNames);
    return NOERROR;
}

String ManagedServices::GetCaption()
{
    return mConfig->mCaption;
}

ECode ManagedServices::OnServiceRemovedLocked(
    /* [in] */ ManagedServiceInfo* info)
{
    return NOERROR;
}

AutoPtr<ManagedServices::ManagedServiceInfo> ManagedServices::NewServiceInfo(
    /* [in] */ IInterface* service,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userid,
    /* [in] */ Boolean isSystem,
    /* [in] */ IServiceConnection* connection,
    /* [in] */ Int32 targetSdkVersion)
{
    AutoPtr<ManagedServiceInfo> info = new ManagedServiceInfo(this, service,
            component, userid, isSystem, connection, targetSdkVersion);
    return info;
}

void ManagedServices::OnBootPhaseAppsCanStart()
{
    mSettingsObserver->Observe();
}

void ManagedServices::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ DumpFilter* filter)
{
    Int32 size;
    ISet::Probe(mEnabledServicesForCurrentProfiles)->GetSize(&size);

    StringBuilder builder;
    builder += "    All ";
    builder += GetCaption();
    builder += "s (";
    builder += size;
    builder += ") enabled for current profiles:";
    pw->Println(builder.ToString());
    AutoPtr<IIterator> iterator;
    ISet::Probe(mEnabledServicesForCurrentProfiles)->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        AutoPtr<IComponentName> cmpt = IComponentName::Probe(obj);
        if (filter != NULL && !filter->Matches(cmpt)) continue;
        builder.Reset();
        builder += "      ";
        builder += cmpt;
        pw->Println(builder.ToString());
    }

    mServices->GetSize(&size);

    builder.Reset();
    builder += "    Live ";
    builder += GetCaption();
    builder += "s (";
    builder += size;
    builder += "):";
    pw->Println(builder.ToString());
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
        if (filter != NULL && !filter->Matches(info->mComponent)) continue;
        builder.Reset();
        builder += "      ";
        builder += info->mComponent;
        builder += " (user ";
        builder += info->mUserid;
        builder += "): ";
        builder += info->mService;
        builder += info->mIsSystem?" SYSTEM":"";
        pw->Println(builder.ToString());
    }
}

void ManagedServices::OnPackagesChanged(
    /* [in] */ Boolean queryReplace,
    /* [in] */ ArrayOf<String>* pkgList)
{
    StringBuilder builder;
    builder += "onPackagesChanged queryReplace=";
    builder += queryReplace;
    builder += " pkgList=";
    if (pkgList != NULL) {
        AutoPtr<IList> list;
        Arrays::AsList(pkgList, (IList**)&list);
        builder += list;
    }
    else {
        builder += "NULL";
    }
    builder += " mEnabledServicesPackageNames=";
    builder += mEnabledServicesPackageNames;
    if (DEBUG) Slogger::D(TAG, builder.ToString());
    Boolean anyServicesInvolved = FALSE;
    if (pkgList != NULL && (pkgList->GetLength() > 0)) {
        for (Int32 i = 0; i < pkgList->GetLength(); i++) {
            String pkgName = (*pkgList)[i];
            Boolean res;
            if (ISet::Probe(mEnabledServicesPackageNames)->Contains(CoreUtils::Convert(pkgName), &res), res) {
                anyServicesInvolved = TRUE;
            }
        }
    }

    if (anyServicesInvolved) {
        // if we're not replacing a package, clean up orphaned bits
        if (!queryReplace) {
            DisableNonexistentServices();
        }
        // make sure we're still bound to any of our services who may have just upgraded
        RebindServices();
    }
}

void ManagedServices::OnUserSwitched()
{
    if (DEBUG) Slogger::D(TAG, "onUserSwitched");
    if (Arrays::Equals(mLastSeenProfileIds, mUserProfiles->GetCurrentProfileIds())) {
        if (DEBUG) Slogger::D(TAG, "Current profile IDs didn't change, skipping rebindServices().");
        return;
    }
    RebindServices();
}

ECode ManagedServices::CheckServiceTokenLocked(
    /* [in] */ IInterface* service,
    /* [out] */ ManagedServiceInfo** _info)
{
    VALIDATE_NOT_NULL(_info);
    *_info = NULL;

    FAIL_RETURN(CheckNotNull(service))
    AutoPtr<IBinder> token = IBinder::Probe(service);
    Int32 N;
    mServices->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
        if (IBinder::Probe(info->mService) == token) {
            *_info = info;
            REFCOUNT_ADD(*_info);
            return NOERROR;
        }
    }
    Slogger::E(TAG, "Disallowed call from unknown %s : %p", GetCaption().string(), service);
    return E_SECURITY_EXCEPTION;
    // throw new SecurityException("Disallowed call from unknown " + getCaption() + ": "
    //         + service);
}

ECode ManagedServices::UnregisterService(
    /* [in] */ IInterface* service,
    /* [in] */ Int32 userid)
{
    FAIL_RETURN(CheckNotNull(service))
    // no need to check permissions; if your service binder is in the list,
    // that's proof that you had permission to add it in the first place
    UnregisterServiceImpl(service, userid);
    return NOERROR;
}

ECode ManagedServices::RegisterService(
    /* [in] */ IInterface* service,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userid)
{
    FAIL_RETURN(CheckNotNull(service))
    AutoPtr<ManagedServiceInfo> info;
    RegisterServiceImpl(service, component, userid, (ManagedServiceInfo**)&info);
    if (info != NULL) {
        OnServiceAdded(info);
    }
    return NOERROR;
}

void ManagedServices::DisableNonexistentServices()
{
    AutoPtr< ArrayOf<Int32> > userIds = mUserProfiles->GetCurrentProfileIds();
    const Int32 N = userIds->GetLength();
    for (Int32 i = 0 ; i < N; ++i) {
        DisableNonexistentServices((*userIds)[i]);
    }
}

void ManagedServices::DisableNonexistentServices(
    /* [in] */ Int32 userId)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);

    String flatIn;
    secure->GetStringForUser(resolver,
            mConfig->mSecureSettingName, userId, &flatIn);
    if (!TextUtils::IsEmpty(flatIn)) {
        if (DEBUG) {
            Slogger::V(TAG, "flat before: %s", flatIn.string());
        }
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IIntent> intent;
        CIntent::New(mConfig->mServiceInterface, (IIntent**)&intent);
        AutoPtr<IList> installedServices;
        pm->QueryIntentServicesAsUser(intent,
                IPackageManager::GET_SERVICES | IPackageManager::GET_META_DATA,
                userId, (IList**)&installedServices);
        if (DEBUG) {
            Slogger::V(TAG, "%s services: %p",
                    mConfig->mServiceInterface.string(), installedServices.Get());
        }
        AutoPtr<ISet> installed;
        CArraySet::New((ISet**)&installed);
        Int32 count;
        installedServices->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            installedServices->Get(i, (IInterface**)&obj);
            AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(obj);
            AutoPtr<IServiceInfo> info;
            resolveInfo->GetServiceInfo((IServiceInfo**)&info);

            String packageName, name;
            IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
            IPackageItemInfo::Probe(info)->GetName(&name);

            String permission;
            info->GetPermission(&permission);
            if (!mConfig->mBindPermission.Equals(permission)) {
                Slogger::W(TAG, "Skipping %s service %s/%s: it does not require the permission %s",
                        GetCaption().string(), packageName.string(), name.string(),
                        mConfig->mBindPermission.string());
                continue;
            }
            AutoPtr<IComponentName> componentName;
            CComponentName::New(packageName, name, (IComponentName**)&componentName);
            installed->Add(componentName);
        }

        String flatOut("");
        Boolean res;
        if (installed->IsEmpty(&res), !res) {
            AutoPtr< ArrayOf<String> > enabled;
            StringUtils::Split(flatIn, ENABLED_SERVICES_SEPARATOR, (ArrayOf<String>**)&enabled);
            Int32 length = enabled->GetLength();
            AutoPtr<IArrayList> remaining;
            CArrayList::New(length, (IArrayList**)&remaining);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IComponentNameHelper> helper;
                CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
                AutoPtr<IComponentName> enabledComponent;
                helper->UnflattenFromString((*enabled)[i], (IComponentName**)&enabledComponent);
                if (installed->Contains(enabledComponent, &res), res) {
                    remaining->Add(CoreUtils::Convert((*enabled)[i]));
                }
            }

            AutoPtr< ArrayOf<IInterface*> > array;
            remaining->ToArray((ArrayOf<IInterface*>**)&array);
            flatOut = TextUtils::Join(CoreUtils::Convert(ENABLED_SERVICES_SEPARATOR), array);
        }
        if (DEBUG) {
            Slogger::V(TAG, "flat after: %s", flatOut.string());
        }
        if (!flatIn.Equals(flatOut)) {
            secure->PutStringForUser(resolver, mConfig->mSecureSettingName,
                    flatOut, userId, &res);
        }
    }
}

ECode ManagedServices::RebindServices()
{
    if (DEBUG) {
        Slogger::D(TAG, "rebindServices");
    }
    AutoPtr< ArrayOf<Int32> > userIds = mUserProfiles->GetCurrentProfileIds();
    const Int32 nUserIds = userIds->GetLength();

    AutoPtr<ISparseArray> flat;
    CSparseArray::New((ISparseArray**)&flat);

    for (Int32 i = 0; i < nUserIds; ++i) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        String str;
        secure->GetStringForUser(resolver,
                mConfig->mSecureSettingName, (*userIds)[i], &str);
        flat->Put((*userIds)[i], CoreUtils::Convert(str));
    }

    AutoPtr<IArrayList> toRemove;
    CArrayList::New((IArrayList**)&toRemove);
    AutoPtr<ISparseArray> toAdd;
    CSparseArray::New((ISparseArray**)&toAdd);

    synchronized(mMutex) {
        // Unbind automatically bound services, retain system services.
        Int32 size;
        mServices->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mServices->Get(i, (IInterface**)&obj);
            AutoPtr<ManagedServiceInfo> service = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
            if (!service->mIsSystem) {
                toRemove->Add((IProxyDeathRecipient*)service);
            }
        }

        AutoPtr<IArraySet> newEnabled;
        CArraySet::New((IArraySet**)&newEnabled);
        AutoPtr<IArraySet> newPackages;
        CArraySet::New((IArraySet**)&newPackages);

        for (Int32 i = 0; i < nUserIds; ++i) {
            AutoPtr<IArrayList> add;
            CArrayList::New((IArrayList**)&add);
            toAdd->Put((*userIds)[i], add);

            // decode the list of components
            AutoPtr<IInterface> obj;
            flat->Get((*userIds)[i], (IInterface**)&obj);
            String toDecode(NULL);
            if (ICharSequence::Probe(obj) != NULL) {
                ICharSequence::Probe(obj)->ToString(&toDecode);
            }
            if (!toDecode.IsNull()) {
                AutoPtr< ArrayOf<String> > components;
                StringUtils::Split(toDecode, ENABLED_SERVICES_SEPARATOR, (ArrayOf<String>**)&components);
                for (Int32 j = 0; j < components->GetLength(); j++) {
                    AutoPtr<IComponentNameHelper> helper;
                    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
                    AutoPtr<IComponentName> component;
                    helper->UnflattenFromString((*components)[j], (IComponentName**)&component);
                    if (component != NULL) {
                        ISet::Probe(newEnabled)->Add(component);
                        add->Add(component);
                        String packageName;
                        IPackageItemInfo::Probe(component)->GetPackageName(&packageName);
                        ISet::Probe(newPackages)->Add(CoreUtils::Convert(packageName));
                    }
                }

            }
        }
        mEnabledServicesForCurrentProfiles = newEnabled;
        mEnabledServicesPackageNames = newPackages;
    }

    Int32 size;
    toRemove->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        toRemove->Get(i, (IInterface**)&obj);
        AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
        AutoPtr<IComponentName> component = info->mComponent;
        Int32 oldUser = info->mUserid;
        Slogger::V(TAG, "disabling %s for user %d: %p",
                GetCaption().string(), oldUser, component.Get());
        UnregisterService(component, info->mUserid);
    }

    for (Int32 i = 0; i < nUserIds; ++i) {
        AutoPtr<IInterface> obj;
        toAdd->Get((*userIds)[i], (IInterface**)&obj);
        AutoPtr<IArrayList> add = IArrayList::Probe(obj);
        Int32 N;
        add->GetSize(&N);
        for (Int32 j = 0; j < N; j++) {
            AutoPtr<IInterface> obj;
            add->Get(j, (IInterface**)&obj);
            AutoPtr<IComponentName> component = IComponentName::Probe(obj);
            Slogger::V(TAG, "enabling %s for user %d: %p",
                    GetCaption().string(), (*userIds)[i], component.Get());
            RegisterService(component, (*userIds)[i]);
        }
    }

    mLastSeenProfileIds = mUserProfiles->GetCurrentProfileIds();
    return NOERROR;
}

ECode ManagedServices::RegisterService(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 userid)
{
    if (DEBUG) {
        Slogger::V(TAG, "registerService: %p u=%d", name, userid);
    }

    synchronized(mMutex) {
        String str;
        name->ToString(&str);
        StringBuilder builder;
        builder += str;
        builder += "/";
        builder += userid;
        String servicesBindingTag = builder.ToString();
        Boolean res;
        if (mServicesBinding->Contains(CoreUtils::Convert(servicesBindingTag), &res), res) {
            // stop registering this thing already! we're working on it
            return NOERROR;
        }
        mServicesBinding->Add(CoreUtils::Convert(servicesBindingTag));

        Int32 N;
        mServices->GetSize(&N);
        for (Int32 i = N-1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mServices->Get(i, (IInterface**)&obj);
            AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
            if ((IObject::Probe(name)->Equals(info->mComponent, &res), res)
                    && info->mUserid == userid) {
                // cut old connections
                if (DEBUG) {
                    Slogger::V(TAG, "    disconnecting old %s: %p", GetCaption().string(), info->mService.Get());
                }
                RemoveServiceLocked(i);
                if (info->mConnection != NULL) {
                    mContext->UnbindService(info->mConnection);
                }
            }
        }

        AutoPtr<IIntent> intent;
        CIntent::New(mConfig->mServiceInterface, (IIntent**)&intent);
        intent->SetComponent(name);

        intent->PutExtra(IIntent::EXTRA_CLIENT_LABEL, mConfig->mClientLabel);

        AutoPtr<IIntent> otherIntent;
        CIntent::New(mConfig->mSettingsAction, (IIntent**)&otherIntent);
        AutoPtr<IPendingIntentHelper> helper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
        AutoPtr<IPendingIntent> pendingIntent;
        helper->GetActivity(
                mContext, 0, otherIntent, 0, (IPendingIntent**)&pendingIntent);
        intent->PutExtra(IIntent::EXTRA_CLIENT_INTENT, IParcelable::Probe(pendingIntent));

        AutoPtr<IApplicationInfo> appInfo;
        // try {
        AutoPtr<IPackageManager> manager;
        mContext->GetPackageManager((IPackageManager**)&manager);
        String packageName;
        name->GetPackageName(&packageName);
        manager->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&appInfo);
        // } catch (NameNotFoundException e) {
        //     // Ignore if the package doesn't exist we won't be able to bind to the service.
        // }
        Int32 targetSdkVersion;
        if (appInfo != NULL) {
            appInfo->GetTargetSdkVersion(&targetSdkVersion);
        }
        else {
            targetSdkVersion = Build::VERSION_CODES::BASE;
        }

        // try {
        if (DEBUG) {
            Slogger::V(TAG, "binding: %p", intent.Get());
        }
        AutoPtr<MyServiceConnection> connection = new MyServiceConnection(
                this, servicesBindingTag, userid, targetSdkVersion);
        AutoPtr<IUserHandle> handle;
        CUserHandle::New(userid, (IUserHandle**)&handle);
        if (mContext->BindServiceAsUser(intent, connection,
                IContext::BIND_AUTO_CREATE, handle, &res), !res) {
            mServicesBinding->Remove(CoreUtils::Convert(servicesBindingTag));
            Slogger::W(TAG, "Unable to bind %s service: %p", GetCaption().string(), intent.Get());
            return NOERROR;
        }
        // } catch (SecurityException ex) {
        //     Slog.e(TAG, "Unable to bind " + getCaption() + " service: " + intent, ex);
        //     return;
        // }
    }
    return NOERROR;
}

ECode ManagedServices::UnregisterService(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 userid)
{
    synchronized(mMutex) {
        Int32 N;
        mServices->GetSize(&N);
        for (Int32 i = N-1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mServices->Get(i, (IInterface**)&obj);
            AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
            Boolean res;
            if ((IObject::Probe(name)->Equals(info->mComponent, &res), res)
                    && info->mUserid == userid) {
                RemoveServiceLocked(i);
                if (info->mConnection != NULL) {
                    // try {
                    ECode ec = mContext->UnbindService(info->mConnection);
                    if (FAILED(ec)) {
                        Slogger::E(TAG, "%s %p could not be unbound: 0x%08x", GetCaption().string(), name, ec);
                    }
                    // } catch (IllegalArgumentException ex) {
                    //     // something happened to the service: we think we have a connection
                    //     // but it's bogus.
                    //     Slog.e(TAG, getCaption() + " " + name + " could not be unbound: " + ex);
                    // }
                }
            }
        }
    }
    return NOERROR;
}

AutoPtr<ManagedServices::ManagedServiceInfo> ManagedServices::RemoveServiceImpl(
    /* [in] */ IInterface* service,
    /* [in] */ Int32 userid)
{
    if (DEBUG) {
        Slogger::D(TAG, "removeServiceImpl service=%p u=%d", service, userid);
    }
    AutoPtr<ManagedServiceInfo> serviceInfo;
    synchronized(mMutex) {
        Int32 N;
        mServices->GetSize(&N);
        for (Int32 i = N-1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mServices->Get(i, (IInterface**)&obj);
            AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
            if (IBinder::Probe(info->mService) == IBinder::Probe(service)
                    && info->mUserid == userid) {
                if (DEBUG) {
                    Slogger::D(TAG, "Removing active service %p", info->mComponent.Get());
                }
                serviceInfo = RemoveServiceLocked(i);
            }
        }
    }
    return serviceInfo;
}

AutoPtr<ManagedServices::ManagedServiceInfo> ManagedServices::RemoveServiceLocked(
    /* [in] */ Int32 i)
{
    AutoPtr<IInterface> obj;
    mServices->Remove(i, (IInterface**)&obj);
    AutoPtr<ManagedServiceInfo> info = (ManagedServiceInfo*)IProxyDeathRecipient::Probe(obj);
    OnServiceRemovedLocked(info);
    return info;
}

ECode ManagedServices::CheckNotNull(
    /* [in] */ IInterface* service)
{
    if (service == NULL) {
        Slogger::E(TAG, "%s must not be null", GetCaption().string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException(getCaption() + " must not be null");
    }
    return NOERROR;
}

ECode ManagedServices::RegisterServiceImpl(
    /* [in] */ IInterface* service,
    /* [in] */ IComponentName* component,
    /* [in] */ Int32 userid,
    /* [out] */ ManagedServiceInfo** _info)
{
    VALIDATE_NOT_NULL(_info);
    *_info = NULL;

    synchronized(mMutex) {
        // try {
        AutoPtr<ManagedServiceInfo> info = NewServiceInfo(service, component, userid,
                TRUE /*isSystem*/, NULL, Build::VERSION_CODES::LOLLIPOP);
        AutoPtr<IProxy> proxy = (IProxy*)service->Probe(EIID_IProxy);
        if (proxy != NULL && FAILED(proxy->LinkToDeath(info, 0))) {
            // already dead
        }
        mServices->Add((IProxyDeathRecipient*)info);
        *_info = info;
        REFCOUNT_ADD(*_info);
        return NOERROR;
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

void ManagedServices::UnregisterServiceImpl(
    /* [in] */ IInterface* service,
    /* [in] */ Int32 userid)
{
    AutoPtr<ManagedServiceInfo> info = RemoveServiceImpl(service, userid);
    if (info != NULL && info->mConnection != NULL) {
        mContext->UnbindService(info->mConnection);
    }
}

} // Notification
} // Server
} // Droid
} // Elastos
