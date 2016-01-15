#include "CStatusBarManagerService.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::StatusBar::IIStatusBarService;
using Elastos::Droid::StatusBar::CStatusBarIconList;
using Elastos::Droid::StatusBar::IStatusBarIcon;
using Elastos::Droid::StatusBar::CStatusBarIcon;
using Elastos::Droid::Utility::CParcelableObjectContainer;

namespace Elastos {
namespace Droid {
namespace Server {

const String CStatusBarManagerService::TAG("StatusBarManagerService");
const Boolean CStatusBarManagerService::SPEW = FALSE;

//==================================================================================
//              CStatusBarManagerService::DisableRecord
//==================================================================================
CAR_INTERFACE_IMPL(CStatusBarManagerService::DisableRecord, IProxyDeathRecipient);

CStatusBarManagerService::DisableRecord::DisableRecord(
    /* [in] */ CStatusBarManagerService* host)
    : mUserId(0)
    , mPkg(String(""))
    , mWhat(0)
    , mHost(host)
{
}

ECode CStatusBarManagerService::DisableRecord::ProxyDied()
{
    Slogger::I(TAG, "binder died for pkg=%s", mPkg.string());
    FAIL_RETURN(mHost->DisableInternal(mUserId, 0, mToken, mPkg));
    AutoPtr<IProxy> proxy = (IProxy*)mToken->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath(this, 0, &result);
    }
    return NOERROR;
}


//==================================================================================
//              CStatusBarManagerService::DisableNotificationRunnable
//==================================================================================
CStatusBarManagerService::DisableNotificationRunnable::DisableNotificationRunnable(
    /* [in] */ Int32 status,
    /* [in] */ CStatusBarManagerService* host)
    : mStatus(status)
    , mHost(host)
{}

ECode CStatusBarManagerService::DisableNotificationRunnable::Run()
{
    if (mHost->mNotificationCallbacks) {
        mHost->mNotificationCallbacks->OnSetDisabled(mStatus);
    }
    return NOERROR;
}


//==================================================================================
//              CStatusBarManagerService::TopAppWindowChangedRunnable
//==================================================================================
CStatusBarManagerService::TopAppWindowChangedRunnable::TopAppWindowChangedRunnable(
    /* [in] */ Boolean menuVisible,
    /* [in] */ CStatusBarManagerService* host)
    : mMenuVisible(menuVisible)
    , mHost(host)
{}

ECode CStatusBarManagerService::TopAppWindowChangedRunnable::Run()
{
    if (mHost->mBar != NULL) {
        mHost->mBar->TopAppWindowChanged(mMenuVisible);
    }
    return NOERROR;
}


//==================================================================================
//              CStatusBarManagerService::SetImeWindowStatusRunnable
//==================================================================================
CStatusBarManagerService::SetImeWindowStatusRunnable::SetImeWindowStatusRunnable(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition,
    /* [in] */ CStatusBarManagerService* host)
    : mToken(token)
    , mVisible(vis)
    , mBackDisposition(backDisposition)
    , mHost(host)
{
}

ECode CStatusBarManagerService::SetImeWindowStatusRunnable::Run()
{
    if (mHost->mBar != NULL) {
        // try {
            mHost->mBar->SetImeWindowStatus(mToken, mVisible, mBackDisposition);
        // } catch (RemoteException ex) {
        // }
    }
    return NOERROR;
}


//==================================================================================
//              CStatusBarManagerService::UpdateUiVisibilityRunnable
//==================================================================================
CStatusBarManagerService::UpdateUiVisibilityRunnable::UpdateUiVisibilityRunnable(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask,
    /* [in] */ CStatusBarManagerService* host)
    : mVis(vis)
    , mMask(mask)
    , mHost(host)
{
}

ECode CStatusBarManagerService::UpdateUiVisibilityRunnable::Run()
{
    if (mHost->mBar != NULL) {
        mHost->mBar->SetSystemUiVisibility(mVis, mMask);
    }
    return NOERROR;
}


//==================================================================================
//              CStatusBarManagerService::SetHardKeyboardEnabledRunnable
//==================================================================================
CStatusBarManagerService::SetHardKeyboardEnabledRunnable::SetHardKeyboardEnabledRunnable(
    /* [in] */ Boolean enable,
    /* [in] */ CStatusBarManagerService* host)
    : mEnabled(enable)
    , mHost(host)
{}

ECode CStatusBarManagerService::SetHardKeyboardEnabledRunnable::Run()
{
    mHost->mWindowManager->SetHardKeyboardEnabled(mEnabled);
    return NOERROR;
}


//==================================================================================
//              CStatusBarManagerService::HardKeyboardStatusChangeRunnable
//==================================================================================
CStatusBarManagerService::HardKeyboardStatusChangeRunnable::HardKeyboardStatusChangeRunnable(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled,
    /* [in] */ CStatusBarManagerService* host)
    : mAvailable(available)
    , mEnabled(enabled)
    , mHost(host)
{}

ECode CStatusBarManagerService::HardKeyboardStatusChangeRunnable::Run()
{
    if (mHost->mBar != NULL) {
        // try {
            mHost->mBar->SetHardKeyboardStatus(mAvailable, mEnabled);
        // } catch (RemoteException ex) {
        // }
    }
    return NOERROR;
}

//==================================================================================
//              CStatusBarManagerService
//==================================================================================
CStatusBarManagerService::CStatusBarManagerService()
    : mDisabled(0)
    , mSystemUiVisibility(0)
    , mMenuVisible(FALSE)
    , mImeWindowVis(0)
    , mImeBackDisposition(0)
    , mCurrentUserId(0)
{
    CHandler::New((IHandler**)&mHandler);
    CStatusBarIconList::New((IStatusBarIconList**)&mIcons);
    CBinder::New((IBinder**)&mSysUiVisToken);
}

ECode CStatusBarManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Handle32 windowManager)
{
    mContext = context;
    mWindowManager = (CWindowManagerService*)windowManager;
    mWindowManager->SetOnHardKeyboardStatusChangeListener((IOnHardKeyboardStatusChangeListener*)this);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<ArrayOf<String> > array;
    res->GetStringArray(R::array::config_statusBarIcons, (ArrayOf<String>**)&array);
    assert(array != NULL);
    mIcons->DefineSlots(*array);

    return NOERROR;
}

UInt32 CStatusBarManagerService::AddRef()
{
    return _CStatusBarManagerService::AddRef();
}

UInt32 CStatusBarManagerService::Release()
{
    return _CStatusBarManagerService::Release();
}

PInterface CStatusBarManagerService::Probe(
    /* [in] */ REIID riid)
{
    return _CStatusBarManagerService::Probe(riid);
}

ECode CStatusBarManagerService::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    return _CStatusBarManagerService::GetInterfaceID(pObject, pIID);
}

ECode CStatusBarManagerService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TAG;
    return NOERROR;
}

ECode CStatusBarManagerService::SetNotificationCallbacks(
    /* [in] */ INotificationCallbacks* listener)
{
    mNotificationCallbacks = listener;
    return NOERROR;
}

ECode CStatusBarManagerService::ExpandNotificationsPanel()
{
    FAIL_RETURN(EnforceExpandStatusBar());

    if (mBar != NULL) {
        mBar->AnimateExpandNotificationsPanel();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::CollapsePanels()
{
    FAIL_RETURN(EnforceExpandStatusBar());

    if (mBar != NULL) {
        mBar->AnimateCollapsePanels();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::ExpandSettingsPanel()
{
    FAIL_RETURN(EnforceExpandStatusBar());

    if (mBar != NULL) {
        mBar->AnimateExpandSettingsPanel();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::Disable(
    /* [in] */ Int32 what,
    /* [in] */ IBinder* token,
    /* [in] */ const String& pkg)
{
    return DisableInternal(mCurrentUserId, what, token, pkg);
}

ECode CStatusBarManagerService::DisableInternal(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 what,
    /* [in] */ IBinder* token,
    /* [in] */ const String& pkg)
{
    FAIL_RETURN(EnforceStatusBar());
    {
        AutoLock lock(this);
        DisableLocked(userId, what, token, pkg);
    }
    return NOERROR;
}

void CStatusBarManagerService::DisableLocked(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 what,
    /* [in] */ IBinder* token,
    /* [in] */ const String& pkg)
{
    // It's important that the the callback and the call to mBar get done
    // in the same order when multiple threads are calling this function
    // so they are paired correctly.  The messages on the handler will be
    // handled in the order they were enqueued, but will be outside the lock.
    ManageDisableListLocked(userId, what, token, pkg);

    // Ensure state for the current user is applied, even if passed a non-current user.
    Int32 net;
    GatherDisableActionsLocked(mCurrentUserId, &net);
    if (net != mDisabled) {
        mDisabled = net;

        Boolean result;
        AutoPtr<IRunnable> runnable = new DisableNotificationRunnable(net, this);
        mHandler->Post(runnable, &result);
        if (mBar != NULL) {
            mBar->Disable(net);
        }
    }
}

ECode CStatusBarManagerService::SetIcon(
    /* [in] */ const String& slot,
    /* [in] */ const String& iconPackage,
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 iconLevel,
    /* [in] */ const String& contentDescription)
{
    FAIL_RETURN(EnforceStatusBar());
    {
        AutoLock Lock(mIconsLock);
        Int32 index = 0;
        mIcons->GetSlotIndex(slot, &index);
        if (index < 0) {
            Slogger::E(TAG, "invalid status bar icon slot: %s", slot.string());
            return E_SECURITY_EXCEPTION;
        }

        AutoPtr<IStatusBarIcon> icon;

        AutoPtr<ICharSequence> seq;
        CString::New(contentDescription, (ICharSequence**)&seq);

        CStatusBarIcon::New(iconPackage, UserHandle::OWNER, iconId,
                iconLevel, 0,
                seq, (IStatusBarIcon**)&icon);

        mIcons->SetIcon(index, icon);

        if (mBar != NULL) {
            mBar->SetIcon(index, icon);
        }
    }
    return NOERROR;
}

ECode CStatusBarManagerService::SetIconVisibility(
    /* [in] */ const String& slot,
    /* [in] */ Boolean visible)
{
    FAIL_RETURN(EnforceStatusBar());
    {
        AutoLock Lock(mIconsLock);
        Int32 index = 0;
        assert(mIcons != NULL);
        mIcons->GetSlotIndex(slot, &index);
        if (index < 0) {
            Slogger::E(TAG, "invalid status bar icon slot: %s", slot.string());
            return E_SECURITY_EXCEPTION;
        }

        AutoPtr<IStatusBarIcon> icon;
        mIcons->GetIcon(index, (IStatusBarIcon**)&icon);
        if (icon == NULL) {
            return E_NULL_POINTER_EXCEPTION;
        }

        Boolean v;
        icon->GetVisible(&v);
        if (v != visible) {
            icon->SetVisible(visible);

            if (mBar != NULL) {
                mBar->SetIcon(index, icon);
            }
        }
    }
    return NOERROR;
}

ECode CStatusBarManagerService::RemoveIcon(
    /* [in] */ const String& slot)
{
    FAIL_RETURN(EnforceStatusBar());
    {
        AutoLock Lock(mIconsLock);
        Int32 index = 0;
        mIcons->GetSlotIndex(slot, &index);
        if (index < 0) {
            Slogger::E(TAG, "invalid status bar icon slot: ", slot.string());
            return E_SECURITY_EXCEPTION;
        }

        mIcons->RemoveIcon(index);

        if (mBar != NULL) {
            mBar->RemoveIcon(index);
        }
    }
    return NOERROR;
}

ECode CStatusBarManagerService::TopAppWindowChanged(
    /* [in] */ Boolean menuVisible)
{
    FAIL_RETURN(EnforceStatusBar());

    if (SPEW)
        Slogger::D(TAG, (mMenuVisible?"showing":"hiding"), " MENU key");
    {
        AutoLock lock(this);
        mMenuVisible = menuVisible;

        Boolean result;
        AutoPtr<IRunnable> runnable = new TopAppWindowChangedRunnable(menuVisible, this);
        return mHandler->Post(runnable, &result);
    }
}

ECode CStatusBarManagerService::SetImeWindowStatus(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 vis,
    /* [in] */ Int32 backDisposition)
{
    FAIL_RETURN(EnforceStatusBar());

    if (SPEW) {
        Slogger::D(TAG, "swetImeWindowStatus vis=%d backDisposition=%d", vis, backDisposition);
    }
    {
        AutoLock lock(this);
        // In case of IME change, we need to call up setImeWindowStatus() regardless of
        // mImeWindowVis because mImeWindowVis may not have been set to false when the
        // previous IME was destroyed.
        mImeWindowVis = vis;
        mImeBackDisposition = backDisposition;
        mImeToken = token;

        Boolean result;
        AutoPtr<IRunnable> runnable = new SetImeWindowStatusRunnable(token, vis, backDisposition, this);
        return mHandler->Post(runnable, &result);
    }
}

ECode CStatusBarManagerService::SetSystemUiVisibility(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask)
{
    // also allows calls from window manager which is in this process.
    FAIL_RETURN(EnforceStatusBarService());

    if (SPEW)
        Slogger::D(TAG, "setSystemUiVisibility(0x%s)", StringUtils::Int32ToString(vis).string());
    {
        AutoLock lock(this);
        UpdateUiVisibilityLocked(vis, mask);

        DisableLocked(
                mCurrentUserId,
                vis & IStatusBarManager::DISABLE_MASK,
                mSysUiVisToken,
                String("WindowManager.LayoutParams"));
    }
    return NOERROR;
}

void CStatusBarManagerService::UpdateUiVisibilityLocked(
    /* [in] */ Int32 vis,
    /* [in] */ Int32 mask)
{
    if (mSystemUiVisibility != vis) {
        mSystemUiVisibility = vis;

        Boolean result;
        AutoPtr<IRunnable> runnable = new UpdateUiVisibilityRunnable(vis, mask, this);
        mHandler->Post(runnable, &result);
    }
}

ECode CStatusBarManagerService::SetHardKeyboardEnabled(
    /* [in] */ Boolean enabled)
{
    Boolean result;
    AutoPtr<IRunnable> runnable = new SetHardKeyboardEnabledRunnable(enabled, this);
    return mHandler->Post(runnable, &result);
}

void CStatusBarManagerService::OnHardKeyboardStatusChange(
    /* [in] */ Boolean available,
    /* [in] */ Boolean enabled)
{
    Boolean result;
    AutoPtr<IRunnable> runnable = new HardKeyboardStatusChangeRunnable(available, enabled, this);
    mHandler->Post(runnable, &result);
}

ECode CStatusBarManagerService::ToggleRecentApps()
{
    if (mBar != NULL) {
        mBar->ToggleRecentApps();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::PreloadRecentApps()
{
    if (mBar != NULL) {
        mBar->PreloadRecentApps();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::CancelPreloadRecentApps()
{
    if (mBar != NULL) {
        mBar->CancelPreloadRecentApps();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::SetCurrentUser(
    /* [in] */ Int32 newUserId)
{
    if (SPEW) {
        Slogger::D(TAG, "Setting current user to user %d", newUserId);
    }
    mCurrentUserId = newUserId;
    return NOERROR;
}

ECode CStatusBarManagerService::EnforceStatusBar()
{
    return mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::STATUS_BAR,
            String("StatusBarManagerService"));
}

ECode CStatusBarManagerService::EnforceExpandStatusBar()
{
    return mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::EXPAND_STATUS_BAR,
            String("StatusBarManagerService"));
}

ECode CStatusBarManagerService::EnforceStatusBarService()
{
    return mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::STATUS_BAR_SERVICE,
            String("StatusBarManagerService"));
}

ECode CStatusBarManagerService::RegisterStatusBar(
    /* [in] */ IIStatusBar* bar,
    /* [out] */ IStatusBarIconList** iconList,
    /* [out] */ IObjectContainer** notificationKeys,/*List<IBinder*>*/
    /* [out] */ IObjectContainer** notifications,/*List<IStatusBarNotification*>*/
    /* [out,callee] */ ArrayOf<Int32>** switches,
    /* [out] */ IObjectContainer** binders)/*List<IBinder*>*/
{
    VALIDATE_NOT_NULL(iconList);
    VALIDATE_NOT_NULL(notificationKeys);
    VALIDATE_NOT_NULL(notifications);
    VALIDATE_NOT_NULL(binders);
    VALIDATE_NOT_NULL(switches);

    CParcelableObjectContainer::New(notificationKeys);
    CParcelableObjectContainer::New(notifications);
    CParcelableObjectContainer::New(binders);
    *switches = ArrayOf<Int32>::Alloc(7);
    REFCOUNT_ADD(*switches);
    CStatusBarIconList::New(iconList);

    FAIL_RETURN(EnforceStatusBarService());

    Slogger::I(TAG, "RegisterStatusBar bar=%p", bar);
    mBar = bar;

    do {
        AutoLock Lock(mIconsLock);
        (*iconList)->CopyFrom(mIcons);
    } while (FALSE);

    do {
        AutoLock Lock(mNotificationsLock);
        HashMap<AutoPtr<IBinder>, AutoPtr<IStatusBarNotification> >::Iterator iter = mNotifications.Begin();
        for(; iter != mNotifications.End(); ++iter) {
            (*notificationKeys)->Add(iter->mFirst);
            (*notifications)->Add(iter->mSecond);
        }
    } while (FALSE);

    do {
        AutoLock lock(this);
        GatherDisableActionsLocked(mCurrentUserId, &((**switches)[0]));
        (**switches)[1] = mSystemUiVisibility;
        (**switches)[2] = mMenuVisible ? 1 : 0;
        (**switches)[3] = mImeWindowVis;
        (**switches)[4] = mImeBackDisposition;
        (*binders)->Add(mImeToken);
    } while (FALSE);

    (**switches)[5] = mWindowManager->IsHardKeyboardAvailable() ? 1 : 0;
    (**switches)[6] = mWindowManager->IsHardKeyboardEnabled() ? 1 : 0;

    return NOERROR;
}

ECode CStatusBarManagerService::OnPanelRevealed()
{
    FAIL_RETURN(EnforceStatusBarService());

    // tell the notification manager to turn off the lights.
    if (mNotificationCallbacks) {
        mNotificationCallbacks->OnPanelRevealed();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::OnNotificationClick(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    FAIL_RETURN(EnforceStatusBarService());

    if (mNotificationCallbacks) {
        mNotificationCallbacks->OnNotificationClick(pkg, tag, id);
    }
    return NOERROR;
}

ECode CStatusBarManagerService::OnNotificationError(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ const String& message)
{
    FAIL_RETURN(EnforceStatusBarService());

    // WARNING: this will call back into us to do the remove.  Don't hold any locks.
    if (mNotificationCallbacks) {
        mNotificationCallbacks->OnNotificationError(pkg, tag, id, uid, initialPid, message);
    }
    return NOERROR;
}

ECode CStatusBarManagerService::OnNotificationClear(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id)
{
    FAIL_RETURN(EnforceStatusBarService());

    if (mNotificationCallbacks) {
        mNotificationCallbacks->OnNotificationClear(pkg, tag, id);
    }
    return NOERROR;
}

ECode CStatusBarManagerService::OnClearAllNotifications()
{
    FAIL_RETURN(EnforceStatusBarService());

    if (mNotificationCallbacks) {
        mNotificationCallbacks->OnClearAll();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::AddNotification(
    /* [in] */ IStatusBarNotification* notification,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    AutoLock Lock(mNotificationsLock);
    AutoPtr<IBinder> key;
    CBinder::New((IBinder**)&key);
    mNotifications[key] = notification;
    if (mBar != NULL) {
        mBar->AddNotification(key, notification);
    }
    *binder = key;
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode CStatusBarManagerService::UpdateNotification(
    /* [in] */ IBinder* key,
    /* [in] */ IStatusBarNotification* notification)
{
    AutoLock Lock(mNotificationsLock);
    HashMap<AutoPtr<IBinder>, AutoPtr<IStatusBarNotification> >::Iterator iter = mNotifications.Find(key);
    if (iter == mNotifications.End()) {
        Slogger::E(TAG, "updateNotification key not found: %p", key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNotifications[key] = notification;
    if (mBar != NULL) {
        mBar->UpdateNotification(key, notification);
    }
    return NOERROR;
}

ECode CStatusBarManagerService::RemoveNotification(
    /* [in] */ IBinder* key)
{
    AutoLock Lock(mNotificationsLock);
    AutoPtr<IStatusBarNotification> n = mNotifications[key];
    mNotifications.Erase(key);
    if (n == NULL) {
        Slogger::E(TAG, "removeNotification key not found: %p", key);
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mBar != NULL) {
        mBar->RemoveNotification(key);
    }
    return NOERROR;
}

ECode CStatusBarManagerService::ManageDisableListLocked(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 what,
    /* [in] */ IBinder* token,
    /* [in] */ const String& pkg)
{
    if (SPEW) {
        Slogger::D(TAG, "manageDisableList userId=%d what=0x%s pkg=%s", userId, (StringUtils::Int32ToString(what)).string(), pkg.string());
    }
    // update the list
    AutoPtr<DisableRecord> tok;
    List<AutoPtr<DisableRecord> >::Iterator iter = mDisableRecords.Begin();
    for (; iter != mDisableRecords.End(); ++iter) {
        AutoPtr<DisableRecord> t = *iter;
        if (t->mToken.Get() == token && t->mUserId == userId) {
            tok = t;
            break;
        }
    }
    Boolean isBinderAlive = TRUE;
    AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
    if (proxy != NULL) {
        //proxy->IsBinderAlive(&isBinderAlive);
    }
    if (what == 0 || !isBinderAlive) {
        if (tok != NULL) {
            mDisableRecords.Remove(*iter);
            AutoPtr<IProxy> tokProxy = (IProxy*)(tok->mToken)->Probe(EIID_IProxy);
            if (tokProxy != NULL) {
                Boolean result;
                tokProxy->UnlinkToDeath(tok, 0, &result);
            }
        }
    }
    else {
        if (tok == NULL) {
            tok = new DisableRecord(this);
            tok->mUserId = userId;
            if (proxy != NULL) {
                proxy->LinkToDeath(tok, 0);
            }
            mDisableRecords.PushBack(tok);
        }
        tok->mWhat = what;
        tok->mToken = token;
        tok->mPkg = pkg;
    }
    return NOERROR;
}

// lock on mDisableRecords
ECode CStatusBarManagerService::GatherDisableActionsLocked(
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // gather the new net flags
    Int32 net = 0;
    List<AutoPtr<DisableRecord> >::Iterator iter = mDisableRecords.Begin();
    for (; iter != mDisableRecords.End(); ++iter) {
        AutoPtr<DisableRecord> rec = *iter;
        if (rec->mUserId == userId) {
            net |= rec->mWhat;
        }
    }
    *result = net;
    return NOERROR;
}

CStatusBarManagerService::MBroadcastReceiver::MBroadcastReceiver(
    /* [in] */ CStatusBarManagerService* host)
        : mHost(host)
{
}

ECode CStatusBarManagerService::MBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS.Equals(action)
            || IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->CollapsePanels();
    }
    return NOERROR;
}


}//namespace Server
}//namespace Droid
}//namespace Elastos
