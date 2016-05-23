
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/server/clipboard/ClipboardService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.SecurityBridge.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::Content::CContentProviderHelper;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IIClipboard;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentProviderHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Clipboard {

const String ClipboardService::TAG("ClipboardService");
const String ClipboardService::SECURITY_BRIDGE_NAME("com.android.services.SecurityBridge.core.ClipboardManagerSB");

//==============================================================================
//          ClipboardService::ListenerInfo
//==============================================================================

ClipboardService::ListenerInfo::ListenerInfo(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
    : mUid(uid)
    , mPackageName(packageName)
{
}

ClipboardService::ListenerInfo::~ListenerInfo()
{}

//==============================================================================
//          ClipboardService::PerUserClipboard
//==============================================================================

ClipboardService::PerUserClipboard::PerUserClipboard(
    /* [in] */ Int32 userId)
    : mUserId(userId)
{
    CRemoteCallbackList::New((IRemoteCallbackList**)&mPrimaryClipListeners);
    CHashSet::New((IHashSet**)&mActivePermissionOwners);
}

ClipboardService::PerUserClipboard::~PerUserClipboard()
{}

//==============================================================================
//          ClipboardService::MyBroadcastReceiver
//==============================================================================

ClipboardService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ ClipboardService* host)
    : mHost(host)
{}

ClipboardService::MyBroadcastReceiver::~MyBroadcastReceiver()
{}

ECode ClipboardService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        Int32 value;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &value);
        mHost->RemoveClipboard(value);
    }
    return NOERROR;
}

//==============================================================================
//          ClipboardService
//==============================================================================

CAR_INTERFACE_IMPL_2(ClipboardService, Object, IIClipboard, IBinder);

ClipboardService::ClipboardService()
{}

ClipboardService::~ClipboardService()
{}

ECode ClipboardService::constructor(
    /* [in] */ IContext *context)
{
    mContext = context;
    mAm = ActivityManagerNative::GetDefault();
    context->GetPackageManager((IPackageManager**)&mPm);
    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;
    serviceManager->GetService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUm = IIUserManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    mAppOps = IAppOpsManager::Probe(obj);
    CSparseArray::New((ISparseArray**)&mClipboards);

    AutoPtr<IBinder> permOwner;
    if (FAILED(mAm->NewUriPermissionOwner(String("clipboard"), (IBinder**)&permOwner))) {
        Slogger::W("clipboard", "AM dead");
    }

    mPermissionOwner = permOwner;

    // Remove the clipboard if a user is removed
    AutoPtr<IIntentFilter> userFilter;
    CIntentFilter::New((IIntentFilter**)&userFilter);
    userFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    AutoPtr<IBroadcastReceiver> myBR = (IBroadcastReceiver*)new MyBroadcastReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(myBR, userFilter, (IIntent**)&intent);

    // try {
    /*
     * load and create the security bridge
     */
    Slogger::E(TAG, "TODO: load and create the security bridge");
    AutoPtr<IInterface> bridgeObject;
    // bridgeObject = getClass().getClassLoader().loadClass(SECURITY_BRIDGE_NAME).newInstance();
    mSecurityBridge = IClipboardManagerMonitor::Probe(bridgeObject);

    // } catch (Exception e){
        // Slogger::W(TAG, "No security bridge jar found, using default");
        // mSecurityBridge = new ClipboardManagerMonitor();
    // }
    return NOERROR;
}

AutoPtr<ClipboardService::PerUserClipboard> ClipboardService::GetClipboard()
{
    return GetClipboard(UserHandle::GetCallingUserId());
}

AutoPtr<ClipboardService::PerUserClipboard> ClipboardService::GetClipboard(
    /* [in] */ Int32 userId)
{
    AutoPtr<PerUserClipboard> puc;
    {    AutoLock syncLock(mClipboards);
        AutoPtr<IInterface> obj;
        mClipboards->Get(userId, (IInterface**)&obj);
        puc = (PerUserClipboard*)IObject::Probe(obj);

        if (puc == NULL) {
            puc = new PerUserClipboard(userId);
            mClipboards->Put(userId, (IObject*)puc);
        }
    }
    return puc;
}

void ClipboardService::RemoveClipboard(
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mClipboards);
        mClipboards->Remove(userId);
    }
}

ECode ClipboardService::SetPrimaryClip(
    /* [in] */ IClipData* clip,
    /* [in] */ const String& callingPackage)
{
    {    AutoLock syncLock(this);
        Int32 itemCount;
        if (clip != NULL && (clip->GetItemCount(&itemCount), itemCount <= 0)) {
            Slogger::E(TAG, "No items");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("No items");
        }

        const Int32 callingUid = Binder::GetCallingUid();
        Int32 value;
        mAppOps->NoteOp(IAppOpsManager::OP_WRITE_CLIPBOARD, callingUid,
                callingPackage, &value);
        if (value != IAppOpsManager::MODE_ALLOWED) {
            return NOERROR;
        }
        CheckDataOwnerLocked(clip, callingUid);
        const Int32 userId = UserHandle::GetUserId(callingUid);
        AutoPtr<PerUserClipboard> clipboard = GetClipboard(userId);
        RevokeUris(clipboard);
        if (mSecurityBridge) {
            FAIL_RETURN(mSecurityBridge->NotifyCopy(Binder::GetCallingUid(), clip))
        }
        SetPrimaryClipInternal(clipboard, clip);
        AutoPtr<IList> related;
        GetRelatedProfiles(userId, (IList**)&related);
        if (related != NULL) {
            Int32 size;
            related->GetSize(&size);
            if (size > 1) { // Related profiles list include the current profile.
                Boolean canCopy = FALSE;
                // try {
                AutoPtr<IBundle> bundle;
                ECode ec = mUm->GetUserRestrictions(userId, (IBundle**)&bundle);
                if (SUCCEEDED(ec)) {
                    Boolean res;
                    bundle->GetBoolean(
                            IUserManager::DISALLOW_CROSS_PROFILE_COPY_PASTE, &res);
                    canCopy = !res;
                }
                else {
                    Slogger::E(TAG, "Remote Exception calling UserManager: " );
                }
                // } catch (RemoteException e) {
                //     Slog.e(TAG, "Remote Exception calling UserManager: " + e);
                // }

                // Copy clip data to related users if allowed. If disallowed, then remove
                // primary clip in related users to prevent pasting stale content.
                if (!canCopy) {
                    clip = NULL;
                }
                else {
                    clip->FixUrisLight(userId);
                }
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> obj;
                    related->Get(i, (IInterface**)&obj);
                    AutoPtr<IUserInfo> info = IUserInfo::Probe(obj);
                    Int32 id;
                    info->GetId(&id);
                    if (id != userId) {
                        SetPrimaryClipInternal(GetClipboard(id), clip);
                    }
                }
            }
        }
    }
    return NOERROR;
}

ECode ClipboardService::GetRelatedProfiles(
    /* [in] */ Int32 userId,
    /* [out] */ IList** userInfo)
{
    VALIDATE_NOT_NULL(userInfo);
    *userInfo = NULL;

    AutoPtr<IList> related;
    const Int64 origId = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mUm->GetProfiles(userId, TRUE, (IList**)&related);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Remote Exception calling UserManager: ");
        Binder::RestoreCallingIdentity(origId);
        return NOERROR;
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Remote Exception calling UserManager: " + e);
    //     return null;
    // } finally{
    Binder::RestoreCallingIdentity(origId);
    // }
    *userInfo = related;
    REFCOUNT_ADD(*userInfo);
    return NOERROR;
}

ECode ClipboardService::SetPrimaryClipInternal(
    /* [in] */ PerUserClipboard* clipboard,
    /* [in] */ IClipData* clip)
{
    clipboard->mActivePermissionOwners->Clear();
    if (clip == NULL && clipboard->mPrimaryClip == NULL) {
        return NOERROR;
    }
    clipboard->mPrimaryClip = clip;
    const Int64 ident = Binder::ClearCallingIdentity();
    Int32 n;
    clipboard->mPrimaryClipListeners->BeginBroadcast(&n);
    // try {
    for (Int32 i = 0; i < n; i++) {
        // try {
        AutoPtr<IInterface> obj;
        clipboard->mPrimaryClipListeners->GetBroadcastCookie(i, (IInterface**)&obj);
        AutoPtr<ListenerInfo> li = (ListenerInfo*)IObject::Probe(obj);
        Int32 value;
        mAppOps->CheckOpNoThrow(IAppOpsManager::OP_READ_CLIPBOARD, li->mUid,
                li->mPackageName, &value);
        if (value == IAppOpsManager::MODE_ALLOWED) {
            AutoPtr<IInterface> object;
            clipboard->mPrimaryClipListeners->GetBroadcastItem(i, (IInterface**)&object);
            IOnPrimaryClipChangedListener::Probe(object)->DispatchPrimaryClipChanged();
        }
        // } catch (RemoteException e) {
        //     // The RemoteCallbackList will take care of removing
        //     // the dead object for us.
        // }
    }
    // } finally {
    clipboard->mPrimaryClipListeners->FinishBroadcast();
    Binder::RestoreCallingIdentity(ident);
    // }

    return NOERROR;
}

ECode ClipboardService::GetPrimaryClip(
    /* [in] */ const String& pkg,
    /* [out] */ IClipData** clip)
{
    VALIDATE_NOT_NULL(clip);
    *clip = NULL;

    {    AutoLock syncLock(this);
        Int32 value;
        mAppOps->NoteOp(IAppOpsManager::OP_READ_CLIPBOARD, Binder::GetCallingUid(),
                pkg, &value);
        if (value != IAppOpsManager::MODE_ALLOWED) {
            return NOERROR;
        }
        FAIL_RETURN(AddActiveOwnerLocked(Binder::GetCallingUid(), pkg));
        AutoPtr<IClipData> _clip = GetClipboard()->mPrimaryClip;
        if (_clip != NULL) {
            Boolean flag = TRUE;
            if (mSecurityBridge) {
                mSecurityBridge->ApprovePasteRequest(Binder::GetCallingUid(), _clip.Get(), &flag);
            }
            if (!flag) {
                _clip = NULL;
            }
        }
        *clip = _clip;
        REFCOUNT_ADD(*clip);
        return NOERROR;
    }
    return NOERROR;
}

ECode ClipboardService::GetPrimaryClipDescription(
    /* [in] */ const String& callingPackage,
    /* [out] */ IClipDescription** description)
{
    VALIDATE_NOT_NULL(description)
    *description = NULL;

    {    AutoLock syncLock(this);
        Int32 value;
        mAppOps->CheckOp(IAppOpsManager::OP_READ_CLIPBOARD, Binder::GetCallingUid(),
                callingPackage, &value);
        if (value != IAppOpsManager::MODE_ALLOWED) {
            return NOERROR;
        }

        AutoPtr<PerUserClipboard> clipboard = GetClipboard();
        if (clipboard->mPrimaryClip != NULL) {
            AutoPtr<IClipDescription> des;
            clipboard->mPrimaryClip->GetDescription((IClipDescription**)&des);
            *description = des;
            REFCOUNT_ADD(*description);
        }

        return NOERROR;
    }
    return NOERROR;
}

ECode ClipboardService::HasPrimaryClip(
    /* [in] */ const String& callingPackage,
    /* [out] */ Boolean* hasPrimaryClip)
{
    VALIDATE_NOT_NULL(hasPrimaryClip);
    *hasPrimaryClip = FALSE;

    {    AutoLock syncLock(this);
        Int32 value;
        mAppOps->CheckOp(IAppOpsManager::OP_READ_CLIPBOARD, Binder::GetCallingUid(),
                callingPackage, &value);
        if (value != IAppOpsManager::MODE_ALLOWED) {
            return NOERROR;
        }

        Boolean hasClip = FALSE;
        AutoPtr<IClipData> cd = GetClipboard()->mPrimaryClip;
        if (cd != NULL) {
            if (mSecurityBridge) {
                mSecurityBridge->ApprovePasteRequest(Binder::GetCallingUid(), cd, &hasClip);
            }
            else {
                hasClip = TRUE;
            }
        }
        *hasPrimaryClip = hasClip;
        return NOERROR;
    }
    return NOERROR;
}

ECode ClipboardService::AddPrimaryClipChangedListener(
    /* [in] */ IOnPrimaryClipChangedListener* listener,
    /* [in] */ const String& callingPackage)
{
    {    AutoLock syncLock(this);
        AutoPtr<ListenerInfo> info = new ListenerInfo(Binder::GetCallingUid(), callingPackage);
        Boolean result;
        return GetClipboard()->mPrimaryClipListeners->Register(listener, TO_IINTERFACE(info), &result);
    }
    return NOERROR;
}

ECode ClipboardService::RemovePrimaryClipChangedListener(
    /* [in] */ IOnPrimaryClipChangedListener* listener)
{
    {    AutoLock syncLock(this);
        Boolean result;
        return GetClipboard()->mPrimaryClipListeners->Unregister(listener, &result);
    }
    return NOERROR;
}

ECode ClipboardService::HasClipboardText(
    /* [in] */ const String& callingPackage,
    /* [out] */ Boolean* hasClipboardText)
{
    VALIDATE_NOT_NULL(hasClipboardText);
    *hasClipboardText = FALSE;

    {    AutoLock syncLock(this);
        Int32 value;
        mAppOps->CheckOp(IAppOpsManager::OP_READ_CLIPBOARD, Binder::GetCallingUid(),
                callingPackage, &value);
        if (value != IAppOpsManager::MODE_ALLOWED) {
            return NOERROR;
        }

        AutoPtr<PerUserClipboard> clipboard = GetClipboard();
        if (clipboard->mPrimaryClip != NULL) {
            AutoPtr<IClipDataItem> item;
            clipboard->mPrimaryClip->GetItemAt(0, (IClipDataItem**)&item);
            AutoPtr<ICharSequence> text;
            item->GetText((ICharSequence**)&text);
            if (text != NULL) {
                Int32 length;
                text->GetLength(&length);
                *hasClipboardText = length > 0;
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode ClipboardService::CheckUriOwnerLocked(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 uid)
{
    String scheme;
    uri->GetScheme(&scheme);
    if (!String("content").Equals(scheme)) {
        return NOERROR;
    }
    Int64 ident = Binder::ClearCallingIdentity();
    //try {
    // This will throw SecurityException for us.

    AutoPtr<IContentProviderHelper> helper;
    CContentProviderHelper::AcquireSingleton((IContentProviderHelper**)&helper);

    AutoPtr<IUri> baseUri;
    helper->GetUriWithoutUserId(uri, (IUri**)&baseUri);
    Int32 userId;
    helper->GetUserIdFromUri(uri, UserHandle::GetUserId(uid), &userId);
    Int32 res;
    ECode ec = mAm->CheckGrantUriPermission(uid, String(NULL), baseUri,
            IIntent::FLAG_GRANT_READ_URI_PERMISSION,
            userId, &res);
    Binder::RestoreCallingIdentity(ident);
    return ec;
    //} catch (RemoteException e) {
    //} finally {
        // Binder::RestoreCallingIdentity(ident);
    //}
}

ECode ClipboardService::CheckItemOwnerLocked(
    /* [in] */ IClipDataItem* item,
    /* [in] */ Int32 uid)
{
    AutoPtr<IUri> uri;
    item->GetUri((IUri**)&uri);
    if (uri != NULL) {
        FAIL_RETURN(CheckUriOwnerLocked(uri, uid))
    }
    AutoPtr<IIntent> intent;
    item->GetIntent((IIntent**)&intent);
    uri = NULL;
    if (intent != NULL && (intent->GetData((IUri**)&uri), uri != NULL)) {
        return CheckUriOwnerLocked(uri, uid);
    }
    return NOERROR;
}

ECode ClipboardService::CheckDataOwnerLocked(
    /* [in] */ IClipData* data,
    /* [in] */ Int32 uid)
{
    VALIDATE_NOT_NULL(data);
    Int32 N;
    data->GetItemCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IClipDataItem> item;
        data->GetItemAt(i, (IClipDataItem**)&item);
        FAIL_RETURN(CheckItemOwnerLocked(item, uid));
    }
    return NOERROR;
}

void ClipboardService::GrantUriLocked(
    /* [in] */ IUri* uri,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    Int64 ident = Binder::ClearCallingIdentity();
    //try {

    AutoPtr<IContentProviderHelper> helper;
    CContentProviderHelper::AcquireSingleton((IContentProviderHelper**)&helper);

    Int32 sourceUserId;
    helper->GetUserIdFromUri(uri, userId, &sourceUserId);
    AutoPtr<IUri> baseUri;
    helper->GetUriWithoutUserId(uri, (IUri**)&baseUri);

    mAm->GrantUriPermissionFromOwner(mPermissionOwner, Process::MyUid(), pkg, baseUri,
            IIntent::FLAG_GRANT_READ_URI_PERMISSION, sourceUserId, userId);
    Binder::RestoreCallingIdentity(ident);
    //} catch (RemoteException e) {
    //} finally {
        // Binder::RestoreCallingIdentity(ident);
    //}
}

void ClipboardService::GrantItemLocked(
    /* [in] */ IClipDataItem* item,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userId)
{
    AutoPtr<IUri> uri;
    if (item->GetUri((IUri**)&uri), uri != NULL) {
        GrantUriLocked(uri, pkg, userId);
    }
    AutoPtr<IIntent> intent;
    item->GetIntent((IIntent**)&intent);
    uri = NULL;
    if (intent != NULL && (intent->GetData((IUri**)&uri), uri != NULL)) {
        GrantUriLocked(uri, pkg, userId);
    }
}

ECode ClipboardService::AddActiveOwnerLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& pkg)
{
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
    Int32 targetUserHandle = UserHandle::GetCallingUserId();
    Int64 oldIdentity = Binder::ClearCallingIdentity();
    //try {
    AutoPtr<IPackageInfo> pi;
    pm->GetPackageInfo(pkg, 0, targetUserHandle, (IPackageInfo**)&pi);
    if (pi == NULL) {
        //throw new IllegalArgumentException("Unknown package " + pkg);
        Slogger::E("clipboard", "Unknown package %s", pkg.string());
        Binder::RestoreCallingIdentity(oldIdentity);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IApplicationInfo> info;
    pi->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 i;
    info->GetUid(&i);
    if (!UserHandle::IsSameApp(i, uid)) {
        //throw new SecurityException("Calling uid " + uid
        //        + " does not own package " + pkg);
        Slogger::E("clipboard", "Calling uid %d does not own package %s", uid, pkg.string());
        Binder::RestoreCallingIdentity(oldIdentity);
        return E_SECURITY_EXCEPTION;
    }
    //} catch (RemoteException e) {
        // Can't happen; the package manager is in the same process
    //} finally {
    Binder::RestoreCallingIdentity(oldIdentity);
    //}
    AutoPtr<PerUserClipboard> clipboard = GetClipboard();
    Boolean res;
    clipboard->mActivePermissionOwners->Contains(CoreUtils::Convert(pkg), &res);

    if (clipboard->mPrimaryClip != NULL && !res) {
        Int32 N;
        clipboard->mPrimaryClip->GetItemCount(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IClipDataItem> item;
            clipboard->mPrimaryClip->GetItemAt(i, (IClipDataItem**)&item);
            GrantItemLocked(item, pkg, UserHandle::GetUserId(uid));
        }
        clipboard->mActivePermissionOwners->Add(CoreUtils::Convert(pkg));
    }

    return NOERROR;
}

void ClipboardService::RevokeUriLocked(
    /* [in] */ IUri* uri)
{
    AutoPtr<IContentProviderHelper> helper;
    CContentProviderHelper::AcquireSingleton((IContentProviderHelper**)&helper);
    Int32 userId;
    helper->GetUserIdFromUri(uri, UserHandle::GetUserId(Binder::GetCallingUid()), &userId);

    Int64 ident = Binder::ClearCallingIdentity();

    //try {
    AutoPtr<IUri> baseUri;
    helper->GetUriWithoutUserId(uri, (IUri**)&baseUri);

    mAm->RevokeUriPermissionFromOwner(mPermissionOwner, baseUri,
            IIntent::FLAG_GRANT_READ_URI_PERMISSION | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION,
            userId);
    //} catch (RemoteException e) {
    //} finally {
    Binder::RestoreCallingIdentity(ident);
    //}
}

void ClipboardService::RevokeItemLocked(
    /* [in] */ IClipDataItem* item)
{
    AutoPtr<IUri> uri;
    item->GetUri((IUri**)&uri);
    if (uri != NULL) {
        RevokeUriLocked(uri);
    }
    AutoPtr<IIntent> intent;
    item->GetIntent((IIntent**)&intent);
    uri = NULL;
    if (intent != NULL && (intent->GetData((IUri**)&uri), uri != NULL)) {
        RevokeUriLocked(uri);
    }
}

void ClipboardService::RevokeUris(
    /* [in] */ PerUserClipboard* clipboard)
{
    if (clipboard->mPrimaryClip == NULL) {
        return;
    }
    Int32 N;
    clipboard->mPrimaryClip->GetItemCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IClipDataItem> item;
        clipboard->mPrimaryClip->GetItemAt(i, (IClipDataItem**)&item);
        RevokeItemLocked(item);
    }
}

}//namespace Clipboard
}//namespace Server
}//namesapce Droid
}//namespace Elastos
