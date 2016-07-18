
#include "elastos/droid/server/telecom/InCallController.h"
#include "R.h"
#include "elastos/droid/server/telecom/CallIdMapper.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/InCallAdapter.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/app/Service.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Droid::Telecomm::Internal::IIInCallService;
using Elastos::Droid::Telecomm::Telecom::CParcelableCall;
using Elastos::Droid::Telecomm::Telecom::ICallProperties;
using Elastos::Droid::Telecomm::Telecom::ICallState;
using Elastos::Droid::Telecomm::Telecom::IDisconnectCause;
using Elastos::Droid::Telecomm::Telecom::IGatewayInfo;
using Elastos::Droid::Telecomm::Telecom::IInCallAdapter;
using Elastos::Droid::Telecomm::Telecom::IInCallService;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecomm::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecomm::Telecom::IStatusHints;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// InCallController::InCallServiceConnection
//=============================================================================
CAR_INTERFACE_IMPL(InCallController::InCallServiceConnection, Object, IServiceConnection)

InCallController::InCallServiceConnection::InCallServiceConnection(
    /* [in] */ InCallController* host)
    : mHost(host)
{}

ECode InCallController::InCallServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    Log::D("InCallController", "onServiceConnected: %s", TO_CSTR(name));
    mHost->OnConnected(name, service);
    return NOERROR;
}

ECode InCallController::InCallServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    Log::D("InCallController", "onDisconnected: %s", TO_CSTR(name));
    mHost->OnDisconnected(name);
    return NOERROR;
}

//=============================================================================
// InCallController::SubCallListenerBase
//=============================================================================
InCallController::SubCallListenerBase::SubCallListenerBase(
    /* [in] */ InCallController* host)
    : mHost(host)
{}

ECode InCallController::SubCallListenerBase::OnCallCapabilitiesChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnCallPropertiesChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnCannedSmsResponsesLoaded(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnVideoCallProviderChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnStatusHintsChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnHandleChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnCallerDisplayNameChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnVideoStateChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnTargetPhoneAccountChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnConferenceableCallsChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

ECode InCallController::SubCallListenerBase::OnCallSubstateChanged(
    /* [in] */ ICall* call)
{
    return mHost->UpdateCall(call);
}

//=============================================================================
// InCallController
//=============================================================================
ECode InCallController::constructor(
    /* [in] */ IContext* context)
{
    mCallListener = new SubCallListenerBase(this);
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&mServiceConnections);
    CArrayMap::New((IMap**)&mInCallServices);
    mCallIdMapper = new CallIdMapper();
    ((CallIdMapper*) mCallIdMapper.Get())->constructor(String("InCall"));
    mContext = context;
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    String str;
    resources->GetString(R::string::ui_default_package, &str);
    String str2;
    resources->GetString(R::string::incall_default_class, &str2);
    CComponentName::New(str, str2, (IComponentName**)&mInCallComponentName);
    return NOERROR;
}

ECode InCallController::OnCallAdded(
    /* [in] */ ICall* call)
{
    Boolean isEmpty;
    mInCallServices->IsEmpty(&isEmpty);
    if (isEmpty) {
        Bind();
    } else {
        Log::I("InCallController", "onCallAdded: %s", TO_CSTR(call));
        // Track the call if we don't already know about it.
        AddCall(call);
        AutoPtr<ISet> entrySet;
        mInCallServices->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            obj = NULL;
            entry->GetKey((IInterface**)&obj);
            AutoPtr<IComponentName> componentName = IComponentName::Probe(obj);
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            AutoPtr<IIInCallService> inCallService = IIInCallService::Probe(obj);
            AutoPtr<IParcelableCall> parcelableCall;
            Boolean isEquals;
            IObject::Probe(componentName)->Equals(mInCallComponentName, &isEquals);
            ToParcelableCall(call,
                    isEquals /* includeVideoProvider */,
                    (IParcelableCall**)&parcelableCall);
            // try {
            inCallService->AddCall(parcelableCall);
            // } catch (RemoteException ignored) {
            // }
        }
    }
    return NOERROR;
}

ECode InCallController::OnCallRemoved(
    /* [in] */ ICall* call)
{
    Log::I("InCallController", "onCallRemoved: ");//TODO, TO_CSTR(call));
    AutoPtr<IList> calls;
    CallsManager::GetInstance()->GetCalls((IList**)&calls);
    Boolean isEmpty;
    calls->IsEmpty(&isEmpty);
    if (isEmpty) {
        // TODO: Wait for all messages to be delivered to the service before unbinding.
        Unbind();
    }
    ((Call*) call)->RemoveListener(mCallListener);
    ((CallIdMapper*) mCallIdMapper.Get())->RemoveCall(call);
    return NOERROR;
}

ECode InCallController::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    return UpdateCall(call);
}

ECode InCallController::OnCallExtrasUpdated(
    /* [in] */ ICall* call)
{
    return UpdateCall(call);
}

ECode InCallController::OnConnectionServiceChanged(
    /* [in] */ ICall* call,
    /* [in] */ IConnectionServiceWrapper* oldService,
    /* [in] */ IConnectionServiceWrapper* newService)
{
    return UpdateCall(call);
}

ECode InCallController::OnAudioStateChanged(
    /* [in] */ IAudioState* oldAudioState,
    /* [in] */ IAudioState* newAudioState)
{
    Boolean isEmpty;
    mInCallServices->IsEmpty(&isEmpty);
    if (!isEmpty) {
        Log::I("InCallController", "Calling onAudioStateChanged, audioState: %s -> %s", TO_CSTR(oldAudioState),
                TO_CSTR(newAudioState));
        AutoPtr<ICollection> values;
        mInCallServices->GetValues((ICollection**)&values);
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IIInCallService> inCallService = IIInCallService::Probe(obj);
            // try {
            inCallService->OnAudioStateChanged(newAudioState);
            // } catch (RemoteException ignored) {
            // }
        }
    }
    return NOERROR;
}

ECode InCallController::OnCanAddCallChanged(
    /* [in] */ Boolean canAddCall)
{
    Boolean isEmpty;
    mInCallServices->IsEmpty(&isEmpty);
    if (!isEmpty) {
        Log::I("InCallController", "onCanAddCallChanged : %d", canAddCall);
        AutoPtr<ICollection> values;
        mInCallServices->GetValues((ICollection**)&values);
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IIInCallService> inCallService = IIInCallService::Probe(obj);
            // try {
            assert(0 && "TODO IIInCallService");
            // inCallService->OnCanAddCallChanged(canAddCall);
            // } catch (RemoteException ignored) {
            // }
        }
    }
    return NOERROR;
}

ECode InCallController::OnPostDialWait(
    /* [in] */ ICall* call,
    /* [in] */ const String& remaining)
{
    Boolean isEmpty;
    mInCallServices->IsEmpty(&isEmpty);
    if (!isEmpty) {
        Log::I("InCallController", "Calling onPostDialWait, remaining = %s", remaining.string());
        AutoPtr<ICollection> values;
        mInCallServices->GetValues((ICollection**)&values);
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IIInCallService> inCallService = IIInCallService::Probe(obj);
            // try {
            String callId;
            ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
            inCallService->SetPostDialWait(callId, remaining);
            // } catch (RemoteException ignored) {
            // }
        }
    }
    return NOERROR;
}

ECode InCallController::OnIsConferencedChanged(
    /* [in] */ ICall* call)
{
    Log::D("InCallController", "onIsConferencedChanged %s", TO_CSTR(call));
    UpdateCall(call);
    return NOERROR;
}

ECode InCallController::BringToForeground(
    /* [in] */ Boolean showDialpad)
{
    Boolean isEmpty;
    mInCallServices->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<ICollection> values;
        mInCallServices->GetValues((ICollection**)&values);
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IIInCallService> inCallService = IIInCallService::Probe(obj);
            // try {
                inCallService->BringToForeground(showDialpad);
            // } catch (RemoteException ignored) {
            // }
        }
    } else {
        Log::W("InCallController", "Asking to bring unbound in-call UI to foreground.");
    }
    return NOERROR;
}

ECode InCallController::Unbind()
{
    ThreadUtil::CheckOnMainThread();
    AutoPtr<ISet> entrySet;
    mServiceConnections->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> iterator;
    entrySet->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        Log::I("InCallController", "Unbinding from InCallService %s", TO_CSTR(obj));
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        obj = NULL;
        entry->GetValue((IInterface**)&obj);
        mContext->UnbindService(IServiceConnection::Probe(obj));
        iterator->Remove();
    }
    mInCallServices->Clear();
    return NOERROR;
}

ECode InCallController::Bind()
{
    ThreadUtil::CheckOnMainThread();
    Boolean isEmpty;
    mInCallServices->IsEmpty(&isEmpty);
    if (isEmpty) {
        AutoPtr<IPackageManager> packageManager;
        mContext->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<IIntent> serviceIntent;
        CIntent::New(IInCallService::SERVICE_INTERFACE, (IIntent**)&serviceIntent);
        AutoPtr<IList> intentServices;
        packageManager->QueryIntentServices(serviceIntent, 0, (IList**)&intentServices);
        AutoPtr<IIterator> it;
        intentServices->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IResolveInfo> entry = IResolveInfo::Probe(obj);
            AutoPtr<IServiceInfo> serviceInfo;
            entry->GetServiceInfo((IServiceInfo**)&serviceInfo);
            if (serviceInfo != NULL) {
                String permission;
                serviceInfo->GetPermission(&permission);
                Boolean isEquals = permission.Equals(
                        Manifest::permission::BIND_INCALL_SERVICE);
                Boolean hasServiceBindPermission = !permission.IsNull() && isEquals;
                String packageName;
                IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
                Int32 permissionCount;
                packageManager->CheckPermission(
                        Manifest::permission::CONTROL_INCALL_EXPERIENCE,
                        packageName, &permissionCount);
                Boolean hasControlInCallPermission = permissionCount == IPackageManager::PERMISSION_GRANTED;
                if (!hasServiceBindPermission) {
                    Log::W("InCallController", "InCallService does not have BIND_INCALL_SERVICE permission: %s",
                            packageName.string());
                    continue;
                }
                if (!hasControlInCallPermission) {
                    Log::W("InCallController",
                            "InCall UI does not have CONTROL_INCALL_EXPERIENCE permission: %s",
                                    packageName.string());
                    continue;
                }
                AutoPtr<IServiceConnection> inCallServiceConnection = new InCallServiceConnection(this);
                String name;
                IPackageItemInfo::Probe(serviceInfo)->GetName(&name);
                AutoPtr<IComponentName> componentName;
                CComponentName::New(packageName, name, (IComponentName**)&componentName);
                Boolean isContainsKey;
                mServiceConnections->ContainsKey(componentName, &isContainsKey);
                Log::I("InCallController", "Attempting to bind to InCall %s %s, is dupe? %d ",
                        packageName.string(), name.string(),
                        isContainsKey);
                if (!isContainsKey) {
                    AutoPtr<IIntent> intent;
                    CIntent::New(IInCallService::SERVICE_INTERFACE, (IIntent**)&intent);
                    intent->SetComponent(componentName);
                    AutoPtr<IUserHandleHelper> userHandleHelper;
                    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&userHandleHelper);
                    AutoPtr<IUserHandle> current;
                    userHandleHelper->GetCURRENT((IUserHandle**)&current);
                    Boolean isBindServiceAsUserOk;
                    mContext->BindServiceAsUser(intent, inCallServiceConnection,
                            IContext::BIND_AUTO_CREATE, current, &isBindServiceAsUserOk);
                    if (isBindServiceAsUserOk) {
                        mServiceConnections->Put(componentName, inCallServiceConnection);
                    }
                }
            }
        }
    }
    return NOERROR;
}

ECode InCallController::OnConnected(
    /* [in] */ IComponentName* componentName,
    /* [in] */ IBinder* service)
{
    ThreadUtil::CheckOnMainThread();
    Log::I("InCallController", "onConnected to %s", TO_CSTR(componentName));
    AutoPtr<IIInCallService> inCallService = IIInCallService::Probe(service);
    // try {
    ECode ec;
    do {
        AutoPtr<InCallAdapter> inCallAdapter = new InCallAdapter();
        inCallAdapter->constructor(CallsManager::GetInstance(), mCallIdMapper);
        ec = inCallService->SetInCallAdapter(inCallAdapter);
        if (FAILED(ec)) break;
        ec = mInCallServices->Put(componentName, inCallService);
    } while(FALSE);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            Log::E("InCallController", "Failed to set the in-call adapter.");
            return NOERROR;
        }
        else {
            return ec;
        }
    }
    // }
    // Upon successful connection, send the state of the world to the service.
    AutoPtr<IList> calls; // ImmutableCollection
    CallsManager::GetInstance()->GetCalls((IList**)&calls);
    Boolean isEmpty;
    calls->IsEmpty(&isEmpty);
    if (!isEmpty) {
        Int32 callsSize;
        calls->GetSize(&callsSize);
        Log::I("InCallController", "Adding %d calls to InCallService after onConnected: %s", callsSize,
                TO_CSTR(componentName));
        AutoPtr<IIterator> it;
        calls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            // try {
            ECode ec;
            do {
                // Track the call if we don't already know about it.
                Log::I("InCallController", "addCall after binding: %s", TO_CSTR(call));
                ec = AddCall(call);
                if (FAILED(ec)) break;
                Boolean isEquals;
                IObject::Probe(componentName)->Equals(mInCallComponentName, &isEquals);
                AutoPtr<IParcelableCall> parcelableCall;
                ec = ToParcelableCall(call,
                        isEquals /* includeVideoProvider */, (IParcelableCall**)&parcelableCall);
                if (FAILED(ec)) break;
                inCallService->AddCall(parcelableCall);
            } while(FALSE);
            // } catch (RemoteException ignored) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION != ec) {
                    return ec;
                }
            }
            // }
        }
        AutoPtr<IAudioState> audioState;
        CallsManager::GetInstance()->GetAudioState((IAudioState**)&audioState);
        OnAudioStateChanged(NULL, audioState);
    } else {
        Unbind();
    }
    return NOERROR;
}

ECode InCallController::OnDisconnected(
    /* [in] */ IComponentName* disconnectedComponent)
{
    Log::I("InCallController", "onDisconnected from %s", TO_CSTR(disconnectedComponent));
    ThreadUtil::CheckOnMainThread();
    Boolean isContainsKey;
    mInCallServices->ContainsKey(disconnectedComponent, &isContainsKey);
    if (isContainsKey) {
        mInCallServices->Remove(disconnectedComponent);
    }
    mServiceConnections->ContainsKey(disconnectedComponent, &isContainsKey);
    if (isContainsKey) {
        // One of the services that we were bound to has disconnected. If the default in-call UI
        // has disconnected, disconnect all calls and un-bind all other InCallService
        // implementations.
        Boolean isEquals;
        IObject::Probe(disconnectedComponent)->Equals(mInCallComponentName, &isEquals);
        if (isEquals) {
            Log::I("InCallController", "In-call UI %s disconnected.", TO_CSTR(disconnectedComponent));
            CallsManager::GetInstance()->DisconnectAllCalls();
            Unbind();
        } else {
            Log::I("InCallController", "In-Call Service %s suddenly disconnected", TO_CSTR(disconnectedComponent));
            // Else, if it wasn't the default in-call UI, then one of the other in-call services
            // disconnected and, well, that's probably their fault.  Clear their state and
            // ignore.
            AutoPtr<IInterface> obj;
            mServiceConnections->Get(disconnectedComponent, (IInterface**)&obj);
            AutoPtr<IServiceConnection> serviceConnection = IServiceConnection::Probe(obj);
            // We still need to call unbind even though it disconnected.
            mContext->UnbindService(serviceConnection);
            mServiceConnections->Remove(disconnectedComponent);
            mInCallServices->Remove(disconnectedComponent);
        }
    }
    return NOERROR;
}

ECode InCallController::UpdateCall(
    /* [in] */ ICall* call)
{
    Boolean isEmpty;
    mInCallServices->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<ISet> entrySet;
        mInCallServices->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            obj = NULL;
            entry->GetKey((IInterface**)&obj);
            AutoPtr<IComponentName> componentName = IComponentName::Probe(obj);
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            AutoPtr<IIInCallService> inCallService = IIInCallService::Probe(obj);
            Boolean isEquals;
            IObject::Probe(componentName)->Equals(mInCallComponentName, &isEquals);
            AutoPtr<IParcelableCall> parcelableCall;
            ToParcelableCall(call,
                    isEquals /* includeVideoProvider */, (IParcelableCall**)&parcelableCall);
            Log::V("InCallController", "updateCall %s ==> %s", TO_CSTR(call), TO_CSTR(parcelableCall));
            // try {
            inCallService->UpdateCall(parcelableCall);
            // } catch (RemoteException ignored) {
            // }
        }
    }
    return NOERROR;
}

ECode InCallController::ToParcelableCall(
    /* [in] */ ICall* call,
    /* [in] */ Boolean includeVideoProvider,
    /* [out] */ IParcelableCall** result)
{
    VALIDATE_NOT_NULL(result)

    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Int32 capabilities;
    ((Call*) call)->GetCallCapabilities(&capabilities);
    // Disable mute and add call for emergency calls.
    Boolean isEmergencyCall;
    ((Call*) call)->IsEmergencyCall(&isEmergencyCall);
    if (isEmergencyCall) {
        capabilities &= ~IPhoneCapabilities::MUTE;
    }
    Int32 properties;
    ((Call*) call)->GetCallProperties(&properties);
    Boolean isConference;
    ((Call*) call)->IsConference(&isConference);
    if (isConference) {
        properties |= ICallProperties::CONFERENCE;
    }
    Int32 state;
    ((Call*) call)->GetState(&state);
    if (state == ICallState::ABORTED) {
        state = ICallState::DISCONNECTED;
    }
    Boolean isLocallyDisconnecting;
    call->IsLocallyDisconnecting(&isLocallyDisconnecting);
    if (isLocallyDisconnecting && state != ICallState::DISCONNECTED) {
        state = ICallState::DISCONNECTING;
    }
    String parentCallId(NULL);
    AutoPtr<ICall> parentCall;
    ((Call*) call)->GetParentCall((ICall**)&parentCall);
    if (parentCall != NULL) {
        String callId;
        ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(parentCall, &callId);
        parentCallId = callId;
    }
    Int64 connectTimeMillis;
    ((Call*) call)->GetConnectTimeMillis(&connectTimeMillis);
    AutoPtr<IList> childCalls;
    ((Call*) call)->GetChildCalls((IList**)&childCalls);
    AutoPtr<IList> childCallIds;
    CArrayList::New((IList**)&childCallIds);
    Boolean isEmpty;
    childCalls->IsEmpty(&isEmpty);
    if (!isEmpty) {
        connectTimeMillis = Elastos::Core::Math::INT64_MAX_VALUE;
        AutoPtr<IIterator> it;
        childCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> child = ICall::Probe(obj);
            Int64 childConnectTimeMillis;
            ((Call*) child.Get())->GetConnectTimeMillis(&childConnectTimeMillis);
            if (childConnectTimeMillis > 0) {
                connectTimeMillis = Elastos::Core::Math::Min(childConnectTimeMillis, connectTimeMillis);
            }
            String callId;
            ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(child, &callId);
            childCallIds->Add(StringUtils::ParseCharSequence(callId));
        }
    }
    Boolean isRespondViaSmsCapable;
    ((Call*) call)->IsRespondViaSmsCapable(&isRespondViaSmsCapable);
    if (isRespondViaSmsCapable) {
        capabilities |= IPhoneCapabilities::RESPOND_VIA_TEXT;
    }
    AutoPtr<IUri> callHandle;
    ((Call*) call)->GetHandle((IUri**)&callHandle);
    Int32 handlePresentation;
    ((Call*) call)->GetHandlePresentation(&handlePresentation);
    AutoPtr<IUri> handle = handlePresentation == ITelecomManager::PRESENTATION_ALLOWED ?
            callHandle : NULL;
    Int32 callerDisplayNamePresentation;
    ((Call*) call)->GetCallerDisplayNamePresentation(&callerDisplayNamePresentation);
    String callerDisplayName(NULL);
    if (callerDisplayNamePresentation == ITelecomManager::PRESENTATION_ALLOWED) {
        ((Call*) call)->GetCallerDisplayName(&callerDisplayName);
    }
    AutoPtr<IList> conferenceableCalls;
    ((Call*) call)->GetConferenceableCalls((IList**)&conferenceableCalls);
    Int32 conferenceableCallsSize;
    conferenceableCalls->GetSize(&conferenceableCallsSize);
    AutoPtr<IList> conferenceableCallIds;
    CArrayList::New(conferenceableCallsSize, (IList**)&conferenceableCallIds);
    AutoPtr<IIterator> it;
    conferenceableCalls->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICall> otherCall = ICall::Probe(obj);
        String otherId;
        ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(otherCall, &otherId);
        if (otherId != NULL) {
            conferenceableCallIds->Add(StringUtils::ParseCharSequence(otherId));
        }
    }
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    ((Call*) call)->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    Int32 videoState;
    call->GetVideoState(&videoState);
    AutoPtr<IDisconnectCause> disconnectCause;
    ((Call*) call)->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
    AutoPtr<IList> cannedSmsResponses;
    ((Call*) call)->GetCannedSmsResponses((IList**)&cannedSmsResponses);
    Int64 creationTimeMillis;
    ((Call*) call)->GetCreationTimeMillis(&creationTimeMillis);
    AutoPtr<IGatewayInfo> gatewayInfo;
    ((Call*) call)->GetGatewayInfo((IGatewayInfo**)&gatewayInfo);
    AutoPtr<IIVideoProvider> iVideoProvider;
    call->GetVideoProvider((IIVideoProvider**)&iVideoProvider);
    AutoPtr<IStatusHints> statusHints;
    call->GetStatusHints((IStatusHints**)&statusHints);
    AutoPtr<IBundle> bundle;
    ((Call*) call)->GetExtras((IBundle**)&bundle);
    Int32 callSubstate;
    call->GetCallSubstate(&callSubstate);
    assert(0 && "TODO CParcelableCall");
    return NOERROR;
    // return CParcelableCall::New(
    //         callId,
    //         state,
    //         disconnectCause,
    //         cannedSmsResponses,
    //         capabilities,
    //         properties,
    //         creationTimeMillis,
    //         connectTimeMillis,
    //         handle,
    //         handlePresentation,
    //         callerDisplayName,
    //         callerDisplayNamePresentation,
    //         gatewayInfo,
    //         phoneAccountHandle,
    //         includeVideoProvider ? iVideoProvider : NULL,
    //         parentCallId,
    //         childCallIds,
    //         statusHints,
    //         videoState,
    //         conferenceableCallIds,
    //         bundle,
    //         ((Call*) call)->mIsActiveSub,
    //         callSubstate, result);
}

ECode InCallController::AddCall(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    if (callId == NULL) {
        ((CallIdMapper*) mCallIdMapper.Get())->AddCall(call);
        ((Call*) call)->AddListener(mCallListener);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
