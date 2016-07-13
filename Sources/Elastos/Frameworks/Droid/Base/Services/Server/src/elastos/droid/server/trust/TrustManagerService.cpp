
#include "elastos/droid/server/trust/TrustManagerService.h"
#include "elastos/droid/server/trust/TrustAgentWrapper.h"
#include "elastos/droid/server/trust/TrustArchive.h"
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/R.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Provider.h>

using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::IRunnable;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::IService;
using Elastos::Droid::App::Trust::EIID_IITrustManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::R;
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::Utility::Xml;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastosx::Net::Ssl::ITrustManager;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Trust {

//=============================================================================
// TrustManagerService::AgentInfo
//=============================================================================
CAR_INTERFACE_IMPL(TrustManagerService::AgentInfo, Object, IAgentInfo)

TrustManagerService::AgentInfo::AgentInfo()
    : mUserId(0)
{}

ECode TrustManagerService::AgentInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IAgentInfo::Probe(other) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<AgentInfo> o = (AgentInfo*) IAgentInfo::Probe(other);
    Boolean bEquals;
    IObject::Probe(mComponent)->Equals(o->mComponent, &bEquals);
    *result = bEquals && mUserId == o->mUserId;
    return NOERROR;
}

ECode TrustManagerService::AgentInfo::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 rev;
    IObject::Probe(mComponent)->GetHashCode(&rev);
    *result = rev * 31 + mUserId;
    return NOERROR;
}

//=============================================================================
// TrustManagerService::Receiver
//=============================================================================
TrustManagerService::Receiver::Receiver(
    /* [in] */ TrustManagerService* host)
    : mHost(host)
{}

ECode TrustManagerService::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IDevicePolicyManager::ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED.Equals(action)) {
        Int32 userId;
        GetSendingUserId(&userId);
        mHost->RefreshAgentList(userId);
        mHost->UpdateDevicePolicyFeatures();
    }
    else if (IIntent::ACTION_USER_PRESENT.Equals(action)) {
        Int32 userId;
        GetSendingUserId(&userId);
        mHost->UpdateUserHasAuthenticated(userId);
    }
    else if (IIntent::ACTION_USER_ADDED.Equals(action)) {
        Int32 userId;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -100, &userId);
        if (userId > 0) {
            mHost->MaybeEnableFactoryTrustAgents(mHost->mLockPatternUtils, userId);
        } else {
            Logger::W(TAG, "EXTRA_USER_HANDLE missing or invalid, value=%d", userId);
        }
    }
    return NOERROR;
}

ECode TrustManagerService::Receiver::Register(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IDevicePolicyManager::ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED);
    filter->AddAction(IIntent::ACTION_USER_PRESENT);
    filter->AddAction(IIntent::ACTION_USER_ADDED);
    AutoPtr<IIntent> tmp;
    context->RegisterReceiverAsUser(IBroadcastReceiver::Probe(this),
            UserHandle::ALL,
            filter,
            String(NULL) /* permission */,
            NULL /* scheduler */,
            (IIntent**)&tmp);
    return NOERROR;
}

//=============================================================================
// TrustManagerService::InnerSub_ITrustManager::InnerSub_Runnable
//=============================================================================
CAR_INTERFACE_IMPL(TrustManagerService::InnerSub_ITrustManager::InnerSub_Runnable, Object, IRunnable)

TrustManagerService::InnerSub_ITrustManager::InnerSub_Runnable::InnerSub_Runnable(
    /* [in] */ InnerSub_ITrustManager* host,
    /* [in] */ IPrintWriter* fout,
    /* [in] */ IList* userInfos,
    /* [in] */ IUserInfo* currentUser)
    : mHost(host)
    , mFout(fout)
    , mUserInfos(userInfos)
    , mCurrentUser(currentUser)
{}

ECode TrustManagerService::InnerSub_ITrustManager::InnerSub_Runnable::Run()
{
    mFout->Println(String("Trust manager state:"));
    FOR_EACH(iter, mUserInfos) {
        AutoPtr<IUserInfo> user = IUserInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        mHost->DumpUser(mFout, user, Ptr(user)->Func(user->GetId) == Ptr(mCurrentUser)->Func(mCurrentUser->GetId));
    }
    return NOERROR;
}

//=============================================================================
// TrustManagerService::InnerSub_ITrustManager
//=============================================================================
CAR_INTERFACE_IMPL_2(TrustManagerService::InnerSub_ITrustManager, Object, IITrustManager, IBinder)

TrustManagerService::InnerSub_ITrustManager::InnerSub_ITrustManager(
    /* [in] */ TrustManagerService* host)
    : mHost(host)
{}

ECode TrustManagerService::InnerSub_ITrustManager::ReportUnlockAttempt(
    /* [in] */ Boolean authenticated,
    /* [in] */ Int32 userId)
{
    EnforceReportPermission();
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_DISPATCH_UNLOCK_ATTEMPT, authenticated ? 1 : 0, userId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TrustManagerService::InnerSub_ITrustManager::ReportEnabledTrustAgentsChanged(
    /* [in] */ Int32 userId)
{
    EnforceReportPermission();
    // coalesce refresh messages.
    mHost->mHandler->RemoveMessages(MSG_ENABLED_AGENTS_CHANGED);
    Boolean b;
    mHost->mHandler->SendEmptyMessage(MSG_ENABLED_AGENTS_CHANGED, &b);
    return NOERROR;
}

ECode TrustManagerService::InnerSub_ITrustManager::ReportRequireCredentialEntry(
    /* [in] */ Int32 userId)
{
    EnforceReportPermission();
    if (userId == IUserHandle::USER_ALL || userId >= IUserHandle::USER_OWNER) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_REQUIRE_CREDENTIAL_ENTRY, userId, 0, (IMessage**)&msg);
        msg->SendToTarget();
    } else {
        Logger::E(TAG, "userId must be an explicit user id or USER_ALL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode TrustManagerService::InnerSub_ITrustManager::RegisterTrustListener(
    /* [in] */ IITrustListener* trustListener)
{
    EnforceListenerPermission();
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_REGISTER_LISTENER, trustListener, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TrustManagerService::InnerSub_ITrustManager::UnregisterTrustListener(
    /* [in] */ IITrustListener* trustListener)
{
    EnforceListenerPermission();
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_UNREGISTER_LISTENER, trustListener, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TrustManagerService::InnerSub_ITrustManager::EnforceReportPermission()
{
    return mHost->mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_KEYGUARD_SECURE_STORAGE, String("reporting trust events"));
}

ECode TrustManagerService::InnerSub_ITrustManager::EnforceListenerPermission()
{
    return mHost->mContext->EnforceCallingPermission(Manifest::permission::TRUST_LISTENER,
            String("register trust listener"));
}

ECode TrustManagerService::InnerSub_ITrustManager::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* fout,
    /* [in] */ ArrayOf<String>* args)
{
    mHost->mContext->EnforceCallingPermission(Manifest::permission::DUMP,
            String("dumping TrustManagerService"));
    AutoPtr<IUserInfo> currentUser;
    AutoPtr<IList> userInfos;
    mHost->mUserManager->GetUsers(TRUE /* excludeDying */, (IList**)&userInfos);
    // try {
    ECode ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&currentUser);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            return E_RUNTIME_EXCEPTION;
        else
            return ec;
    }
    // }
    Boolean bNoUse;
    mHost->mHandler->RunWithScissors(new InnerSub_Runnable(this, fout, userInfos, currentUser), 1500, &bNoUse);
    return NOERROR;
}

ECode TrustManagerService::InnerSub_ITrustManager::DumpUser(
    /* [in] */ IPrintWriter* fout,
    /* [in] */ IUserInfo* user,
    /* [in] */ Boolean isCurrent)
{
    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, StringUtils::ParseCharSequence(Ptr(user)->Func(user->GetName)));
    AutoPtr<IInteger32> i32;
    CInteger32::New(Ptr(user)->Func(user->GetId), (IInteger32**)&i32);
    args->Set(1, i32);
    i32 = NULL;
    CInteger32::New(Ptr(user)->Func(user->GetFlags), (IInteger32**)&i32);
    args->Set(2, i32);
    fout->Printf(String(" User \"%s\" (id=%d, flags=%#x)"), args);
    if (isCurrent) {
        fout->Print(String(" (current)"));
    }
    fout->Print(String(": trusted=") + DumpBool(mHost->AggregateIsTrusted(Ptr(user)->Func(user->GetId))));
    fout->Print(String(", trustManaged=") + DumpBool(mHost->AggregateIsTrustManaged(Ptr(user)->Func(user->GetId))));
    fout->Println();
    fout->Println(String("   Enabled agents:"));
    Boolean duplicateSimpleNames = FALSE;
    AutoPtr<IArraySet> simpleNames;
    CArraySet::New((IArraySet**)&simpleNames);
    FOR_EACH(iter, mHost->mActiveAgents) {
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        if (info->mUserId != Ptr(user)->Func(user->GetId)) { continue; }
        Boolean trusted;
        info->mAgent->IsTrusted(&trusted);
        fout->Print(String("    "));
        fout->Println(Ptr(info->mComponent)->Func(IComponentName::FlattenToShortString));
        Boolean b;
        info->mAgent->IsBound(&b);
        fout->Print(String("     bound=") + DumpBool(b));
        info->mAgent->IsConnected(&b);
        fout->Print(String(", connected=") + DumpBool(b));
        info->mAgent->IsManagingTrust(&b);
        fout->Print(String(", managingTrust=") + DumpBool(b));
        fout->Print(String(", trusted=") + DumpBool(trusted));
        fout->Println();
        if (trusted) {
            AutoPtr<ICharSequence> csq;
            info->mAgent->GetMessage((ICharSequence**)&csq);
            fout->Println(String("      message=\"") + Object::ToString(csq) + "\"");
        }
        info->mAgent->IsConnected(&b);
        if (!b) {
            String restartTime;
            Int64 scheduledRestartUptimeMillis;
            info->mAgent->GetScheduledRestartUptimeMillis(&scheduledRestartUptimeMillis);
            TrustArchive::FormatDuration(
                    scheduledRestartUptimeMillis - SystemClock::GetUptimeMillis(), &restartTime);
            fout->Println(String("      restartScheduledAt=") + restartTime);
        }
        String simpleName;
        TrustArchive::GetSimpleName(info->mComponent, &simpleName);
        ISet::Probe(simpleNames)->Add(StringUtils::ParseCharSequence(simpleName));
        if (!b) {
            duplicateSimpleNames = TRUE;
        }
    }
    fout->Println(String("   Events:"));
    mHost->mArchive->Dump(fout, 50, Ptr(user)->Func(user->GetId), String("    ") /* linePrefix */, duplicateSimpleNames);
    fout->Println();
    return NOERROR;
}

String TrustManagerService::InnerSub_ITrustManager::DumpBool(
    /* [in] */ Boolean b)
{
    return b ? String("1") : String("0");
}

ECode TrustManagerService::InnerSub_ITrustManager::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return IObject::ToString(result);
}

//=============================================================================
// TrustManagerService::InnerSub_Handler
//=============================================================================
TrustManagerService::InnerSub_Handler::InnerSub_Handler(
    /* [in] */ TrustManagerService* host)
    : mHost(host)
{}

ECode TrustManagerService::InnerSub_Handler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    if (what == MSG_REGISTER_LISTENER) {
        mHost->AddListener(IITrustListener::Probe(obj));
    }
    else if (what == MSG_UNREGISTER_LISTENER) {
        mHost->RemoveListener(IITrustListener::Probe(obj));
    }
    else if (what == MSG_DISPATCH_UNLOCK_ATTEMPT) {
        mHost->DispatchUnlockAttempt(arg1 != 0, arg2);
    }
    else if (what == MSG_ENABLED_AGENTS_CHANGED) {
        mHost->RefreshAgentList(IUserHandle::USER_ALL);
    }
    else if (what == MSG_REQUIRE_CREDENTIAL_ENTRY) {
        mHost->RequireCredentialEntry(arg1);
    }
    return NOERROR;
}

//=============================================================================
// TrustManagerService::InnerSub_PackageMonitor
//=============================================================================
TrustManagerService::InnerSub_PackageMonitor::InnerSub_PackageMonitor(
    /* [in] */ TrustManagerService* host)
    : mHost(host)
{}

ECode TrustManagerService::InnerSub_PackageMonitor::OnSomePackagesChanged()
{
    return mHost->RefreshAgentList(IUserHandle::USER_ALL);
}

ECode TrustManagerService::InnerSub_PackageMonitor::OnPackageChanged(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ ArrayOf<String>* components,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // We're interested in all changes, even if just some components get enabled / disabled.
    *result = TRUE;
    return NOERROR;
}

ECode TrustManagerService::InnerSub_PackageMonitor::OnPackageDisappeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    return mHost->RemoveAgentsOfPackage(packageName);
}

//=============================================================================
// TrustManagerService
//=============================================================================
const Boolean TrustManagerService::DEBUG = FALSE;
const String TrustManagerService::TAG("TrustManagerService");
const String TrustManagerService::PERMISSION_PROVIDE_AGENT = Manifest::permission::PROVIDE_TRUST_AGENT;
const Int32 TrustManagerService::MSG_REGISTER_LISTENER = 1;
const Int32 TrustManagerService::MSG_UNREGISTER_LISTENER = 2;
const Int32 TrustManagerService::MSG_DISPATCH_UNLOCK_ATTEMPT = 3;
const Int32 TrustManagerService::MSG_ENABLED_AGENTS_CHANGED = 4;
const Int32 TrustManagerService::MSG_REQUIRE_CREDENTIAL_ENTRY = 5;
AutoPtr<IIntent> TrustManagerService::TRUST_AGENT_INTENT = InitIntent();

TrustManagerService::TrustManagerService()
{
    mPackageMonitor = new InnerSub_PackageMonitor(this);
}

ECode TrustManagerService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    mArchive = new TrustArchive();
    CArraySet::New((IArraySet**)&mActiveAgents);
    CArrayList::New((IArrayList**)&mTrustListeners);
    mReceiver = new Receiver(this);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mUserHasAuthenticatedSinceBoot);
    mService = IBinder::Probe(new InnerSub_ITrustManager(this));
    mHandler = new InnerSub_Handler(this);

    mContext = context;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);
    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);
    return NOERROR;
}

ECode TrustManagerService::OnStart()
{
    PublishBinderService(IContext::TRUST_SERVICE, mService);
    return NOERROR;
}

ECode TrustManagerService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    Boolean isSafeMode;
    IsSafeMode(&isSafeMode);
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY && !isSafeMode) {
        mPackageMonitor->Register(mContext, Ptr(mHandler)->Func(mHandler->GetLooper), UserHandle::ALL, TRUE);
        mReceiver->Register(mContext);
        RefreshAgentList(IUserHandle::USER_ALL);
    }
    else if (phase == ISystemService::PHASE_BOOT_COMPLETED && !isSafeMode) {
        MaybeEnableFactoryTrustAgents(mLockPatternUtils, IUserHandle::USER_OWNER);
    }
    return NOERROR;
}

ECode TrustManagerService::UpdateTrustAll()
{
    AutoPtr<IList> userInfos;
    mUserManager->GetUsers(TRUE /* excludeDying */, (IList**)&userInfos);
    FOR_EACH(iter, userInfos) {
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        UpdateTrust(Ptr(userInfo)->Func(userInfo->GetId), FALSE);
    }
    return NOERROR;
}

ECode TrustManagerService::UpdateTrust(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean initiatedByUser)
{
    DispatchOnTrustManagedChanged(AggregateIsTrustManaged(userId), userId);
    DispatchOnTrustChanged(AggregateIsTrusted(userId), userId, initiatedByUser);
    return NOERROR;
}

ECode TrustManagerService::RefreshAgentList(
    /* [in] */ Int32 userId)
{
    if (DEBUG) Slogger::D(TAG, "refreshAgentList()");
    if (userId != IUserHandle::USER_ALL && userId < IUserHandle::USER_OWNER) {
        Logger::E(TAG, "refreshAgentList(userId=%d): Invalid user handle, must be USER_ALL or a specific user.",
        userId/* , new Throwable("here")*/);
        userId = IUserHandle::USER_ALL;
    }
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> userInfos;
    if (userId == IUserHandle::USER_ALL) {
        mUserManager->GetUsers(TRUE /* excludeDying */, (IList**)&userInfos);
    }
    else {
        CArrayList::New((IList**)&userInfos);
        AutoPtr<IUserInfo> userInfo;
        mUserManager->GetUserInfo(userId, (IUserInfo**)&userInfo);
        userInfos->Add(userInfo);
    }
    AutoPtr<ILockPatternUtils> lockPatternUtils = mLockPatternUtils;
    AutoPtr<IArraySet> obsoleteAgents;
    CArraySet::New((IArraySet**)&obsoleteAgents);
    obsoleteAgents->AddAll(mActiveAgents);
    FOR_EACH(iter, userInfos) {
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        if (!Ptr(userInfo)->Func(userInfo->SupportsSwitchTo)) continue;
        Int32 quality;
        lockPatternUtils->GetKeyguardStoredPasswordQuality(Ptr(userInfo)->Func(userInfo->GetId), &quality);
        if (quality == IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) continue;
        Boolean b;
        mUserHasAuthenticatedSinceBoot->Get(Ptr(userInfo)->Func(userInfo->GetId), &b);
        if (!b) continue;
        AutoPtr<IDevicePolicyManager> dpm;
        lockPatternUtils->GetDevicePolicyManager((IDevicePolicyManager**)&dpm);
        Int32 disabledFeatures;
        dpm->GetKeyguardDisabledFeatures(NULL, Ptr(userInfo)->Func(userInfo->GetId), &disabledFeatures);
        const Boolean disableTrustAgents =
                (disabledFeatures & IDevicePolicyManager::KEYGUARD_DISABLE_TRUST_AGENTS) != 0;
        AutoPtr<IList> enabledAgents;
        lockPatternUtils->GetEnabledTrustAgents(Ptr(userInfo)->Func(userInfo->GetId), (IList**)&enabledAgents);
        if (enabledAgents == NULL) {
            continue;
        }
        AutoPtr<IList> resolveInfos = ResolveAllowedTrustAgents(pm, Ptr(userInfo)->Func(userInfo->GetId));
        FOR_EACH(iter, resolveInfos) {
            AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
            AutoPtr<IComponentName> name = GetComponentName(resolveInfo);
            Boolean b;
            enabledAgents->Contains(name, &b);
            if (!b) continue;
            if (disableTrustAgents) {
                AutoPtr<IList> features;
                dpm->GetTrustAgentFeaturesEnabled(NULL /* admin */, name, (IList**)&features);
                // Disable agent if no features are enabled.
                if (features == NULL || Ptr(features)->Func(features->IsEmpty)) continue;
            }
            AutoPtr<AgentInfo> agentInfo = new AgentInfo();
            agentInfo->mComponent = name;
            userInfo->GetId(&(agentInfo->mUserId));
            ISet::Probe(mActiveAgents)->Contains(TO_IINTERFACE(agentInfo), &b);
            if (!b) {
                resolveInfo->LoadLabel(pm, (ICharSequence**)&(agentInfo->mLabel));
                resolveInfo->LoadIcon(pm, (IDrawable**)&(agentInfo->mIcon));
                GetSettingsComponentName(pm, resolveInfo, (IComponentName**)&(agentInfo->mSettings));
                agentInfo->mAgent = new TrustAgentWrapper();
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                intent->SetComponent(name);
                agentInfo->mAgent->constructor(mContext, this,
                        intent, Ptr(userInfo)->Func(userInfo->GetUserHandle));
                ISet::Probe(mActiveAgents)->Add(TO_IINTERFACE(agentInfo));
            } else {
                ISet::Probe(obsoleteAgents)->Remove(TO_IINTERFACE(agentInfo));
            }
        }
    }
    Boolean trustMayHaveChanged = FALSE;
    for (Int32 i = 0; i < Ptr(ISet::Probe(obsoleteAgents))->Func(ISet::GetSize); i++) {
        AutoPtr<IInterface> obj;
        obsoleteAgents->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(obj);
        if (userId == IUserHandle::USER_ALL || userId == info->mUserId) {
            if (Ptr(info->mAgent)->Func(info->mAgent->IsManagingTrust)) {
                trustMayHaveChanged = TRUE;
            }
            info->mAgent->Unbind();
            ISet::Probe(mActiveAgents)->Remove(TO_IINTERFACE(info));
        }
    }
    if (trustMayHaveChanged) {
        UpdateTrustAll();
    }
    return NOERROR;
}

ECode TrustManagerService::UpdateDevicePolicyFeatures()
{
    for (Int32 i = 0; i < Ptr(ISet::Probe(mActiveAgents))->Func(ISet::GetSize); i++) {
        AutoPtr<IInterface> obj;
        mActiveAgents->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(obj);
        Boolean b;
        info->mAgent->IsConnected(&b);
        if (b) {
            info->mAgent->UpdateDevicePolicyFeatures(&b);
        }
    }
    return NOERROR;
}

ECode TrustManagerService::RemoveAgentsOfPackage(
    /* [in] */ const String& packageName)
{
    Boolean trustMayHaveChanged = FALSE;
    for (Int32 i = Ptr(ISet::Probe(mActiveAgents))->Func(ISet::GetSize) - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mActiveAgents->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(obj);
        if (packageName.Equals(Ptr(info->mComponent)->Func(info->mComponent->GetPackageName))) {
            Logger::I(TAG, "Resetting agent %s", Ptr(info->mComponent)->Func(info->mComponent->FlattenToShortString).string());
            if (Ptr(info->mAgent)->Func(info->mAgent->IsManagingTrust)) {
                trustMayHaveChanged = TRUE;
            }
            info->mAgent->Unbind();
            obj = NULL;
            mActiveAgents->RemoveAt(i, (IInterface**)&obj);
        }
    }
    if (trustMayHaveChanged) {
        UpdateTrustAll();
    }
    return NOERROR;
}

ECode TrustManagerService::ResetAgent(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 userId)
{
    Boolean trustMayHaveChanged = FALSE;
    for (Int32 i = Ptr(ISet::Probe(mActiveAgents))->Func(ISet::GetSize) - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mActiveAgents->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(obj);
        Boolean bEquals;
        IObject::Probe(name)->Equals(info->mComponent, &bEquals);
        if (bEquals && userId == info->mUserId) {
            Logger::I(TAG, "Resetting agent %s", Ptr(info->mComponent)->Func(info->mComponent->FlattenToShortString).string());
            if (Ptr(info->mAgent)->Func(info->mAgent->IsManagingTrust)) {
                trustMayHaveChanged = TRUE;
            }
            info->mAgent->Unbind();
            obj = NULL;
            mActiveAgents->RemoveAt(i, (IInterface**)&obj);
        }
    }
    if (trustMayHaveChanged) {
        UpdateTrust(userId, FALSE);
    }
    RefreshAgentList(userId);
    return NOERROR;
}

ECode TrustManagerService::GetSettingsComponentName(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IResolveInfo* resolveInfo,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IServiceInfo> serviceInfo;
    resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
    if (resolveInfo == NULL || serviceInfo == NULL
            || Ptr(IPackageItemInfo::Probe(serviceInfo))->Func(IPackageItemInfo::GetMetaData) == NULL) FUNC_RETURN(NULL);
    String cn(NULL);
    AutoPtr<IXmlResourceParser> parser;
    ECode caughtException = NOERROR;
    // try {
    ECode ec;
    do {
        IPackageItemInfo::Probe(Ptr(resolveInfo)->Func(resolveInfo->GetServiceInfo))->LoadXmlMetaData(pm,
                ITrustAgentService::TRUST_AGENT_META_DATA, (IXmlResourceParser**)&parser);
        if (parser == NULL) {
            Slogger::W(TAG, "Can't find %s meta-data", ITrustAgentService::TRUST_AGENT_META_DATA.string());
            *result = NULL;
            return NOERROR;
        }
        AutoPtr<IResources> res;
        ec = pm->GetResourcesForApplication(Ptr(IComponentInfo::Probe(serviceInfo))->Func(IComponentInfo::GetApplicationInfo), (IResources**)&res);
        if (FAILED(ec)) break;
        AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));
        Int32 type;
        while (TRUE) {
            ec = IXmlPullParser::Probe(parser)->Next(&type);
            if (FAILED(ec)) break;
            if (type != IXmlPullParser::END_DOCUMENT
                && type != IXmlPullParser::START_TAG) continue;
            break;
            // Drain preamble.
        }
        if (FAILED(ec)) break;
        String nodeName;
        IXmlPullParser::Probe(parser)->GetName(&nodeName);
        if (!nodeName.Equals("trust-agent")) {
            Slogger::W(TAG, "Meta-data does not start with trust-agent tag");
            *result = NULL;
            return NOERROR;
        }
        AutoPtr<ITypedArray> sa;
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(3);
        (*array)[0] = R::styleable::TrustAgent[0];
        (*array)[1] = R::styleable::TrustAgent[1];
        (*array)[2] = R::styleable::TrustAgent[2];
        ec = res->ObtainAttributes(attrs, array, (ITypedArray**)&sa);
        if (FAILED(ec)) break;
        ec = sa->GetString(R::styleable::TrustAgent_settingsActivity, &cn);
        if (FAILED(ec)) break;
        ec = sa->Recycle();
        if (FAILED(ec)) break;
    } while(FALSE);
    if (FAILED(ec)) {
        // } catch (PackageManager.NameNotFoundException e) {
        if ((ECode)E_NAME_NOT_FOUND_EXCEPTION == ec) {
            caughtException = ec;
            ec = NOERROR;
        }
        // } catch (IOException e) {
        else if ((ECode)E_IO_EXCEPTION == ec) {
            caughtException = ec;
            ec = NOERROR;
        }
        // } catch (XmlPullParserException e) {
        else if ((ECode)E_XML_PULL_PARSER_EXCEPTION == ec) {
            caughtException = ec;
            ec = NOERROR;
        }
    }
    // } finally {
    if (parser != NULL) parser->Close();
    // }
    if (FAILED(ec)) return ec;
    if (caughtException != NOERROR) {
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        Slogger::W(TAG, "Error parsing : %s %d", Ptr(IPackageItemInfo::Probe(serviceInfo))->Func(IPackageItemInfo::GetPackageName).string(), caughtException);
        *result = NULL;
        return NOERROR;
    }
    if (cn == NULL) {
        *result = NULL;
        return NOERROR;
    }
    if (cn.IndexOf('/') < 0) {
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        String s;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&s);
        cn = s + "/" + cn;
    }
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    return helper->UnflattenFromString(cn, result);
}

AutoPtr<IComponentName> TrustManagerService::GetComponentName(
    /* [in] */ IResolveInfo* resolveInfo)
{
    AutoPtr<IComponentName> rev;
    AutoPtr<IServiceInfo> serviceInfo;
    resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
    if (resolveInfo == NULL || serviceInfo == NULL) return NULL;
    String s;
    IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&s);
    CComponentName::New(Ptr(IPackageItemInfo::Probe(serviceInfo))->Func(IPackageItemInfo::GetPackageName),
            Ptr(IPackageItemInfo::Probe(serviceInfo))->Func(IPackageItemInfo::GetName), (IComponentName**)&rev);
    return rev;
}

ECode TrustManagerService::MaybeEnableFactoryTrustAgents(
    /* [in] */ ILockPatternUtils* utils,
    /* [in] */ Int32 userId)
{
    Int32 value;
    Settings::Secure::GetInt32ForUser(Ptr(mContext)->Func(mContext->GetContentResolver),
            ISettingsSecure::TRUST_AGENTS_INITIALIZED, 0, userId, &value);
    if (0 != value) {
        return NOERROR;
    }
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> resolveInfos = ResolveAllowedTrustAgents(pm, userId);
    AutoPtr<IArraySet> discoveredAgents;
    CArraySet::New((IArraySet**)&discoveredAgents);
    FOR_EACH(iter, resolveInfos) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IComponentName> componentName = GetComponentName(resolveInfo);
        Int32 applicationInfoFlags;
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        Ptr(IComponentInfo::Probe(serviceInfo))->Func(IComponentInfo::GetApplicationInfo)->GetFlags(&applicationInfoFlags);
        if ((applicationInfoFlags & IApplicationInfo::FLAG_SYSTEM) == 0) {
            Logger::I(TAG, "Leaving agent %s disabled because package is not a system package.", TO_CSTR(componentName));
            continue;
        }
        ISet::Probe(discoveredAgents)->Add(componentName);
    }
    AutoPtr<IList> previouslyEnabledAgents;
    utils->GetEnabledTrustAgents(userId, (IList**)&previouslyEnabledAgents);
    if (previouslyEnabledAgents != NULL) {
        discoveredAgents->AddAll(IArraySet::Probe(previouslyEnabledAgents));
    }
    utils->SetEnabledTrustAgents(ICollection::Probe(discoveredAgents), userId);
    Boolean bNoUse;
    Settings::Secure::PutInt32ForUser(Ptr(mContext)->Func(mContext->GetContentResolver),
            ISettingsSecure::TRUST_AGENTS_INITIALIZED, 1, userId, &bNoUse);
    return NOERROR;
}

AutoPtr<IList> TrustManagerService::ResolveAllowedTrustAgents(
    /* [in] */ IPackageManager* pm,
    /* [in] */ Int32 userId)
{
    AutoPtr<IList> resolveInfos;
    pm->QueryIntentServicesAsUser(TRUST_AGENT_INTENT,
            0 /* flags */, userId, (IList**)&resolveInfos);
    AutoPtr<IArrayList> allowedAgents;
    CArrayList::New(Ptr(resolveInfos)->Func(resolveInfos->GetSize), (IArrayList**)&allowedAgents);
    FOR_EACH(iter, resolveInfos) {
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        if (serviceInfo == NULL) continue;
        if (Ptr(IComponentInfo::Probe(serviceInfo))->Func(IComponentInfo::GetApplicationInfo) == NULL) continue;
        String packageName;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
        Int32 permission;
        pm->CheckPermission(PERMISSION_PROVIDE_AGENT, packageName, &permission);
        if (permission != IPackageManager::PERMISSION_GRANTED) {
            AutoPtr<IComponentName> name = GetComponentName(resolveInfo);
            Logger::W(TAG, "Skipping agent %s because package does not have permission %s.", TO_CSTR(name), PERMISSION_PROVIDE_AGENT.string());
            continue;
        }
        allowedAgents->Add(resolveInfo);
    }
    return IList::Probe(allowedAgents);
}

Boolean TrustManagerService::AggregateIsTrusted(
    /* [in] */ Int32 userId)
{
    Boolean b;
    mUserHasAuthenticatedSinceBoot->Get(userId, &b);
    if (!b) {
        return FALSE;
    }
    for (Int32 i = 0; i < Ptr(ISet::Probe(mActiveAgents))->Func(ISet::GetSize); i++) {
        AutoPtr<IInterface> obj;
        mActiveAgents->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(obj);
        if (info->mUserId == userId) {
            if (Ptr(info->mAgent)->Func(info->mAgent->IsTrusted)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean TrustManagerService::AggregateIsTrustManaged(
    /* [in] */ Int32 userId)
{
    Boolean b;
    mUserHasAuthenticatedSinceBoot->Get(userId, &b);
    if (!b) {
            return FALSE;
    }
    for (Int32 i = 0; i < Ptr(ISet::Probe(mActiveAgents))->Func(ISet::GetSize); i++) {
        AutoPtr<IInterface> obj;
        mActiveAgents->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(obj);
        if (info->mUserId == userId) {
            if (Ptr(info->mAgent)->Func(info->mAgent->IsManagingTrust)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

ECode TrustManagerService::DispatchUnlockAttempt(
    /* [in] */ Boolean successful,
    /* [in] */ Int32 userId)
{
    for (Int32 i = 0; i < Ptr(ISet::Probe(mActiveAgents))->Func(ISet::GetSize); i++) {
        AutoPtr<IInterface> obj;
        mActiveAgents->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<AgentInfo> info = (AgentInfo*)IAgentInfo::Probe(obj);
        if (info->mUserId == userId) {
            info->mAgent->OnUnlockAttempt(successful);
        }
    }
    if (successful) {
        UpdateUserHasAuthenticated(userId);
    }
    return NOERROR;
}

ECode TrustManagerService::UpdateUserHasAuthenticated(
    /* [in] */ Int32 userId)
{
    Boolean b;
    mUserHasAuthenticatedSinceBoot->Get(userId, &b);
    if (!b) {
        mUserHasAuthenticatedSinceBoot->Put(userId, TRUE);
        RefreshAgentList(userId);
    }
    return NOERROR;
}

ECode TrustManagerService::RequireCredentialEntry(
    /* [in] */ Int32 userId)
{
    if (userId == IUserHandle::USER_ALL) {
        mUserHasAuthenticatedSinceBoot->Clear();
        RefreshAgentList(IUserHandle::USER_ALL);
    } else {
        mUserHasAuthenticatedSinceBoot->Put(userId, FALSE);
        RefreshAgentList(userId);
    }
    return NOERROR;
}

ECode TrustManagerService::AddListener(
    /* [in] */ IITrustListener* listener)
{
    for (Int32 i = 0; i < Ptr(mTrustListeners)->Func(mTrustListeners->GetSize); i++) {
        AutoPtr<IInterface> obj;
        mTrustListeners->Get(i, (IInterface**)&obj);
        if (IBinder::Probe(obj) == IBinder::Probe(listener)) {
            return NOERROR;
        }
    }
    mTrustListeners->Add(listener);
    UpdateTrustAll();
    return NOERROR;
}

ECode TrustManagerService::RemoveListener(
    /* [in] */ IITrustListener* listener)
{
    for (Int32 i = 0; i < Ptr(mTrustListeners)->Func(IArrayList::GetSize); i++) {
        AutoPtr<IInterface> obj;
        mTrustListeners->Get(i, (IInterface**)&obj);
        if (IBinder::Probe(obj) == IBinder::Probe(listener)) {
            mTrustListeners->Remove(i);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode TrustManagerService::DispatchOnTrustChanged(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean initiatedByUser)
{
    if (!enabled) initiatedByUser = FALSE;
    for (Int32 i = 0; i < Ptr(mTrustListeners)->Func(IArrayList::GetSize); i++) {
        // try {
        ECode ec;
        do {
            AutoPtr<IInterface> obj;
            ec = mTrustListeners->Get(i, (IInterface**)&obj);
            if (FAILED(ec)) break;
            ec = IITrustListener::Probe(obj)->OnTrustChanged(enabled, userId, initiatedByUser);
            if (FAILED(ec)) break;
        } while(FALSE);
        if (FAILED(ec)) {
            // } catch (DeadObjectException e) {
            if ((ECode)E_DEAD_OBJECT_EXCEPTION == ec) {
                Slogger::D(TAG, "Removing dead TrustListener.");
                mTrustListeners->Remove(i);
                i--;
            }
            // } catch (RemoteException e) {
            else if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "Exception while notifying TrustListener. %d", ec);
            }
            else
                return ec;
            // }
        }
    }
    return NOERROR;
}

ECode TrustManagerService::DispatchOnTrustManagedChanged(
    /* [in] */ Boolean managed,
    /* [in] */ Int32 userId)
{
    for (Int32 i = 0; i < Ptr(mTrustListeners)->Func(IArrayList::GetSize); i++) {
        ECode ec;
        // try {
        do {
            AutoPtr<IInterface> obj;
            ec = mTrustListeners->Get(i, (IInterface**)&obj);
            if (FAILED(ec)) break;
            ec = IITrustListener::Probe(obj)->OnTrustManagedChanged(managed, userId);
            if (FAILED(ec)) break;
        } while(FALSE);
        if (FAILED(ec)) {
            // } catch (DeadObjectException e) {
            if ((ECode)E_DEAD_OBJECT_EXCEPTION == ec) {
                Slogger::D(TAG, "Removing dead TrustListener.");
                mTrustListeners->Remove(i);
                i--;
            }
            // } catch (RemoteException e) {
            else if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "Exception while notifying TrustListener. %d", ec);
            }
            // }
            else
                return ec;
        }
    }
    return NOERROR;
}

AutoPtr<IIntent> TrustManagerService::InitIntent()
{
    AutoPtr<IIntent> rev;
    CIntent::New(ITrustAgentService::SERVICE_INTERFACE, (IIntent**)&rev);
    return rev;
}

} // namespace Trust
} // namespace Server
} // namespace Droid
} // namespace Elastos
