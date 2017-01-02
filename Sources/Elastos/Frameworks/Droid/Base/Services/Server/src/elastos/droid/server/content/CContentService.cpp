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

#include "elastos/droid/server/content/CContentService.h"
#include <elastos/droid/Manifest.h>
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.Droid.Net.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Content::EIID_IIContentService;
using Elastos::Droid::Content::IContentResolverHelper;
using Elastos::Droid::Content::CContentResolverHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

const String CContentService::TAG("CContentService");
const Boolean CContentService::DEBUG = FALSE;

//===================================================================================
// CContentService::ObserverCall
//===================================================================================
CContentService::ObserverCall::ObserverCall(
    /* [in] */ ObserverNode* node,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean selfChange)
    : mNode(node)
    , mObserver(observer)
    , mSelfChange(selfChange)
{}

//===================================================================================
// CContentService::ObserverNode::ObserverEntry
//===================================================================================
CAR_INTERFACE_IMPL(CContentService::ObserverNode::ObserverEntry, Object, IProxyDeathRecipient)

CContentService::ObserverNode::ObserverEntry::ObserverEntry(
    /* [in] */ ObserverNode* host,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean notify,
    /* [in] */ Object* observersLock,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 userHandle)
    : mObserver(observer)
    , mUid(uid)
    , mPid(pid)
    , mNotifyForDescendants(notify)
    , mUserHandle(userHandle)
    , mHost(host)
    , mObserversLock(observersLock)
{
    // try {
    AutoPtr<IProxy> proxy = (IProxy*)observer->Probe(EIID_IProxy);
    if (proxy != NULL) {
        if (FAILED(proxy->LinkToDeath(this, 0))) {
            ProxyDied();
        }
    }
    // } catch (RemoteException e) {
    //     binderDied();
    // }
}

ECode CContentService::ObserverNode::ObserverEntry::ProxyDied()
{
    AutoLock lock(mObserversLock);
    mHost->RemoveObserverLocked(mObserver);
    return NOERROR;
}

void CContentService::ObserverNode::ObserverEntry::DumpLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ const String& name,
    /* [in] */ const String& prefix,
    /* [in] */ HashMap<Int32, Int32>* pidCounts)
{
    (*pidCounts)[mPid] = (*pidCounts)[mPid] + 1;
    pw->Print(prefix);
    pw->Print(name);
    pw->Print(String(": pid="));
    pw->Print(mPid);
    pw->Print(String(" uid="));
    pw->Print(mUid);
    pw->Print(String(" user="));
    pw->Print(mUserHandle);
    pw->Print(String(" target="));
    String info = Object::ToString(mObserver);
    pw->Print(info);
}

//===================================================================================
// CContentService::ObserverNode
//===================================================================================

CContentService::ObserverNode::ObserverNode(
    /* [in] */ const String& name)
    : mName(name)
{}

void CContentService::ObserverNode::DumpLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ const String& name,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<Int32>* counts,
    /* [in] */ HashMap<Int32, Int32>* pidCounts)
{
    String innerName;
    if (!mObservers.IsEmpty()) {
        if (name.Equals("")) {
            innerName = mName;
        }
        else {
            innerName = name;
            innerName += "/";
            innerName += mName;
        }
        List<AutoPtr<ObserverEntry> >::Iterator it1 = mObservers.Begin();
        for (; it1 != mObservers.End(); ++it1) {
            (*counts)[1]++;
            (*it1)->DumpLocked(fd, pw, args, innerName, prefix, pidCounts);
        }
    }

    if (!mChildren.IsEmpty()) {
        if (innerName.IsNull()) {
            if (name.Equals("")) {
                innerName = mName;
            }
            else {
                innerName = name;
                innerName += "/";
                innerName += mName;
            }
        }
        List<AutoPtr<ObserverNode> >::Iterator it2 = mChildren.Begin();
        for (; it2 != mChildren.End(); ++it2) {
            (*counts)[0]++;
            (*it2)->DumpLocked(fd, pw, args, innerName, prefix, counts, pidCounts);
        }
    }
}

String CContentService::ObserverNode::GetUriSegment(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 index)
{
    if (uri != NULL) {
        if (index == 0) {
            String s;
            uri->GetAuthority(&s);
            return s;
        }
        else {
            AutoPtr<IList> pathSegments;
            uri->GetPathSegments((IList**)&pathSegments);
            AutoPtr<IInterface> obj;
            pathSegments->Get(index - 1, (IInterface**)&obj);
            return Object::ToString(obj);
        }
    }

    return String(NULL);
}

Int32 CContentService::ObserverNode::CountUriSegments(
    /* [in] */ IUri* uri)
{
    if (uri == NULL) {
        return 0;
    }

    AutoPtr<IList> pathSegments;
    uri->GetPathSegments((IList**)&pathSegments);
    Int32 size;
    pathSegments->GetSize(&size);
    return size + 1;
}

ECode CContentService::ObserverNode::AddObserverLocked(
    /* [in] */ IUri* uri,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean notifyForDescendants,
    /* [in] */ Object* observersLock,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 userHandle)
{
    return AddObserverLocked(uri, 0, observer, notifyForDescendants,
        observersLock, uid, pid, userHandle);
}

ECode CContentService::ObserverNode::AddObserverLocked(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 index,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean notifyForDescendants,
    /* [in] */ Object* observersLock,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 userHandle)
{
    VALIDATE_NOT_NULL(observer);

    // If this is the leaf node add the observer
    if (index == CountUriSegments(uri)) {
        AutoPtr<ObserverEntry> oberverEntry = new ObserverEntry(
            this, observer, notifyForDescendants, observersLock, uid, pid, userHandle);
        mObservers.PushBack(oberverEntry);
        return NOERROR;
    }

    // Look to see if the proper child already exists
    String segment = GetUriSegment(uri, index);
    if (segment.IsNull()) {
        Slogger::E(TAG, "Invalid Uri [%s] used for observer [%s]", TO_CSTR(uri), TO_CSTR(observer));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    List<AutoPtr<ObserverNode> >::Iterator it = mChildren.Begin();
    for (; it != mChildren.End(); ++it) {
        AutoPtr<ObserverNode> node = *it;
        if (node->mName.Equals(segment)) {
            return node->AddObserverLocked(uri, index + 1, observer, notifyForDescendants,
                observersLock, uid, pid, userHandle);
        }
    }

    // No child found, create one
    AutoPtr<ObserverNode> node = new ObserverNode(segment);
    mChildren.PushBack(node);
    return node->AddObserverLocked(uri, index + 1, observer, notifyForDescendants,
        observersLock, uid, pid, userHandle);
}

Boolean CContentService::ObserverNode::RemoveObserverLocked(
    /* [in] */ IIContentObserver* observer)
{
    assert(observer != NULL);

    List<AutoPtr<ObserverNode> >::Iterator it = mChildren.Begin();
    while (it != mChildren.End()) {
        Boolean empty = (*it)->RemoveObserverLocked(observer);
        if (empty) {
            it = mChildren.Erase(it);
        }
        else {
            ++it;
        }
    }

    AutoPtr<IProxy> proxy = (IProxy*)observer->Probe(EIID_IProxy);
    Boolean result;
    List<AutoPtr<ObserverEntry> >::Iterator it2 = mObservers.Begin();
    for (; it2 != mObservers.End();) {
        AutoPtr<ObserverEntry> entry = *it2;
        if (entry->mObserver.Get() == observer) {
            // We no longer need to listen for death notifications. Remove it.
            if (proxy != NULL) {
                proxy->UnlinkToDeath(entry, 0, &result);
            }
            it2 = mObservers.Erase(it2);
        }
        else {
            ++it2;
        }
    }


    if (mChildren.IsEmpty() && mObservers.IsEmpty()) {
        return TRUE;
    }
    return FALSE;
}

void CContentService::ObserverNode::CollectMyObserversLocked(
    /* [in] */ Boolean leaf,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean observerWantsSelfNotifications,
    /* [in] */ Int32 targetUserHandle,
    /* [in] */ List<AutoPtr<ObserverCall> >* calls)
{
    List<AutoPtr<ObserverEntry> >::Iterator it = mObservers.Begin();
    for (; it != mObservers.End(); ++it) {
        AutoPtr<ObserverEntry> entry = *it;

        // Don't notify the observer if it sent the notification and isn't interested
        // in self notifications
        Boolean selfChange = (entry->mObserver.Get() == observer);
        if (selfChange && !observerWantsSelfNotifications) {
            continue;
        }

        // Does this observer match the target user?
        if (targetUserHandle == IUserHandle::USER_ALL
                || entry->mUserHandle == IUserHandle::USER_ALL
                || targetUserHandle == entry->mUserHandle) {
            // Make sure the observer is interested in the notification
            if (leaf || (!leaf && entry->mNotifyForDescendants)) {
                AutoPtr<ObserverCall> call = new ObserverCall(this, entry->mObserver, selfChange);
                calls->PushBack(call);
            }
        }
    }
}

void CContentService::ObserverNode::CollectObserversLocked(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 index,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean observerWantsSelfNotifications,
    /* [in] */ Int32 targetUserHandle,
    /* [in] */ List<AutoPtr<ObserverCall> >* calls)
{
    String segment;
    Int32 segmentCount = CountUriSegments(uri);
    List<AutoPtr<ObserverNode> >::Iterator it;
    if (index >= segmentCount) {
        // This is the leaf node, notify all observers
        CollectMyObserversLocked(TRUE, observer, observerWantsSelfNotifications, targetUserHandle, calls);
    }
    else if (index < segmentCount) {
        segment = GetUriSegment(uri, index);
        // Notify any observers at this level who are interested in descendants
        CollectMyObserversLocked(FALSE, observer, observerWantsSelfNotifications, targetUserHandle, calls);
    }

    List<AutoPtr<ObserverNode> >::Iterator it2 = mChildren.Begin();
    for (; it2 != mChildren.End(); it2++) {
        AutoPtr<ObserverNode> node = *it2;
        if (segment.IsNull() || node->mName.Equals(segment)) {
            node->CollectObserversLocked(uri, index + 1,
                    observer, observerWantsSelfNotifications, targetUserHandle, calls);
            if (!segment.IsNull()) {
                break;
            }
        }
    }
}

//===================================================================================
// CContentService
//===================================================================================
CAR_INTERFACE_IMPL_2(CContentService, Object, IIContentService, IBinder)

CAR_OBJECT_IMPL(CContentService)

CContentService::CContentService()
    : mFactoryTest(FALSE)
{
    mRootNode = new ObserverNode(String(""));
}

AutoPtr<SyncManager> CContentService::GetSyncManager()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Boolean bval;
    sysProp->GetBoolean(String("config.disable_network"), FALSE, &bval);
    if (bval) {
        return NULL;
    }

    AutoLock lock(mSyncManagerLock);
    // try {
        // Try to create the SyncManager, return NULL if it fails (e.g. the disk is full).
    if (mSyncManager == NULL) {
        mSyncManager = new SyncManager(mContext, mFactoryTest);
    }
    // } catch (SQLiteException e) {
    //     Log.e(TAG, "Can't create SyncManager", e);
    // }
    return mSyncManager;
}

ECode CContentService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0 && "TODO");
    // FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Manifest::permission::DUMP,
    //         "caller doesn't have the DUMP permission");

    // // This makes it so that future permission checks will be in the context of this
    // // process rather than the caller's process. We will restore this before returning.
    // Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    //     if (mSyncManager == NULL) {
    //         pw.println("No SyncManager created!  (Disk full?)");
    //     } else {
    //         mSyncManager.dump(fd, pw);
    //     }
    //     pw.println();
    //     pw.println("Observer tree:");
    //     {    AutoLock syncLock(mRootNode);
    //         int[] counts = new int[2];
    //         final SparseIntArray pidCounts = new SparseIntArray();
    //         mRootNode.dumpLocked(fd, pw, args, "", "  ", counts, pidCounts);
    //         pw.println();
    //         ArrayList<Integer> sorted = new ArrayList<Integer>();
    //         for (int i=0; i<pidCounts.size(); i++) {
    //             sorted.add(pidCounts.keyAt(i));
    //         }
    //         Collections.sort(sorted, new Comparator<Integer>() {
    //             @Override
    //             public int compare(Integer lhs, Integer rhs) {
    //                 int lc = pidCounts.get(lhs);
    //                 int rc = pidCounts.get(rhs);
    //                 if (lc < rc) {
    //                     return 1;
    //                 } else if (lc > rc) {
    //                     return -1;
    //                 }
    //                 return 0;
    //             }

    //         });
    //         for (int i=0; i<sorted.size(); i++) {
    //             int pid = sorted.get(i);
    //             pw.print("  pid "); pw.print(pid); pw.print(": ");
    //                     pw.print(pidCounts.get(pid)); pw.println(" observers");
    //         }
    //         pw.println();
    //         pw.print(" Total number of nodes: "); pw.println(counts[0]);
    //         pw.print(" Total number of observers: "); pw.println(counts[1]);
    //     }
    // } finally {
    //     Binder::RestoreCallingIdentity(identityToken);
    // }
    return E_NOT_IMPLEMENTED;
}

ECode CContentService::SystemReady()
{
    GetSyncManager();
    return NOERROR;
}

ECode CContentService::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendants,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Int32 userHandle)
{
    if (NULL == observer || NULL == uri) {
        Slogger::E(TAG, "RegisterContentObserver: You must pass a valid uri(%s) and observer(%s)",
            TO_CSTR(uri), TO_CSTR(observer));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (DEBUG) {
        Logger::V(TAG, "Registered observer %p : [%s] at [%s] with notifyForDescendants %d",
            observer, TO_CSTR(observer), TO_CSTR(uri), notifyForDescendants);
    }

    FAIL_RETURN(EnforceCrossUserPermission(userHandle,
        String("no permission to observe other users' provider view")))

    if (userHandle < 0) {
        if (userHandle == IUserHandle::USER_CURRENT) {
            AutoPtr<IActivityManagerHelper> amHelper;
            CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
            FAIL_RETURN(amHelper->GetCurrentUser(&userHandle))
        }
        else if (userHandle != IUserHandle::USER_ALL) {
            Slogger::E(TAG, "Bad user handle for registerContentObserver: %d", userHandle);
            return E_INVALID_ARGUMENT;
        }
    }

    AutoLock lock(mRootNode);
    FAIL_RETURN(mRootNode->AddObserverLocked(uri, observer, notifyForDescendants,
        mRootNode.Get(), Binder::GetCallingUid(), Binder::GetCallingPid(), userHandle));
    return NOERROR;
}

ECode CContentService::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendants,
    /* [in] */ IIContentObserver* observer)
{
    return RegisterContentObserver(uri, notifyForDescendants, observer,
            UserHandle::GetCallingUserId());
}

ECode CContentService::UnregisterContentObserver(
    /* [in] */ IIContentObserver* observer)
{
    if (observer == NULL) {
        Slogger::E(TAG, "UnregisterContentObserver: You must pass a valid observer.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (DEBUG) {
        Slogger::V(TAG, "Unregistered observer %p : [%s]", observer, TO_CSTR(observer));
    }

    AutoLock lock(mRootNode);
    mRootNode->RemoveObserverLocked(observer);
    return NOERROR;
}

ECode CContentService::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean observerWantsSelfNotifications,
    /* [in] */ Boolean syncToNetwork,
    /* [in] */ Int32 userHandle)
{
    if (DEBUG) {
        Logger::V(TAG, "Notifying update of %s for user %d from observer %s, syncToNetwork %d",
            TO_CSTR(uri), userHandle, TO_CSTR(observer), syncToNetwork);
    }

    // Notify for any user other than the caller's own requires permission.
    Int32 callingUserHandle = UserHandle::GetCallingUserId();
    if (userHandle != callingUserHandle) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL,
            String("no permission to notify other users")))
    }

    // We passed the permission check; resolve pseudouser targets as appropriate
    if (userHandle < 0) {
        if (userHandle == IUserHandle::USER_CURRENT) {
            AutoPtr<IActivityManagerHelper> amHelper;
            FAIL_RETURN(CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper))
            FAIL_RETURN(amHelper->GetCurrentUser(&userHandle))
        }
        else if (userHandle != IUserHandle::USER_ALL) {
            Slogger::E(TAG, "Bad user handle for notifyChange: %d", userHandle);
            return E_INVALID_ARGUMENT;
            // throw new InvalidParameterException("Bad user handle for notifyChange: "
            //         + userHandle);
        }
    }

    Int32 uid = Binder::GetCallingUid();
    // This makes it so that future permission checks will be in the context of this
    // process rather than the caller's process. We will restore this before returning.
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    List<AutoPtr<ObserverCall> > calls;
    {
        AutoLock lock(mRootNode);
        mRootNode->CollectObserversLocked(uri, 0, observer, observerWantsSelfNotifications,
                userHandle, &calls);
    }
    List<AutoPtr<ObserverCall> >::Iterator it = calls.Begin();
    for (; it != calls.End(); ++it) {
        AutoPtr<ObserverCall> oc = *it;
        if (DEBUG) {
            Slogger::I(TAG, "NotifyChange %s, OnChange: %s", TO_CSTR(oc->mObserver), TO_CSTR(uri));
        }
        // try {
        if (FAILED(oc->mObserver->OnChange(oc->mSelfChange, uri, userHandle))) {
            AutoLock lock(mRootNode);
            Slogger::W(TAG, "Found dead observer, removing");
            List<AutoPtr<ObserverNode::ObserverEntry> >& list = oc->mNode->mObservers;
            List<AutoPtr<ObserverNode::ObserverEntry> >::Iterator it2 = list.Begin();
            while (it2 != list.End()) {
                AutoPtr<ObserverNode::ObserverEntry> oe = *it2;
                if (oe->mObserver == oc->mObserver) {
                    it2 = list.Erase(it2);
                }
                else {
                    ++it2;
                }
            }
        }
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "Notified " + oc.mObserver + " of " + "update at " + uri);
        // }
        // } catch (RemoteException ex) {
        //     {    AutoLock syncLock(mRootNode);
        //         Log.w(TAG, "Found dead observer, removing");
        //         IBinder binder = oc.mObserver.asBinder();
        //         final ArrayList<ObserverNode.ObserverEntry> list
        //                 = oc.mNode.mObservers;
        //         int numList = list.size();
        //         for (int j=0; j<numList; j++) {
        //             ObserverNode.ObserverEntry oe = list.get(j);
        //             if (oe.observer.asBinder() == binder) {
        //                 list.remove(j);
        //                 j--;
        //                 numList--;
        //             }
        //         }
        //     }
        // }
    }
    if (syncToNetwork) {
        AutoPtr<SyncManager> syncManager = GetSyncManager();
        if (syncManager != NULL) {
            String auth;
            uri->GetAuthority(&auth);
            ECode ec = syncManager->ScheduleLocalSync(
                NULL /* all accounts */, callingUserHandle, uid, auth);
            if (FAILED(ec)) {
                Binder::RestoreCallingIdentity(identityToken);
                return ec;
            }
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

ECode CContentService::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean observerWantsSelfNotifications,
    /* [in] */ Boolean syncToNetwork)
{
    return NotifyChange(uri, observer, observerWantsSelfNotifications, syncToNetwork,
        UserHandle::GetCallingUserId());
}

ECode CContentService::RequestSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IContentResolverHelper> contentResolverHelper;
    FAIL_RETURN(CContentResolverHelper::AcquireSingleton((IContentResolverHelper**)&contentResolverHelper))
    FAIL_RETURN(contentResolverHelper->ValidateSyncExtrasBundle(extras))
    Int32 userId = UserHandle::GetCallingUserId();
    Int32 uId = Binder::GetCallingUid();

    // This makes it so that future permission checks will be in the context of this
    // process rather than the caller's process. We will restore this before returning.
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    ECode ec = NOERROR;
    if (syncManager != NULL) {
        ec = syncManager->ScheduleSync(account, userId, uId, authority, extras,
                0 /* no delay */, 0 /* no delay */,
                FALSE /* onlyThoseWithUnkownSyncableState */);
    }
    // } finally {
    //     Binder::RestoreCallingIdentity(identityToken);
    // }
    Binder::RestoreCallingIdentity(identityToken);
    return ec;
}

ECode CContentService::Sync(
    /* [in] */ ISyncRequest* request)
{
    return SyncAsUser(request, UserHandle::GetCallingUserId());
}

ECode CContentService::SyncAsUser(
    /* [in] */ ISyncRequest* request,
    /* [in] */ Int32 userId)
{
    if (request == NULL) {
        Logger::E(TAG, "line:%d, func:%s, request is null", __LINE__, __func__);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    StringBuilder sb("no permission to request sync as user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    Binder::GetCallingUid();
    // This makes it so that future permission checks will be in the context of this
    // process rather than the caller's process. We will restore this before returning.
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager == NULL) {
        Binder::RestoreCallingIdentity(identityToken);
        return NOERROR;
    }

    AutoPtr<IBundle> extras;
    request->GetBundle((IBundle**)&extras);
    Int64 flextime, runAtTime;
    request->GetSyncFlexTime(&flextime);
    request->GetSyncRunTime(&runAtTime);
    Boolean bval;
    request->IsPeriodic(&bval);
    if (bval) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::WRITE_SYNC_SETTINGS,
            String("no permission to write the sync settings")))
        AutoPtr<IAccount> account;
        request->GetAccount((IAccount**)&account);
        String provider;
        request->GetProvider(&provider);
        AutoPtr<EndPoint> info;
        info = new EndPoint(account, provider, userId);
        if (runAtTime < 60) {
            Slogger::W(TAG, "Requested poll frequency of %lld seconds being rounded up to 60 seconds.",
                runAtTime);
            runAtTime = 60;
        }
        // Schedule periodic sync.
        syncManager->GetSyncStorageEngine()->UpdateOrAddPeriodicSync(
            info, runAtTime, flextime, extras);
    }
    else {
        Int32 callerUid = Binder::GetCallingUid();

        Int64 beforeRuntimeMillis = (flextime) * 1000;
        Int64 runtimeMillis = runAtTime * 1000;
        AutoPtr<IAccount> account;
        request->GetAccount((IAccount**)&account);
        String provider;
        request->GetProvider(&provider);
        syncManager->ScheduleSync(
            account, userId, callerUid, provider, extras,
            beforeRuntimeMillis, runtimeMillis,
            FALSE /* onlyThoseWithUnknownSyncableState */);
    }

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::CancelSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname)
{
    return CancelSyncAsUser(account, authority, cname, UserHandle::GetCallingUserId());
}

ECode CContentService::CancelSyncAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname,
    /* [in] */ Int32 userId)
{
    if (authority.IsNullOrEmpty()) {
        Slogger::E(TAG, "Authority must be non-empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb("no permission to modify the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    // This makes it so that future permission checks will be in the context of this
    // process rather than the caller's process. We will restore this before returning.
    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL) {
        AutoPtr<EndPoint> info;
        if (cname == NULL) {
            info = new EndPoint(account, authority, userId);
        }
        else {
            info = new EndPoint(cname, userId);
        }
        syncManager->ClearScheduledSyncOperations(info);
        syncManager->CancelActiveSync(info, NULL /* all syncs for this adapter */);
    }

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::CancelRequest(
    /* [in ]*/ ISyncRequest* request)
{
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager == NULL) return NOERROR;
    Int32 userId = UserHandle::GetCallingUserId();

    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<IBundle> b;
    request->GetBundle((IBundle**)&b);
    AutoPtr<EndPoint> info;
    AutoPtr<IBundle> extras;
    CBundle::New(b, (IBundle**)&extras);
    AutoPtr<IAccount> account;
    request->GetAccount((IAccount**)&account);
    String provider;
    request->GetProvider(&provider);
    info = new EndPoint(account, provider, userId);
    Boolean bval;
    request->IsPeriodic(&bval);
    if (bval) {
        // Remove periodic sync.
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::WRITE_SYNC_SETTINGS,
            String("no permission to write the sync settings")))
        syncManager->GetSyncStorageEngine()->RemovePeriodicSync(info, extras);
    }
    // Cancel active syncs and clear pending syncs from the queue.
    syncManager->CancelScheduledSyncOperation(info, extras);
    syncManager->CancelActiveSync(info, extras);

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::GetSyncAdapterTypes(
    /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result)
{
    return GetSyncAdapterTypesAsUser(UserHandle::GetCallingUserId(), result);
}

ECode CContentService::GetSyncAdapterTypesAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    StringBuilder sb("no permission to read the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    // This makes it so that future permission checks will be in the context of this
    // process rather than the caller's process. We will restore this before returning.
    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    AutoPtr<ArrayOf<ISyncAdapterType*> > types = syncManager->GetSyncAdapterTypes(userId);
    *result = types;
    REFCOUNT_ADD(*result)

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::GetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [out] */ Boolean* result)
{
    return GetSyncAutomaticallyAsUser(account, providerName, UserHandle::GetCallingUserId(), result);
}

ECode CContentService::GetSyncAutomaticallyAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    StringBuilder sb("no permission to read the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_SETTINGS,
        String("no permission to read the sync settings")))

    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL) {
        *result = syncManager->GetSyncStorageEngine()->GetSyncAutomatically(account, userId, providerName);
    }

    Binder::RestoreCallingIdentity(identityToken);

    return NOERROR;
}

ECode CContentService::SetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Boolean sync)
{
    return SetSyncAutomaticallyAsUser(account, providerName, sync, UserHandle::GetCallingUserId());
}

ECode CContentService::SetSyncAutomaticallyAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Boolean sync,
    /* [in] */ Int32 userId)
{
    if (TextUtils::IsEmpty(providerName)) {
        Slogger::E(TAG, "Authority must be non-empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Manifest::permission::WRITE_SYNC_SETTINGS,
        String("no permission to write the sync settings")))

    StringBuilder sb("no permission modify read the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL) {
        syncManager->GetSyncStorageEngine()->SetSyncAutomatically(account, userId, providerName, sync);
    }

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::AddPeriodicSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 pollFrequency)
{
    if (account == NULL) {
        Slogger::E(TAG, "Account must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (TextUtils::IsEmpty(authority)) {
        Slogger::E(TAG, "Authority must not be empty.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::WRITE_SYNC_SETTINGS,
        String("no permission to write the sync settings")))

    Int32 userId = UserHandle::GetCallingUserId();
    if (pollFrequency < 60) {
        Slogger::W(TAG, "Requested poll frequency of %lld"
            " seconds being rounded up to 60 seconds.", pollFrequency);
        pollFrequency = 60;
    }
    Int64 defaultFlex = SyncStorageEngine::CalculateDefaultFlexTime(pollFrequency);

    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<EndPoint> info = new EndPoint(account, authority, userId);
    GetSyncManager()->GetSyncStorageEngine()->UpdateOrAddPeriodicSync(
        info, pollFrequency, defaultFlex, extras);

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::RemovePeriodicSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras)
{
    if (account == NULL) {
        Slogger::E(TAG, "Account must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (TextUtils::IsEmpty(authority)) {
        Slogger::E(TAG, "Authority must not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::WRITE_SYNC_SETTINGS,
        String("no permission to write the sync settings")))

    Int32 userId = UserHandle::GetCallingUserId();
    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<EndPoint> ep = new EndPoint(account, authority, userId);
    GetSyncManager()->GetSyncStorageEngine()->RemovePeriodicSync(
        ep, extras);

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::GetPeriodicSyncs(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [out] */ IList** periodicSyncList)
{
    VALIDATE_NOT_NULL(periodicSyncList)
    *periodicSyncList = NULL;

    if (account == NULL) {
        Slogger::E(TAG, "Account must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (TextUtils::IsEmpty(providerName)) {
        Slogger::E(TAG, "Authority must not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_SETTINGS,
        String("no permission to read the sync settings")))

    Int32 userId = UserHandle::GetCallingUserId();
    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    AutoPtr<EndPoint> ep = new EndPoint(account, providerName, userId);
    AutoPtr<List<AutoPtr<IPeriodicSync> > > list;
    list = syncManager->GetSyncStorageEngine()->GetPeriodicSyncs(ep);

    if (list != NULL) {
        Boolean bval;
        AutoPtr<IArrayList> al;
        CArrayList::New((IArrayList**)&al);
        List<AutoPtr<IPeriodicSync> >::Iterator it;
        for (it = list->Begin(); it != list->End(); ++it) {
            al->Add(TO_IINTERFACE(*it), &bval);
        }

        *periodicSyncList = IList::Probe(al);
        REFCOUNT_ADD(*periodicSyncList)
    }

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::GetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [out] */ Int32* value)
{
    return GetIsSyncableAsUser(account, providerName, UserHandle::GetCallingUserId(), value);
}

ECode CContentService::GetIsSyncableAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;

    StringBuilder sb("no permission read the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_SETTINGS,
        String("no permission to read the sync settings")))

    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL) {
        *value = syncManager->GetIsSyncable(account, userId, providerName);
    }

    Binder::RestoreCallingIdentity(identityToken);

    return NOERROR;
}

ECode CContentService::SetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Int32 syncable)
{
    if (TextUtils::IsEmpty(providerName)) {
        Slogger::E(TAG, "Authority must not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::WRITE_SYNC_SETTINGS,
        String("no permission to write the sync settings")))

    Int32 userId = UserHandle::GetCallingUserId();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL) {
        syncManager->GetSyncStorageEngine()->SetIsSyncable(
            account, userId, providerName, syncable);
    }

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::GetMasterSyncAutomatically(
    /* [out] */ Boolean* result)
{
    return GetMasterSyncAutomaticallyAsUser(UserHandle::GetCallingUserId(), result);
}

ECode CContentService::GetMasterSyncAutomaticallyAsUser(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    StringBuilder sb("no permission read the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_SETTINGS,
        String("no permission to read the sync settings")))

    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL) {
        *result = syncManager->GetSyncStorageEngine()->GetMasterSyncAutomatically(userId);
    }
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::SetMasterSyncAutomatically(
    /* [in] */ Boolean flag)
{
    return SetMasterSyncAutomaticallyAsUser(flag, UserHandle::GetCallingUserId());
}

ECode CContentService::SetMasterSyncAutomaticallyAsUser(
    /* [in] */ Boolean flag,
    /* [in] */ Int32 userId)
{
    StringBuilder sb("no permission set the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::WRITE_SYNC_SETTINGS,
        String("no permission to write the sync settings")))

    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL) {
        syncManager->GetSyncStorageEngine()->SetMasterSyncAutomatically(flag, userId);
    }

    Binder::RestoreCallingIdentity(identityToken);

    return NOERROR;
}

ECode CContentService::IsSyncActive(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname,
    /* [out] */ Boolean* isActive)
{
    VALIDATE_NOT_NULL(isActive)
    *isActive = FALSE;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_STATS,
        String("no permission to read the sync stats")))
    Int32 userId = UserHandle::GetCallingUserId();
    Binder::GetCallingUid();
    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager == NULL) {
        return NOERROR;
    }

    AutoPtr<EndPoint> ep = new EndPoint(account, authority, userId);
    *isActive = syncManager->GetSyncStorageEngine()->IsSyncActive(ep);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::GetCurrentSyncs(
    /* [out] */ IList** syncInfoList)
{
    return GetCurrentSyncsAsUser(UserHandle::GetCallingUserId(), syncInfoList);
}

ECode CContentService::GetCurrentSyncsAsUser(
    /* [in] */ Int32 userId,
    /* [out] */ IList** syncInfoList)
{
    VALIDATE_NOT_NULL(syncInfoList)
    *syncInfoList = NULL;

    StringBuilder sb("no permission read the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_STATS,
        String("no permission to read the sync stats")))

    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    AutoPtr<IList> list = syncManager->GetSyncStorageEngine()->GetCurrentSyncsCopy(userId);
    *syncInfoList = list;
    REFCOUNT_ADD(*syncInfoList)

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::GetSyncStatus(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname,
    /* [out] */ ISyncStatusInfo** result)
{
    return GetSyncStatusAsUser(account, authority, cname, UserHandle::GetCallingUserId(), result);
}

ECode CContentService::GetSyncStatusAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname,
    /* [in] */ Int32 userId,
    /* [out] */ ISyncStatusInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (TextUtils::IsEmpty(authority)) {
        Slogger::E(TAG, "Authority must not be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    StringBuilder sb("no permission read the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_STATS,
        String("no permission to read the sync stats")))

    Binder::GetCallingUid();
    Int64 identityToken = Binder::ClearCallingIdentity();

    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager == NULL) {
        return NOERROR;
    }

    AutoPtr<EndPoint> info;
    if (!(account == NULL || authority == NULL)) {
        info = new EndPoint(account, authority, userId);
    }
    else {
        Slogger::E(TAG, "Must call sync status with valid authority");
    }
    AutoPtr<ISyncStatusInfo> ssi = syncManager->GetSyncStorageEngine()->GetStatusByAuthority(info);
    *result = ssi;
    REFCOUNT_ADD(*result)

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::IsSyncPending(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname,
    /* [out] */ Boolean* isPending)
{
    return IsSyncPendingAsUser(account, authority, cname, UserHandle::GetCallingUserId(), isPending);
}

ECode CContentService::IsSyncPendingAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* isPending)
{
    VALIDATE_NOT_NULL(isPending)
    *isPending = FALSE;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::READ_SYNC_STATS,
        String("no permission to read the sync stats")))

    StringBuilder sb("no permission retrieve the sync settings for user: ");
    sb += userId;
    FAIL_RETURN(EnforceCrossUserPermission(userId, sb.ToString()))

    Binder::GetCallingUid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager == NULL) return NOERROR;

    AutoPtr<EndPoint> info;
    if (!(account == NULL || authority == NULL)) {
        info = new EndPoint(account, authority, userId);
    }
    else {
        Slogger::E(TAG, "Invalid authority specified");
    }

    *isPending = syncManager->GetSyncStorageEngine()->IsSyncPending(info);

    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::AddStatusChangeListener(
    /* [in] */ Int32 mask,
    /* [in] */ IISyncStatusObserver* callback)
{
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL && callback != NULL) {
        syncManager->GetSyncStorageEngine()->AddStatusChangeListener(mask, callback);
    }
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::RemoveStatusChangeListener(
    /* [in] */ IISyncStatusObserver* callback)
{
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<SyncManager> syncManager = GetSyncManager();
    if (syncManager != NULL && callback != NULL) {
        syncManager->GetSyncStorageEngine()->RemoveStatusChangeListener(callback);
    }
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
}

ECode CContentService::EnforceCrossUserPermission(
    /* [in] */ Int32 userHandle,
    /* [in] */ const String& message)
{
    Int32 callingUser = UserHandle::GetCallingUserId();
    if (callingUser != userHandle) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::INTERACT_ACROSS_USERS_FULL, message))
    }
    return NOERROR;
}

AutoPtr<IIContentService> CContentService::Main(
    /* [in] */ IContext* context,
    /* [in] */ Boolean factoryTest)
{
    AutoPtr<IIContentService> service;
    CContentService::New(context, factoryTest, (IIContentService**)&service);
    AutoPtr<IServiceManager> mgr;
    CServiceManager::AcquireSingleton((IServiceManager**)&mgr);
    mgr->AddService(IContentResolver::CONTENT_SERVICE_NAME, service.Get());
    return service;
}

ECode CContentService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean factoryTest)
{
    mContext = context;
    mFactoryTest = factoryTest;
    return NOERROR;
}

}
}
}
}

