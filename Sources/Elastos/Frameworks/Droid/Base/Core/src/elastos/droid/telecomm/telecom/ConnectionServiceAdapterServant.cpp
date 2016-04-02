
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/ConnectionServiceAdapterServant.h"
#include "elastos/droid/telecomm/telecom/CConnectionServiceAdapterStub.h"

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Telecomm::Internal::IRemoteServiceCallback;
using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// ConnectionServiceAdapterServant::MyHandler::
//===============================================================
ConnectionServiceAdapterServant::MyHandler::MyHandler(
    /* [in] */ ConnectionServiceAdapterServant* host)
    : mHost(host)
{}

ECode ConnectionServiceAdapterServant::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    InternalHandleMessage(msg);
    return NOERROR;
}

// Internal method defined to centralize handling of RemoteException
void ConnectionServiceAdapterServant::MyHandler::InternalHandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case IConnectionServiceAdapterServant::MSG_HANDLE_CREATE_CONNECTION_COMPLETE: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IConnectionRequest> pArg2 = IConnectionRequest::Probe(arg2);
            AutoPtr<IInterface> arg3;
            args->GetObjectArg(3, (IInterface**)&arg3);
            AutoPtr<IParcelableConnection> pArg3 = IParcelableConnection::Probe(arg3);
            mHost->mDelegate->HandleCreateConnectionComplete(
                    strArg1,
                    pArg2,
                    pArg3);

            args->Recycle();

            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_ACTIVE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->mDelegate->SetActive(strObj);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_RINGING: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->mDelegate->SetRinging(strObj);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_DIALING: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->mDelegate->SetDialing(strObj);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_DISCONNECTED: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IDisconnectCause> pArg2 = IDisconnectCause::Probe(arg2);
            mHost->mDelegate->SetDisconnected(strArg1, pArg2);
            args->Recycle();

            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_ON_HOLD: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->mDelegate->SetOnHold(strObj);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_RINGBACK_REQUESTED: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->mDelegate->SetRingbackRequested(strObj, arg1 == 1);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_CALL_CAPABILITIES: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->mDelegate->SetCallCapabilities(strObj, arg1);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_IS_CONFERENCED: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<ICharSequence> pArg2 = ICharSequence::Probe(arg2);
            String strArg2;
            pArg2->ToString(&strArg2);
            mHost->mDelegate->SetIsConferenced(strArg1, strArg2);
            args->Recycle();
            break;
        }
        case IConnectionServiceAdapterServant::MSG_ADD_CONFERENCE_CALL: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IParcelableConference> pArg2 = IParcelableConference::Probe(arg2);
            mHost->mDelegate->AddConferenceCall(
                        strArg1, pArg2);
            args->Recycle();
            break;
        }
        case IConnectionServiceAdapterServant::MSG_REMOVE_CALL: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->mDelegate->RemoveCall(strObj);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_ON_POST_DIAL_WAIT: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<ICharSequence> pArg2 = ICharSequence::Probe(arg2);
            String strArg2;
            pArg2->ToString(&strArg2);
            mHost->mDelegate->OnPostDialWait(strArg1, strArg2);
            args->Recycle();
            break;
        }
        case IConnectionServiceAdapterServant::MSG_QUERY_REMOTE_CALL_SERVICES: {
            AutoPtr<IRemoteServiceCallback> pObj = IRemoteServiceCallback::Probe(obj);
            mHost->mDelegate->QueryRemoteConnectionServices(pObj);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_VIDEO_STATE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->mDelegate->SetVideoState(strObj, arg1);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_VIDEO_CALL_PROVIDER: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IIVideoProvider> pArg2 = IIVideoProvider::Probe(arg2);
            mHost->mDelegate->SetVideoProvider(strArg1, pArg2);
            args->Recycle();
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_IS_VOIP_AUDIO_MODE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            Int32 arg1 = 0;
            msg->GetArg1(&arg1);
            mHost->mDelegate->SetIsVoipAudioMode(strObj, arg1 == 1);
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_STATUS_HINTS: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IStatusHints> pArg2 = IStatusHints::Probe(arg2);
            mHost->mDelegate->SetStatusHints(strArg1, pArg2);
            args->Recycle();
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_ADDRESS: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IUri> pArg2 = IUri::Probe(arg2);
            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            mHost->mDelegate->SetAddress(strArg1, pArg2, argi1);
            args->Recycle();
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_CALLER_DISPLAY_NAME: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<ICharSequence> pArg2 = ICharSequence::Probe(arg2);
            String strArg2;
            pArg2->ToString(&strArg2);
            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            mHost->mDelegate->SetCallerDisplayName(strArg1, strArg2, argi1);
            args->Recycle();
            break;
        }
        case IConnectionServiceAdapterServant::MSG_SET_CONFERENCEABLE_CONNECTIONS: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String strArg1;
            pArg1->ToString(&strArg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IList> pArg2 = IList::Probe(arg2);
            mHost->mDelegate->SetConferenceableConnections(strArg1, pArg2);
            args->Recycle();
            break;
        }
    }
}

//===============================================================
// ConnectionServiceAdapterServant::
//===============================================================
CAR_INTERFACE_IMPL(ConnectionServiceAdapterServant, Object, IConnectionServiceAdapterServant)

ConnectionServiceAdapterServant::ConnectionServiceAdapterServant()
{
    mHandler = new MyHandler(this);
    CConnectionServiceAdapterStub::New(mHandler, (IIConnectionServiceAdapter**)&mStub);
}

ECode ConnectionServiceAdapterServant::constructor(
    /* [in] */ IIConnectionServiceAdapter* delegate)
{
    mDelegate = delegate;
    return NOERROR;
}

ECode ConnectionServiceAdapterServant::GetStub(
    /* [out] */ IIConnectionServiceAdapter** result)
{
    *result = mStub;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos