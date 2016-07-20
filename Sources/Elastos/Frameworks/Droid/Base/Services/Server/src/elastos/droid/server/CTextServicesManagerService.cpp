
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/CTextServicesManagerService.h"
#include "elastos/droid/server/CTSMSUserSwitchObserver.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::EIID_IIUserSwitchObserver;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::TextService::CSpellCheckerInfo;
using Elastos::Droid::Internal::TextService::EIID_IISpellCheckerService;
using Elastos::Droid::Internal::TextService::EIID_IITextServicesManager;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CTextServicesManagerService::TAG("CTextServicesManagerService");
const Boolean CTextServicesManagerService::DBG = FALSE;

//===============================================================================
//                  CTextServicesManagerService::TSMSUserSwitchObserver
//===============================================================================

CAR_INTERFACE_IMPL_2(CTextServicesManagerService::TSMSUserSwitchObserver, Object, IIUserSwitchObserver, IBinder);

CTextServicesManagerService::TSMSUserSwitchObserver::TSMSUserSwitchObserver()
{}

CTextServicesManagerService::TSMSUserSwitchObserver::~TSMSUserSwitchObserver()
{}

ECode CTextServicesManagerService::TSMSUserSwitchObserver::constructor(
    /* [in] */ IITextServicesManager* tsm)
{
    mHost = (CTextServicesManagerService*)tsm;
    return NOERROR;
}

ECode CTextServicesManagerService::TSMSUserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    {
        AutoLock lock(mHost->mSpellCheckerMapLock);
        mHost->SwitchUserLocked(newUserId);
    }

    if (reply != NULL) {
        //try {
        reply->SendResult(NULL);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CTextServicesManagerService::TSMSUserSwitchObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    Slogger::I("CTextServicesManagerService::CTSMSUserSwitchObserver", "onUserSwitchComplete");
    return NOERROR;
}

ECode CTextServicesManagerService::TSMSUserSwitchObserver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CTextServicesManagerService::TSMSUserSwitchObserver";
    return NOERROR;
}

//===============================================================================
//                  CTextServicesManagerService::TextServicesMonitor
//===============================================================================

CTextServicesManagerService::TextServicesMonitor::TextServicesMonitor(
    /* [in] */ CTextServicesManagerService* owner)
    : mHost(owner)
{}

CTextServicesManagerService::TextServicesMonitor::~TextServicesMonitor()
{}

ECode CTextServicesManagerService::TextServicesMonitor::OnSomePackagesChanged()
{
    if (!IsChangingPackagesOfCurrentUser()) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    {
        AutoLock lock(mHost->mSpellCheckerMapLock);
        BuildSpellCheckerMapLocked(
            mHost->mContext, mHost->mSpellCheckerList, mHost->mSpellCheckerMap, mHost->mSettings);
        AutoPtr<ISpellCheckerInfo> sci;
        mHost->GetCurrentSpellChecker(String(NULL), (ISpellCheckerInfo**)&sci);
        // If no spell checker is enabled, just return. The user should explicitly
        // enable the spell checker.
        if (sci == NULL) return NOERROR;
        String packageName;
        sci->GetPackageName(&packageName);
        Int32 change;
        IsPackageDisappearing(packageName, &change);
        Boolean isModified;
        IsPackageModified(packageName, &isModified);
        if (// Package disappearing
                change == IPackageMonitor::PACKAGE_PERMANENT_CHANGE || change == IPackageMonitor::PACKAGE_TEMPORARY_CHANGE
                // Package modified
                || isModified) {
            sci = NULL;
            ec = mHost->FindAvailSpellCheckerLocked(String(NULL), packageName, (ISpellCheckerInfo**)&sci);
            if (sci != NULL) {
                String id;
                sci->GetId(&id);
                ec = mHost->SetCurrentSpellCheckerLocked(id);
            }
        }
    }
    return ec;
}

Boolean CTextServicesManagerService::TextServicesMonitor::IsChangingPackagesOfCurrentUser()
{
    Boolean result;
    Int32 userId, currentUserId;
    GetChangingUserId(&userId);
    mHost->mSettings->GetCurrentUserId(&currentUserId);

    result = userId == currentUserId;
    if (CTextServicesManagerService::DBG) {
        Slogger::D(TAG, "--- ignore this call back from a background user: %d", userId);
    }

    return result;
}

//===============================================================================
//                  CTextServicesManagerService::InternalServiceConnection
//===============================================================================

CTextServicesManagerService::InternalServiceConnection::InternalServiceConnection(
    /* [in] */ const String& id,
    /* [in] */ const String& locale,
    /* [in] */ IBundle* bundle,
    /* [in] */ CTextServicesManagerService* owner)
    : mSciId(id)
    , mLocale(locale)
    , mBundle(bundle)
    , mHost(owner)
{}

CTextServicesManagerService::InternalServiceConnection::~InternalServiceConnection()
{}

CAR_INTERFACE_IMPL(CTextServicesManagerService::InternalServiceConnection, Object, IServiceConnection);

ECode CTextServicesManagerService::InternalServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    ECode ec = NOERROR;
    {
        AutoLock lock(mHost->mSpellCheckerMapLock);
        ec = OnServiceConnectedInnerLocked(name, service);
    }
    return ec;
}

ECode CTextServicesManagerService::InternalServiceConnection::OnServiceConnectedInnerLocked(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    String strCompName;
    assert(name != NULL);
    name->ToString(&strCompName);
    if (CTextServicesManagerService::DBG) {
        Slogger::W(TAG, "onServiceConnected: %s", strCompName.string());
    }
    AutoPtr<IISpellCheckerService> spellChecker = IISpellCheckerService::Probe(service);
    ManagedSpellCheckerBindGroupMapIt it = mHost->mSpellCheckerBindGroups.Find(mSciId);

    if (it != mHost->mSpellCheckerBindGroups.End() && this == it->mSecond->mInternalConnection) {
        it->mSecond->OnServiceConnected(spellChecker);
    }
    return NOERROR;
}

ECode CTextServicesManagerService::InternalServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    {
        AutoLock lock(mHost->mSpellCheckerMapLock);
        ManagedSpellCheckerBindGroupMapIt it = mHost->mSpellCheckerBindGroups.Find(mSciId);
        if (it != mHost->mSpellCheckerBindGroups.End() && this == it->mSecond->mInternalConnection) {
            mHost->mSpellCheckerBindGroups.Erase(mSciId);
        }
    }
    return NOERROR;
}


String CTextServicesManagerService::InternalServiceConnection::GetSciId()
{
    return mSciId;
}

ECode CTextServicesManagerService::InternalServiceConnection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "InternalServiceConnection";
    return NOERROR;
}

//===============================================================================
//                  CTextServicesManagerService::TextServicesSettings
//===============================================================================

CTextServicesManagerService::TextServicesSettings::TextServicesSettings(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 userId,
    /* [in] */ CTextServicesManagerService* owner)
    : mResolver(resolver)
    , mCurrentUserId(userId)
    , mHost(owner)
{}

CTextServicesManagerService::TextServicesSettings::~TextServicesSettings()
{}

ECode CTextServicesManagerService::TextServicesSettings::SetCurrentUserId(
    /* [in] */ Int32 userId)
{
    String strSelectesc;
    GetSelectedSpellChecker(&strSelectesc);
    if (CTextServicesManagerService::DBG) {
        Slogger::D(TAG, "--- Swtich the current user from %d to %d,  new ime = %s", mCurrentUserId, userId, strSelectesc.string());
    }
    // TSMS settings are kept per user, so keep track of current user
    mCurrentUserId = userId;
    return NOERROR;
}

ECode CTextServicesManagerService::TextServicesSettings::GetCurrentUserId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mCurrentUserId;
    return NOERROR;
}

ECode CTextServicesManagerService::TextServicesSettings::PutSelectedSpellChecker(
    /* [in] */ const String& sciId)
{
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    return settingsSecure->PutStringForUser(mResolver,
            ISettingsSecure::SELECTED_SPELL_CHECKER, sciId, mCurrentUserId, &result);
}

ECode CTextServicesManagerService::TextServicesSettings::PutSelectedSpellCheckerSubtype(
    /* [in] */ Int32 hashCode)
{
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    return settingsSecure->PutStringForUser(mResolver,
            ISettingsSecure::SELECTED_SPELL_CHECKER_SUBTYPE, StringUtils::ToString(hashCode, 10),
            mCurrentUserId, &result);
}

ECode CTextServicesManagerService::TextServicesSettings::SetSpellCheckerEnabled(
    /* [in] */ Boolean enabled)
{
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    return settingsSecure->PutInt32ForUser(mResolver,
            ISettingsSecure::SPELL_CHECKER_ENABLED, enabled ? 1 : 0, mCurrentUserId, &result);
}

ECode CTextServicesManagerService::TextServicesSettings::GetSelectedSpellChecker(
    /* [out] */ String* spellChecker)
{
    VALIDATE_NOT_NULL(spellChecker);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    return settingsSecure->GetStringForUser(mResolver,
                        ISettingsSecure::SELECTED_SPELL_CHECKER, mCurrentUserId, spellChecker);
}

ECode CTextServicesManagerService::TextServicesSettings::GetSelectedSpellCheckerSubtype(
    /* [out] */ String* subtype)
{
    VALIDATE_NOT_NULL(subtype);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    return settingsSecure->GetStringForUser(mResolver,
            ISettingsSecure::SELECTED_SPELL_CHECKER_SUBTYPE, mCurrentUserId, subtype);
}

ECode CTextServicesManagerService::TextServicesSettings::IsSpellCheckerEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    Int32 value;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32ForUser(mResolver,
            ISettingsSecure::SPELL_CHECKER_ENABLED, 0, mCurrentUserId, &value);
    *enabled = (value == 1);
    return NOERROR;
}

//===============================================================================
//                  CTextServicesManagerService::SpellCheckerBindGroup
//===============================================================================

CTextServicesManagerService::SpellCheckerBindGroup::SpellCheckerBindGroup(
    /* [in] */ InternalServiceConnection* connection,
    /* [in] */ IITextServicesSessionListener* listener,
    /* [in] */ const String& locale,
    /* [in] */ IISpellCheckerSessionListener* scListener,
    /* [in] */ Int32 uid,
    /* [in] */ IBundle* bundle,
    /* [in] */ CTextServicesManagerService* owner)
    : mInternalConnection(connection)
    , mBound(TRUE)
    , mConnected(FALSE)
    , mHost(owner)
{
    AutoPtr<InternalDeathRecipient> recipient;
    AddListener(listener, locale, scListener, uid, bundle, (InternalDeathRecipient**)&recipient);
}

CTextServicesManagerService::SpellCheckerBindGroup::~SpellCheckerBindGroup()
{}

ECode CTextServicesManagerService::SpellCheckerBindGroup::OnServiceConnected(
    /* [in] */ IISpellCheckerService* spellChecker)
{
    if (CTextServicesManagerService::DBG) {
        Slogger::D(TAG, "onServiceConnected");
    }
    assert(spellChecker != NULL);
    ManagedInternalDeathRecipientListIt it = mListeners.Begin();
    for (; it != mListeners.End(); ++it) {
        //try {
        ECode ec = NOERROR;
        AutoPtr<InternalDeathRecipient> listener = (*it).Get();
        AutoPtr<IISpellCheckerSession> session;
        ec = spellChecker->GetISpellCheckerSession(
                listener->mScLocale, listener->mScListener, listener->mBundle, (IISpellCheckerSession**)&session);

        {
            AutoLock lock(mHost->mSpellCheckerMapLock);
            ManagedInternalDeathRecipientListIt it2 = Find(mListeners.Begin(), mListeners.End(), listener);
            if (it2 != mListeners.End()) {
                assert(listener->mTsListener != NULL);
                ec = listener->mTsListener->OnServiceConnected(session);
            }
        }

        if(FAILED(ec)) {
            RemoveAll();
        }
            //}
        // } catch (RemoteException e) {
        //     Slog.e(TAG, "Exception in getting the spell checker session."
        //             + "Reconnect to the spellchecker. ", e);
        //     removeAll();
        //     return;
        // }
    }

    {
        AutoLock lock(mHost->mSpellCheckerMapLock);
        mSpellChecker = spellChecker;
        mConnected = TRUE;
    }
    return NOERROR;
}

ECode CTextServicesManagerService::SpellCheckerBindGroup::AddListener(
    /* [in] */ IITextServicesSessionListener* tsListener,
    /* [in] */ const String& locale,
    /* [in] */ IISpellCheckerSessionListener* scListener,
    /* [in] */ Int32 uid,
    /* [in] */ IBundle* bundle,
    /* [out] */ InternalDeathRecipient** recipient)
{
    VALIDATE_NOT_NULL(recipient);
    *recipient = NULL;

    if (CTextServicesManagerService::DBG) {
        Slogger::D(TAG, "addListener: %s", locale.string());
    }
    AutoPtr<InternalDeathRecipient> localRecipient;

    {
        AutoLock lock(mHost->mSpellCheckerMapLock);
        ManagedInternalDeathRecipientListIt it = mListeners.Begin();
        //try {
        for (; it != mListeners.End(); ++it) {
            if ((*it)->HasSpellCheckerListener(scListener)) {
                // do not add the lister if the group already contains this.
                return NOERROR;
            }
        }
        localRecipient = new InternalDeathRecipient(this, tsListener, locale, scListener, uid, bundle, mHost);
        AutoPtr<IProxy> proxy = (IProxy*)scListener->Probe(EIID_IProxy);
        if (proxy != NULL) proxy->LinkToDeath(localRecipient, 0);
        mListeners.PushBack(localRecipient);
        // } catch(RemoteException e) {
        //     // do nothing
        // }
        CleanLocked();
        *recipient = localRecipient;
        REFCOUNT_ADD(*recipient);
    }
    return NOERROR;
}

ECode CTextServicesManagerService::SpellCheckerBindGroup::RemoveListener(
    /* [in] */ IISpellCheckerSessionListener* listener)
{
    /*if (CTextServicesManagerService::DBG) {
        Slogger::W(TAG, "remove listener: " + listener.hashCode());
    }*/
    {
        AutoLock lock(mHost->mSpellCheckerMapLock);

        List<AutoPtr<InternalDeathRecipient> > removeList;
        ManagedInternalDeathRecipientListIt it = mListeners.Begin();
        for (; it != mListeners.End(); ++it) {
            AutoPtr<InternalDeathRecipient> tempRecipient = (*it);

            if(tempRecipient->HasSpellCheckerListener(listener)) {
                if (CTextServicesManagerService::DBG) {
                    Slogger::W(TAG, "found existing listener.");
                }
                removeList.PushBack(tempRecipient);
            }
        }
        ManagedInternalDeathRecipientListIt it2 = removeList.Begin();
        for (Int32 i = 0; it2 != removeList.End(); ++it2, ++i) {
            if (CTextServicesManagerService::DBG) {
                Slogger::W(TAG, "SpellCheckerBindGroup::RemoveListener Remove %d", i);
            }
            AutoPtr<InternalDeathRecipient> idr = (*it2);
            AutoPtr<IProxy> proxy = (IProxy*)idr->mScListener->Probe(EIID_IProxy);
            if (proxy != NULL) {
                Boolean result;
                proxy->UnlinkToDeath(idr, 0, &result);
            }
            mListeners.Remove(idr);
        }
        CleanLocked();
        removeList.Clear();
    }

    return NOERROR;
}

ECode CTextServicesManagerService::SpellCheckerBindGroup::RemoveAll()
{
    Slogger::E(TAG, "Remove the spell checker bind unexpectedly.");
    ECode ec = NOERROR;
    {
        AutoLock lock(mHost->mSpellCheckerMapLock);
        List<AutoPtr<InternalDeathRecipient> >::Iterator it = mListeners.Begin();
        for (; it != mListeners.End(); ++it) {
            AutoPtr<InternalDeathRecipient> idr = (*it);
            AutoPtr<IProxy> proxy = (IProxy*)idr->mScListener->Probe(EIID_IProxy);
            if (proxy != NULL) {
                Boolean result;
                proxy->UnlinkToDeath(idr, 0, &result);
            }
        }
        mListeners.Clear();
        ec = CleanLocked();
    }
    return ec;
}

ECode CTextServicesManagerService::SpellCheckerBindGroup::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "SpellCheckerBindGroup";
    return NOERROR;
}

ECode CTextServicesManagerService::SpellCheckerBindGroup::CleanLocked()
{
    if (DBG) {
        Slogger::D(TAG, "cleanLocked");
    }
    // If there are no more active listeners, clean up.  Only do this
    // once.
    ECode ec = NOERROR;
    if (mBound && mListeners.IsEmpty()) {
        mBound = FALSE;
        const String sciId = mInternalConnection->GetSciId();
        ManagedSpellCheckerBindGroupMapIt it = mHost->mSpellCheckerBindGroups.Find(sciId);
        if ((SpellCheckerBindGroup*)it->mSecond == this) {
            if (DBG) {
                Slogger::D(TAG, "Remove bind group.");
            }
            mHost->mSpellCheckerBindGroups.Erase(sciId);
        }
        ec = mHost->mContext->UnbindService(mInternalConnection);
    }

    return ec;
}

//===============================================================================
//                  CTextServicesManagerService::InternalDeathRecipient
//===============================================================================

CAR_INTERFACE_IMPL(CTextServicesManagerService::InternalDeathRecipient, Object, IProxyDeathRecipient)

CTextServicesManagerService::InternalDeathRecipient::InternalDeathRecipient(
    /* [in] */ SpellCheckerBindGroup* group,
    /* [in] */ IITextServicesSessionListener* tsListener,
    /* [in] */ const String& scLocale,
    /* [in] */ IISpellCheckerSessionListener* scListener,
    /* [in] */ Int32 uid,
    /* [in] */ IBundle* bundle,
    /* [in] */ CTextServicesManagerService* owner)
    : mGroup(group)
    , mTsListener(tsListener)
    , mScLocale(scLocale)
    , mScListener(scListener)
    , mUid(uid)
    , mBundle(bundle)
    , mHost(owner)
{}

CTextServicesManagerService::InternalDeathRecipient::~InternalDeathRecipient()
{}

Boolean CTextServicesManagerService::InternalDeathRecipient::HasSpellCheckerListener(
    /* [in] */ IISpellCheckerSessionListener* listener)
{
    return (IBinder::Probe(listener) == IBinder::Probe(mScListener));
}

ECode CTextServicesManagerService::InternalDeathRecipient::ProxyDied()
{
    assert(mGroup != NULL);
    return mGroup->RemoveListener(mScListener);
}

//===============================================================================
//                  CTextServicesManagerService
//===============================================================================

CAR_INTERFACE_IMPL_2(CTextServicesManagerService, Object, IITextServicesManager, IBinder)

CAR_OBJECT_IMPL(CTextServicesManagerService)

ECode CTextServicesManagerService::constructor(
    /* [in] */ IContext* context)
{
    mSystemReady = FALSE;
    mContext = context;
    Int32 userId = IUserHandle::USER_OWNER;
    //try {
    AutoPtr<IIUserSwitchObserver> userSwithchObserver;
    CTSMSUserSwitchObserver::New((IITextServicesManager*)this, (IIUserSwitchObserver**)&userSwithchObserver);
    ActivityManagerNative::GetDefault()->RegisterUserSwitchObserver(userSwithchObserver);

    AutoPtr<IUserInfo> userInfo;
    ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&userInfo);
    assert(userInfo != NULL);
    userInfo->GetId(&userId);

    // } catch (RemoteException e) {
    //     Slog.w(TAG, "Couldn't get current user ID; guessing it's 0", e);
    // }
    mMonitor = new TextServicesMonitor(this);
    mMonitor->Register(context, NULL, TRUE);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    mSettings = new TextServicesSettings(contentResolver, userId, this);

    // "switchUserLocked" initializes the states for the foreground user
    return SwitchUserLocked(userId);
}

ECode CTextServicesManagerService::GetCurrentSpellChecker(
    /* [in] */ const String& locale,
    /* [out] */ ISpellCheckerInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        Slogger::W(TAG, "called from invalid user");
        return NOERROR;
    }
    {
        AutoLock lock(mSpellCheckerMapLock);
        String curSpellCheckerId;
        mSettings->GetSelectedSpellChecker(&curSpellCheckerId);
        if (DBG) {
            Slogger::W(TAG, "GetCurrentSpellChecker: %s", curSpellCheckerId.string());
        }
        if (curSpellCheckerId.IsNullOrEmpty()) {
            return NOERROR;
        }
        ManagedISpellCheckerInfoMapIt it = mSpellCheckerMap.Find(curSpellCheckerId);
        if (it != mSpellCheckerMap.End()) {
            *info = it->mSecond;
            REFCOUNT_ADD(*info);
        }
    }
    return NOERROR;
}

ECode CTextServicesManagerService::GetCurrentSpellCheckerSubtype(
    /* [in] */ const String& locale,
    /* [in] */ Boolean allowImplicitlySelectedSubtype,
    /* [out] */ ISpellCheckerSubtype** subtype)
{
    VALIDATE_NOT_NULL(subtype);
    *subtype = NULL;

    // TODO: Make this work even for non-current users?
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    {
        AutoLock lock(mSpellCheckerMapLock);
        String subtypeHashCodeStr;
        mSettings->GetSelectedSpellCheckerSubtype(&subtypeHashCodeStr);
        if (DBG) {
            Slogger::W(TAG, "getCurrentSpellCheckerSubtype: %s", subtypeHashCodeStr.string());
        }
        AutoPtr<ISpellCheckerInfo> sci;
        GetCurrentSpellChecker(String(NULL), (ISpellCheckerInfo**)&sci);
        Int32 subtypeCount;

        if (sci == NULL || (sci->GetSubtypeCount(&subtypeCount), subtypeCount == 0)) {
            if (DBG) {
                Slogger::W(TAG, "Subtype not found.");
            }
            return NOERROR;
        }

        Int32 hashCode;
        if (!subtypeHashCodeStr.IsNullOrEmpty()) {
            hashCode = StringUtils::ParseInt32(subtypeHashCodeStr);
        }
        else {
            hashCode = 0;
        }
        if (hashCode == 0 && !allowImplicitlySelectedSubtype) {
            return NOERROR;
        }

        String candidateLocale(NULL);
        if (hashCode == 0) {
            // Spell checker language settings == "auto"
            AutoPtr<IInterface> obj;
            mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
            AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
            if (imm != NULL) {
                AutoPtr<IInputMethodSubtype> currentInputMethodSubtype;
                imm->GetCurrentInputMethodSubtype((IInputMethodSubtype**)&currentInputMethodSubtype);
                if (currentInputMethodSubtype != NULL) {
                    String localeString;
                    currentInputMethodSubtype->GetLocale(&localeString);
                    if (!localeString.IsNullOrEmpty()) {
                        // 1. Use keyboard locale if available in the spell checker
                        candidateLocale = localeString;
                    }
                }
            }
            if (candidateLocale == NULL) {
                // 2. Use System locale if available in the spell checker
                AutoPtr<IResources> res;
                mContext->GetResources((IResources**)&res);
                AutoPtr<IConfiguration> configuration;
                res->GetConfiguration((IConfiguration**)&configuration);
                AutoPtr<ILocale> iLocale;
                configuration->GetLocale((ILocale**)&iLocale);
                iLocale->ToString(&candidateLocale);
            }
        }

        AutoPtr<ISpellCheckerSubtype> candidate;

        for (Int32 i = 0; i < subtypeCount; ++i) {
            AutoPtr<ISpellCheckerSubtype> scs;
            sci->GetSubtypeAt(i, (ISpellCheckerSubtype**)&scs);
            assert(scs != NULL);
            Int32 hashCode2 = Object::GetHashCode(scs);
            if (hashCode == 0) {
                String scsLocale;
                scs->GetLocale(&scsLocale);
                if (candidateLocale.Equals(scsLocale)) {
                    *subtype = scs;
                    REFCOUNT_ADD(*subtype);
                    return NOERROR;
                }
                else if (candidate == NULL) {
                    if (candidateLocale.GetLength() >= 2 && scsLocale.GetLength() >= 2
                            && candidateLocale.StartWith(scsLocale)) {
                        // Fall back to the applicable language
                        candidate = scs;
                    }
                }
            }
            else if (hashCode2 == hashCode) {
                if (DBG) {
                    Slogger::W(TAG, "Return subtype:%d , input= %s",  hashCode2, locale.string());
                           // + ", " + scs.getLocale());
                }
                // 3. Use the user specified spell check language
                *subtype = scs;
                REFCOUNT_ADD(*subtype);
                return NOERROR;
            }
        }
        // 4. Fall back to the applicable language and return it if not null
        // 5. Simply just return it even if it's null which means we could find no suitable
        // spell check languages
        *subtype = candidate;
        REFCOUNT_ADD(*subtype);
        return NOERROR;
    }
}

ECode CTextServicesManagerService::GetSpellCheckerService(
    /* [in] */ const String& sciId,
    /* [in] */ const String& locale,
    /* [in] */ IITextServicesSessionListener* tsListener,
    /* [in] */ IISpellCheckerSessionListener* scListener,
    /* [in] */ IBundle* bundle)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    if (!mSystemReady) {
        return NOERROR;
    }
    if (sciId.IsNullOrEmpty() || tsListener == NULL || scListener == NULL) {
        Slogger::E(TAG, "getSpellCheckerService: Invalid input.");
        return NOERROR;
    }

    {
        AutoLock lock(mSpellCheckerMapLock);
        ManagedISpellCheckerInfoMapIt it = mSpellCheckerMap.Find(sciId);
        if (it == mSpellCheckerMap.End()) {
            return NOERROR;
        }
        AutoPtr<ISpellCheckerInfo> sci;
        sci = it->mSecond;
        Int32 uid = Binder::GetCallingUid();
        ManagedSpellCheckerBindGroupMapIt it2 = mSpellCheckerBindGroups.Find(sciId);
        if (it2 != mSpellCheckerBindGroups.End()) {
           AutoPtr<SpellCheckerBindGroup> bindGroup = it2->mSecond;
           if (bindGroup != NULL) {
                AutoPtr<InternalDeathRecipient> recipient;
                bindGroup->AddListener(tsListener, locale, scListener, uid, bundle, (InternalDeathRecipient**)&recipient);
                if (recipient == NULL) {
                    if (DBG) {
                        Slogger::W(TAG, "Didn't create a death recipient.");
                        return NOERROR;
                    }
                }
                if ((bindGroup->mSpellChecker == NULL) && bindGroup->mConnected) {
                    Slogger::E(TAG, "The state of the spell checker bind group is illegal.");
                    bindGroup->RemoveAll();
                }
                else if (bindGroup->mSpellChecker != NULL) {
                    if (DBG) {
                        Slogger::W(TAG, "Existing bind found. Return a spell checker session now. Listeners count = %d", bindGroup->mListeners.GetSize());
                    }
                    //try {
                    AutoPtr<IISpellCheckerSession> session;
                            bindGroup->mSpellChecker->GetISpellCheckerSession(
                                    recipient->mScLocale, recipient->mScListener, bundle, (IISpellCheckerSession**)&session);
                    if (session != NULL) {
                        tsListener->OnServiceConnected(session);
                        return NOERROR;
                    }
                    else {
                        if (DBG) {
                            Slogger::W(TAG, "Existing bind already expired. ");
                        }
                        bindGroup->RemoveAll();
                    }
                    //}
                    /*catch (RemoteException e) {
                        Slog.e(TAG, "Exception in getting spell checker session: " + e);
                        bindGroup.removeAll();
                    }*/
                }
           }
        }
        Int64 ident = Binder::ClearCallingIdentity();
        //try
        {
        StartSpellCheckerServiceInnerLocked(
                sci, locale, tsListener, scListener, uid, bundle);
        Binder::RestoreCallingIdentity(ident);
        /*} finally {
            Binder.restoreCallingIdentity(ident);
        }*/
        }
    }
    return NOERROR;
}

ECode CTextServicesManagerService::FinishSpellCheckerService(
    /* [in] */ IISpellCheckerSessionListener* listener)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    if (DBG) {
        Slogger::D(TAG, "FinishSpellCheckerService");
    }

    {
        AutoLock lock(mSpellCheckerMapLock);
        List<AutoPtr<SpellCheckerBindGroup> > removeList;
        ManagedSpellCheckerBindGroupMapIt it = mSpellCheckerBindGroups.Begin();
        for (; it != mSpellCheckerBindGroups.End(); ++it) {
            AutoPtr<SpellCheckerBindGroup> group = it->mSecond;
            if (group == NULL) continue;
            removeList.PushBack(group);
        }

        List<AutoPtr<SpellCheckerBindGroup> >::Iterator it2 = removeList.Begin();
        for (; it2 != removeList.End(); ++it2) {
            ((*it2).Get())->RemoveListener(listener);
        }
        removeList.Clear();
    }

    return NOERROR;
}

ECode CTextServicesManagerService::SetCurrentSpellChecker(
    /* [in] */ const String& locale,
    /* [in] */ const String& sciId)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }

    ECode ec = NOERROR;
    {
        AutoLock lock(mSpellCheckerMapLock);
        Int32 permission;
        FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS, &permission));
        if (permission != IPackageManager::PERMISSION_GRANTED) {
            /*throw new SecurityException(
                    "Requires permission "
                    + Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS);*/
            Slogger::D(TAG, "Requires permission Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS");
            return E_SECURITY_EXCEPTION;
        }

        ec = SetCurrentSpellCheckerLocked(sciId);
    }
    return ec;
}

ECode CTextServicesManagerService::SetCurrentSpellCheckerSubtype(
    /* [in] */ const String& locale,
    /* [in] */ Int32 hashCode)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    {
        AutoLock lock(mSpellCheckerMapLock);
        Int32 permission;
        FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS, &permission));
        if (permission != IPackageManager::PERMISSION_GRANTED) {
            /*throw new SecurityException(
                    "Requires permission "
                    + Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS);*/
            Slogger::D(TAG, "Requires permission Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS");
            return E_SECURITY_EXCEPTION;
        }

        ec = SetCurrentSpellCheckerSubtypeLocked(hashCode);
    }
    return ec;
}

ECode CTextServicesManagerService::SetSpellCheckerEnabled(
    /* [in] */ Boolean enabled)
{
    if (!CalledFromValidUser()) {
        return NOERROR;
    }
    ECode ec = NOERROR;
    {
        AutoLock lock(mSpellCheckerMapLock);
        Int32 permission;
        FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS, &permission));
        if (permission != IPackageManager::PERMISSION_GRANTED) {
            /*throw new SecurityException(
                    "Requires permission "
                    + Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS);*/
            Slogger::D(TAG, "Requires permission Elastos::Droid::Manifest::permission::WRITE_SECURE_SETTINGS");
            return E_SECURITY_EXCEPTION;
        }

        ec = SetSpellCheckerEnabledLocked(enabled);
    }
    return ec;
}

ECode CTextServicesManagerService::IsSpellCheckerEnabled(
    /* [out] */ Boolean* ebl)
{
    VALIDATE_NOT_NULL(ebl);
    if (!CalledFromValidUser()) {
        *ebl = FALSE;
        return NOERROR;
    }
    ECode ec = NOERROR;
    {
        AutoLock lock(mSpellCheckerMapLock);
        ec = IsSpellCheckerEnabledLocked(ebl);
    }
    return ec;
}

ECode CTextServicesManagerService::GetEnabledSpellCheckers(
    /* [out, callee] */ ArrayOf<ISpellCheckerInfo*>** infoArray)
{
    // TODO: Make this work even for non-current users?
    VALIDATE_NOT_NULL(infoArray);
    Int32 size = mSpellCheckerList.GetSize();
    if (!CalledFromValidUser()) {
        *infoArray = NULL;
        return NOERROR;
    }
    if (DBG) {
        Slogger::D(TAG, "getEnabledSpellCheckers: %d", size);
        String packageName;
        ManagedISpellCheckerInfoListIt it = mSpellCheckerList.Begin();
        for (; it != mSpellCheckerList.End(); ++it) {
            ((*it).Get())->GetPackageName(&packageName);
            Slogger::D(TAG, "EnabledSpellCheckers: %s", packageName.string());
        }
    }
    AutoPtr<ArrayOf<ISpellCheckerInfo*> > array = ArrayOf<ISpellCheckerInfo*>::Alloc(size);
    ManagedISpellCheckerInfoListIt it2 = mSpellCheckerList.Begin();
    for(Int32 i = 0; it2 != mSpellCheckerList.End(); ++it2, ++i) {
        array->Set(i, (*it2).Get());
    }
    *infoArray = array;
    REFCOUNT_ADD(*infoArray);
    return NOERROR;
}

ECode CTextServicesManagerService::SystemRunning()
{
    Slogger::I(TAG, "CTextServicesManagerService::SystemRunning");
    if (!mSystemReady) {
        mSystemReady = TRUE;
    }
    return NOERROR;
}

ECode CTextServicesManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("CTextServicesManagerService");
    return NOERROR;
}

ECode CTextServicesManagerService::SwitchUserLocked(
    /* [in] */ Int32 userId)
{
    mSettings->SetCurrentUserId(userId);
    UnbindServiceLocked();
    BuildSpellCheckerMapLocked(mContext, mSpellCheckerList, mSpellCheckerMap, mSettings);
    AutoPtr<ISpellCheckerInfo> sci;
    GetCurrentSpellChecker(String(NULL), (ISpellCheckerInfo**)&sci);
    if (sci == NULL) {
        FindAvailSpellCheckerLocked(String(NULL), String(NULL), (ISpellCheckerInfo**)&sci);
        if (sci != NULL) {
            // Set the current spell checker if there is one or more spell checkers
            // available. In this case, "sci" is the first one in the available spell
            // checkers.
            String id;
            sci->GetId(&id);
            SetCurrentSpellCheckerLocked(id);
        }
    }
    return NOERROR;
}

Boolean CTextServicesManagerService::CalledFromValidUser()
{
    Int32 uId = Binder::GetCallingUid();
    Int32 userId = UserHandle::GetUserId(uId);
    Int32 currentUserId;
    mSettings->GetCurrentUserId(&currentUserId);
    if (DBG) {
        Slogger::D(TAG, "--- calledFromForegroundUserOrSystemProcess ? calling uid = %d system uid = %d calling userId = %d, foreground user id = %d", uId ,IProcess::SYSTEM_UID, userId, currentUserId);

        //try {
        AutoPtr<ArrayOf<String> > packageNames;
        AppGlobals::GetPackageManager()->GetPackagesForUid(uId, (ArrayOf<String>**)&packageNames);
        for (Int32 i = 0; packageNames && i < packageNames->GetLength(); ++i) {
            if (DBG) {
                Slogger::D(TAG, "--- process name for %d = %s", uId, (*packageNames)[i].string());
            }
        }
        // } catch (RemoteException e) {
        // }
    }
    if (uId == IProcess::SYSTEM_UID || userId == currentUserId) {
        return TRUE;
    }
    else {
        String trace;
        GetStackTrace(&trace);
        Slogger::W(TAG, "--- IPC called from background users. Ignore. \n %s", trace.string());
        return FALSE;
    }
}

ECode CTextServicesManagerService::BindCurrentSpellCheckerService(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 currentUserId;

    if (service == NULL || conn == NULL) {
        Slogger::E(TAG, "--- bind failed: service or conn was set to NULL, please have a check");
        return NOERROR;
    }
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(currentUserId, (IUserHandle**)&userHandle);
    mSettings->GetCurrentUserId(&currentUserId);
    return mContext->BindServiceAsUser(service, conn, flags, userHandle, result);
}

ECode CTextServicesManagerService::UnbindServiceLocked()
{
    ManagedSpellCheckerBindGroupMapIt it = mSpellCheckerBindGroups.Begin();
    for (; it != mSpellCheckerBindGroups.End(); ++it) {
        AutoPtr<SpellCheckerBindGroup> scbg = it->mSecond;
        assert(scbg != NULL);
        scbg->RemoveAll();
    }
    mSpellCheckerBindGroups.Clear();
    return NOERROR;
}

ECode CTextServicesManagerService::FindAvailSpellCheckerLocked(
    /* [in] */ const String& locale,
    /* [in] */ const String& prefPackage,
    /* [out] */ ISpellCheckerInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    const Int32 spellCheckersCount = mSpellCheckerList.GetSize();
    if (spellCheckersCount == 0) {
        Slogger::W(TAG, "no available spell checker services found");
        return NOERROR;
    }

    if (prefPackage != NULL) {
        ManagedISpellCheckerInfoListIt it = mSpellCheckerList.Begin();
        for (; it != mSpellCheckerList.End(); ++it) {
            AutoPtr<ISpellCheckerInfo> sci = (*it).Get();
            String packageName;
            assert(sci != NULL);
            sci->GetPackageName(&packageName);
            if (prefPackage.Equals(packageName)) {
                if (DBG) {
                    Slogger::D(TAG, "findAvailSpellCheckerLocked: %s", packageName.string());
                }
                *info = sci;
                REFCOUNT_ADD(*info);
                return NOERROR;
            }
        }
    }
    if (spellCheckersCount > 1) {
        Slogger::W(TAG, "more than one spell checker service found, picking first");
    }
    *info = mSpellCheckerList[0];
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CTextServicesManagerService::StartSpellCheckerServiceInnerLocked(
    /* [in] */ ISpellCheckerInfo* info,
    /* [in] */ const String& locale,
    /* [in] */ IITextServicesSessionListener* tsListener,
    /* [in] */ IISpellCheckerSessionListener* scListener,
    /* [in] */ Int32 uid,
    /* [in] */ IBundle* bundle)
{
    if (DBG) {
        Slogger::W(TAG, "Start spell checker session inner locked.");
    }
    String sciId;
    assert(info != NULL);
    info->GetId(&sciId);
    const AutoPtr<InternalServiceConnection> connection = new InternalServiceConnection(sciId, locale, bundle, this);
    const AutoPtr<IIntent> serviceIntent;
    FAIL_RETURN(CIntent::New(String("android.service.textservice.SpellCheckerService"), (IIntent**)&serviceIntent));
    AutoPtr<IComponentName> componentName;
    info->GetComponent((IComponentName**)&componentName);
    assert(serviceIntent != NULL);
    serviceIntent->SetComponent(componentName);
    if (DBG) {
        String id;
        info->GetId(&id);
        Slogger::W(TAG, "bind service: %s", id.string());
    }
    Boolean result;
    BindCurrentSpellCheckerService(serviceIntent, connection, IContext::BIND_AUTO_CREATE, &result);
    if (!result) {
        Slogger::E(TAG, "Failed to get a spell checker service.");
        return NOERROR;
    }

    const AutoPtr<SpellCheckerBindGroup> group = new SpellCheckerBindGroup(connection, tsListener, locale, scListener, uid, bundle, this);
    mSpellCheckerBindGroups[sciId] = group;
    return NOERROR;
}

ECode CTextServicesManagerService::SetCurrentSpellCheckerLocked(
    /* [in] */ const String& sciId)
{
    if (DBG) {
        Slogger::W(TAG, "setCurrentSpellChecker: %s", sciId.string());
    }
    HashMap<String, AutoPtr<ISpellCheckerInfo> >::Iterator it = mSpellCheckerMap.Find(sciId);
    if (sciId.IsNullOrEmpty() || it == mSpellCheckerMap.End()) return NOERROR;
    const AutoPtr<ISpellCheckerInfo> currentSci;
    GetCurrentSpellChecker(String(NULL), (ISpellCheckerInfo**)&currentSci);
    String id;

    if (currentSci != NULL && (currentSci->GetId(&id), id.Equals(sciId))) {
        // Do nothing if the current spell checker is same as new spell checker.
        return NOERROR;
    }
    const Int64 ident = Binder::ClearCallingIdentity();
    //try {
    mSettings->PutSelectedSpellChecker(sciId);
    SetCurrentSpellCheckerSubtypeLocked(0);
    //} finally {
    Binder::RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode CTextServicesManagerService::SetCurrentSpellCheckerSubtypeLocked(
    /* [in] */ Int32 hashCode)
{
    if (DBG) {
        Slogger::W(TAG, "setCurrentSpellCheckerSubtype: %d", hashCode);
    }
    AutoPtr<ISpellCheckerInfo> sci;
    GetCurrentSpellChecker(String(NULL), (ISpellCheckerInfo**)&sci);
    Int32 tempHashCode = 0;
    if (sci != NULL) {
        Int32 subtypeCount;
        sci->GetSubtypeCount(&subtypeCount);
        for (Int32 i = 0; i < subtypeCount; ++i) {
            AutoPtr<ISpellCheckerSubtype> subtype;
            sci->GetSubtypeAt(i, (ISpellCheckerSubtype**)&subtype);
            assert(subtype != NULL);
            Int32 hashCode2 = Object::GetHashCode(subtype);
            if(hashCode2 == hashCode) {
                tempHashCode = hashCode;
                break;
            }
        }
    }

    const Int64 ident = Binder::ClearCallingIdentity();
    //try {
    mSettings->PutSelectedSpellCheckerSubtype(tempHashCode);
    //} finally {
    Binder::RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode CTextServicesManagerService::SetSpellCheckerEnabledLocked(
    /* [in] */ Boolean enabled)
{
    if (DBG) {
        Slogger::W(TAG, "setSpellCheckerEnabled: %d", enabled);
    }
    const Int64 ident = Binder::ClearCallingIdentity();
    //try {
    mSettings->SetSpellCheckerEnabled(enabled);
    //} finally {
    Binder::RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode CTextServicesManagerService::IsSpellCheckerEnabledLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    const Int64 ident = Binder::ClearCallingIdentity();
    //try {
    Boolean retval;
    mSettings->IsSpellCheckerEnabled(&retval);
    if (DBG) {
        Slogger::W(TAG, "getSpellCheckerEnabled: %d", retval);
    }
    *result = retval;
    //} finally {
    Binder::RestoreCallingIdentity(ident);
    //}
    return NOERROR;
}

ECode CTextServicesManagerService::BuildSpellCheckerMapLocked(
    /* [in] */ IContext* context,
    /* [in] */ List<AutoPtr<ISpellCheckerInfo> >& list,
    /* [in] */ HashMap<String, AutoPtr<ISpellCheckerInfo> >& map,
    /* [in] */ TextServicesSettings* settings)
{
    list.Clear();
    map.Clear();
    const AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    assert(pm != NULL);
    AutoPtr<IList> services;
    AutoPtr<IIntent> intent;
    Int32 currentUserId;
    FAIL_RETURN(CIntent::New(String("android.service.textservice.SpellCheckerService"), (IIntent**)&intent));
    settings->GetCurrentUserId(&currentUserId);
    FAIL_RETURN(pm->QueryIntentServicesAsUser(intent, IPackageManager::GET_META_DATA,
        currentUserId, (IList**)&services));
    if (services != NULL) {
        AutoPtr<IIterator> it;
        services->GetIterator((IIterator**)&it);
        String packageName, name, permission;
        Boolean hasNext = FALSE;
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(obj);
            assert(ri != NULL);
            AutoPtr<IServiceInfo> si;
            ri->GetServiceInfo((IServiceInfo**)&si);
            assert(si != NULL);
            IPackageItemInfo::Probe(si)->GetPackageName(&packageName);
            IPackageItemInfo::Probe(si)->GetName(&name);
            si->GetPermission(&permission);
            AutoPtr<IComponentName> compName;
            CComponentName::New(packageName, name, (IComponentName**)&compName);
            String strCompName;
            compName->ToString(&strCompName);
            if (!Elastos::Droid::Manifest::permission::BIND_TEXT_SERVICE.Equals(permission)) {
                Slogger::W(TAG, "Skipping text service %s : it does not require the permission: %s ", strCompName.string(), Elastos::Droid::Manifest::permission::BIND_TEXT_SERVICE.string());
                continue;
            }
            if (DBG) Slogger::D(TAG, "Add: %s", strCompName.string());
            //try {
            AutoPtr<ISpellCheckerInfo> sci;
            CSpellCheckerInfo::New(context, ri, (ISpellCheckerInfo**)&sci);
            Int32 subtypeCount;
            sci->GetSubtypeCount(&subtypeCount);
            if (subtypeCount <= 0) {
                Slogger::W(TAG, "Skipping text service %s : it does not contain subtypes.", strCompName.string());
                continue;
            }
            list.PushBack(sci);
            String sciId;
            sci->GetId(&sciId);
            map[sciId] = sci;
            // } catch (XmlPullParserException e) {
            //     Slog.w(TAG, "Unable to load the spell checker " + compName, e);
            // } catch (IOException e) {
            //     Slog.w(TAG, "Unable to load the spell checker " + compName, e);
            // }
        }
        if (DBG) {
            Slogger::D(TAG, "buildSpellCheckerMapLocked: %d, %d", list.GetSize(), map.GetSize());
        }
    }
    return NOERROR;
}

// ----------------------------------------------------------------------
// Utilities for debug
ECode CTextServicesManagerService::GetStackTrace(
    /* [out] */ String* trace)
{
    /*
    final StringBuilder sb = new StringBuilder();
    try {
        throw new RuntimeException();
    } catch (RuntimeException e) {
        final StackTraceElement[] frames = e.getStackTrace();
        // Start at 1 because the first frame is here and we don't care about it
        for (int j = 1; j < frames.length; ++j) {
            sb.append(frames[j].toString() + "\n");
        }
    }
    return sb.toString();
    */
    VALIDATE_NOT_NULL(trace);
    *trace = String("");
    return NOERROR;
}

ECode CTextServicesManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 result;
    assert(pw != NULL);
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        pw->Println(String("Permission Denial: can't dump TextServicesManagerService from from pid=")
                + StringUtils::ToString(Binder::GetCallingPid())
                + String(", uid=") + StringUtils::ToString(Binder::GetCallingUid()));
        return NOERROR;
    }

    {
        AutoLock lock(mSpellCheckerMapLock);
        pw->Println(String("Current Text Services Manager state:"));
        pw->Println(String("  Spell Checker Map:"));
        ManagedISpellCheckerInfoMapIt it = mSpellCheckerMap.Begin();
        String str;

        for(; it != mSpellCheckerMap.End(); ++it) {
            pw->Print(String("    ")); pw->Print(it->mFirst); pw->Println(String(":"));
            AutoPtr<ISpellCheckerInfo> info = it->mSecond;
            assert(info != NULL);
            info->GetId(&str);
            pw->Print(String("      ")); pw->Print(String("id=")); pw->Println(str);
            pw->Print(String("      ")); pw->Print(String("comp="));
            AutoPtr<IComponentName> comp;
            info->GetComponent((IComponentName**)&comp);
            assert(comp != NULL);
            comp->ToShortString(&str);
            pw->Println(str);
            Int32 NS;
            info->GetSubtypeCount(&NS);
            for (Int32 i=0; i<NS; i++) {
                AutoPtr<ISpellCheckerSubtype> st;
                info->GetSubtypeAt(i, (ISpellCheckerSubtype**)&st);
                assert(st != NULL);
                pw->Print(String("      ")); pw->Print(String("Subtype #")); pw->Print(i); pw->Println(String(":"));
                st->GetLocale(&str);
                pw->Print(String("        ")); pw->Print(String("locale=")); pw->Println(str);
                st->GetExtraValue(&str);
                pw->Print(String("        ")); pw->Print(String("extraValue="));
                        pw->Println(str);
            }
        }

        pw->Println(String(""));
        pw->Println(String("  Spell Checker Bind Groups:"));
        ManagedSpellCheckerBindGroupMapIt it2 = mSpellCheckerBindGroups.Begin();
        for(; it2 != mSpellCheckerBindGroups.End(); ++it2) {
            AutoPtr<SpellCheckerBindGroup> grp = it2->mSecond;
            assert(grp != NULL);
            pw->Print(String("    ")); pw->Print(it2->mFirst); pw->Print(String(" "));
            pw->Print(Object::ToString(grp)); pw->Println(String(":"));
            pw->Print(String("      ")); pw->Print(String("mInternalConnection="));
            pw->Println(Object::ToString(grp->mInternalConnection));
            pw->Print(String("      ")); pw->Print(String("mSpellChecker="));
                    /*pw->Println(grp->mSpellChecker);*/pw->Println(String("grp.mSpellChecker"));
            pw->Print(String("      ")); pw->Print(String("mBound=")); pw->Print(grp->mBound);
            pw->Print(String(" mConnected=")); pw->Println(grp->mConnected);

            List<AutoPtr<InternalDeathRecipient> >::Iterator it3 = grp->mListeners.Begin();
            for (Int32 i = 0; it3 != grp->mListeners.End(); i++, it3++) {
                AutoPtr<InternalDeathRecipient> listener = (*it3).Get();
                pw->Print(String("      ")); pw->Print(String("Listener #")); pw->Print(i); pw->Println(String(":"));
                pw->Print(String("        ")); pw->Print(String("mTsListener="));
                        /*pw->Println(listener.mTsListener);*/pw->Println(String(String("listener.mTsListener")));
                pw->Print(String("        ")); pw->Print(String("mScListener="));
                        /*pw->Println(listener.mScListener);*/pw->Println(String("listener.mScListener"));
                pw->Print(String("        ")); pw->Print(String("mGroup="));
                        pw->Println(Object::ToString(listener->mGroup));
                pw->Print(String("        ")); pw->Print(String("mScLocale="));
                        pw->Print(listener->mScLocale);
                        pw->Print(String(" mUid=")); pw->Println(listener->mUid);
            }
        }
    }
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos