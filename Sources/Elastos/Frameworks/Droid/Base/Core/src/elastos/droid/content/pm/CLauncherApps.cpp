#include "elastos/droid/content/pm/CLauncherApps.h"
#include "elastos/droid/content/pm/CLauncherActivityInfo.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CHandler;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

//==========================================================================
// CLauncherApps::CallbackMessageHandler
//==========================================================================
const Int32 CLauncherApps::CallbackMessageHandler::MSG_ADDED = 1;
const Int32 CLauncherApps::CallbackMessageHandler::MSG_REMOVED = 2;
const Int32 CLauncherApps::CallbackMessageHandler::MSG_CHANGED = 3;
const Int32 CLauncherApps::CallbackMessageHandler::MSG_AVAILABLE = 4;
const Int32 CLauncherApps::CallbackMessageHandler::MSG_UNAVAILABLE = 5;

CLauncherApps::CallbackMessageHandler::CallbackMessageHandler()
{
}

ECode CLauncherApps::CallbackMessageHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ ILauncherAppsCallback* callback)
{
    FAIL_RETURN(Handler::constructor(looper, NULL, TRUE))
    mCallback = callback;
    return NOERROR;
}

ECode CLauncherApps::CallbackMessageHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    if (mCallback == NULL || !obj) {
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    CallbackInfo* info = (CallbackInfo*)IObject::Probe(obj);
    switch (what) {
        case MSG_ADDED:
            mCallback->OnPackageAdded(info->mPackageName, info->mUser);
            break;
        case MSG_REMOVED:
            mCallback->OnPackageRemoved(info->mPackageName, info->mUser);
            break;
        case MSG_CHANGED:
            mCallback->OnPackageChanged(info->mPackageName, info->mUser);
            break;
        case MSG_AVAILABLE:
            mCallback->OnPackagesAvailable(info->mPackageNames, info->mUser, info->mReplacing);
            break;
        case MSG_UNAVAILABLE:
            mCallback->OnPackagesUnavailable(info->mPackageNames, info->mUser, info->mReplacing);
            break;
    }
    return NOERROR;
}

ECode CLauncherApps::CallbackMessageHandler::PostOnPackageAdded(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<CLauncherApps::CallbackInfo> info = new CLauncherApps::CallbackInfo();
    info->mPackageName = packageName;
    info->mUser = user;
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_ADDED, TO_IINTERFACE(info), (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CLauncherApps::CallbackMessageHandler::PostOnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<CLauncherApps::CallbackInfo> info = new CLauncherApps::CallbackInfo();
    info->mPackageName = packageName;
    info->mUser = user;
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_REMOVED, TO_IINTERFACE(info), (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CLauncherApps::CallbackMessageHandler::PostOnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<CLauncherApps::CallbackInfo> info = new CLauncherApps::CallbackInfo();
    info->mPackageName = packageName;
    info->mUser = user;
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_CHANGED, TO_IINTERFACE(info), (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CLauncherApps::CallbackMessageHandler::PostOnPackagesAvailable(
    /* [in] */ ArrayOf<String>* packageName,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean replacing)
{
    AutoPtr<CLauncherApps::CallbackInfo> info = new CLauncherApps::CallbackInfo();
    info->mPackageNames = packageName;
    info->mReplacing = replacing;
    info->mUser = user;
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_AVAILABLE, TO_IINTERFACE(info), (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CLauncherApps::CallbackMessageHandler::PostOnPackagesUnavailable(
    /* [in] */ ArrayOf<String>* packageName,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean replacing)
{
    AutoPtr<CLauncherApps::CallbackInfo> info = new CLauncherApps::CallbackInfo();
    info->mPackageNames = packageName;
    info->mReplacing = replacing;
    info->mUser = user;
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_UNAVAILABLE, TO_IINTERFACE(info), (IMessage**)&msg);
    return msg->SendToTarget();
}

//==========================================================================
// CLauncherApps::MyOnAppsChangedListener
//==========================================================================

CAR_INTERFACE_IMPL_2(CLauncherApps::MyOnAppsChangedListener, Object, IOnAppsChangedListener, IBinder)

CLauncherApps::MyOnAppsChangedListener::MyOnAppsChangedListener(
    /* [in] */ IWeakReference* weakHost)
    : mWeakHost(weakHost)
{
}

ECode CLauncherApps::MyOnAppsChangedListener::OnPackageRemoved(
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& packageName)
{
    if (CLauncherApps::DEBUG) {
        Logger::D(CLauncherApps::TAG, "OnPackageRemoved %s, %s",
            TO_CSTR(user), packageName.string());
    }

    AutoPtr<ILauncherApps> la;
    mWeakHost->Resolve(EIID_ILauncherApps, (IInterface**)&la);
    if (la == NULL) {
        return NOERROR;
    }

    CLauncherApps* cla = (CLauncherApps*)la.Get();
    ISynchronize* sync = ISynchronize::Probe(la);
    synchronized(sync) {
        AutoPtr<IIterator> it;
        cla->mCallbacks->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            CallbackMessageHandler* callback = (CallbackMessageHandler*)IHandler::Probe(obj);
            callback->PostOnPackageRemoved(packageName, user);
        }
    }
    return NOERROR;
}

ECode CLauncherApps::MyOnAppsChangedListener::OnPackageAdded(
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& packageName)
{
    if (CLauncherApps::DEBUG) {
        Logger::D(CLauncherApps::TAG, "OnPackageAdded %s, %s",
            TO_CSTR(user), packageName.string());
    }

    AutoPtr<ILauncherApps> la;
    mWeakHost->Resolve(EIID_ILauncherApps, (IInterface**)&la);
    if (la == NULL) {
        return NOERROR;
    }

    CLauncherApps* cla = (CLauncherApps*)la.Get();
    ISynchronize* sync = ISynchronize::Probe(la);
    synchronized(sync) {
        AutoPtr<IIterator> it;
        cla->mCallbacks->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            CallbackMessageHandler* callback = (CallbackMessageHandler*)IHandler::Probe(obj);
            callback->PostOnPackageAdded(packageName, user);
        }
    }
    return NOERROR;
}

ECode CLauncherApps::MyOnAppsChangedListener::OnPackageChanged(
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& packageName)
{
    if (CLauncherApps::DEBUG) {
        Logger::D(CLauncherApps::TAG, "OnPackageChanged %s, %s",
            TO_CSTR(user), packageName.string());
    }

    AutoPtr<ILauncherApps> la;
    mWeakHost->Resolve(EIID_ILauncherApps, (IInterface**)&la);
    if (la == NULL) {
        return NOERROR;
    }

    CLauncherApps* cla = (CLauncherApps*)la.Get();
    ISynchronize* sync = ISynchronize::Probe(la);
    synchronized(sync) {
        AutoPtr<IIterator> it;
        cla->mCallbacks->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            CallbackMessageHandler* callback = (CallbackMessageHandler*)IHandler::Probe(obj);
            callback->PostOnPackageChanged(packageName, user);
        }
    }
    return NOERROR;
}

ECode CLauncherApps::MyOnAppsChangedListener::OnPackagesAvailable(
    /* [in] */ IUserHandle* user,
    /* [in] */ ArrayOf<String>* packageNames,
    /* [in] */ Boolean replacing)
{
    if (CLauncherApps::DEBUG) {
        Logger::D(CLauncherApps::TAG, "OnPackagesAvailable %s, %s",
            TO_CSTR(user), Arrays::ToString(packageNames).string());
    }

    AutoPtr<ILauncherApps> la;
    mWeakHost->Resolve(EIID_ILauncherApps, (IInterface**)&la);
    if (la == NULL) {
        return NOERROR;
    }

    CLauncherApps* cla = (CLauncherApps*)la.Get();
    ISynchronize* sync = ISynchronize::Probe(la);
    synchronized(sync) {
        AutoPtr<IIterator> it;
        cla->mCallbacks->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            CallbackMessageHandler* callback = (CallbackMessageHandler*)IHandler::Probe(obj);
            callback->PostOnPackagesAvailable(packageNames, user, replacing);
        }
    }
    return NOERROR;
}

ECode CLauncherApps::MyOnAppsChangedListener::OnPackagesUnavailable(
    /* [in] */ IUserHandle* user,
    /* [in] */ ArrayOf<String>* packageNames,
    /* [in] */ Boolean replacing)
{
    if (CLauncherApps::DEBUG) {
        Logger::D(CLauncherApps::TAG, "OnPackagesUnavailable %s, %s",
            TO_CSTR(user), Arrays::ToString(packageNames).string());
    }

    AutoPtr<ILauncherApps> la;
    mWeakHost->Resolve(EIID_ILauncherApps, (IInterface**)&la);
    if (la == NULL) {
        return NOERROR;
    }

    CLauncherApps* cla = (CLauncherApps*)la.Get();
    ISynchronize* sync = ISynchronize::Probe(la);
    synchronized(sync) {
        AutoPtr<IIterator> it;
        cla->mCallbacks->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            CallbackMessageHandler* callback = (CallbackMessageHandler*)IHandler::Probe(obj);
            callback->PostOnPackagesUnavailable(packageNames, user, replacing);
        }
    }
    return NOERROR;
}

ECode CLauncherApps::MyOnAppsChangedListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("CLauncherApps::MyOnAppsChangedListener");
    return NOERROR;
}

//==========================================================================
// CLauncherApps
//==========================================================================
const String CLauncherApps::TAG("LauncherApps");
const Boolean CLauncherApps::DEBUG = FALSE;

CAR_INTERFACE_IMPL(CLauncherApps, Object, ILauncherApps)

CAR_OBJECT_IMPL(CLauncherApps)

CLauncherApps::CLauncherApps()
{}

CLauncherApps::~CLauncherApps()
{}

ECode CLauncherApps::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IILauncherApps* service)
{
    CArrayList::New((IList**)&mCallbacks);
    mContext = context;
    mService = service;
    context->GetPackageManager((IPackageManager**)&mPm);
    return NOERROR;
}

ECode CLauncherApps::GetActivityList(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user,
    /* [out] */ IList** infos)
{
    VALIDATE_NOT_NULL(infos)
    *infos = NULL;

    AutoPtr<IList> activities;
    // try {
        mService->GetLauncherActivities(packageName, user, (IList**)&activities);
    // } catch (RemoteException re) {
    // }
    if (activities == NULL) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        return collections->GetEmptyList(infos);
    }

    AutoPtr<IArrayList> lais;
    CArrayList::New((IArrayList**)&lais);
    Int64 firstInstallTime = 0;
    Int32 count;
    activities->GetSize(&count);
    String pkgName;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        activities->Get(i, (IInterface**)&obj);
        IResolveInfo* ri = IResolveInfo::Probe(obj);

        // try {
        AutoPtr<IActivityInfo> ai;
        ri->GetActivityInfo((IActivityInfo**)&ai);
        IPackageItemInfo::Probe(ai)->GetPackageName(&pkgName);
        AutoPtr<IPackageInfo> pi;
        mPm->GetPackageInfo(pkgName, IPackageManager::GET_UNINSTALLED_PACKAGES, (IPackageInfo**)&pi);
        pi->GetFirstInstallTime(&firstInstallTime);
        // } catch (NameNotFoundException nnfe) {
        //     // Sorry, can't find package
        // }
        AutoPtr<ILauncherActivityInfo> lai;
        CLauncherActivityInfo::New(mContext, ri, user, firstInstallTime, (ILauncherActivityInfo**)&lai);
        if (DEBUG) {
            AutoPtr<IComponentName> cn;
            lai->GetComponentName((IComponentName**)&cn);
            Logger::V(TAG, "Returning activity for profile %s : %s",
                TO_CSTR(user), TO_CSTR(cn));
        }
        lais->Add(TO_IINTERFACE(lai));
    }

    *infos = IList::Probe(lais);
    REFCOUNT_ADD(*infos)
    return NOERROR;
}

AutoPtr<IComponentName> CLauncherApps::GetComponentName(
    /* [in] */ IResolveInfo* info)
{
    AutoPtr<IActivityInfo> ai;
    info->GetActivityInfo((IActivityInfo**)&ai);
    String packageName, name;
    IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
    IPackageItemInfo::Probe(ai)->GetName(&name);
    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, name, (IComponentName**)&cn);
    return cn;
}

ECode CLauncherApps::ResolveActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [out] */ ILauncherActivityInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    // try {
    AutoPtr<IResolveInfo> ri;
    mService->ResolveActivity(intent, user, (IResolveInfo**)&ri);
    if (ri != NULL) {
        Int64 firstInstallTime = 0;
        // try {
        AutoPtr<IActivityInfo> ai;
        ri->GetActivityInfo((IActivityInfo**)&ai);
        String packageName;
        IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
        AutoPtr<IPackageInfo> pi;
        mPm->GetPackageInfo(packageName, IPackageManager::GET_UNINSTALLED_PACKAGES, (IPackageInfo**)&pi);
        pi->GetFirstInstallTime(&firstInstallTime);
        // } catch (NameNotFoundException nnfe) {
        //     // Sorry, can't find package
        // }
        return CLauncherActivityInfo::New(mContext, ri, user, firstInstallTime, info);
    }
    // } catch (RemoteException re) {
    //     throw new RuntimeException("Failed to call LauncherAppsService");
    // }
    // return NULL;
    return NOERROR;
}

ECode CLauncherApps::StartMainActivity(
    /* [in] */ IComponentName* component,
    /* [in] */ IUserHandle* user,
    /* [in] */ IRect* sourceBounds,
    /* [in] */ IBundle* opts)
{
    if (DEBUG) {
        Logger::I(TAG, "StartMainActivity %s user %s",
            TO_CSTR(component), TO_CSTR(user));
    }
    // try {
    return mService->StartActivityAsUser(component, sourceBounds, opts, user);
    // } catch (RemoteException re) {
    //     // Oops!
    // }
}

ECode CLauncherApps::StartAppDetailsActivity(
    /* [in] */ IComponentName* component,
    /* [in] */ IUserHandle* user,
    /* [in] */ IRect* sourceBounds,
    /* [in] */ IBundle* opts)
{
    // try {
    return mService->ShowAppDetailsAsUser(component, sourceBounds, opts, user);
    // } catch (RemoteException re) {
    //     // Oops!
    // }
}

ECode CLauncherApps::IsPackageEnabled(
    /* [in] */ const String& packageName,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    // try {
    return mService->IsPackageEnabled(packageName, user, result);
    // } catch (RemoteException re) {
    //     throw new RuntimeException("Failed to call LauncherAppsService");
    // }
    return NOERROR;
}

ECode CLauncherApps::IsActivityEnabled(
    /* [in] */ IComponentName* component,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* result)
{
    // try {
    return mService->IsActivityEnabled(component, user, result);
    // } catch (RemoteException re) {
    //     throw new RuntimeException("Failed to call LauncherAppsService");
    // }
    return NOERROR;
}

ECode CLauncherApps::RegisterCallback(
    /* [in] */ ILauncherAppsCallback* callback)
{
    return RegisterCallback(callback, NULL);
}

ECode CLauncherApps::RegisterCallback(
    /* [in] */ ILauncherAppsCallback* callback,
    /* [in] */ IHandler* handler)
{
    synchronized(this) {
        Boolean contains;
        if (callback != NULL && (mCallbacks->Contains(callback, &contains), !contains)) {
            Int32 size;
            mCallbacks->GetSize(&size);
            Boolean addedFirstCallback = size == 0;
            AddCallbackLocked(callback, handler);
            if (addedFirstCallback) {
                // try {
                    mService->AddOnAppsChangedListener(mAppsChangedListener);
                // } catch (RemoteException re) {
                // }
            }
        }
    }
    return NOERROR;
}

ECode CLauncherApps::UnregisterCallback(
    /* [in] */ ILauncherAppsCallback* callback)
{
    synchronized(this) {
        RemoveCallbackLocked(callback);
        Int32 size;
        mCallbacks->GetSize(&size);
        if (size == 0) {
            // try {
                mService->RemoveOnAppsChangedListener(mAppsChangedListener);
            // } catch (RemoteException re) {
            // }
        }
    }
    return NOERROR;
}

ECode CLauncherApps::RemoveCallback(
    /* [in] */ ILauncherAppsCallback* callback)
{
    return UnregisterCallback(callback);
}

ECode CLauncherApps::AddCallback(
    /* [in] */ ILauncherAppsCallback* callback)
{
    return RegisterCallback(callback);
}

ECode CLauncherApps::RemoveCallbackLocked(
    /* [in] */ ILauncherAppsCallback* callback)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("Callback cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 size;
    mCallbacks->GetSize(&size);
    for (int i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        CallbackMessageHandler* cmh = (CallbackMessageHandler*)IHandler::Probe(obj);
        if (cmh->mCallback.Get() == callback) {
            mCallbacks->Remove(i);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CLauncherApps::AddCallbackLocked(
    /* [in] */ ILauncherAppsCallback* callback,
    /* [in] */ IHandler* handler)
{
    // Remove if already present.
    RemoveCallbackLocked(callback);
    AutoPtr<IHandler> h = handler;
    if (h == NULL) {
        CHandler::New((IHandler**)&h);
    }

    AutoPtr<ILooper> looper;
    h->GetLooper((ILooper**)&looper);
    AutoPtr<CallbackMessageHandler> toAdd = new CallbackMessageHandler();
    toAdd->constructor(looper, callback);
    mCallbacks->Add(TO_IINTERFACE(toAdd));
    return NOERROR;
}


}
}
}
}
