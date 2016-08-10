#include <Elastos.CoreLibrary.Utility.h>
#define HASH_FOR_CONTENT
#include <elastos/droid/ext/frameworkhash.h>
#include "elastos/droid/server/ServiceWatcher.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/Arrays.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Arrays;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

const String ServiceWatcher::EXTRA_SERVICE_VERSION("serviceVersion");
const String ServiceWatcher::EXTRA_SERVICE_IS_MULTIUSER("serviceIsMultiuser");
const Boolean ServiceWatcher::DBG = FALSE;

//========================================================================
// ServiceWatcher::ServiceWatcherPackageMonitor
//========================================================================
ServiceWatcher::ServiceWatcherPackageMonitor::ServiceWatcherPackageMonitor(
    /* [in] */ ServiceWatcher* host) : mHost(host)
{}

ECode ServiceWatcher::ServiceWatcherPackageMonitor::OnPackageUpdateFinished(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    Object& obj = mHost->mLock;
    AutoLock lock(obj);
    if(packageName.Equals(mHost->mPackageName)) {
        // package updated, make sure to rebind
        mHost->UnbindLocked();
    }

    // check the updated package in case it is better
    mHost->BindBestPackageLocked(String(NULL));
    return NOERROR;
}

ECode ServiceWatcher::ServiceWatcherPackageMonitor::OnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    Object& obj = mHost->mLock;
    AutoLock lock(obj);

    if (packageName.Equals(mHost->mPackageName)) {
        // package updated, make sure to rebind
        mHost->UnbindLocked();
    }
    // check the new package is case it is better
    mHost->BindBestPackageLocked(String(NULL));

    return NOERROR;
}

ECode ServiceWatcher::ServiceWatcherPackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    Object& obj = mHost->mLock;
    AutoLock lock(obj);

    if (packageName.Equals(mHost->mPackageName)) {
        mHost->UnbindLocked();
        // the currently bound package was removed,
        // need to search for a new package
        mHost->BindBestPackageLocked(String(NULL));
    }
    return NOERROR;
}

ECode ServiceWatcher::ServiceWatcherPackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ ArrayOf<String>* components,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {
        Object& obj = mHost->mLock;
        AutoLock lock(obj);

        if (packageName.Equals(mHost->mPackageName)) {
            mHost->UnbindLocked();
            // the currently bound package was removed,
            // need to search for a new package
            mHost->BindBestPackageLocked(String(NULL));
        }
    }

    return PackageMonitor::OnPackageChanged(packageName, uid, components, result);
}

//========================================================================
// ServiceWatcher::MyBroadcastReceiver
//========================================================================
ServiceWatcher::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ ServiceWatcher* host)
    : mHost(host)
{}

ECode ServiceWatcher::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        mHost->SwitchUser();
    }
    return NOERROR;
}

//========================================================================
// ServiceWatcher
//========================================================================

CAR_INTERFACE_IMPL(ServiceWatcher, Object, IServiceConnection)

AutoPtr<List<AutoPtr<IHashSet> > > ServiceWatcher::GetSignatureSets(
    /* [in] */ IContext* context,
    /* [in] */ List<String>* initialPackageNames)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<List<AutoPtr<IHashSet> > > sigSets = new List<AutoPtr<IHashSet> >;

    List<String>::Iterator it = initialPackageNames->Begin();
    for (; it != initialPackageNames->End(); it++) {
        String pkg = *it;
        AutoPtr<IHashSet> set;
        CHashSet::New((IHashSet**)&set);
        AutoPtr<IPackageInfo> packageInfo;
        if (SUCCEEDED(pm->GetPackageInfo(pkg, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&packageInfo))) {
            AutoPtr<ArrayOf<ISignature*> > sigs;
            packageInfo->GetSignatures((ArrayOf<ISignature*>**)&sigs);
            for(Int32 i = 0; i < sigs->GetLength(); i++) {
                set->Add((*sigs)[i]);
            }
            sigSets->PushBack(set);
        }
        else {
            Logger::W("ServiceWatcher", "%s not found", pkg.string());
        }
    }
    return sigSets;
}

ServiceWatcher::ServiceWatcher(
    /* [in] */ IContext* context,
    /* [in] */ const String& logTag,
    /* [in] */ const String& action,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNamesResId,
    /* [in] */ IRunnable* newServiceWork,
    /* [in] */ IHandler* handler)
    : mTag(logTag)
    , mVersion(0)
    , mIsMultiuser(Elastos::Core::Math::INT32_MIN_VALUE)
{
    mContext = context;
    mTag = logTag;
    mAction = action;
    mContext->GetPackageManager((IPackageManager**)&mPm);
    mNewServiceWork = newServiceWork;
    mHandler = handler;

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    // Whether to enable service overlay.
    Boolean enableOverlay;
    resources->GetBoolean(overlaySwitchResId, &enableOverlay);
    List<String> initialPackageNames;
    if (enableOverlay) {
        // A list of package names used to create the signatures.
        AutoPtr<ArrayOf<String> > pkgs;
        resources->GetStringArray(initialPackageNamesResId, (ArrayOf<String>**)&pkgs);
        if (pkgs != NULL) {
            for (Int32 i = 0; i < pkgs->GetLength(); ++i) {
                initialPackageNames.PushBack((*pkgs)[i]);
            }
        }
        mServicePackageName = NULL;
        if (DBG) Logger::D(mTag, "Overlay enabled, packages=%s",
            Arrays::ToString(pkgs).string());
    }
    else {
        // The default package name that is searched for service implementation when overlay is
        // disabled.
        String servicePackageName;
        resources->GetString(defaultServicePackageNameResId, &servicePackageName);
        if (servicePackageName != NULL) initialPackageNames.PushBack(servicePackageName);
        mServicePackageName = servicePackageName;
        if (DBG) Logger::D(mTag, "Overlay disabled, default package=%s", servicePackageName.string());
    }
    mSignatureSets = GetSignatureSets(context, &initialPackageNames);

    mPackageMonitor = new ServiceWatcherPackageMonitor(this);
}

Boolean ServiceWatcher::Start()
{
    {
        AutoLock lock(mLock);
        if (!BindBestPackageLocked(String(NULL))) return FALSE;
    }

    // listen for user change
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    AutoPtr<IBroadcastReceiver> br = new MyBroadcastReceiver(this);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    AutoPtr<IIntent> stickyIntent;
    mContext->RegisterReceiverAsUser(br, all, intentFilter, String(NULL), mHandler,
        (IIntent**)&stickyIntent);

    // listen for relevant package changes if service overlay is enabled.
    if (mServicePackageName == NULL) {
        mPackageMonitor->Register(mContext, NULL, all, TRUE);
    }

    return TRUE;
}

Boolean ServiceWatcher::BindBestPackageLocked(
    /* [in] */ const String& justCheckThisPackage)
{
    AutoPtr<IIntent> intent;
    CIntent::New(mAction, (IIntent**)&intent);
    if (!justCheckThisPackage.IsNull()) {
        intent->SetPackage(justCheckThisPackage);
    }



    AutoPtr<IIntent> paraIntent;
    CIntent::New(mAction, (IIntent**)&paraIntent);
    AutoPtr<IList> rInfos;
    mPm->QueryIntentServicesAsUser(paraIntent, IPackageManager::GET_META_DATA,
        IUserHandle::USER_OWNER, (IList**)&rInfos);
    Int32 bestVersion = Elastos::Core::Math::INT32_MIN_VALUE;
    String bestPackage;
    Boolean bestIsMultiuser = FALSE;
    if (rInfos != NULL) {
        AutoPtr<IIterator> it;
        rInfos->GetIterator((IIterator**)&it);

        Boolean hasNext;
        String packageName;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IResolveInfo> rInfo = IResolveInfo::Probe(obj);
            AutoPtr<IServiceInfo> sInfo;
            rInfo->GetServiceInfo((IServiceInfo**)&sInfo);
            IPackageItemInfo::Probe(sInfo)->GetPackageName(&packageName);

            AutoPtr<IPackageInfo> pInfo;
            ECode ec = mPm->GetPackageInfo(packageName, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&pInfo);

            AutoPtr<ArrayOf<ISignature*> > sigsTemp;
            pInfo->GetSignatures((ArrayOf<ISignature*>**)&sigsTemp);
            if (!IsSignatureMatch(sigsTemp)) {
    //            Log.w(mTag, packageName + " resolves service " + mAction +
    //                  ", but has wrong signature, ignoring");
                Logger::W(mTag, "%s resolves service , but has wrong signature, ignoring", packageName.string());
                continue;
            }
            else if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                continue;
            }

            Int32 version = Elastos::Core::Math::INT32_MIN_VALUE;
            Boolean isMultiuser = FALSE;
            AutoPtr<IBundle> metaData;
            IPackageItemInfo::Probe(sInfo)->GetMetaData((IBundle**)&metaData);
            if (metaData != NULL) {
                metaData->GetInt32(EXTRA_SERVICE_VERSION, 0, &version);
                metaData->GetBoolean(EXTRA_SERVICE_IS_MULTIUSER, 0, &isMultiuser);
            }

            if (version > mVersion) {
                bestVersion = version;
                bestPackage = packageName;
                bestIsMultiuser = isMultiuser;
            }
        }

    //    if (DBG) Log.d(mTag, String.format("bindBestPackage for %s : %s found %d, %s", mAction,
    //            (justCheckThisPackage == null ? "" : "(" + justCheckThisPackage + ") "),
    //            rInfos.size(),
    //            (bestPackage == null ? "no new best package" : "new best packge: " + bestPackage)));
    }
    else {
        if (DBG) Logger::D(mTag, "Unable to query intent services for action: %s", mAction.string());
    }

    if (!bestPackage.IsNull()) {
        BindToPackageLocked(bestPackage, bestVersion, bestIsMultiuser);
        return TRUE;
    }
    return FALSE;
}

void ServiceWatcher::UnbindLocked()
{
    String pkg(mPackageName);
    mPackageName = NULL;
    mVersion = Elastos::Core::Math::INT32_MIN_VALUE;
    mIsMultiuser = FALSE;
    if (!pkg.IsNull()) {
        if (DBG) Logger::D(mTag, "unbinding %s", pkg.string());
        mContext->UnbindService(this);
    }
}

void ServiceWatcher::BindToPackageLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 version,
    /* [in] */ Boolean isMultiuser)
{
    UnbindLocked();
    AutoPtr<IIntent> intent;
    CIntent::New(mAction, (IIntent**)&intent);
    intent->SetPackage(packageName);
    mPackageName = packageName;
    mVersion = version;
    // if (D) Log.d(mTag, "binding " + packageName + " (version " + version + ") ("
    //         + (isMultiuser ? "multi" : "single") + "-user)");

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> user;
    if (mIsMultiuser) {
        helper->GetOWNER((IUserHandle**)&user);
    }
    else {
        helper->GetCURRENT((IUserHandle**)&user);
    }

    Boolean result;
    mContext->BindServiceAsUser(intent, this,
        IContext::BIND_AUTO_CREATE | IContext::BIND_NOT_FOREGROUND | IContext::BIND_NOT_VISIBLE,
        user, &result);
}

Boolean ServiceWatcher::IsSignatureMatch(
    /* [in] */ ArrayOf<ISignature*>* signatures,
    /* [in] */ List<AutoPtr<IHashSet> >* sigSets)
{
    if (signatures == NULL) return FALSE;

    // build hashset of input to test against
    AutoPtr<IHashSet> inputSet;
    CHashSet::New((IHashSet**)&inputSet);
    for (Int32 i = 0; i < signatures->GetLength(); i++) {
        inputSet->Add((*signatures)[i]);
    }

    // test input against each of the signature sets
    List<AutoPtr<IHashSet> >::ConstIterator it = sigSets->Begin();
    for (; it != sigSets->End(); it++) {
        AutoPtr<IHashSet> referenceSet = *it;
        Boolean equals = FALSE;
        referenceSet->Equals(inputSet, &equals);
        if (equals)
            return TRUE;
    }
    return FALSE;
}

Boolean ServiceWatcher::IsSignatureMatch(
    /* [in] */ ArrayOf<ISignature*>* signatures)
{
    return IsSignatureMatch(signatures, mSignatureSets);
}

ECode ServiceWatcher::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* binder)
{
    AutoLock lock(mLock);
    String packageName;
    name->GetPackageName(&packageName);
    if (packageName.Equals(mPackageName)) {
        if (DBG) Logger::D(mTag, "%s connected", packageName.string());
        mBinder = binder;
        if (mHandler != NULL && mNewServiceWork != NULL) {
            Boolean rst;
            mHandler->Post(mNewServiceWork, &rst);
        }
    }
    else {
        Logger::W(mTag, "unexpected onServiceConnected: %s", packageName.string());
    }
    return NOERROR;
}

ECode ServiceWatcher::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoLock lock(mLock);
    String packageName;
    name->GetPackageName(&packageName);
    if (DBG) Logger::D(mTag, "%s disconnected", packageName.string());

    if (packageName.Equals(mPackageName)) {
        mBinder = NULL;
    }
    return NOERROR;
}

String ServiceWatcher::GetBestPackageName()
{
    AutoLock lock(mLock);
    return mPackageName;
}

Int32 ServiceWatcher::GetBestVersion()
{
    AutoLock lock(mLock);
    return mVersion;
}

AutoPtr<IBinder> ServiceWatcher::GetBinder()
{
    AutoLock lock(mLock);
    return mBinder;
}

void ServiceWatcher::SwitchUser()
{
    AutoLock lock(mLock);
    if (!mIsMultiuser) {
        UnbindLocked();
        BindBestPackageLocked(mServicePackageName);
    }
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
