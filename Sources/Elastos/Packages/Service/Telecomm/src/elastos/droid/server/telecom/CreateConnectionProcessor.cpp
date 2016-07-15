
#include "elastos/droid/server/telecom/CreateConnectionProcessor.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/ConnectionServiceWrapper.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/TelephonyUtil.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <elastos/droid/os/Process.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Telecomm::Telecom::CDisconnectCause;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// CreateConnectionProcessor::CallAttemptRecord
//=============================================================================
CAR_INTERFACE_IMPL(CreateConnectionProcessor::CallAttemptRecord, Object, ICreateConnectionProcessorCallAttemptRecord)

ECode CreateConnectionProcessor::CallAttemptRecord::constructor(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IPhoneAccountHandle* targetPhoneAccount)
{
    mConnectionManagerPhoneAccount = connectionManagerPhoneAccount;
    mTargetPhoneAccount = targetPhoneAccount;
    return NOERROR;
}

ECode CreateConnectionProcessor::CallAttemptRecord::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = String("CallAttemptRecord(")
            + Objects::ToString(mConnectionManagerPhoneAccount) + ","
            + Objects::ToString(mTargetPhoneAccount) + ")";
    return NOERROR;
}

ECode CreateConnectionProcessor::CallAttemptRecord::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (ICreateConnectionProcessorCallAttemptRecord::Probe(obj) != NULL) {
        AutoPtr<CallAttemptRecord> other = (CallAttemptRecord*) ICreateConnectionProcessorCallAttemptRecord::Probe(obj);
        return Objects::Equals(mConnectionManagerPhoneAccount,
                other->mConnectionManagerPhoneAccount) &&
                Objects::Equals(mTargetPhoneAccount, other->mTargetPhoneAccount);
    }
    *result = FALSE;
    return NOERROR;
}

//=============================================================================
// CreateConnectionProcessor::Response
//=============================================================================
CAR_INTERFACE_IMPL(CreateConnectionProcessor::Response, Object, ICreateConnectionResponse)

CreateConnectionProcessor::Response::Response(
    /* [in] */ CreateConnectionProcessor* host)
    : mHost(host)
{}

ECode CreateConnectionProcessor::Response::constructor(
    /* [in] */ IConnectionServiceWrapper* service)
{
    mService = service;
    return NOERROR;
}

ECode CreateConnectionProcessor::Response::HandleCreateConnectionSuccess(
    /* [in] */ ICallIdMapper* idMapper,
    /* [in] */ IParcelableConnection* connection)
{
    if (mHost->mResponse == NULL) {
        // Nobody is listening for this connection attempt any longer; ask the responsible
        // ConnectionService to tear down any resources associated with the call
        ((ConnectionServiceWrapper*) mService.Get())->Abort(mHost->mCall);
    } else {
        // Success -- share the good news and remember that we are no longer interested
        // in hearing about any more attempts
        //NOTE, because in HandleCreateConnectionSuccess will destroy mHost,
        //so here use a variable to hold it;
        AutoPtr<CreateConnectionProcessor> tmp = mHost;
        mHost->mResponse->HandleCreateConnectionSuccess(idMapper, connection);
        mHost->mResponse = NULL;
    }
    return NOERROR;
}

ECode CreateConnectionProcessor::Response::HandleCreateConnectionFailure(
    /* [in] */ IDisconnectCause* errorDisconnectCause)
{
    // Failure of some sort; record the reasons for failure and try again if possible
    Log::D("CreateConnectionProcessor", "Connection failed: (%s)", TO_CSTR(errorDisconnectCause));
    mHost->mLastErrorDisconnectCause = errorDisconnectCause;
    mHost->AttemptNextPhoneAccount();
    return NOERROR;
}

//=============================================================================
// CreateConnectionProcessor
//=============================================================================
ECode CreateConnectionProcessor::constructor(
    /* [in] */ ICall* call,
    /* [in] */ ConnectionServiceRepository* repository,
    /* [in] */ ICreateConnectionResponse* response,
    /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar,
    /* [in] */ IContext* context)
{
    mCall = call;
    mRepository = repository;
    mResponse = response;
    mPhoneAccountRegistrar = phoneAccountRegistrar;
    mContext = context;
    return NOERROR;
}

ECode CreateConnectionProcessor::Process()
{
    Log::V("CreateConnectionProcessor", "process");
    CArrayList::New((IList**)&mAttemptRecords);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    ((Call*) mCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
    if (phoneAccountHandle != NULL) {
        AutoPtr<CallAttemptRecord> record = new CallAttemptRecord();
        record->constructor(
                phoneAccountHandle, phoneAccountHandle);
        mAttemptRecords->Add(TO_IINTERFACE(record));
    }
    AdjustAttemptsForConnectionManager();
    AdjustAttemptsForEmergency();
    mAttemptRecords->GetIterator((IIterator**)&mAttemptRecordIterator);
    AttemptNextPhoneAccount();
    return NOERROR;
}

ECode CreateConnectionProcessor::Abort()
{
    Log::V("CreateConnectionProcessor", "abort");
    // Clear the response first to prevent attemptNextConnectionService from attempting any
    // more services.
    AutoPtr<ICreateConnectionResponse> response = mResponse;
    mResponse = NULL;
    AutoPtr<IConnectionServiceWrapper> service;
    ((Call*) mCall.Get())->GetConnectionService((IConnectionServiceWrapper**)&service);
    if (service != NULL) {
        ((ConnectionServiceWrapper*) service.Get())->Abort(mCall);
        ((Call*) mCall.Get())->ClearConnectionService();
    }
    if (response != NULL) {
        AutoPtr<IDisconnectCause> disconnectCause;
        CDisconnectCause::New(IDisconnectCause::LOCAL, (IDisconnectCause**)&disconnectCause);
        response->HandleCreateConnectionFailure(disconnectCause);
    }
    return NOERROR;
}

ECode CreateConnectionProcessor::AttemptNextPhoneAccount()
{
    Log::V("CreateConnectionProcessor", "attemptNextPhoneAccount");
    AutoPtr<CallAttemptRecord> attempt;
    Boolean hasNext;
    mAttemptRecordIterator->HasNext(&hasNext);
    if (hasNext) {
        AutoPtr<IInterface> obj;
        mAttemptRecordIterator->GetNext((IInterface**)&obj);
        attempt = (CallAttemptRecord*) IObject::Probe(obj);
        Boolean hasManagerPermission;
        mPhoneAccountRegistrar->PhoneAccountHasPermission(
                attempt->mConnectionManagerPhoneAccount, &hasManagerPermission);
        if (!hasManagerPermission) {
            Log::W("CreateConnectionProcessor",
                    "Connection mgr does not have BIND_CONNECTION_SERVICE for attempt: %s",
                    TO_CSTR(attempt));
            AttemptNextPhoneAccount();
            return NOERROR;
        }
        // If the target PhoneAccount differs from the ConnectionManager phone acount, ensure it
        // also has BIND_CONNECTION_SERVICE permission.
        Boolean hasPermission;
        mPhoneAccountRegistrar->PhoneAccountHasPermission(attempt->mTargetPhoneAccount, &hasPermission);
        Boolean isEquals;
        IObject::Probe(attempt->mConnectionManagerPhoneAccount)->Equals(attempt->mTargetPhoneAccount, &isEquals);
        if (!isEquals && !hasPermission) {
            Log::W("CreateConnectionProcessor",
                    "Target PhoneAccount does not have BIND_CONNECTION_SERVICE for attempt: %s",
                    TO_CSTR(attempt));
            AttemptNextPhoneAccount();
            return NOERROR;
        }
    }
    if (mResponse != NULL && attempt != NULL) {
        String attemptStr;
        (attempt)->ToString(&attemptStr);
        Log::I("CreateConnectionProcessor", "Trying attempt %s", attemptStr.string());
        AutoPtr<IComponentName> componentName;
        attempt->mConnectionManagerPhoneAccount->GetComponentName((IComponentName**)&componentName);
        AutoPtr<IConnectionServiceWrapper> service;
        mRepository->GetService(componentName, (IConnectionServiceWrapper**)&service);
        if (service == NULL) {
            Log::I("CreateConnectionProcessor", "Found no connection service for attempt %s", TO_CSTR(attempt));
            AttemptNextPhoneAccount();
        } else {
            ((Call*) mCall.Get())->SetConnectionManagerPhoneAccount(attempt->mConnectionManagerPhoneAccount);
            ((Call*) mCall.Get())->SetTargetPhoneAccount(attempt->mTargetPhoneAccount);
            ((Call*) mCall.Get())->SetConnectionService(service);
            AutoPtr<IComponentName> componentName;
            ((ConnectionServiceWrapper*) service.Get())->GetComponentName((IComponentName**)&componentName);
            Log::I("CreateConnectionProcessor", "Attempting to call from %s", TO_CSTR(componentName));
            AutoPtr<Response> response = new Response(this);
            response->constructor(service);
            ((ConnectionServiceWrapper*) service.Get())->CreateConnection(mCall, response);
        }
    } else {
        Log::V("CreateConnectionProcessor", "attemptNextPhoneAccount, no more accounts, failing");
        if (mResponse != NULL) {
            AutoPtr<IDisconnectCause> disconnectCause;
            CDisconnectCause::New(IDisconnectCause::ERROR, (IDisconnectCause**)&disconnectCause);
            //NOTE: tmp here only keep this not destroyed in the called function
            AutoPtr<CreateConnectionProcessor> tmp = this;
            mResponse->HandleCreateConnectionFailure(mLastErrorDisconnectCause != NULL ?
                    mLastErrorDisconnectCause : disconnectCause);
            mResponse = NULL;
            ((Call*) mCall.Get())->ClearConnectionService();
        }
    }
    return NOERROR;
}

ECode CreateConnectionProcessor::ShouldSetConnectionManager(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 mAttemptRecordsSize;
    mAttemptRecords->GetSize(&mAttemptRecordsSize);
    if (mAttemptRecordsSize == 0) {
        *result = FALSE;
        return NOERROR;
    }
    if (mAttemptRecordsSize > 1) {
        Log::D("CreateConnectionProcessor", "shouldSetConnectionManager, error, mAttemptRecords should not have more than 1 record");
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IPhoneAccountHandle> connectionManager;
    mPhoneAccountRegistrar->GetSimCallManager((IPhoneAccountHandle**)&connectionManager);
    if (connectionManager == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mAttemptRecords->Get(0, (IInterface**)&obj);
    AutoPtr<IPhoneAccountHandle> targetPhoneAccountHandle = ((CallAttemptRecord*) IObject::Probe(obj))->mTargetPhoneAccount;
    if (Objects::Equals(connectionManager, targetPhoneAccountHandle)) {
        *result = FALSE;
        return NOERROR;
    }
    // Connection managers are only allowed to manage SIM subscriptions.
    AutoPtr<IPhoneAccount> targetPhoneAccount;
    mPhoneAccountRegistrar->GetPhoneAccount(
            targetPhoneAccountHandle, (IPhoneAccount**)&targetPhoneAccount);
    Int32 capabilities;
    targetPhoneAccount->GetCapabilities(&capabilities);
    Boolean isSimSubscription = (capabilities & IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION) != 0;
    if (!isSimSubscription) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CreateConnectionProcessor::AdjustAttemptsForConnectionManager()
{
    Boolean shouldSetConnectionManager;
    ShouldSetConnectionManager(&shouldSetConnectionManager);
    if (shouldSetConnectionManager) {
        AutoPtr<IInterface> obj;
        mAttemptRecords->Get(0, (IInterface**)&obj);
        AutoPtr<CallAttemptRecord> callAttemptRecord = (CallAttemptRecord*) IObject::Probe(obj);
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        mPhoneAccountRegistrar->GetSimCallManager((IPhoneAccountHandle**)&phoneAccountHandle);
        AutoPtr<CallAttemptRecord> record = new CallAttemptRecord();
        record->constructor(phoneAccountHandle, callAttemptRecord->mTargetPhoneAccount);
        Log::V("CreateConnectionProcessor", "setConnectionManager, changing %s -> %s",
                TO_CSTR(callAttemptRecord->mTargetPhoneAccount), TO_CSTR(record));
        mAttemptRecords->Set(0, TO_IINTERFACE(record));
    } else {
        Log::V("CreateConnectionProcessor", "setConnectionManager, not changing");
    }
    return NOERROR;
}

ECode CreateConnectionProcessor::AdjustAttemptsForEmergency()
{
    AutoPtr<IUri> mCallHandle;
    ((Call*) mCall.Get())->GetHandle((IUri**)&mCallHandle);
    Boolean shouldProcessAsEmergency;
    TelephonyUtil::ShouldProcessAsEmergency(mContext, mCallHandle, &shouldProcessAsEmergency);
    if (shouldProcessAsEmergency) {
        Log::I("CreateConnectionProcessor", "Emergency number detected");
        mAttemptRecords->Clear();
        AutoPtr<IList> allAccounts;
        mPhoneAccountRegistrar->GetAllPhoneAccounts((IList**)&allAccounts);
        Boolean isEmpty;
        allAccounts->IsEmpty(&isEmpty);
        if (isEmpty) {
            // If the list of phone accounts is empty at this point, it means Telephony hasn't
            // registered any phone accounts yet. Add a fallback emergency phone account so
            // that emergency calls can still go through. We create a new ArrayLists here just
            // in case the implementation of PhoneAccountRegistrar ever returns an unmodifiable
            // list.
            Log::V("CreateConnectionProcessor", "telephony hasn't registered any phone accounts yet");
            CArrayList::New((IList**)&allAccounts);
            AutoPtr<IPhoneAccount> phoneAccount;
            TelephonyUtil::GetDefaultEmergencyPhoneAccount((IPhoneAccount**)&phoneAccount);
            allAccounts->Add(phoneAccount);
        }
        // First, add SIM phone accounts which can place emergency calls.
        AutoPtr<IIterator> it;
        allAccounts->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IPhoneAccount> phoneAccount = IPhoneAccount::Probe(obj);
            Boolean hasCapabilities;
            phoneAccount->HasCapabilities(IPhoneAccount::CAPABILITY_PLACE_EMERGENCY_CALLS, &hasCapabilities);
            Boolean hasCapabilitiesSub;
            phoneAccount->HasCapabilities(IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION, &hasCapabilitiesSub);
            if (hasCapabilities && hasCapabilitiesSub) {
                AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
                phoneAccount->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
                Log::I("CreateConnectionProcessor", "Will try PSTN account %s for emergency",
                        TO_CSTR(phoneAccountHandle));
                AutoPtr<CallAttemptRecord> record = new CallAttemptRecord();
                record->constructor(phoneAccountHandle, phoneAccountHandle);
                mAttemptRecords->Add(TO_IINTERFACE(record));
            }
        }
        // Next, add the connection manager account as a backup if it can place emergency calls.
        AutoPtr<IPhoneAccountHandle> callManagerHandle;
        mPhoneAccountRegistrar->GetSimCallManager((IPhoneAccountHandle**)&callManagerHandle);
        if (callManagerHandle != NULL) {
            AutoPtr<IPhoneAccount> callManager;
            mPhoneAccountRegistrar->GetPhoneAccount(callManagerHandle, (IPhoneAccount**)&callManager);
            Boolean hasCapabilities;
            callManager->HasCapabilities(IPhoneAccount::CAPABILITY_PLACE_EMERGENCY_CALLS, &hasCapabilities);
            if (hasCapabilities) {
                AutoPtr<IUri> mCallHandle;
                ((Call*) mCall.Get())->GetHandle((IUri**)&mCallHandle);
                String scheme;
                mCallHandle->GetScheme(&scheme);
                AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
                mPhoneAccountRegistrar->GetDefaultOutgoingPhoneAccount(scheme, (IPhoneAccountHandle**)&phoneAccountHandle);
                AutoPtr<CallAttemptRecord> callAttemptRecord = new CallAttemptRecord();
                callAttemptRecord->constructor(callManagerHandle, phoneAccountHandle);
                Boolean isContains;
                mAttemptRecords->Contains(TO_IINTERFACE(callAttemptRecord), &isContains);
                if (!isContains) {
                    Log::I("CreateConnectionProcessor", "Will try Connection Manager account %s for emergency",
                            TO_CSTR(callManager));
                    mAttemptRecords->Add(TO_IINTERFACE(callAttemptRecord));
                }
            }
        }
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
