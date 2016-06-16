
#include "elastos/droid/services/telephony/CTelephonyConnectionService.h"

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
    connection->GetState(&state);
    if (state == IConnection::STATE_DISCONNECTED) {
        // If the connection has already been disconnected, do nothing.
    }
    else if (isRadioReady) {
        connection->SetInitialized();
        PlaceOutgoingConnection(connection, phone, request);
    }
    else {
        Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, failed to turn on radio");
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.POWER_OFF,
                String("Failed to turn on radio."));
        connection->SetDisconnected(cause);
        connection->Destroy();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTelephonyConnectionService, ConnectionService, ITelephonyConnectionService)

CTelephonyConnectionService::CTelephonyConnectionService()
{
    mGsmConferenceController = new GsmConferenceController(this);
    mCdmaConferenceController = new CdmaConferenceController(this);
}

ECode CTelephonyConnectionService::OnCreate()
{
    ConnectionService::OnCreate();
    CComponentName::New(this, this.getClass(), (IComponentName**)&mExpectedComponentName);
    mEmergencyTonePlayer = new EmergencyTonePlayer(this);
    return NOERROR;
}

ECode CTelephonyConnectionService::OnCreateOutgoingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IConnection** result)
{
    Logger::I("CTelephonyConnectionService", "onCreateOutgoingConnection, request: %s", TO_CSTR(request));

    AutoPtr<IUri> handle;
    request->GetAddress((IUri**)&handle);
    if (handle == NULL) {
        Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, handle is null");
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.NO_PHONE_NUMBER_SUPPLIED,
                Stringf("No phone number supplied"));
        return Connection::CreateFailedConnection(cause, result);
    }

    String scheme;
    handle->GetScheme(&scheme);
    String number;
    if (IPhoneAccount::SCHEME_VOICEMAIL.Equals(scheme)) {
        // TODO: We don't check for SecurityException here (requires
        // CALL_PRIVILEGED permission).
        utoPtr<IPhoneAccountHandle> handle;
        request->GetAccountHandle((IPhoneAccountHandle**)&handle);
        AutoPtr<IPhone> phone = GetPhoneForAccount(handle, FALSE);
        if (phone == NULL) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, phone is null");
            AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil.toTelecomDisconnectCause(
                    android.telephony.DisconnectCause.OUTGOING_FAILURE,
                    Srting("Phone is null"));
            return Connection::CreateFailedConnection(cause, result);
        }
        phone->GetVoiceMailNumber(&number);
        if (TextUtils::IsEmpty(number)) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, no voicemail number set.");
            AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                    android.telephony.DisconnectCause.VOICEMAIL_NUMBER_MISSING,
                    Srting("Voicemail scheme provided but no voicemail number set."))
            return Connection::CreateFailedConnection(cause, result);
        }

        // Convert voicemail: to tel:
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->FromParts(IPhoneAccount::SCHEME_TEL, number, NULL, (IPhoneAccountHandle**)&handle);
    }
    else {
        if (!IPhoneAccount::SCHEME_TEL.Equals(scheme)) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, Handle %s is not type tel",
                    scheme.string());
            AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                    android.telephony.DisconnectCause.INVALID_NUMBER,
                    String("Handle scheme is not type tel"));
            return Connection::CreateFailedConnection(cause, result);
        }

        handle->GetSchemeSpecificPart(&number);
        if (TextUtils::IsEmpty(number)) {
            Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, unable to parse number");
            AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                    android.telephony.DisconnectCause.INVALID_NUMBER,
                    String("Unable to parse number"));
            return Connection::CreateFailedConnection(cause, result);
        }
    }

    Boolean isEmergencyNumber = PhoneNumberUtils::IsPotentialEmergencyNumber(number);

    // Get the right phone object from the account data passed in.
    AutoPtr<IPhoneAccountHandle> handle;
    request->GetAccountHandle((IPhoneAccountHandle**)&handle);
    AutoPtr<IPhone> phone = GetPhoneForAccount(handle, isEmergencyNumber);
    if (phone == NULL) {
        Logger::D("CTelephonyConnectionService", "onCreateOutgoingConnection, phone is null");
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.OUTGOING_FAILURE, String("Phone is null"));
        return Connection::CreateFailedConnection(cause, result);
    }

    AutoPtr<IServiceState> sstate;
    phone->GetServiceState((IServiceState**)&sstate);
    Int32 state;
    sstate->GetState(&state);
    Boolean useEmergencyCallHelper = FALSE;

    if (isEmergencyNumber) {
        if (state == iServiceState::STATE_POWER_OFF) {
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
                AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                        android.telephony.DisconnectCause.OUT_OF_SERVICE,
                        String("ServiceState.STATE_OUT_OF_SERVICE"));
                return Connection::CreateFailedConnection(cause, result);
            }
            case IServiceState::STATE_POWER_OFF:
            {
                AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                        android.telephony.DisconnectCause.POWER_OFF,
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
                AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                        android.telephony.DisconnectCause.OUTGOING_FAILURE,
                        sb.ToString());
                return Connection::CreateFailedConnection(cause, result);
            }
        }
    }

    AutoPtr<ITelephonyConnection> connection =
            CreateConnectionFor(phone, NULL, TRUE /* isOutgoing */);
    if (connection == NULL) {
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.OUTGOING_FAILURE,
                String("Invalid phone type"));
        return Connection::CreateFailedConnection(cause, result);
    }
    connection->SetAddress(handle, IPhoneConstants::PRESENTATION_ALLOWED);
    connection->SetInitializing();
    Int32 tmp;
    request->GetVideoState(&tmp);
    connection->SetVideoState(tmp);

    if (useEmergencyCallHelper) {
        if (mEmergencyCallHelper == NULL) {
            mEmergencyCallHelper = new EmergencyCallHelper(this);
        }
        AutoPtr<IEmergencyCallHelperCallback>  c = new MyEmergencyCallHelperCallback(this);
        mEmergencyCallHelper->StartTurnOnRadioSequence(phone, c);
    }
    else {
        PlaceOutgoingConnection(connection, phone, request);
    }

    *result = connection;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyConnectionService::OnCreateIncomingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IConnection** result)
{
    Logger::I("CTelephonyConnectionService", "onCreateIncomingConnection, request: %s", TO_CSTR(request));

    AutoPtr<IPhoneAccountHandle> handle;
    request->GetAccountHandle((IPhoneAccountHandle**)&handle);
    AutoPtr<IPhone> phone = GetPhoneForAccount(handle, FALSE);
    if (phone == NULL) {
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.ERROR_UNSPECIFIED);
        return Connection::CreateFailedConnection(cause, result);
    }

    AutoPtr<ICall> call;
    phone->GetRingingCall((ICall**)&call);

    AutoPtr<ICallState> state;
    call->GetState((ICallState**)&state);
    Boolean res;
    if (state->IsRinging(&res), !res) {
        Logger::I("CTelephonyConnectionService", "onCreateIncomingConnection, no ringing call");
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.INCOMING_MISSED,
                String("Found no ringing call"));
        return Connection::CreateFailedConnection(cause, result);
    }

    AutoPtr<com.android.internal.telephony.Connection> originalConnection;
    if (state == ICallState_WAITING) {
        originalConnection = call->GetLatestConnection()
    }
    else {
        originalConnection = call->GetEarliestConnection();
    }

    if (IsOriginalConnectionKnown(originalConnection, &res), res) {
        Logger::I("CTelephonyConnectionService",
                "onCreateIncomingConnection, original connection already registered");
        return Connection::CreateCanceledConnection(result);
    }

    AutoPtr<IConnection> connection =
            CreateConnectionFor(phone, originalConnection, FALSE /* isOutgoing */);
    if (connection == NULL) {
        Connection::CreateCanceledConnection((IConnection**)&connection);
    }
    *result = connection;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTelephonyConnectionService::OnCreateUnknownConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IConnection** result)
{
    Logger::I("CTelephonyConnectionService", "onCreateUnknownConnection, request: %s", TO_CSTR(request));

    AutoPtr<IPhoneAccountHandle> handle;
    request->GetAccountHandle((IPhoneAccountHandle**)&handle);
    AutoPtr<IPhone> phone = GetPhoneForAccount(handle, FALSE);
    if (phone == NULL) {
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.ERROR_UNSPECIFIED)
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
        allConnections->AddAll(list);
    }

    AutoPtr<ICall> foregroundCall;
    phone->GetForegroundCall((ICall**)&foregroundCall);
    if (foregroundCall->HasConnections(&res), res) {
        AutoPtr<IList> list;
        foregroundCall->GetConnections((IList**)&list);
        allConnections->AddAll(list);
    }

    AutoPtr<ICall> backgroundCall;
    phone->GetBackgroundCall((ICall**)&backgroundCall);
    if (backgroundCall->HasConnections(&res), res) {
        AutoPtr<IList> list;
        backgroundCall->GetConnections((IList**)&list);
        allConnections->AddAll(list);
    }

    AutoPtr<com.android.internal.telephony.Connection> unknownConnection;
    Int32 size;
    allConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        allConnections->Get(i, (IInterface**)&obj);
        AutoPtr<com.android.internal.telephony.Connection telephonyConnection> telephonyConnection = ::Probe(obj);

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
        *result = connection;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
}

ECode CTelephonyConnectionService::OnConference(
    /* [in] */ IConnection* connection1,
    /* [in] */ IConnection* connection2)
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
    connection->GetAddress(&uri);
    String number;
    uri->GetSchemeSpecificPart(&number);

    com.android.internal.telephony.Connection originalConnection;
    //try {
    Int32 state;
    request->GetVideoState(&state);
    ECode ec = phone->Dial(number, state, ()&originalConnection);
    //} catch (CallStateException e) {
    if (ec == (ECode)CallStateException) {
        Logger::E("CTelephonyConnectionService", "placeOutgoingConnection, phone.dial exception: %d",ec);
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                android.telephony.DisconnectCause.OUTGOING_FAILURE /*e.getMessage()*/);
        connection->SetDisconnected(cause);
        return;
    }
    //}

    if (originalConnection == NULL) {
        Int32 telephonyDisconnectCause = android.telephony.DisconnectCause.OUTGOING_FAILURE;
        // On GSM phones, null connection means that we dialed an MMI code
        Int32 type;
        phone->GetPhoneType(&type);
        if (type == IPhoneConstants::PHONE_TYPE_GSM) {
            Logger::D("CTelephonyConnectionService", "dialed MMI code");
            telephonyDisconnectCause = android.telephony.DisconnectCause.DIALED_MMI;
            AutoPtr<IIntent> intent;
            CIntent::New(this, MMIDialogActivity.class, (IIntent**)&intent);
            intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                    IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
            StartActivity(intent);
        }
        Logger::D("CTelephonyConnectionService", "placeOutgoingConnection, phone.dial returned null");
        AutoPtr<IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
                telephonyDisconnectCause, String("Connection is null"));
        connection->SetDisconnected(cause);
    }
    else {
        connection->SetOriginalConnection(originalConnection);
    }
}

AutoPtr<ITelephonyConnection> CTelephonyConnectionService::CreateConnectionFor(
    /* [in] */ IPhone* phone,
    /* [in] */ com.android.internal.telephony.Connection originalConnection,
    /* [in] */ Boolean isOutgoing)
{
    Int32 phoneType;
    phone->GetPhoneType(&phoneType);
    if (phoneType == ITelephonyManager::PHONE_TYPE_GSM) {
        AutoPtr<IGsmConnection> connection = new GsmConnection(originalConnection);
        mGsmConferenceController->Add(TO_IINTERFACE(connection));
        return ITelephonyConnection::Probe(connection);
    }
    else if (phoneType == ITelephonyManager::PHONE_TYPE_CDMA) {
        Boolean allowMute = AllowMute(phone);
        AutoPtr<ICdmaConnection> connection = new CdmaConnection(
                originalConnection, mEmergencyTonePlayer, allowMute, isOutgoing);
        mCdmaConferenceController->Add(TO_IINTERFACE(connection));
        return ITelephonyConnection::Probe(connection);
    } else {
        return NULL;
    }
}

Boolean CTelephonyConnectionService::IsOriginalConnectionKnown(
    /* [in] */ com.android.internal.telephony.Connection originalConnection)
{
    AutoPtr<ICollection> con;
    GetAllConnections((ICollection**)&con);
    AutoPtr<ArrayOf<IInterface*> > array;
    con->ToArray((ArrayOf<IInterface*>**)&array);

    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IConnection> connection = IConnection::Probe((*array)[i]);

        AutoPtr<ITelephonyConnection> telephonyConnection = ITelephonyConnection::Probe(connection);
        if (ITelephonyConnection::Probe(connection) != NULL) {

            if (telephonyConnection.getOriginalConnection() == originalConnection) {
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
        AutoPtr<IPhone> phone;
        PhoneFactory::GetDefaultPhone((IPhone**)&phone);
        return phone
    }

    AutoPtr<IComponentName> name;
    accountHandle->GetComponentName((IComponentName**)&name);
    Boolean res;
    if (IObjects::Probe(mExpectedComponentName)->Equals(name, &res), res) {
        String id;
        accountHandle->GetId(&id);
        if (!id.IsNull()) {
            //try {
            AutoPtr<ISubscriptionController> controller;
            SubscriptionController::GetInstance((ISubscriptionController**)&controller);
            Int32 phoneId;
            FAIL_GOTO(controller->GetPhoneId(StringUtils::ParseInt64(id), &phoneId), ERROR)
            AutoPtr<IPhone> phone;
            PhoneFactory::GetPhone(phoneId, (IPhone**)&phone);
            return phone;
            //} catch (NumberFormatException e) {
        ERROR:
            if (ec == (ECode)NumberFormatException) {
                Logger::W("CTelephonyConnectionService", "Could not get subId from account: %s", id.tostring());
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
            cdmaPhone->IsInEcm(&res);
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