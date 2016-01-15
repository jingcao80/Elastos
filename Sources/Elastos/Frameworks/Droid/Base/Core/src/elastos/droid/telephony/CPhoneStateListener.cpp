
#include "CPhoneStateListener.h"
#include "CCellLocationHelper.h"
#include "elastos/droid/privacy/CPrivacySettingsManager.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Internal::Telephony::EIID_IIPhoneStateListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Privacy::CPrivacySettingsManager;
using Elastos::Droid::Privacy::IIPrivacySettingsManager;
using Elastos::Droid::Privacy::IPrivacySettings;
using Elastos::Droid::Privacy::IPrivacySettingsManager;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CPhoneStateListener::TAG("CPhoneStateListener");

CPhoneStateListener::CPhoneStateListenerCallback::CPhoneStateListenerCallback(
            /* [in] */ CPhoneStateListener* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(CPhoneStateListener::CPhoneStateListenerCallback, IIPhoneStateListener);

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnServiceStateChanged(
    /* [in] */ IServiceState* serviceState)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_SERVICE_STATE, 0, 0, (IInterface*)serviceState, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnSignalStrengthChanged(
    /* [in] */ Int32 asu)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_SIGNAL_STRENGTH, asu, 0, (IInterface*)NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnMessageWaitingIndicatorChanged(
    /* [in] */ Boolean mwi)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_MESSAGE_WAITING_INDICATOR, mwi ? 1 : 0, 0, (IInterface*)NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnCallForwardingIndicatorChanged(
    /* [in] */ Boolean cfi)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CALL_FORWARDING_INDICATOR, cfi ? 1 : 0, 0, (IInterface*)NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

// we use bundle here instead of CellLocation so it can get the right subclass
ECode CPhoneStateListener::CPhoneStateListenerCallback::OnCellLocationChanged(
    /* [in] */ IBundle* location)
{
    AutoPtr<ICellLocation> cellLocation;
    AutoPtr<ICellLocationHelper> helper;
    CCellLocationHelper::AcquireSingleton((ICellLocationHelper**)&helper);
    helper->NewFromBundle(location, (ICellLocation**)&cellLocation);
    //Message.obtain(mHandler, LISTEN_CELL_LOCATION, 0, 0, location).sendToTarget();
    // BEGIN privacy-modified
    if (mHost->mContext != NULL) {
        AutoPtr<IPrivacySettingsManager> pSetMan;
        mHost->mContext->GetSystemService(String("privacy"), (IInterface**)&pSetMan);
        AutoPtr<IPrivacySettings> pSet;
        pSetMan->GetSettings(mHost->mPackageName, (IPrivacySettings**)&pSet);
    //                String output;
        Byte bLns;
        pSet->GetLocationNetworkSetting(&bLns);
        Int16 iLns = static_cast<Int16>(bLns);
        if (pSet != NULL && iLns != IPrivacySettings::REAL) {
            // simply block the method call, since simulating cell location is not feasible
    //                    output = "[no output]";
            pSetMan->Notification(mHost->mPackageName, bLns, IPrivacySettings::DATA_LOCATION_NETWORK, String(NULL));
        } else {
    //                    output = location.toString();
            AutoPtr<IMessage> msg;
            mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CELL_LOCATION, 0, 0, reinterpret_cast<IInterface*>(cellLocation.Get()), (IMessage**)&msg);
            msg->SendToTarget();
            pSetMan->Notification(mHost->mPackageName, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_NETWORK, String(NULL));
        }
    //                Log.d(TAG, "onCellLocationChanged - " + context.getPackageName() + " (" +
    //                        Binder.getCallingUid() + ") output: " + output);
    }
    // END privacy-modified

    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    //Message.obtain(mHandler, LISTEN_CALL_STATE, state, 0, incomingNumber).sendToTarget();
    // BEGIN privacy-modified
    //            Log.d(TAG, "onCallStateChanged - state:" + state + " incoming number:" + incomingNumber);
    // only take action if an incoming phone number is actually transmitted
    if (mHost->mContext != NULL && !incomingNumber.IsNull() && !incomingNumber.IsEmpty()) {
        AutoPtr<IPrivacySettingsManager> pSetMan;
        mHost->mContext->GetSystemService(String("privacy"), (IInterface**)&pSetMan);
        AutoPtr<IPrivacySettings> pSet;
        pSetMan->GetSettings(mHost->mPackageName, (IPrivacySettings**)&pSet);
        String output;
        Byte bIcs;
        pSet->GetIncomingCallsSetting(&bIcs);
        Int32 iIcs = static_cast<Int16>(bIcs);
        if (pSet != NULL && iIcs != IPrivacySettings::REAL) {
            output = String("");
            AutoPtr<ICharSequence> cs;
            CString::New(output, (ICharSequence**)&cs);
            AutoPtr<IMessage> msg;
            mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CALL_STATE, state, 0, reinterpret_cast<IInterface*>(cs.Get()), (IMessage**)&msg);            msg->SendToTarget();
    //                    Log.d(TAG, "onCallStateChanged BLOCK - package:" + mHost->mPackageName + " uid:" + uid + " state:" + state + " output: " + output);
            pSetMan->Notification(mHost->mPackageName, IPrivacySettings::EMPTY, IPrivacySettings::DATA_INCOMING_CALL, output);
        } else {
            AutoPtr<ICharSequence> cs;
            CString::New(incomingNumber, (ICharSequence**)&cs);
            AutoPtr<IMessage> msg;
            mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CALL_STATE, state, 0, reinterpret_cast<IInterface*>(cs.Get()), (IMessage**)&msg);
            msg->SendToTarget();
    //                    Log.d(TAG, "onCallStateChanged REAL 1 - package:" + mHost->mPackageName + " uid:" + uid + " state:" + state + " output: " + incomingNumber);
            pSetMan->Notification(mHost->mPackageName, IPrivacySettings::REAL, IPrivacySettings::DATA_INCOMING_CALL, incomingNumber);
        }
    } else {
    //                Log.d(TAG, "onCallStateChanged REAL 2 - package:" + mHost->mPackageName + " uid:" + uid + " state:" + state + " output: " + incomingNumber);
        AutoPtr<ICharSequence> cs;
        CString::New(incomingNumber, (ICharSequence**)&cs);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CALL_STATE, state, 0, reinterpret_cast<IInterface*>(cs.Get()), (IMessage**)&msg);
        msg->SendToTarget();
    }
    // END privacy-modified

    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE, state, networkType, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnDataActivity(
    /* [in] */ Int32 direction)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_DATA_ACTIVITY, direction, 0, NULL, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnSignalStrengthsChanged(
    /* [in] */ ISignalStrength* signalStrength)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS, 0, 0, reinterpret_cast<IInterface*>(signalStrength), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_OTASP_CHANGED, otaspMode, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CPhoneStateListener::CPhoneStateListenerCallback::OnCellInfoChanged(
    /* [in] */ ArrayOf<ICellInfo*>* cellInfo)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(IPhoneStateListener::LISTEN_CELL_INFO, 0, 0, reinterpret_cast<IInterface*>(cellInfo), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

//==============================================================================
// CPhoneStateListener::MyHandler
//==============================================================================

CPhoneStateListener::MyHandler::MyHandler(
    /* [in] */ CPhoneStateListener* host)
    : mHost(host)
{
}

ECode CPhoneStateListener::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    //Log.d("TelephonyRegistry", "what=0x" + Integer.toHexString(what) + " msg=" + msg);
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
            assert(0 && "TODO");
            mHost->OnCellInfoChanged(reinterpret_cast< ArrayOf<ICellInfo *>* >(obj.Get()));
    }

    return NOERROR;
}

ECode CPhoneStateListener::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode CPhoneStateListener::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CPhoneStateListener::SetUid(
    /* [in] */ Int32 uid)
{
    mUid = uid;
    return NOERROR;
}

ECode CPhoneStateListener::OnServiceStateChanged(
    /* [in] */ IServiceState* serviceState)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnSignalStrengthChanged(
    /* [in] */ Int32 asu)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnMessageWaitingIndicatorChanged(
    /* [in] */ Boolean mwi)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnCallForwardingIndicatorChanged(
    /* [in] */ Boolean cfi)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnCellLocationChanged(
    /* [in] */ ICellLocation* location)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{   // default implementation empty

    return NOERROR;
}

ECode CPhoneStateListener::OnDataConnectionStateChanged(
    /* [in] */ Int32 state)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType)
{   // default implementation empty

    return NOERROR;
}

ECode CPhoneStateListener::OnDataActivity(
    /* [in] */ Int32 direction)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnSignalStrengthsChanged(
    /* [in] */ ISignalStrength* signalStrength)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::OnCellInfoChanged(
    /* [in] */ ArrayOf<ICellInfo *>* cellInfo)
{
    // default implementation empty
    return NOERROR;
}

ECode CPhoneStateListener::constructor()
{
    return NOERROR;
}

}
}
}

