#include "elastos/droid/server/statusbar/CStatusBarManagerService.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::StatusBar::EIID_IIStatusBarService;
using Elastos::Droid::Internal::StatusBar::CStatusBarIconList;
using Elastos::Droid::Internal::StatusBar::IStatusBarIcon;
using Elastos::Droid::Internal::StatusBar::CStatusBarIcon;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Server::StatusBar::EIID_IStatusBarManagerInternal;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace Server {
namespace StatusBar {

const String CStatusBarManagerService::TAG("StatusBarManagerService");
const Boolean CStatusBarManagerService::SPEW = FALSE;


//==================================================================================
//              CStatusBarManagerService::MyStatusBarManagerInternal
//==================================================================================
CAR_INTERFACE_IMPL(CStatusBarManagerService::MyStatusBarManagerInternal, Object, IStatusBarManagerInternal)

CStatusBarManagerService::MyStatusBarManagerInternal::MyStatusBarManagerInternal(
    /* [in] */ CStatusBarManagerService* host)
    : mNotificationLightOn(FALSE)
    , mHost(host)
{}

ECode CStatusBarManagerService::MyStatusBarManagerInternal::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CStatusBarManagerService::MyStatusBarManagerInternal";
    return NOERROR;
}

ECode CStatusBarManagerService::MyStatusBarManagerInternal::SetNotificationDelegate(
    /* [in] */ INotificationDelegate* delegate)
{
    mHost->mNotificationDelegate = delegate;
    return NOERROR;
}

ECode CStatusBarManagerService::MyStatusBarManagerInternal::BuzzBeepBlinked()
{
    if (mHost->mBar != NULL) {
        mHost->mBar->BuzzBeepBlinked();
    }
    return NOERROR;
}

ECode CStatusBarManagerService::MyStatusBarManagerInternal::NotificationLightPulse(
    /* [in] */ Int32 argb,
    /* [in] */ Int32 onMillis,
    /* [in] */ Int32 offMillis)
{
    mNotificationLightOn = true;
    if (mHost->mBar != NULL) {
        mHost->mBar->NotificationLightPulse(argb, onMillis, offMillis);
    }
    return NOERROR;
}

ECode CStatusBarManagerService::MyStatusBarManagerInternal::NotificationLightOff()
{
    if (mNotificationLightOn) {
        mNotificationLightOn = FALSE;
        if (mHost->mBar != NULL) {
            mHost->mBar->NotificationLightOff();
        }
    }
    return NOERROR;
}

//==================================================================================
//              CStatusBarManagerService::DisableRecord
//==================================================================================
CAR_INTERFACE_IMPL(CStatusBarManagerService::DisableRecord, Object, IProxyDeathRecipient);

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
    if (mHost->mNotificationDelegate) {
        mHost->mNotificationDelegate->OnSetDisabled(mStatus);
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
    /* [in] */ Boolean showImeSwitcher,
    /* [in] */ CStatusBarManagerService* host)
    : mToken(token)
    , mVisible(vis)
    , mBackDisposition(backDisposition)
    , mShowImeSwitcher(showImeSwitcher)
    , mHost(host)
{
}

ECode CStatusBarManagerService::SetImeWindowStatusRunnable::Run()
{
    if (mHost->mBar != NULL) {
        // try {
            mHost->mBar->SetImeWindowStatus(mToken, mVisible, mBackDisposition, mShowImeSwitcher);
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
//              CStatusBarManagerService
//==================================================================================
CAR_INTERFACE_IMPL_2(CStatusBarManagerService, Object, IIStatusBarService, IBinder)

CAR_OBJECT_IMPL(CStatusBarManagerService)

CStatusBarManagerService::CStatusBarManagerService()
    : mDisabled(0)
    , mSystemUiVisibility(0)
    , mMenuVisible(FALSE)
    , mImeWindowVis(0)
    , mImeBackDisposition(0)
    , mShowImeSwitcher(FALSE)
    , mCurrentUserId(0)
{
    CHandler::New((IHandler**)&mHandler);
    CStatusBarIconList::New((IStatusBarIconList**)&mIcons);
    CBinder::New((IBinder**)&mSysUiVisToken);
}

ECode CStatusBarManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIWindowManager* wm)
{
    mContext = context;
    // mWindowManager = (mWindowManager*)windowManager;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<ArrayOf<String> > array;
    res->GetStringArray(R::array::config_statusBarIcons, (ArrayOf<String>**)&array);
    assert(array != NULL);
    mIcons->DefineSlots(*array);

    mInternalService = new MyStatusBarManagerInternal(this);
    LocalServices::AddService(EIID_IStatusBarManagerInternal, mInternalService);
    return NOERROR;
}

ECode CStatusBarManagerService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TAG;
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
    /* [in] */ Int32 backDisposition,
    /* [in] */ Boolean showImeSwitcher)
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
        mShowImeSwitcher = showImeSwitcher;

        Boolean result;
        AutoPtr<IRunnable> runnable = new SetImeWindowStatusRunnable(
            token, vis, backDisposition, showImeSwitcher, this);
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
        Slogger::D(TAG, "setSystemUiVisibility(0x%s)", StringUtils::ToString(vis).string());
    {
        AutoLock lock(this);
        UpdateUiVisibilityLocked(vis, mask);

        DisableLocked(
                mCurrentUserId,
                (vis & IStatusBarManager::DISABLE_MASK),
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

ECode CStatusBarManagerService::ShowRecentApps(
    /* [in] */ Boolean triggeredFromAltTab)
{
    if (mBar != NULL) {
        mBar->ShowRecentApps(triggeredFromAltTab);
    }
    return NOERROR;
}

ECode CStatusBarManagerService::HideRecentApps(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ Boolean triggeredFromHomeKey)
{
    if (mBar != NULL) {
        mBar->HideRecentApps(triggeredFromAltTab, triggeredFromHomeKey);
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

ECode CStatusBarManagerService::SetWindowState(
    /* [in] */ Int32 window,
    /* [in] */ Int32 state)
{
    if (mBar != NULL) {
        mBar->SetWindowState(window, state);
    }
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
    /* [in] */ IStatusBarIconList* inIconList,
    /* [in] */ ArrayOf<Int32>* inSwitches,
    /* [in] */ IList* inBinders,
    /* [out] */ IStatusBarIconList** outIconList,
    /* [out, callee] */ ArrayOf<Int32>** outSwitches,
    /* [out] */ IList** outBinders)/*List<IBinder*>*/
{
    VALIDATE_NOT_NULL(outIconList);
    VALIDATE_NOT_NULL(outSwitches);
    VALIDATE_NOT_NULL(outBinders);

    FAIL_RETURN(EnforceStatusBarService());

    Slogger::I(TAG, "RegisterStatusBar bar=%p", bar);
    mBar = bar;

    {
        AutoLock Lock(mIconsLock);
        inIconList->CopyFrom(mIcons);
        *outIconList = inIconList;
        REFCOUNT_ADD(*outIconList);
    }

    {
        AutoLock lock(this);
        GatherDisableActionsLocked(mCurrentUserId, &((*inSwitches)[0]));
        (*inSwitches)[1] = mSystemUiVisibility;
        (*inSwitches)[2] = mMenuVisible ? 1 : 0;
        (*inSwitches)[3] = mImeWindowVis;
        (*inSwitches)[4] = mImeBackDisposition;
        (*inSwitches)[5] = mShowImeSwitcher ? 1 : 0;
        *outSwitches = inSwitches;
        REFCOUNT_ADD(*outSwitches);

        inBinders->Add(mImeToken);
        *outBinders = inBinders;
        REFCOUNT_ADD(*outBinders);
    }

    return NOERROR;
}

ECode CStatusBarManagerService::OnPanelRevealed()
{
    FAIL_RETURN(EnforceStatusBarService());

    Int64 identity = Binder::ClearCallingIdentity();
    // tell the notification manager to turn off the lights.
    if (mNotificationDelegate) {
        mNotificationDelegate->OnPanelRevealed();
    }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CStatusBarManagerService::OnPanelHidden()
{
    FAIL_RETURN(EnforceStatusBarService());

    Int64 identity = Binder::ClearCallingIdentity();
    // tell the notification manager to turn off the lights.
    if (mNotificationDelegate) {
        mNotificationDelegate->OnPanelHidden();
    }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CStatusBarManagerService::OnNotificationClick(
    /* [in] */ const String& key)
{
    FAIL_RETURN(EnforceStatusBarService());

    Int32 callingUid = Binder::GetCallingUid();
    Int32 callingPid = Binder::GetCallingPid();
    Int64 identity = Binder::ClearCallingIdentity();
    if (mNotificationDelegate) {
        mNotificationDelegate->OnNotificationClick(callingUid, callingPid, key);
    }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CStatusBarManagerService::OnNotificationError(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ const String& message,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(EnforceStatusBarService());

    Int32 callingUid = Binder::GetCallingUid();
    Int32 callingPid = Binder::GetCallingPid();
    Int64 identity = Binder::ClearCallingIdentity();
    // WARNING: this will call back into us to do the remove.  Don't hold any locks.
    if (mNotificationDelegate) {
        mNotificationDelegate->OnNotificationError(callingUid, callingPid,
            pkg, tag, id, uid, initialPid, message, userId);
    }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CStatusBarManagerService::OnNotificationClear(
    /* [in] */ const String& pkg,
    /* [in] */ const String& tag,
    /* [in] */ Int32 id,
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(EnforceStatusBarService());

    Int32 callingUid = Binder::GetCallingUid();
    Int32 callingPid = Binder::GetCallingPid();
    Int64 identity = Binder::ClearCallingIdentity();
    if (mNotificationDelegate) {
        mNotificationDelegate->OnNotificationClear(callingUid, callingPid, pkg, tag, id, userId);
    }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CStatusBarManagerService::OnNotificationVisibilityChanged(
    /* [in] */ ArrayOf<String>* newlyVisibleKeys,
    /* [in] */ ArrayOf<String>* noLongerVisibleKeys)
{
    FAIL_RETURN(EnforceStatusBarService());
    Int64 identity = Binder::ClearCallingIdentity();
    ECode ec = NOERROR;
    if (mNotificationDelegate != NULL) {
        ec = mNotificationDelegate->OnNotificationVisibilityChanged(
                newlyVisibleKeys, noLongerVisibleKeys);
    }
    Binder::RestoreCallingIdentity(identity);
    return ec;
}

ECode CStatusBarManagerService::OnNotificationExpansionChanged(
    /* [in] */ const String& key,
    /* [in] */ Boolean userAction,
    /* [in] */ Boolean expanded)
{
    FAIL_RETURN(EnforceStatusBarService());
    Int64 identity = Binder::ClearCallingIdentity();
    ECode ec = NOERROR;

    if (mNotificationDelegate != NULL) {
        ec = mNotificationDelegate->OnNotificationExpansionChanged(
                key, userAction, expanded);
    }
    Binder::RestoreCallingIdentity(identity);
    return ec;
}

ECode CStatusBarManagerService::OnClearAllNotifications(
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(EnforceStatusBarService());

    Int32 callingUid = Binder::GetCallingUid();
    Int32 callingPid = Binder::GetCallingPid();
    Int64 identity = Binder::ClearCallingIdentity();
    if (mNotificationDelegate) {
        mNotificationDelegate->OnClearAll(callingUid, callingPid, userId);
    }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CStatusBarManagerService::ManageDisableListLocked(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 what,
    /* [in] */ IBinder* token,
    /* [in] */ const String& pkg)
{
    if (SPEW) {
        Slogger::D(TAG, "manageDisableList userId=%d what=0x%s pkg=%s", userId,
            StringUtils::ToString(what).string(), pkg.string());
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

}//namespace StatusBar
}//namespace Server
}//namespace Droid
}//namespace Elastos
