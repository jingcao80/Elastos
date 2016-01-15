
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/ContentProviderRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/CContentProviderConnection.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CContentProviderHolder;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

//=============================================================================
// ContentProviderRecord::ExternalProcessHandle
//=============================================================================

const String ContentProviderRecord::ExternalProcessHandle::TAG("ExternalProcessHanldle");

CAR_INTERFACE_IMPL(ContentProviderRecord::ExternalProcessHandle, Object, IProxyDeathRecipient)

ContentProviderRecord::ExternalProcessHandle::ExternalProcessHandle(
    /* [in] */ IBinder* token,
    /* [in] */ ContentProviderRecord* owner)
    : mToken(token)
    , mAcquisitionCount(0)
    , mOwner(owner)
{
    AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
    assert(proxy != NULL);
    if (FAILED(proxy->LinkToDeath(this, 0))) {
        Slogger::E(TAG, "Couldn't register for death for token: %p", mToken.Get());
    }
}

ECode ContentProviderRecord::ExternalProcessHandle::UnlinkFromOwnDeathLocked()
{
    AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
    assert(proxy != NULL);
    Boolean res;
    return proxy->UnlinkToDeath(this, 0, &res);
}

// @Override
ECode ContentProviderRecord::ExternalProcessHandle::ProxyDied()
{
    AutoLock lock(mOwner->mService);
    if (mOwner->HasExternalProcessHandles() &&
        mOwner->mExternalProcessTokenToHandle->Find(mToken)
        != mOwner->mExternalProcessTokenToHandle->End()) {
        mOwner->RemoveExternalProcessHandleInternalLocked(mToken);
    }
    return NOERROR;
}

//=============================================================================
// ContentProviderRecord
//=============================================================================

ContentProviderRecord::ContentProviderRecord(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IProviderInfo* info,
    /* [in] */ IApplicationInfo* ai,
    /* [in] */ IComponentName* name,
    /* [in] */ Boolean singleton)
    : mService(service)
    , mInfo(info)
    , mAppInfo(ai)
    , mName(name)
    , mSingleton(singleton)
    , mNoReleaseNeeded(FALSE)
    , mExternalProcessNoHandleCount(0)
    , mProc(NULL)
    , mLaunchingApp(NULL)
{
    ai->GetUid(&mUid);
    mNoReleaseNeeded = mUid == 0 || mUid == IProcess::SYSTEM_UID;
}

ContentProviderRecord::ContentProviderRecord(
    /* [in] */ ContentProviderRecord* cpr)
    : mLaunchingApp(NULL)
{
    mService = cpr->mService;
    mInfo = cpr->mInfo;
    mUid = cpr->mUid;
    mAppInfo = cpr->mAppInfo;
    mName = cpr->mName;
    mSingleton = cpr->mSingleton;
    mNoReleaseNeeded = cpr->mNoReleaseNeeded;
    mName = cpr->mName;
}

ContentProviderRecord::~ContentProviderRecord()
{}

AutoPtr<IContentProviderHolder> ContentProviderRecord::NewHolder(
    /* [in] */ CContentProviderConnection* conn)
{
     AutoPtr<IContentProviderHolder> holder;
     CContentProviderHolder::New(mInfo, (IContentProviderHolder**)&holder);
     holder->SetContentProvider(mProvider);
     holder->SetNoReleaseNeeded(mNoReleaseNeeded);
     holder->SetConnection(conn);
     return holder;
}

Boolean ContentProviderRecord::CanRunHere(
    /* [in] */ ProcessRecord* app)
{
    Boolean multiprocess;
    mInfo->GetMultiprocess(&multiprocess);
    String processName;
    IComponentInfo::Probe(mInfo)->GetProcessName(&processName);

    Int32 appUid;
    app->mInfo->GetUid(&appUid);
    return (multiprocess || processName.Equals(app->mProcessName)) && mUid == appUid;
}

void ContentProviderRecord::AddExternalProcessHandleLocked(
    /* [in] */ IBinder* token)
{
    if (token == NULL) {
        mExternalProcessNoHandleCount++;
    }
    else {
        if (mExternalProcessTokenToHandle == NULL)
            mExternalProcessTokenToHandle = new ProcessHandleHashMap;

        AutoPtr<ExternalProcessHandle> handle = (*mExternalProcessTokenToHandle)[token];
        if (handle == NULL) {
            handle = new ExternalProcessHandle(token, this);
            (*mExternalProcessTokenToHandle)[token] = handle;
        }
        handle->mAcquisitionCount++;
    }
}

Boolean ContentProviderRecord::RemoveExternalProcessHandleLocked(
    /* [in] */ IBinder* token)
{
    if (HasExternalProcessHandles()) {
        Boolean hasHandle = FALSE;
        if (mExternalProcessTokenToHandle != NULL) {
            HashMap<AutoPtr<IBinder>, AutoPtr<ExternalProcessHandle> >::Iterator it;
            it = mExternalProcessTokenToHandle->Find(token);
            if (it != mExternalProcessTokenToHandle->End()) {
                AutoPtr<ExternalProcessHandle> handle = it->mSecond;
                hasHandle = TRUE;
                handle->mAcquisitionCount--;
                if (handle->mAcquisitionCount == 0) {
                    RemoveExternalProcessHandleInternalLocked(token);
                    return TRUE;
                }
            }
        }

        if (!hasHandle) {
            mExternalProcessNoHandleCount--;
            return TRUE;
        }
    }
    return FALSE;
}

void ContentProviderRecord::RemoveExternalProcessHandleInternalLocked(
    /* [in] */ IBinder* token)
{
    AutoPtr<ExternalProcessHandle> handle = (*mExternalProcessTokenToHandle)[token];
    handle->UnlinkFromOwnDeathLocked();
    mExternalProcessTokenToHandle->Erase(token);
    if (mExternalProcessTokenToHandle->GetSize() == 0) {
        mExternalProcessTokenToHandle = NULL;
    }
}

Boolean ContentProviderRecord::HasExternalProcessHandles()
{
    return (mExternalProcessTokenToHandle != NULL || mExternalProcessNoHandleCount > 0);
}

void ContentProviderRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ Boolean full)
{
    if (full) {
        pw->Print(prefix);
        pw->Print(String("package="));
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String packageName;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
        pw->Print(packageName);
        pw->Print(String(" process="));
        String processName;
        IComponentInfo::Probe(mInfo)->GetProcessName(&processName);
        pw->Println(processName);
    }
    pw->Print(prefix);
    pw->Print(String("proc="));
    pw->Println(mProc->ToString());
    if (mLaunchingApp != NULL) {
        pw->Print(prefix);
        pw->Print(String("mLaunchingApp="));
        pw->Println(mLaunchingApp->ToString());
    }
    if (full) {
        pw->Print(prefix);
        pw->Print(String("uid="));
        pw->Print(mUid);
        pw->Print(String(" provider="));
        pw->Println(mProvider);
    }
    if (mSingleton) {
        pw->Print(prefix);
        pw->Print(String("singleton="));
        pw->Println(mSingleton);
    }
    pw->Print(prefix);
    pw->Print(String("authority="));
    String authority;
    mInfo->GetAuthority(&authority);
    pw->Println(authority);
    if (full) {
        Boolean isSyncable;
        mInfo->GetIsSyncable(&isSyncable);
        Boolean multiprocess;
        mInfo->GetMultiprocess(&multiprocess);
        Int32 initOrder;
        mInfo->GetInitOrder(&initOrder);
        if (isSyncable || multiprocess || initOrder != 0) {
            pw->Print(prefix);
            pw->Print(String("isSyncable="));
            pw->Print(isSyncable);
            pw->Print(String(" multiprocess="));
            pw->Print(multiprocess);
            pw->Print(String(" initOrder="));
            pw->Println(initOrder);
        }
    }
    if (full) {
        if (HasExternalProcessHandles()) {
            pw->Print(prefix);
            pw->Print(String("externals:"));
            if (mExternalProcessTokenToHandle != NULL) {
                pw->Print(String(" w/token="));
                pw->Print((Int32)mExternalProcessTokenToHandle->GetSize());
            }
            if (mExternalProcessNoHandleCount > 0) {
                pw->Print(String(" notoken="));
                pw->Print(mExternalProcessNoHandleCount);
            }
            pw->Println();
        }
    }
    else {
        if (mConnections.GetSize() > 0 || mExternalProcessNoHandleCount > 0) {
            pw->Print(prefix);
            pw->Print((Int32)mConnections.GetSize());
            pw->Print(String(" connections, "));
            pw->Print(mExternalProcessNoHandleCount);
            pw->Println(String(" external handles"));
        }
    }
    if (mConnections.GetSize() > 0) {
        if (full) {
            pw->Print(prefix);
            pw->Println(String("Connections:"));
        }
        List<AutoPtr<CContentProviderConnection> >::Iterator iter;
        for (iter = mConnections.Begin(); iter != mConnections.End(); ++iter) {
            AutoPtr<CContentProviderConnection> conn = *iter;
            pw->Print(prefix);
            pw->Print(String("  -> "));
            pw->Println(conn->ToClientString());
            if (conn->mProvider.Get() != this) {
                pw->Print(prefix);
                pw->Print(String("    *** WRONG PROVIDER: "));
                pw->Println(conn->mProvider->ToString());
            }
        }
    }
}

String ContentProviderRecord::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }

    String nameStr;
    mName->FlattenToShortString(&nameStr);

    StringBuilder sb(128);
    sb += "ContentProviderRecord{";
    sb += StringUtils::ToString((Int32)this, 16);
    sb += " u";
    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    Int32 userId;
    uhHelper->GetUserId(mUid, &userId);
    sb += userId;
    sb += ' ';
    sb += nameStr;
    sb += '}';
    return mStringName = sb.ToString();
}

String ContentProviderRecord::ToShortString()
{
    if (!mShortStringName.IsNull()) {
        return mShortStringName;
    }

    String nameStr;
    mName->FlattenToShortString(&nameStr);

    StringBuilder sb (128);
    sb += StringUtils::ToString((Int32)this, 16);
    sb += '/';
    sb += nameStr;
    return mShortStringName = sb.ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
