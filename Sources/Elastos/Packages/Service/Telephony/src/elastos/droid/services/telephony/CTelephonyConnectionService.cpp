
#include "elastos/droid/services/telephony/CTelephonyConnectionService.h"
#include "elastos/droid/services/telephony/DisconnectCauseUtil.h"
#include "elastos/droid/services/telephony/EmergencyCallHelper.h"
#include "elastos/droid/services/telephony/GsmConnection.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Internal::Telephony::IISub;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::ICallState_WAITING;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

CAR_INTERFACE_IMPL(CTelephonyConnectionService::MyEmergencyCallHelperCallback, Object,
        IEmergencyCallHelperCallback)

ECode CTelephonyConnectionService::MyEmergencyCallHelperCallback::OnComplete(
    /* [in] */ Boolean isRadioReady)
{
    Int32 state;
    Elastos::Droid::Telecomm::Telecom::IConnection::Probe(mConnection)->GetState(&state);
    if (state == Elastos::Droid::Telecomm::Telecom::IConnection::STATE_DISCONNECTED) {
        // If the connection has already been disconnected, do nothing.
    }
    else if (isRadioReady) {
        Elastos::Droid::Telecomm::Telecom::IConnection::Probe(mConnection)->SetInitialized();
        mHost->PlaceOutgoingConnection(mConnection, mPhone, mRequest);
    }
    else {
        Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, failed to turn on radio");
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::POWER_OFF,
                String("Failed to turn on radio."));
        Elastos::Droid::Telecomm::Telecom::IConnection::Probe(mConnection)->SetDisconnected(cause);
        Elastos::Droid::Telecomm::Telecom::IConnection::Probe(mConnection)->Destroy();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTelephonyConnectionService, ConnectionService, ITelephonyConnectionService)

CAR_OBJECT_IMPL(CTelephonyConnectionService)

ECode CTelephonyConnectionService::constructor()
{
    return NOERROR;
}

CTelephonyConnectionService::CTelephonyConnectionService()
{
    mGsmConferenceController = new GsmConferenceController(this);
    mCdmaConferenceController = new CdmaConferenceController(this);
}

ECode CTelephonyConnectionService::OnCreate()
{
    assert(0);
    //ConnectionService::OnCreate();
    //CComponentName::New(this, this.getClass(), (IComponentName**)&mExpectedComponentName);
    mEmergencyTonePlayer = new EmergencyTonePlayer((IContext*)this);
    return NOERROR;
}

ECode CTelephonyConnectionService::OnCreateOutgoingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** result)
{
    Logger::I("CTelephonyConnectionService", "onCreateOutgoingConnection, request: %s", TO_CSTR(request));

    AutoPtr<IUri> handle;
    request->GetAddress((IUri**)&handle);
    if (handle == NULL) {
        Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, handle is null");
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::NO_PHONE_NUMBER_SUPPLIED,
                String("No phone number supplied"));
        return Connection::CreateFailedConnection(cause, result);
    }

    String scheme;
    handle->GetScheme(&scheme);
    String number;
    if (IPhoneAccount::SCHEME_VOICEMAIL.Equals(scheme)) {
        // TODO: We don't check for SecurityException here (requires
        // CALL_PRIVILEGED permission).
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle> _handle;
        request->GetAccountHandle((Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle**)&_handle);
        AutoPtr<IPhone> phone = GetPhoneForAccount(_handle, FALSE);
        if (phone == NULL) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, phone is null");
            AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                    Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE,
                    String("Phone is null"));
            return Connection::CreateFailedConnection(cause, result);
        }
        phone->GetVoiceMailNumber(&number);
        if (TextUtils::IsEmpty(number)) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, no voicemail number set.");
            AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                    Elastos::Droid::Telephony::IDisconnectCause::VOICEMAIL_NUMBER_MISSING,
                    String("Voicemail scheme provided but no voicemail number set."));
            return Connection::CreateFailedConnection(cause, result);
        }

        // Convert voicemail: to tel:
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->FromParts(IPhoneAccount::SCHEME_TEL, number, String(NULL), (IUri**)&handle);
    }
    else {
        if (!IPhoneAccount::SCHEME_TEL.Equals(scheme)) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, Handle %s is not type tel",
                    scheme.string());
            AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                    Elastos::Droid::Telephony::IDisconnectCause::INVALID_NUMBER,
                    String("Handle scheme is not type tel"));
            return Connection::CreateFailedConnection(cause, result);
        }

        handle->GetSchemeSpecificPart(&number);
        if (TextUtils::IsEmpty(number)) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, unable to parse number");
            AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                    Elastos::Droid::Telephony::IDisconnectCause::INVALID_NUMBER,
                    String("Unable to parse number"));
            return Connection::CreateFailedConnection(cause, result);
        }
    }

    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    Boolean isEmergencyNumber;
    helper->IsPotentialEmergencyNumber(number, &isEmergencyNumber);

    // Get the right phone object from the account data passed in.
    AutoPtr<Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle> handle2;
    request->GetAccountHandle((Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle**)&handle2);
    AutoPtr<IPhone> phone = GetPhoneForAccount(handle2, isEmergencyNumber);
    if (phone == NULL) {
        Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, phone is null");
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE, String("Phone is null"));
        return Connection::CreateFailedConnection(cause, result);
    }

    AutoPtr<IServiceState> sstate;
    phone->GetServiceState((IServiceState**)&sstate);
    Int32 state;
    sstate->GetState(&state);
    Boolean useEmergencyCallHelper = FALSE;

    if (isEmergencyNumber) {
        if (state == IServiceState::STATE_POWER_OFF) {
            useEmergencyCallHelper = TRUE;
        }
    }
    else {
        switch (state) {
            case IServiceState::STATE_IN_SERVICE:
            case IServiceState::STATE_EMERGENCY_ONLY:
                break;
            case IServiceState::STATE_OUT_OF_SERVICE:
            {
                AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                        Elastos::Droid::Telephony::IDisconnectCause::OUT_OF_SERVICE,
                        String("ServiceState.STATE_OUT_OF_SERVICE"));
                return Connection::CreateFailedConnection(cause, result);
            }
            case IServiceState::STATE_POWER_OFF:
            {
                AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                        Elastos::Droid::Telephony::IDisconnectCause::POWER_OFF,
                        String("ServiceState.STATE_POWER_OFF"));
                return Connection::CreateFailedConnection(cause, result);
            }
            default:
            {
                Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, unknown service state: %d",
                        state);

                StringBuilder sb;
                sb += "Unknown service state ";
                sb += state;
                AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                        Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE,
                        sb.ToString());
                return Connection::CreateFailedConnection(cause, result);
            }
        }
    }

    AutoPtr<ITelephonyConnection> connection =
            CreateConnectionFor(phone, NULL, TRUE /* isOutgoing */);
    if (connection == NULL) {
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE,
                String("Invalid phone type"));
        return Connection::CreateFailedConnection(cause, result);
    }
    Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection)->SetAddress(handle, IPhoneConstants::PRESENTATION_ALLOWED);
    Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection)->SetInitializing();
    Int32 tmp;
    request->GetVideoState(&tmp);
    Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection)->SetVideoState(tmp);

    if (useEmergencyCallHelper) {
        if (mEmergencyCallHelper == NULL) {
            mEmergencyCallHelper = new EmergencyCallHelper((IContext*)this);
        }
        AutoPtr<IEmergencyCallHelperCallback>  c = new MyEmergencyCallHelperCallback(this, connection, phone, request);
        mEmergencyCallHelper->StartTurnOnRadioSequence(phone, c);
    }
    else {
        PlaceOutgoingConnection(connection, phone, request);
    }

    *result = Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyConnectionService::OnCreateIncomingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** result)
{
    Logger::I("CTelephonyConnectionService", "onCreateIncomingConnection, request: %s", TO_CSTR(request));

    AutoPtr<IPhoneAccountHandle> handle;
    request->GetAccountHandle((IPhoneAccountHandle**)&handle);
    AutoPtr<IPhone> phone = GetPhoneForAccount(handle, FALSE);
    if (phone == NULL) {
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::ERROR_UNSPECIFIED);
        return Connection::CreateFailedConnection(cause, result);
    }

    AutoPtr<ICall> call;
    phone->GetRingingCall((ICall**)&call);

    ICallState state;
    call->GetState(&state);
    assert(0);
    //Boolean res;
    // if (state->IsRinging(&res), !res) {
    //     Logger::I("CTelephonyConnectionService", "onCreateIncomingConnection, no ringing call");
    //     AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
    //             Elastos::Droid::Telephony::IDisconnectCause::INCOMING_MISSED,
    //             String("Found no ringing call"));
    //     return Connection::CreateFailedConnection(cause, result);
    // }

    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> originalConnection;
    if (state == ICallState_WAITING) {
        call->GetLatestConnection((Elastos::Droid::Internal::Telephony::IConnection**)&originalConnection);
    }
    else {
        call->GetEarliestConnection((Elastos::Droid::Internal::Telephony::IConnection**)&originalConnection);
    }

    if (IsOriginalConnectionKnown(originalConnection)) {
        Logger::I("CTelephonyConnectionService",
                "onCreateIncomingConnection, original connection already registered");
        return Connection::CreateCanceledConnection(result);
    }

    AutoPtr<ITelephonyConnection> tconnection =
            CreateConnectionFor(phone, originalConnection, FALSE /* isOutgoing */);
    AutoPtr<Elastos::Droid::Telecomm::Telecom::IConnection> connection = Elastos::Droid::Telecomm::Telecom::IConnection::Probe(tconnection);
    if (connection == NULL) {
        Connection::CreateCanceledConnection((Elastos::Droid::Telecomm::Telecom::IConnection**)&connection);
    }
    *result = connection;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyConnectionService::OnCreateUnknownConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** result)
{
    Logger::I("CTelephonyConnectionService", "onCreateUnknownConnection, request: %s", TO_CSTR(request));

    AutoPtr<IPhoneAccountHandle> handle;
    request->GetAccountHandle((IPhoneAccountHandle**)&handle);
    AutoPtr<IPhone> phone = GetPhoneForAccount(handle, FALSE);
    if (phone == NULL) {
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::ERROR_UNSPECIFIED);
        return Connection::CreateFailedConnection(cause, result);
    }

    AutoPtr<IList> allConnections;
    CArrayList::New((IList**)&allConnections);
    AutoPtr<ICall> ringingCall;
    phone->GetRingingCall((ICall**)&ringingCall);

    Boolean res;
    if (ringingCall->HasConnections(&res), res) {
        AutoPtr<IList> list;
        ringingCall->GetConnections((IList**)&list);
        allConnections->AddAll(ICollection::Probe(list));
    }

    AutoPtr<ICall> foregroundCall;
    phone->GetForegroundCall((ICall**)&foregroundCall);
    if (foregroundCall->HasConnections(&res), res) {
        AutoPtr<IList> list;
        foregroundCall->GetConnections((IList**)&list);
        allConnections->AddAll(ICollection::Probe(list));
    }

    AutoPtr<ICall> backgroundCall;
    phone->GetBackgroundCall((ICall**)&backgroundCall);
    if (backgroundCall->HasConnections(&res), res) {
        AutoPtr<IList> list;
        backgroundCall->GetConnections((IList**)&list);
        allConnections->AddAll(ICollection::Probe(list));
    }

    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> unknownConnection;
    Int32 size;
    allConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        allConnections->Get(i, (IInterface**)&obj);
        AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> telephonyConnection =
                Elastos::Droid::Internal::Telephony::IConnection::Probe(obj);

        if (!IsOriginalConnectionKnown(telephonyConnection)) {
            unknownConnection = telephonyConnection;
            break;
        }
    }

    if (unknownConnection == NULL) {
        Logger::I("CTelephonyConnectionService",
                "onCreateUnknownConnection, did not find previously unknown connection.");
        return Connection::CreateCanceledConnection(result);
    }

    AutoPtr<ITelephonyConnection> connection = CreateConnectionFor(phone, unknownConnection,
            (unknownConnection->IsIncoming(&res), !res) /* isOutgoing */);

    if (connection == NULL) {
        return Connection::CreateCanceledConnection(result);
    }
    else {
        connection->UpdateState();
        *result = Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
}

ECode CTelephonyConnectionService::OnConference(
    /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection1,
    /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection2)
{
    if (ITelephonyConnection::Probe(connection1) != NULL &&
            ITelephonyConnection::Probe(connection2) != NULL) {
        ITelephonyConnection::Probe(connection1)->PerformConference(
            ITelephonyConnection::Probe(connection2));
    }
    return NOERROR;
}

void CTelephonyConnectionService::PlaceOutgoingConnection(
    /* [in] */ ITelephonyConnection* connection,
    /* [in] */ IPhone* phone,
    /* [in] */ IConnectionRequest* request)
{
    AutoPtr<IUri> uri;
    Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection)->GetAddress((IUri**)&uri);
    String number;
    uri->GetSchemeSpecificPart(&number);

    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> originalConnection;
    //try {
    Int32 state;
    request->GetVideoState(&state);
    ECode ec = phone->Dial(number, state, (Elastos::Droid::Internal::Telephony::IConnection**)&originalConnection);
    //} catch (CallStateException e) {
    if (ec == (ECode)E_TELEPHONEY_CALL_STATE_EXCEPTION) {
        Logger::E("CTelephonyConnectionService", "placeOutgoingConnection, phone.dial exception: %d",ec);
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE /*e.getMessage()*/);
        Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection)->SetDisconnected(cause);
        return;
    }
    //}

    if (originalConnection == NULL) {
        Int32 telephonyDisconnectCause = Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE;
        // On GSM phones, null connection means that we dialed an MMI code
        Int32 type;
        phone->GetPhoneType(&type);
        if (type == IPhoneConstants::PHONE_TYPE_GSM) {
            Logger::D("CTelephonyConnectionService", "dialed MMI code");
            telephonyDisconnectCause = Elastos::Droid::Telephony::IDisconnectCause::DIALED_MMI;
            AutoPtr<IIntent> intent;
            assert(0);
            //CIntent::New(this, MMIDialogActivity.class, (IIntent**)&intent);
            intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                    IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
            assert(0);
            //StartActivity(intent);
        }
        Logger::D("CTelephonyConnectionService", "placeOutgoingConnection, phone.dial returned null");
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                telephonyDisconnectCause, String("Connection is null"));
        Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection)->SetDisconnected(cause);
    }
    else {
        connection->SetOriginalConnection(originalConnection);
    }
}

AutoPtr<ITelephonyConnection> CTelephonyConnectionService::CreateConnectionFor(
    /* [in] */ IPhone* phone,
    /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* originalConnection,
    /* [in] */ Boolean isOutgoing)
{
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == ITelephonyManager::PHONE_TYPE_GSM) {
        AutoPtr<GsmConnection> connection = new GsmConnection(originalConnection);
        mGsmConferenceController->Add(connection);
        return ITelephonyConnection::Probe(connection);
    }
    else if (phoneType == ITelephonyManager::PHONE_TYPE_CDMA) {
        Boolean allowMute = AllowMute(phone);
        AutoPtr<CdmaConnection> connection = new CdmaConnection(
                originalConnection, mEmergencyTonePlayer, allowMute, isOutgoing);
        mCdmaConferenceController->Add(connection);
        return ITelephonyConnection::Probe(connection);
    } else {
        return NULL;
    }
}

Boolean CTelephonyConnectionService::IsOriginalConnectionKnown(
    /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* originalConnection)
{
    AutoPtr<ICollection> con;
    GetAllConnections((ICollection**)&con);
    AutoPtr<ArrayOf<IInterface*> > array;
    con->ToArray((ArrayOf<IInterface*>**)&array);

    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IConnection> connection = IConnection::Probe((*array)[i]);

        AutoPtr<ITelephonyConnection> telephonyConnection = ITelephonyConnection::Probe(connection);
        if (ITelephonyConnection::Probe(connection) != NULL) {
            AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> tmp;
            telephonyConnection->GetOriginalConnection((Elastos::Droid::Internal::Telephony::IConnection**)&tmp);
            if (TO_IINTERFACE(tmp) == TO_IINTERFACE(originalConnection)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

AutoPtr<IPhone> CTelephonyConnectionService::GetPhoneForAccount(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Boolean isEmergency)
{
    if (isEmergency) {
        AutoPtr<IPhoneFactory> helper;
        CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
        AutoPtr<IPhone> phone;
        helper->GetDefaultPhone((IPhone**)&phone);
        return phone;
    }

    AutoPtr<IComponentName> name;
    accountHandle->GetComponentName((IComponentName**)&name);
    Boolean res;
    if (IObject::Probe(mExpectedComponentName)->Equals(name, &res), res) {
        String id;
        accountHandle->GetId(&id);
        if (!id.IsNull()) {
            ECode ec = NOERROR;
            //try
            {
                AutoPtr<ISubscriptionController> controller;
                assert(0);
                //SubscriptionController::GetInstance((ISubscriptionController**)&controller);
                Int32 phoneId;
                FAIL_GOTO(ec = IISub::Probe(controller)->GetPhoneId(StringUtils::ParseInt64(id), &phoneId), ERROR)
                AutoPtr<IPhoneFactory> helper;
                CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
                AutoPtr<IPhone> phone;
                helper->GetPhone(phoneId, (IPhone**)&phone);
                return phone;
            }
            // catch (NumberFormatException e) {
        ERROR:
            if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
                Logger::W("CTelephonyConnectionService", "Could not get subId from account: %s", id.string());
            }
            //}
        }
    }
    return NULL;
}

Boolean CTelephonyConnectionService::AllowMute(
    /* [in] */ IPhone* phone)
{
    // For CDMA phones, check if we are in Emergency Callback Mode (ECM).  Mute is disallowed
    // in ECM mode.
    Int32 type;
    phone->GetPhoneType(&type);
    if (type == ITelephonyManager::PHONE_TYPE_CDMA) {
        AutoPtr<IPhoneProxy> phoneProxy = IPhoneProxy::Probe(phone);
        AutoPtr<IPhone> phone;
        phoneProxy->GetActivePhone((IPhone**)&phone);
        AutoPtr<ICDMAPhone> cdmaPhone = ICDMAPhone::Probe(phone);
        if (cdmaPhone != NULL) {
            Boolean res;
            IPhoneBase::Probe(cdmaPhone)->IsInEcm(&res);
            if (res) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos