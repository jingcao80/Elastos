
#include "elastos/droid/teleservice/services/telephony/sip/CSipConnectionService.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipConnection.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipUtil.h"
#include "elastos/droid/teleservice/services/telephony/DisconnectCauseUtil.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/StringUtils.h>
#include "elastos/utility/Objects.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecomm::Telecom::ITelecomManagerHelper;
using Elastos::Droid::Telecomm::Telecom::CTelecomManagerHelper;
using Elastos::Core::StringUtils;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

const String CSipConnectionService::PREFIX("[SipConnectionService] ");
const Boolean CSipConnectionService::VERBOSE = FALSE; /* STOP SHIP if true */

CAR_OBJECT_IMPL(CSipConnectionService)

ECode CSipConnectionService::OnCreate()
{
    mSipProfileDb = new SipProfileDb((IContext*)this);
    CHandler::New((IHandler**)&mHandler);
    assert(0);
    //return ConnectionService::OnCreate();
    return NOERROR;
}

ECode CSipConnectionService::OnCreateOutgoingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** res)
{
    VALIDATE_NOT_NULL(res)

    if (VERBOSE) Log(String("onCreateOutgoingConnection, request: ") + TO_CSTR(request));

    AutoPtr<IBundle> extras;
    request->GetExtras((IBundle**)&extras);
    if (extras != NULL) {
        String str;
        extras->GetString(ITelecomManager::GATEWAY_PROVIDER_PACKAGE, &str);
        if (!str.IsNull()) {
            AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause =
                DisconnectCauseUtil::ToTelecomDisconnectCause(
                    Elastos::Droid::Telephony::IDisconnectCause::CALL_BARRED,
                    String("Cannot make a SIP call with a gateway number."));
            return Connection::CreateFailedConnection(cause, res);
        }
    }

    AutoPtr<IPhoneAccountHandle> accountHandle;
    request->GetAccountHandle((IPhoneAccountHandle**)&accountHandle);
    AutoPtr<IComponentName> sipComponentName;
    assert(0);
    //CComponentName::New(this, SipConnectionService.class, (IComponentName**)&sipComponentName);
    AutoPtr<IComponentName> name;
    accountHandle->GetComponentName((IComponentName**)&name);
    if (!Objects::Equals(name, sipComponentName)) {
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause =
                DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::OUTGOING_FAILURE,
                String("Did not match service connection"));
        return Connection::CreateFailedConnection(cause, res);
    }

    AutoPtr<SipConnection> connection = new SipConnection();
    AutoPtr<IUri> address;
    request->GetAddress((IUri**)&address);
    connection->SetAddress(address, ITelecomManager::PRESENTATION_ALLOWED);
    connection->SetInitializing();
    connection->OnAddedToCallService();
    Boolean attemptCall = TRUE;

    if (!SipUtil::IsVoipSupported((IContext*)this)) {
        AutoPtr<ICharSequence> description;
        assert(0);
        //GetString(R::string::no_voip, (ICharSequence**)&description);
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause;
        Elastos::Droid::Telecomm::Telecom::CDisconnectCause::New(
                Elastos::Droid::Telecomm::Telecom::IDisconnectCause::ERROR, NULL, description,
                String("VoIP unsupported"), (Elastos::Droid::Telecomm::Telecom::IDisconnectCause**)&cause);
        connection->SetDisconnected(cause);
        attemptCall = FALSE;
    }

    if (attemptCall && !IsNetworkConnected()) {
        if (VERBOSE) Log(String("start, network not connected, dropping call"));
        assert(0);
        //Boolean wifiOnly = SipManager.isSipWifiOnly(this);
        AutoPtr<ICharSequence> description;
        assert(0);
        //GetString(wifiOnly ? R::string::no_wifi_available
        //        : R::string::no_internet_available, (ICharSequence**)&description);

        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause;
        Elastos::Droid::Telecomm::Telecom::CDisconnectCause::New(
                Elastos::Droid::Telecomm::Telecom::IDisconnectCause::ERROR, NULL, description,
                String("Network not connected"), (Elastos::Droid::Telecomm::Telecom::IDisconnectCause**)&cause);
        connection->SetDisconnected(cause);
        attemptCall = FALSE;
    }

    if (attemptCall) {
        // The ID used for SIP-based phone account is the SIP profile Uri. Use it to find
        // the actual profile.
        String profileUri;
        accountHandle->GetId(&profileUri);
        assert(0);
        // FindProfile(profileUri, new IProfileFinderCallback() {
        //     @Override
        //     public void onFound(SipProfile profile) {
        //         if (profile == null) {
        //             connection.setDisconnected(DisconnectCauseUtil.toTelecomDisconnectCause(
        //                     DisconnectCause.OUTGOING_FAILURE, "SIP profile not found."));
        //             connection.destroy();
        //         } else {
        //             com.android.internal.telephony.Connection chosenConnection =
        //                     createConnectionForProfile(profile, request);
        //             if (chosenConnection == null) {
        //                 connection.setDisconnected(DisconnectCauseUtil.toTelecomDisconnectCause(
        //                         DisconnectCause.OUTGOING_FAILURE, "Connection failed."));
        //                 connection.destroy();
        //             } else {
        //                 if (VERBOSE) log("initializing connection");
        //                 connection.initialize(chosenConnection);
        //             }
        //         }
        //     }
        // });
    }

    *res = Elastos::Droid::Telecomm::Telecom::IConnection::Probe(connection);
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CSipConnectionService::OnCreateIncomingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** res)
{
    VALIDATE_NOT_NULL(res)

    if (VERBOSE) Log(String("onCreateIncomingConnection, request: ") + TO_CSTR(request));

    AutoPtr<IBundle> bundle;
    request->GetExtras((IBundle**)&bundle);
    if (bundle == NULL) {
        if (VERBOSE) Log(String("onCreateIncomingConnection, no extras"));
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause =
                DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::ERROR_UNSPECIFIED,
                String("No extras on request."));
        return Connection::CreateFailedConnection(cause, res);
    }

    AutoPtr<IParcelable> p;
    bundle->GetParcelable(SipUtil::EXTRA_INCOMING_CALL_INTENT, (IParcelable**)&p);
    AutoPtr<IIntent> sipIntent = IIntent::Probe(p);
    if (sipIntent == NULL) {
        if (VERBOSE) Log(String("onCreateIncomingConnection, no SIP intent"));
        AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause =
                DisconnectCauseUtil::ToTelecomDisconnectCause(
                Elastos::Droid::Telephony::IDisconnectCause::ERROR_UNSPECIFIED,
                String("No SIP intent"));
        return Connection::CreateFailedConnection(cause, res);
    }

    assert(0);
    // SipAudioCall sipAudioCall;
    // try {
    //     sipAudioCall = SipManager.newInstance(this).takeAudioCall(sipIntent, null);
    // } catch (SipException e) {
    //     log("onCreateIncomingConnection, takeAudioCall exception: " + e);
    //     return Connection.createCanceledConnection();
    // }

    assert(0);
    AutoPtr<ISipPhone> phone;// = findPhoneForProfile(sipAudioCall.getLocalProfile());
    if (phone == NULL) {
        assert(0);
        //phone = CreatePhoneForProfile(sipAudioCall.getLocalProfile());
    }
    if (phone != NULL) {
        AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> originalConnection;
        assert(0);
        //phone->TakeIncomingCall(sipAudioCall);
        if (VERBOSE) Log(String("onCreateIncomingConnection, new connection: ") + TO_CSTR(originalConnection));
        if (originalConnection != NULL) {
            AutoPtr<SipConnection> sipConnection = new SipConnection();
            sipConnection->Initialize(originalConnection);
            sipConnection->OnAddedToCallService();
            *res = Elastos::Droid::Telecomm::Telecom::IConnection::Probe(sipConnection);
            REFCOUNT_ADD(*res);
            return NOERROR;
        }
        else {
            if (VERBOSE) Log(String("onCreateIncomingConnection, takingIncomingCall failed"));
            return Connection::CreateCanceledConnection(res);
        }
    }
    AutoPtr<Elastos::Droid::Telecomm::Telecom::IDisconnectCause> cause = DisconnectCauseUtil::ToTelecomDisconnectCause(
            Elastos::Droid::Telephony::IDisconnectCause::ERROR_UNSPECIFIED);
    return Connection::CreateFailedConnection(cause, res);
}

// CARAPI_(AutoPtr<Elastos::Droid::Internal::Telephony::IConnection>) CreateConnectionForProfile(
//     /* [in] */ SipProfile* profile,
//     /* [in] */ IConnectionRequest* request);


void CSipConnectionService::FindProfile(
    /* [in] */ const String& profileUri,
    /* [in] */ IProfileFinderCallback* callback)
{
    if (VERBOSE) Log(String("findProfile"));
    assert(0);
    // new Thread(new Runnable() {
    //     @Override
    //     public void run() {
    //         SipProfile profileToUse = null;
    //         List<SipProfile> profileList = mSipProfileDb.retrieveSipProfileList();
    //         if (profileList != null) {
    //             for (SipProfile profile : profileList) {
    //                 if (Objects.equals(profileUri, profile.getUriString())) {
    //                     profileToUse = profile;
    //                     break;
    //                 }
    //             }
    //         }

    //         final SipProfile profileFound = profileToUse;
    //         mHandler.post(new Runnable() {
    //             @Override
    //             public void run() {
    //                 callback.onFound(profileFound);
    //             }
    //         });
    //     }
    // }).start();
}

// CARAPI_(AutoPtr<ISipPhone>) FindPhoneForProfile(
//     /* [in] */ SipProfile* profile);

// CARAPI_(AutoPtr<ISipPhone>) CreatePhoneForProfile(
//     /* [in] */ SipProfile* profile);

AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> CSipConnectionService::StartCallWithPhone(
    /* [in] */ ISipPhone* phone,
    /* [in] */ IConnectionRequest* request)
{
    AutoPtr<IUri> address;
    request->GetAddress((IUri**)&address);
    String number;
    address->GetSchemeSpecificPart(&number);
    if (VERBOSE) Log(String("startCallWithPhone, number: ") + number);

    ECode ec = NOERROR;
    //try
    {
        ECode ec = NOERROR;
        Int32 state;
        FAIL_GOTO(ec = request->GetVideoState(&state), ERROR)
        AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> originalConnection;
        FAIL_GOTO(ec = IPhone::Probe(phone)->Dial(number, state,
                (Elastos::Droid::Internal::Telephony::IConnection**)&originalConnection), ERROR)
        return originalConnection;
    }
    // catch (CallStateException e) {
ERROR:
    if (ec == (ECode)E_CALL_STATE_EXCEPTION) {
        Log(String("startCallWithPhone, exception: ") + StringUtils::ToString(ec));
        return NULL;
    }
    return NULL;
}

Boolean CSipConnectionService::IsNetworkConnected()
{
    AutoPtr<IInterface> obj;
    assert(0);
    //GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    if (cm != NULL) {
        AutoPtr<INetworkInfo> ni;
        cm->GetActiveNetworkInfo((INetworkInfo**)&ni);
        Boolean res;
        if (ni != NULL && (ni->IsConnected(&res), res)) {
            Int32 type;
            ni->GetType(&type);
            assert(0);
            // return type == IConnectivityManager::TYPE_WIFI ||
            //         !SipManager::IsSipWifiOnly(this);
        }
    }
    return FALSE;
}

void CSipConnectionService::Log(
    /* [in] */ const String& msg)
{
    Logger::D(SipUtil::TAG, PREFIX + msg);
}

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos