
#include "elastos/droid/server/telecom/TelecomServiceImpl.h"
#include "R.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallReceiver.h"
#include "elastos/droid/server/telecom/InCallController.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/Ringer.h"
#include "elastos/droid/server/telecom/TelephonyUtil.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/R.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Looper.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Telecomm::Internal::EIID_IITelecomService;
using Elastos::Droid::Telecomm::Telecom::ICallState;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Core::AutoLock;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::ISynchronize;
using Elastos::Core::StringUtils;
using Elastos::IO::IWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// TelecomServiceImpl::MainThreadRequest
//=============================================================================
CAR_INTERFACE_IMPL(TelecomServiceImpl::MainThreadRequest, Object, ITelecomServiceImplMainThreadRequest)

//=============================================================================
// TelecomServiceImpl::MainThreadHandler
//=============================================================================
TelecomServiceImpl::MainThreadHandler::MainThreadHandler(
    /* [in] */ TelecomServiceImpl* host)
    : mHost(host)
{}

ECode TelecomServiceImpl::MainThreadHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    if (ITelecomServiceImplMainThreadRequest::Probe(msgObj) != NULL) {
        AutoPtr<ITelecomServiceImplMainThreadRequest> request = ITelecomServiceImplMainThreadRequest::Probe(msgObj);
        AutoPtr<IObject> result;
        if (msgWhat == MSG_SILENCE_RINGER) {
            AutoPtr<Ringer> ringer;
            mHost->mCallsManager->GetRinger((Ringer**)&ringer);
            ringer->Silence();
        }
        else if (msgWhat == MSG_SHOW_CALL_SCREEN) {
            AutoPtr<InCallController> inCallController;
            mHost->mCallsManager->GetInCallController((InCallController**)&inCallController);
            inCallController->BringToForeground(msgArg1 == 1);
        }
        else if (msgWhat == MSG_END_CALL) {
            Boolean b;
            mHost->EndCallInternal(&b);
            AutoPtr<IBoolean> ibool;
            CBoolean::New(b, (IBoolean**)&ibool);
            result = IObject::Probe(ibool);
        }
        else if (msgWhat == MSG_ACCEPT_RINGING_CALL) {
            mHost->AcceptRingingCallInternal();
        }
        else if (msgWhat == MSG_CANCEL_MISSED_CALLS_NOTIFICATION) {
            mHost->mMissedCallNotifier->ClearMissedCalls();
        }
        else if (msgWhat == MSG_IS_TTY_SUPPORTED) {
            Boolean isTtySupported;
            mHost->mCallsManager->IsTtySupported(&isTtySupported);
            AutoPtr<IBoolean> ibool;
            CBoolean::New(isTtySupported, (IBoolean**)&ibool);
            result = IObject::Probe(ibool);
        }
        else if (msgWhat == MSG_GET_CURRENT_TTY_MODE) {
            Int32 mode;
            mHost->mCallsManager->GetCurrentTtyMode(&mode);
            AutoPtr<IInteger32> i32;
            CInteger32::New(mode, (IInteger32**)&i32);
            result = IObject::Probe(i32);
        }
        if (result != NULL) {
            ((MainThreadRequest*) request.Get())->mResult = result;
            {
                AutoLock lock(request);
                ISynchronize::Probe(request)->NotifyAll();
            }
        }
    }
    return NOERROR;
}

//=============================================================================
// TelecomServiceImpl
//=============================================================================
CAR_INTERFACE_IMPL_2(TelecomServiceImpl, Object, IITelecomService, IBinder)

const String TelecomServiceImpl::REGISTER_PROVIDER_OR_SUBSCRIPTION("com.android.server.telecom.permission.REGISTER_PROVIDER_OR_SUBSCRIPTION");
const String TelecomServiceImpl::REGISTER_CONNECTION_MANAGER("com.android.server.telecom.permission.REGISTER_CONNECTION_MANAGER");
const String TelecomServiceImpl::TAG("TelecomServiceImpl");
const String TelecomServiceImpl::SERVICE_NAME("telecom");
const Int32 TelecomServiceImpl::MSG_SILENCE_RINGER = 1;
const Int32 TelecomServiceImpl::MSG_SHOW_CALL_SCREEN = 2;
const Int32 TelecomServiceImpl::MSG_END_CALL = 3;
const Int32 TelecomServiceImpl::MSG_ACCEPT_RINGING_CALL = 4;
const Int32 TelecomServiceImpl::MSG_CANCEL_MISSED_CALLS_NOTIFICATION = 5;
const Int32 TelecomServiceImpl::MSG_IS_TTY_SUPPORTED = 6;
const Int32 TelecomServiceImpl::MSG_GET_CURRENT_TTY_MODE = 7;

TelecomServiceImpl::TelecomServiceImpl()
{
    AutoPtr<MainThreadHandler> mMainThreadHandler = new MainThreadHandler(this);
}

ECode TelecomServiceImpl::AsBinder(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)

    *result = IBinder::Probe(this);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomServiceImpl::constructor(
    /* [in] */ MissedCallNotifier* missedCallNotifier,
    /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar,
    /* [in] */ CallsManager* callsManager,
    /* [in] */ IContext* context)
{
        mMissedCallNotifier = missedCallNotifier;
        mPhoneAccountRegistrar = phoneAccountRegistrar;
        mCallsManager = callsManager;
        mContext = context;
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
        mAppOpsManager = IAppOpsManager::Probe(obj);
        return NOERROR;
}

ECode TelecomServiceImpl::GetDefaultOutgoingPhoneAccount(
    /* [in] */ const String& uriScheme,
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetDefaultOutgoingPhoneAccount(uriScheme, result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getDefaultOutgoingPhoneAccount %d", ec);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetUserSelectedOutgoingPhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetUserSelectedOutgoingPhoneAccount(result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getUserSelectedOutgoingPhoneAccount %d", ec);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::SetUserSelectedOutgoingPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    EnforceModifyPermission();
    // try {
    ECode ec = mPhoneAccountRegistrar->SetUserSelectedOutgoingPhoneAccount(accountHandle);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "setUserSelectedOutgoingPhoneAccount %d", ec);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetCallCapablePhoneAccounts(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetCallCapablePhoneAccounts(result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getCallCapablePhoneAccounts %d", ec);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetPhoneAccountsSupportingScheme(
    /* [in] */ const String& uriScheme,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetCallCapablePhoneAccounts(uriScheme, result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getPhoneAccountsSupportingScheme %d", ec);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetPhoneAccountsForPackage(
    /* [in] */ const String& packageName,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetPhoneAccountsForPackage(packageName, result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getPhoneAccountsForPackage %d", ec);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IPhoneAccount** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetPhoneAccount(accountHandle, result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getPhoneAccount %d %s", ec, TO_CSTR(accountHandle));
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetAllPhoneAccountsCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetAllPhoneAccountsCount(result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getAllPhoneAccountsCount %d", ec);
        return ec;;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetAllPhoneAccounts(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetAllPhoneAccounts(result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getAllPhoneAccounts %d", ec);
        return ec;;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetAllPhoneAccountHandles(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetAllPhoneAccountHandles(result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getAllPhoneAccounts %d", ec);
        return ec;;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetSimCallManager(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetSimCallManager(result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getSimCallManager %d", ec);
        return ec;;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::SetSimCallManager(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    EnforceModifyPermission();
    // try {
    ECode ec = mPhoneAccountRegistrar->SetSimCallManager(accountHandle);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "setSimCallManager %d", ec);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::GetSimCallManagers(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    ECode ec = mPhoneAccountRegistrar->GetConnectionManagerPhoneAccounts(result);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "getSimCallManagers %d", ec);
        return ec;;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::RegisterPhoneAccount(
    /* [in] */ IPhoneAccount* account)
{
    // try {
    ECode ec;
    do {
        AutoPtr<IComponentName> componentName;
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ec = account->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
        if (FAILED(ec)) break;
        ec = phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
        if (FAILED(ec)) break;
        String packageName;
        componentName->GetPackageName(&packageName);
        ec = EnforcePhoneAccountModificationForPackage(packageName);
        if (FAILED(ec)) break;
        Boolean hasCapabilities;
        account->HasCapabilities(IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION, &hasCapabilities);
        Boolean hasCapabilitiesCall;
        account->HasCapabilities(IPhoneAccount::CAPABILITY_CALL_PROVIDER, &hasCapabilitiesCall);
        if (hasCapabilitiesCall || hasCapabilities) {
            ec = EnforceRegisterProviderOrSubscriptionPermission();
            if (FAILED(ec)) break;
        }
        Boolean hasCapabilitiesConnection;
        account->HasCapabilities(IPhoneAccount::CAPABILITY_CONNECTION_MANAGER, &hasCapabilitiesConnection);
        if (hasCapabilitiesConnection) {
            ec = EnforceRegisterConnectionManagerPermission();
            if (FAILED(ec)) break;
        }
        ec = mPhoneAccountRegistrar->RegisterPhoneAccount(account);
    } while(FALSE);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "registerPhoneAccount %d %s", ec, account);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::UnregisterPhoneAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    // try {
    ECode ec;
    do {
        AutoPtr<IComponentName> componentName;
        accountHandle->GetComponentName((IComponentName**)&componentName);
        String packageName;
        componentName->GetPackageName(&packageName);
        ec = EnforcePhoneAccountModificationForPackage(packageName);
        if (FAILED(ec)) break;
        ec = mPhoneAccountRegistrar->UnregisterPhoneAccount(accountHandle);
    } while(FALSE);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "unregisterPhoneAccount %d %s", ec, accountHandle);
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::ClearAccounts(
    /* [in] */ const String& packageName)
{
    // try {
    ECode ec;
    do {
        ec = EnforcePhoneAccountModificationForPackage(packageName);
        if (FAILED(ec)) break;
        ec = mPhoneAccountRegistrar->ClearAccounts(packageName);
    } while(FALSE);
    // } catch (Exception e) {
    if (FAILED(ec)) {
        Log::E("TelecomServiceImpl", "clearAccounts %d %s", ec, packageName.string());
        return ec;
    }
    // }
    return NOERROR;
}

ECode TelecomServiceImpl::SilenceRinger()
{
    Log::D("TelecomServiceImpl", "silenceRinger");
    EnforceModifyPermission();
    AutoPtr<ITelecomServiceImplMainThreadRequest> request;
    SendRequestAsync(MSG_SILENCE_RINGER, 0, (ITelecomServiceImplMainThreadRequest**)&request);
    return NOERROR;
}

ECode TelecomServiceImpl::GetDefaultPhoneApp(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    String resStr;
    resources->GetString(R::string::ui_default_package, &resStr);
    String resStr2;
    resources->GetString(R::string::dialer_default_class, &resStr2);
    return CComponentName::New(resStr, resStr2, result);
}

ECode TelecomServiceImpl::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    // Do not use sendRequest() with this method since it could cause a deadlock with
    // audio service, which we call into from the main thread: AudioManager.setMode().
    Int32 callState;
    mCallsManager->GetCallState(&callState);
    *result = callState == ITelephonyManager::CALL_STATE_OFFHOOK
            || callState == ITelephonyManager::CALL_STATE_RINGING;
    return NOERROR;
}

ECode TelecomServiceImpl::IsRinging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    Int32 callState;
    mCallsManager->GetCallState(&callState);
    *result = callState == ITelephonyManager::CALL_STATE_RINGING;
    return NOERROR;
}

ECode TelecomServiceImpl::GetCallState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    return mCallsManager->GetCallState(result);
}

ECode TelecomServiceImpl::GetActiveSubscription(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    String activeSub;
    mCallsManager->GetActiveSubscription(&activeSub);
    *result = (activeSub == NULL) ? ISubscriptionManager::INVALID_SUB_ID : StringUtils::ParseInt64(activeSub);
    return NOERROR;
}

ECode TelecomServiceImpl::SwitchToOtherActiveSub(
    /* [in] */ Int64 subId)
{
    EnforceModifyPermission();
    String activeSub = (subId == ISubscriptionManager::INVALID_SUB_ID)
            ? String(NULL) : StringUtils::ToString(subId);
    mCallsManager->SwitchToOtherActiveSub(activeSub, FALSE);
    return NOERROR;
}

ECode TelecomServiceImpl::EndCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermission();
    AutoPtr<IInterface> obj;
    SendRequest(MSG_END_CALL, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode TelecomServiceImpl::AcceptRingingCall()
{
    EnforceModifyPermission();
    AutoPtr<ITelecomServiceImplMainThreadRequest> request;
    SendRequestAsync(MSG_ACCEPT_RINGING_CALL, 0, (ITelecomServiceImplMainThreadRequest**)&request);
    return NOERROR;
}

ECode TelecomServiceImpl::ShowInCallScreen(
    /* [in] */ Boolean showDialpad)
{
    EnforceReadPermissionOrDefaultDialer();
    AutoPtr<ITelecomServiceImplMainThreadRequest> request;
    SendRequestAsync(MSG_SHOW_CALL_SCREEN, showDialpad ? 1 : 0, (ITelecomServiceImplMainThreadRequest**)&request);
    return NOERROR;
}

ECode TelecomServiceImpl::CancelMissedCallsNotification()
{
    EnforceModifyPermissionOrDefaultDialer();
    AutoPtr<ITelecomServiceImplMainThreadRequest> request;
    SendRequestAsync(MSG_CANCEL_MISSED_CALLS_NOTIFICATION, 0, (ITelecomServiceImplMainThreadRequest**)&request);
    return NOERROR;
}

ECode TelecomServiceImpl::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceModifyPermissionOrDefaultDialer();
    // Switch identity so that TelephonyManager checks Telecom's permissions instead.
    Int64 token = Binder::ClearCallingIdentity();
    Boolean retval = FALSE;
    // try {
    ECode ec;
    do {
        AutoPtr<ITelephonyManager> tm;
        ec = GetTelephonyManager((ITelephonyManager**)&tm);
        if (FAILED(ec)) break;
        ec = tm->HandlePinMmi(dialString, &retval);
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }
    *result = retval;
    return ec;
}

ECode TelecomServiceImpl::IsTtySupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    AutoPtr<IInterface> obj;
    SendRequest(MSG_IS_TTY_SUPPORTED, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode TelecomServiceImpl::GetCurrentTtyMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    EnforceReadPermission();
    AutoPtr<IInterface> obj;
    SendRequest(MSG_GET_CURRENT_TTY_MODE, (IInterface**)&obj);
    IInteger32::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode TelecomServiceImpl::AddNewIncomingCall(
    /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IComponentName> componentName;
    phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
    if (phoneAccountHandle != NULL && componentName != NULL) {
        AutoPtr<IComponentName> componentName;
        phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
        String packageName;
        componentName->GetPackageName(&packageName);
        mAppOpsManager->CheckPackage(Binder::GetCallingUid(), packageName);
        AutoPtr<IIntent> intent;
        CIntent::New(ITelecomManager::ACTION_INCOMING_CALL, (IIntent**)&intent);
        mContext->GetPackageName(&packageName);
        intent->SetPackage(packageName);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        intent->PutExtra(ITelecomManager::EXTRA_PHONE_ACCOUNT_HANDLE, IParcelable::Probe(phoneAccountHandle));
        if (extras != NULL) {
            intent->PutExtra(ITelecomManager::EXTRA_INCOMING_CALL_EXTRAS, extras);
        }
        Int64 token = Binder::ClearCallingIdentity();
        AutoPtr<IUserHandleHelper> userHandleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
        AutoPtr<IUserHandle> current;
        userHandleHelper->GetCURRENT((IUserHandle**)&current);
        mContext->StartActivityAsUser(intent, current);
        Binder::RestoreCallingIdentity(token);
    }
    return NOERROR;
}

ECode TelecomServiceImpl::AddNewUnknownCall(
    /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IComponentName> componentName;
    phoneAccountHandle->GetComponentName((IComponentName**)&componentName);
    Boolean isPstnComponentName;
    TelephonyUtil::IsPstnComponentName(componentName, &isPstnComponentName);
    if (phoneAccountHandle != NULL && componentName != NULL &&
            isPstnComponentName) {
        String packageName;
        componentName->GetPackageName(&packageName);
        mAppOpsManager->CheckPackage(
                Binder::GetCallingUid(), packageName);
        AutoPtr<IIntent> intent;
        CIntent::New(ITelecomManager::ACTION_NEW_UNKNOWN_CALL, (IIntent**)&intent);
        intent->SetClass(mContext, ECLSID_CCallReceiver);
        intent->SetFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        intent->PutExtras(extras);
        intent->PutExtra(CallReceiver::KEY_IS_UNKNOWN_CALL, TRUE);
        intent->PutExtra(ITelecomManager::EXTRA_PHONE_ACCOUNT_HANDLE, IParcelable::Probe(phoneAccountHandle));
        AutoPtr<IUserHandleHelper> userHandleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
        AutoPtr<IUserHandle> owner;
        userHandleHelper->GetOWNER((IUserHandle**)&owner);
        mContext->SendBroadcastAsUser(intent, owner);
    } else {
        Log::I("TelecomServiceImpl", "Null phoneAccountHandle or not initiated by Telephony. Ignoring request"
                " to add new unknown call.");
    }
    return NOERROR;
}

ECode TelecomServiceImpl::AcceptRingingCallInternal()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
    (*array)[0] = ICallState::RINGING;
    AutoPtr<ICall> call;
    mCallsManager->GetFirstCallWithState(array, (ICall**)&call);
    Int32 videoState;
    call->GetVideoState(&videoState);
    if (call != NULL) {
        ((Call*) call.Get())->Answer(videoState);
    }
    return NOERROR;
}

ECode TelecomServiceImpl::EndCallInternal(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Always operate on the foreground call if one exists, otherwise get the first call in
    // priority order by call-state.
    AutoPtr<ICall> call;
    mCallsManager->GetForegroundCall((ICall**)&call);
    if (call == NULL) {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
        (*array)[0] = ICallState::ACTIVE;
        (*array)[1] = ICallState::DIALING;
        (*array)[2] = ICallState::RINGING;
        (*array)[3] = ICallState::ON_HOLD;
        mCallsManager->GetFirstCallWithState(
                array,
                (ICall**)&call);
    }
    if (call != NULL) {
        Int32 callState;
        ((Call*) call.Get())->GetState(&callState);
        if (callState == ICallState::RINGING) {
            ((Call*) call.Get())->Reject(FALSE /* rejectWithMessage */, String(NULL));
        } else {
            ((Call*) call.Get())->Disconnect();
        }
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode TelecomServiceImpl::EnforcePhoneAccountModificationForPackage(
    /* [in] */ const String& packageName)
{
    // TODO: Use a new telecomm permission for this instead of reusing modify.
    Int32 result;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::MODIFY_PHONE_STATE, &result);
    // Callers with MODIFY_PHONE_STATE can use the PhoneAccount mechanism to implement
    // built-in behavior even when PhoneAccounts are not exposed as a third-part API. They
    // may also modify PhoneAccounts on behalf of any 'packageName'.
    if (result != IPackageManager::PERMISSION_GRANTED) {
        // Other callers are only allowed to modify PhoneAccounts if the relevant system
        // feature is enabled ...
        EnforceConnectionServiceFeature();
        // ... and the PhoneAccounts they refer to are for their own package.
        EnforceCallingPackage(packageName);
    }
    return NOERROR;
}

ECode TelecomServiceImpl::EnforceReadPermissionOrDefaultDialer()
{
    Boolean isDefaultDialerCalling;
    IsDefaultDialerCalling(&isDefaultDialerCalling);
    if (!isDefaultDialerCalling) {
        EnforceReadPermission();
    }
    return NOERROR;
}

ECode TelecomServiceImpl::EnforceModifyPermissionOrDefaultDialer()
{
    Boolean isDefaultDialerCalling;
    IsDefaultDialerCalling(&isDefaultDialerCalling);
    if (!isDefaultDialerCalling) {
        EnforceModifyPermission();
    }
    return NOERROR;
}

ECode TelecomServiceImpl::EnforceCallingPackage(
    /* [in] */ const String& packageName)
{
    return mAppOpsManager->CheckPackage(Binder::GetCallingUid(), packageName);
}

ECode TelecomServiceImpl::EnforceConnectionServiceFeature()
{
    return EnforceFeature(IPackageManager::FEATURE_CONNECTION_SERVICE);
}

ECode TelecomServiceImpl::EnforceRegisterProviderOrSubscriptionPermission()
{
    return EnforcePermission(REGISTER_PROVIDER_OR_SUBSCRIPTION);
}

ECode TelecomServiceImpl::EnforceRegisterConnectionManagerPermission()
{
    return EnforcePermission(REGISTER_CONNECTION_MANAGER);
}

ECode TelecomServiceImpl::EnforceReadPermission()
{
    return EnforcePermission(Manifest::permission::READ_PHONE_STATE);
}

ECode TelecomServiceImpl::EnforceModifyPermission()
{
    return EnforcePermission(Manifest::permission::MODIFY_PHONE_STATE);
}

ECode TelecomServiceImpl::EnforcePermission(
    /* [in] */ const String& permission)
{
    return mContext->EnforceCallingOrSelfPermission(permission, String(NULL));
}

ECode TelecomServiceImpl::EnforceFeature(
    /* [in] */ const String& feature)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Boolean hasSystemFeature;
    pm->HasSystemFeature(feature, &hasSystemFeature);
    if (!hasSystemFeature) {
        Log::E("PhoneAccountRegistrar", "System does not support feature %s", feature.string());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode TelecomServiceImpl::IsDefaultDialerCalling(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IComponentName> defaultDialerComponent;
    GetDefaultPhoneApp((IComponentName**)&defaultDialerComponent);
    if (defaultDialerComponent != NULL) {
        // try {
        ECode ec;
        do {
            String packageName;
            ec = defaultDialerComponent->GetPackageName(&packageName);
            if (FAILED(ec)) break;
            ec = mAppOpsManager->CheckPackage(Binder::GetCallingUid(), packageName);
            if (FAILED(ec)) break;
            *result = TRUE;
            return NOERROR;
        } while(FALSE);
        // } catch (SecurityException e) {
        if (FAILED(ec)) {
            if ((ECode) E_SECURITY_EXCEPTION == ec) {
                Log::E(TAG, "Could not get default dialer. %d", ec);
            }
            else {
                return ec;
            }
        }
        // }
    }
    *result = FALSE;
    return NOERROR;
}

ECode TelecomServiceImpl::GetTelephonyManager(
    /* [out] */ ITelephonyManager** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    *result = ITelephonyManager::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomServiceImpl::SendRequestAsync(
    /* [in] */ Int32 command,
    /* [in] */ Int32 arg1,
    /* [out] */ ITelecomServiceImplMainThreadRequest** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ITelecomServiceImplMainThreadRequest> request = new MainThreadRequest();
    AutoPtr<IMessage> msg;
    mMainThreadHandler->ObtainMessage(command, arg1, 0, request, (IMessage**)&msg);
    msg->SendToTarget();
    *result = request;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomServiceImpl::SendRequest(
    /* [in] */ Int32 command,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ILooper> looper;
    mMainThreadHandler->GetLooper((ILooper**)&looper);
    if (Looper::GetMyLooper() == looper) {
        AutoPtr<ITelecomServiceImplMainThreadRequest> request = new MainThreadRequest();
        AutoPtr<IMessage> msg;
        mMainThreadHandler->ObtainMessage(command, request, (IMessage**)&msg);
        mMainThreadHandler->HandleMessage(msg);
        *result = ((MainThreadRequest*) request.Get())->mResult;
        REFCOUNT_ADD(*result)
        return NOERROR;
    } else {
        AutoPtr<ITelecomServiceImplMainThreadRequest> request;
        SendRequestAsync(command, 0, (ITelecomServiceImplMainThreadRequest**)&request);
        // Wait for the request to complete
        {
            AutoLock lock(request);
            while (((MainThreadRequest*) request.Get())->mResult == NULL) {
                // try {
                ISynchronize::Probe(request)->Wait();
                // } catch (InterruptedException e) {
                    // Do nothing, go back and wait until the request is complete
                // }
            }
        }
        *result = ((MainThreadRequest*) request.Get())->mResult;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode TelecomServiceImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    mContext->EnforceCallingOrSelfPermission(Droid::Manifest::permission::DUMP, TAG);
    AutoPtr<IIndentingPrintWriter> pw;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), (IIndentingPrintWriter**)&pw);
    if (mCallsManager != NULL) {
        IPrintWriter::Probe(pw)->Println(String("mCallsManager: "));
        pw->IncreaseIndent();
        mCallsManager->Dump(pw);
        pw->DecreaseIndent();
    }
    return NOERROR;
}

ECode TelecomServiceImpl::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev("TelecomServiceImpl");
    rev.AppendFormat(":%s;%s;%s;%s;%s;", TO_CSTR(mMainThreadHandler), TO_CSTR(mCallsManager), TO_CSTR(mMissedCallNotifier), TO_CSTR(mPhoneAccountRegistrar), TO_CSTR(mAppOpsManager));
    *result = rev;
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
