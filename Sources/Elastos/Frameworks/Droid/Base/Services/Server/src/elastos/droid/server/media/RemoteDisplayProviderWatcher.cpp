
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/media/RemoteDisplayProviderWatcher.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

// {A515C487-74FB-4911-9A8B-33ABCD1D2A85}
extern "C" const InterfaceID EIID_IRemoteDisplayProviderWatcherCallback =
        { 0xa515c487, 0x74fb, 0x4911, { 0x9a, 0x8b, 0x33, 0xab, 0xcd, 0x1d, 0x2a, 0x85 } };

//==============================================================================
//                  RemoteDisplayProviderWatcher::ScanPackagesReceiver
//==============================================================================

ECode RemoteDisplayProviderWatcher::ScanPackagesReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (DEBUG) {
        Slogger::D(RemoteDisplayProviderWatcher::TAG, "Received package manager broadcast: %p", intent);
    }
    mHost->ScanPackages();
    return NOERROR;
}

ECode RemoteDisplayProviderWatcher::ScanPackagesReceiver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("RemoteDisplayProviderWatcher::ScanPackagesReceiver: ");
    (*str).AppendFormat("%p", this);
    return NOERROR;
}


//==============================================================================
//                  RemoteDisplayProviderWatcher::ScanPackagesRunnable
//==============================================================================

ECode RemoteDisplayProviderWatcher::ScanPackagesRunnable::Run()
{
    mHost->ScanPackages();
    return NOERROR;
}


//==============================================================================
//                  RemoteDisplayProviderWatcher
//==============================================================================

const String RemoteDisplayProviderWatcher::TAG("RemoteDisplayProvider");
const Boolean RemoteDisplayProviderWatcher::DEBUG;

RemoteDisplayProviderWatcher::RemoteDisplayProviderWatcher(
    /* [in] */ IContext* context,
    /* [in] */ ICallback* callback,
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 userId)
    : mContext(context)
    , mCallback(callback)
    , mHandler(handler)
    , mUserId(userId)
{
    context->GetPackageManager((IPackageManager**)&mPackageManager);
    CArrayList::New((IArrayList**)&mProviders);
    mScanPackagesReceiver = (IBroadcastReceiver*)new ScanPackagesReceiver(this);
    mScanPackagesRunnable = (IRunnable*)new ScanPackagesRunnable(this);
}

void RemoteDisplayProviderWatcher::Start()
{
    if (!mRunning) {
        mRunning = TRUE;

        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
        filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
        filter->AddAction(IIntent::ACTION_PACKAGE_RESTARTED);
        filter->AddDataScheme(String("package"));
        AutoPtr<IUserHandle> userH;
        CUserHandle::New(mUserId, (IUserHandle**)&userH);
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiverAsUser(mScanPackagesReceiver,
                userH, filter, String(NULL), mHandler, (IIntent**)&intent);

        // Scan packages.
        // Also has the side-effect of restarting providers if needed.
        Boolean result;
        mHandler->Post(mScanPackagesRunnable, &result);
    }
}

void RemoteDisplayProviderWatcher::Stop()
{
    if (mRunning) {
        mRunning = FALSE;

        mContext->UnregisterReceiver(mScanPackagesReceiver);
        mHandler->RemoveCallbacks(mScanPackagesRunnable);

        // Stop all providers.
        Int32 size;
        mProviders->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> value;
            mProviders->Get(i, (IInterface**)&value);
            AutoPtr<RemoteDisplayProviderProxy> provider = (RemoteDisplayProviderProxy*)(IObject*)value.Get();
            provider->Stop();
        }
    }
}

void RemoteDisplayProviderWatcher::ScanPackages()
{
    if (!mRunning) {
        return;
    }

    // Add providers for all new services.
    // Reorder the list so that providers left at the end will be the ones to remove.
    Int32 targetIndex = 0;
    AutoPtr<IIntent> intent;
    CIntent::New(IRemoteDisplayState::SERVICE_INTERFACE, (IIntent**)&intent);
    AutoPtr<IList> services;
    mPackageManager->QueryIntentServicesAsUser(intent, 0, mUserId, (IList**)&services);
    AutoPtr<IIterator> iterator;
    services->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iterator->GetNext((IInterface**)&value);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(value);
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        if (serviceInfo != NULL && VerifyServiceTrusted(serviceInfo)) {
            AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(serviceInfo);
            String packageName, name;
            pii->GetPackageName(&packageName);
            pii->GetName(&name);
            Int32 sourceIndex = FindProvider(packageName, name);
            if (sourceIndex < 0) {
                AutoPtr<IComponentName> cn;
                CComponentName::New(packageName, name, (IComponentName**)&cn);
                AutoPtr<RemoteDisplayProviderProxy> provider =
                        new RemoteDisplayProviderProxy(mContext, cn, mUserId);
                provider->Start();
                mProviders->Add(targetIndex++, (IObject*)provider.Get());
                mCallback->AddProvider(provider);
            }
            else if (sourceIndex >= targetIndex) {
                AutoPtr<IInterface> value;
                mProviders->Get(sourceIndex, (IInterface**)&value);
                AutoPtr<RemoteDisplayProviderProxy> provider = (RemoteDisplayProviderProxy*)(IObject*)value.Get();
                provider->Start(); // restart the provider if needed
                provider->RebindIfDisconnected();
                AutoPtr<ICollections> cols;
                CCollections::AcquireSingleton((ICollections**)&cols);
                cols->Swap(IList::Probe(mProviders), sourceIndex, targetIndex++);
            }
        }
    }

    // Remove providers for missing services.
    Int32 size;
    mProviders->GetSize(&size);
    if (targetIndex < size) {
        for (Int32 i = size - 1; i >= targetIndex; i--) {
            AutoPtr<IInterface> value;
            mProviders->Get(i, (IInterface**)&value);
            AutoPtr<RemoteDisplayProviderProxy> provider = (RemoteDisplayProviderProxy*)(IObject*)value.Get();
            mCallback->RemoveProvider(provider);
            mProviders->Remove(value);
            provider->Stop();
        }
    }
}

Boolean RemoteDisplayProviderWatcher::VerifyServiceTrusted(
    /* [in] */ IServiceInfo* serviceInfo)
{
    AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(serviceInfo);
    String permission;
    if ((serviceInfo->GetPermission(&permission), permission.IsNull())
            || !permission.Equals(Elastos::Droid::Manifest::permission::BIND_REMOTE_DISPLAY)) {
        // If the service does not require this permission then any app could
        // potentially bind to it and cause the remote display service to
        // misbehave.  So we only want to trust providers that require the
        // correct permissions.
        String packageName, name;
        pii->GetPackageName(&packageName);
        pii->GetName(&name);
        Slogger::W(TAG, "Ignoring remote display provider service because it did not require the BIND_REMOTE_DISPLAY permission in its manifest: %s/%s",
                packageName.string(), name.string());
        return FALSE;
    }
    String packageName;
    pii->GetPackageName(&packageName);
    if (!HasCaptureVideoPermission(packageName)) {
        // If the service does not have permission to capture video then it
        // isn't going to be terribly useful as a remote display, is it?
        // Kind of makes you wonder what it's doing there in the first place.
        String name;
        pii->GetName(&name);
        Slogger::W(TAG, "Ignoring remote display provider service because it does not have the CAPTURE_VIDEO_OUTPUT or CAPTURE_SECURE_VIDEO_OUTPUT permission: %s/%s",
                packageName.string(), name.string());
        return FALSE;
    }
    // Looks good.
    return TRUE;
}

Boolean RemoteDisplayProviderWatcher::HasCaptureVideoPermission(
    /* [in] */ const String& packageName)
{
    Int32 perm;
    if (mPackageManager->CheckPermission(Elastos::Droid::Manifest::permission::CAPTURE_VIDEO_OUTPUT,
            packageName, &perm), perm == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }
    if (mPackageManager->CheckPermission(Elastos::Droid::Manifest::permission::CAPTURE_SECURE_VIDEO_OUTPUT,
            packageName, &perm), perm == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }
    return FALSE;
}

Int32 RemoteDisplayProviderWatcher::FindProvider(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    Int32 count;
    mProviders->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> value;
        mProviders->Get(i, (IInterface**)&value);
        AutoPtr<RemoteDisplayProviderProxy> provider = (RemoteDisplayProviderProxy*)(IObject*)value.Get();
        if (provider->HasComponentName(packageName, className)) {
            return i;
        }
    }
    return -1;
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
