
#include "elastos/droid/telephony/PhoneStateListener.h"
#include "elastos/droid/telephony/CCellLocationHelper.h"
#include "elastos/droid/telephony/CPhoneStateListenerCallback.h"
#include "elastos/droid/os/Looper.h"

using Elastos::Droid::Internal::Telephony::EIID_IIPhoneStateListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Privacy::IIPrivacySettingsManager;
using Elastos::Droid::Privacy::IPrivacySettings;
using Elastos::Droid::Privacy::IPrivacySettingsManager;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String PhoneStateListener::TAG("PhoneStateListener");
const Boolean PhoneStateListener::DBG = FALSE;

CAR_INTERFACE_IMPL(PhoneStateListener, Object, IPhoneStateListener)

//==============================================================================
// PhoneStateListener::PhoneStateListenerCallback
//==============================================================================

CAR_INTERFACE_IMPL_2(PhoneStateListener::PhoneStateListenerCallback,
        Object, IIPhoneStateListener, IBinder);

ECode PhoneStateListener::PhoneStateListenerCallback::constructor(
    /* [in] */ IPhoneStateListener* host)
{
    mHost = (PhoneStateListener*)host;
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnServiceStateChanged(
    /* [in] */ IServiceState* serviceState)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_SERVICE_STATE,
            0, 0, (IInterface*)serviceState, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnSignalStrengthChanged(
    /* [in] */ Int32 asu)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_SIGNAL_STRENGTH,
            asu, 0, (IInterface*)NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnMessageWaitingIndicatorChanged(
    /* [in] */ Boolean mwi)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_MESSAGE_WAITING_INDICATOR,
            mwi ? 1 : 0, 0, (IInterface*)NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnCallForwardingIndicatorChanged(
    /* [in] */ Boolean cfi)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CALL_FORWARDING_INDICATOR,
            cfi ? 1 : 0, 0, (IInterface*)NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

// we use bundle here instead of CellLocation so it can get the right subclass
ECode PhoneStateListener::PhoneStateListenerCallback::OnCellLocationChanged(
    /* [in] */ IBundle* location)
{
    AutoPtr<ICellLocation> cellLocation;
    AutoPtr<ICellLocationHelper> helper;
    CCellLocationHelper::AcquireSingleton((ICellLocationHelper**)&helper);
    helper->NewFromBundle(location, (ICellLocation**)&cellLocation);

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CELL_LOCATION,
            0, 0, location, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    AutoPtr<ICharSequence> cs;
    CString::New(incomingNumber, (ICharSequence**)&cs);
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CALL_STATE,
            state, 0, cs, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE,
            state, networkType, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnDataActivity(
    /* [in] */ Int32 direction)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_DATA_ACTIVITY,
            direction, 0, NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnSignalStrengthsChanged(
    /* [in] */ ISignalStrength* signalStrength)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS,
            0, 0, signalStrength, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_OTASP_CHANGED,
            otaspMode, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnCellInfoChanged(
    /* [in] */ IList* cellInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CELL_INFO,
            0, 0, cellInfo, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnPreciseCallStateChanged(
    /* [in] */ IPreciseCallState* callState)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_PRECISE_CALL_STATE,
            0, 0, callState, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnPreciseDataConnectionStateChanged(
    /* [in] */ IPreciseDataConnectionState* dataConnectionState)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_PRECISE_DATA_CONNECTION_STATE,
            0, 0, dataConnectionState, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnDataConnectionRealTimeInfoChanged(
    /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_DATA_CONNECTION_REAL_TIME_INFO,
            0, 0, dcRtInfo, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnVoLteServiceStateChanged(
    /* [in] */ IVoLteServiceState* lteState)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_VOLTE_STATE,
            0, 0, lteState, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::OnOemHookRawEvent(
    /* [in] */ ArrayOf<Byte>* rawData)
{
    AutoPtr<IByteBuffer> buffer;
    AutoPtr<IByteBufferHelper> bufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bufferHelper);
    bufferHelper->Wrap(rawData, (IByteBuffer**)&buffer);

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_OEM_HOOK_RAW_EVENT,
            0, 0, buffer, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode PhoneStateListener::PhoneStateListenerCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//==============================================================================
// PhoneStateListener::MyHandler
//==============================================================================

ECode PhoneStateListener::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case IPhoneStateListener::LISTEN_SERVICE_STATE:
            mHost->OnServiceStateChanged(IServiceState::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_SIGNAL_STRENGTH:
            mHost->OnSignalStrengthChanged(arg1);
            break;
        case IPhoneStateListener::LISTEN_MESSAGE_WAITING_INDICATOR:
            mHost->OnMessageWaitingIndicatorChanged(arg1 != 0);
            break;
        case IPhoneStateListener::LISTEN_CALL_FORWARDING_INDICATOR:
            mHost->OnCallForwardingIndicatorChanged(arg1 != 0);
            break;
        case IPhoneStateListener::LISTEN_CELL_LOCATION:
            mHost->OnCellLocationChanged(ICellLocation::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_CALL_STATE:
            {
                AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj.Get());
                String str;
                cs->ToString(&str);
                mHost->OnCallStateChanged(arg1, str);
            }
            break;
        case IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE:
            mHost->OnDataConnectionStateChanged(arg1, arg2);
            mHost->OnDataConnectionStateChanged(arg1);
            break;
        case IPhoneStateListener::LISTEN_DATA_ACTIVITY:
            mHost->OnDataActivity(arg1);
            break;
        case IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS:
            mHost->OnSignalStrengthsChanged(ISignalStrength::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_OTASP_CHANGED:
            mHost->OnOtaspChanged(arg1);
            break;
        case IPhoneStateListener::LISTEN_CELL_INFO:
            mHost->OnCellInfoChanged(IList::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_PRECISE_CALL_STATE:
            mHost->OnPreciseCallStateChanged(IPreciseCallState::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_PRECISE_DATA_CONNECTION_STATE:
            mHost->OnPreciseDataConnectionStateChanged(
                    IPreciseDataConnectionState::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_DATA_CONNECTION_REAL_TIME_INFO:
            mHost->OnDataConnectionRealTimeInfoChanged(
                    IDataConnectionRealTimeInfo::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_VOLTE_STATE:
            mHost->OnVoLteServiceStateChanged(IVoLteServiceState::Probe(obj.Get()));
            break;
        case IPhoneStateListener::LISTEN_OEM_HOOK_RAW_EVENT:
            {
                AutoPtr<IByteBuffer> buffer = IByteBuffer::Probe(obj.Get());
                AutoPtr<ArrayOf<Byte> > array;
                buffer->GetArray((ArrayOf<Byte>**)&array);

                mHost->OnOemHookRawEvent(array);
            }
            break;
    }

    return NOERROR;
}

//==============================================================================
// PhoneStateListener
//==============================================================================

PhoneStateListener::PhoneStateListener()
    : mSubId(0)
{
}

ECode PhoneStateListener::constructor()
{
    return constructor(ISubscriptionManager::DEFAULT_SUB_ID, Looper::GetMyLooper());
}

ECode PhoneStateListener::constructor(
    /* [in] */ ILooper* looper)
{
    return constructor(ISubscriptionManager::DEFAULT_SUB_ID, looper);
}

ECode PhoneStateListener::constructor(
    /* [in] */ Int64 subId)
{
    return constructor(subId, Looper::GetMyLooper());
}

ECode PhoneStateListener::constructor(
    /* [in] */ Int64 subId,
    /* [in] */ ILooper* looper)
{
    CPhoneStateListenerCallback::New(this, (IIPhoneStateListener**)&mCallback);
    mSubId = subId;
    mHandler = new MyHandler(this, looper);
    return NOERROR;
}

ECode PhoneStateListener::OnServiceStateChanged(
    /* [in] */ IServiceState* serviceState)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnSignalStrengthChanged(
    /* [in] */ Int32 asu)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnMessageWaitingIndicatorChanged(
    /* [in] */ Boolean mwi)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnCallForwardingIndicatorChanged(
    /* [in] */ Boolean cfi)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnCellLocationChanged(
    /* [in] */ ICellLocation* location)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnDataConnectionStateChanged(
    /* [in] */ Int32 state)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType)
{
   // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnDataActivity(
    /* [in] */ Int32 direction)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnSignalStrengthsChanged(
    /* [in] */ ISignalStrength* signalStrength)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnCellInfoChanged(
    /* [in] */ IList* cellInfo)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnPreciseCallStateChanged(
    /* [in] */ IPreciseCallState* callState)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnPreciseDataConnectionStateChanged(
    /* [in] */ IPreciseDataConnectionState* dataConnectionState)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnDataConnectionRealTimeInfoChanged(
    /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnVoLteServiceStateChanged(
    /* [in] */ IVoLteServiceState* stateInfo)
{
    // default implementation empty
    return NOERROR;
}

ECode PhoneStateListener::OnOemHookRawEvent(
    /* [in] */ ArrayOf<Byte>* rawData)
{
    // default implementation empty
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

