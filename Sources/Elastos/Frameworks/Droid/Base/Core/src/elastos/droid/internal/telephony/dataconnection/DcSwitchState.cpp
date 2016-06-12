
#include "elastos/droid/internal/telephony/dataconnection/DcSwitchState.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <_Elastos.Droid.Core.h>

using Elastos::Droid::Internal::Utility::IProtocol;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcSwitchState::IdleState
//=============================================================================
DcSwitchState::IdleState::IdleState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::IdleState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mIdleRegistrants.notifyRegistrants();

#endif
}

ECode DcSwitchState::IdleState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                boolean retVal;
                switch (msg.what) {
                    case DcSwitchAsyncChannel.REQ_CONNECT:
                    case EVENT_CONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("IdleState: REQ_CONNECT/EVENT_CONNECT(" +
                                msg.what + ") type=" + type);
                        }
                        boolean isPrimarySubFeatureEnable =
                                SystemProperties.getBoolean("persist.radio.primarycard", false);
                        PhoneBase pb = (PhoneBase)((PhoneProxy)mPhone).getActivePhone();
                        long subId = pb.getSubId();
                        log("Setting default DDS on " + subId + " primary Sub feature"
                                + isPrimarySubFeatureEnable);
                        // When isPrimarySubFeatureEnable is enabled apps will take care
                        // of sending DDS request during device power-up.
                        if (!isPrimarySubFeatureEnable) {
                            SubscriptionController subscriptionController
                                    = SubscriptionController.getInstance();
                            subscriptionController.setDefaultDataSubId(subId);
                        }
                        int result = setupConnection(type);
                        if (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                                mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT, result);
                        }
                        transitionTo(mActingState);
                        retVal = HANDLED;
                        break;
                    }
                    case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("IdleState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                        }
                        mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT,
                            PhoneConstants.APN_ALREADY_INACTIVE);
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CLEANUP_ALL: {
                        if (DBG) {
                            log("IdleState: EVENT_CLEANUP_ALL" );
                        }
                        requestDataIdle();
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CONNECTED: {
                        if (DBG) {
                            log("IdleState: Receive invalid event EVENT_CONNECTED!");
                        }
                        retVal = HANDLED;
                        break;
                    }
                    default:
                        if (VDBG) {
                            log("IdleState: nothandled msg.what=0x" +
                                    Integer.toHexString(msg.what));
                        }
                        retVal = NOT_HANDLED;
                        break;
                }
                return retVal;

#endif
}

//=============================================================================
// DcSwitchState::ActingState
//=============================================================================
DcSwitchState::ActingState::ActingState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::ActingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                boolean retVal;
                switch (msg.what) {
                    case DcSwitchAsyncChannel.REQ_CONNECT:
                    case EVENT_CONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("ActingState: REQ_CONNECT/EVENT_CONNECT(" + msg.what +
                                ") type=" + type);
                        }
                        int result = setupConnection(type);
                        if (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                            mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT, result);
                        }
                        retVal = HANDLED;
                        break;
                    }
                    case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("ActingState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                        }
                        int result = teardownConnection(type);
                        mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT, result);
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CONNECTED: {
                        if (DBG) {
                            log("ActingState: EVENT_CONNECTED");
                        }
                        transitionTo(mActedState);
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CLEANUP_ALL: {
                        if (DBG) {
                            log("ActingState: EVENT_CLEANUP_ALL" );
                        }
                        requestDataIdle();
                        transitionTo(mDeactingState);
                        retVal = HANDLED;
                        break;
                    }
                    default:
                        if (VDBG) {
                            log("ActingState: nothandled msg.what=0x" +
                                    Integer.toHexString(msg.what));
                        }
                        retVal = NOT_HANDLED;
                        break;
                }
                return retVal;

#endif
}

//=============================================================================
// DcSwitchState::ActedState
//=============================================================================
DcSwitchState::ActedState::ActedState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::ActedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                boolean retVal;
                switch (msg.what) {
                    case DcSwitchAsyncChannel.REQ_CONNECT:
                    case EVENT_CONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("ActedState: REQ_CONNECT/EVENT_CONNECT(" + msg.what + ") type=" + type);
                        }
                        int result = setupConnection(type);
                        if (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                            mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT, result);
                        }
                        retVal = HANDLED;
                        break;
                    }
                    case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("ActedState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                        }
                        int result = teardownConnection(type);
                        mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT, result);
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CONNECTED: {
                        if (DBG) {
                            log("ActedState: EVENT_CONNECTED");
                        }
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CLEANUP_ALL: {
                        if (DBG) {
                            log("ActedState: EVENT_CLEANUP_ALL" );
                        }
                        requestDataIdle();
                        transitionTo(mDeactingState);
                        retVal = HANDLED;
                        break;
                    }
                    default:
                        if (VDBG) {
                            log("ActingState: nothandled msg.what=0x" +
                                    Integer.toHexString(msg.what));
                        }
                        retVal = NOT_HANDLED;
                        break;
                }
                return retVal;

#endif
}

//=============================================================================
// DcSwitchState::DeactingState
//=============================================================================
DcSwitchState::DeactingState::DeactingState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::DeactingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                boolean retVal;
                switch (msg.what) {
                    case DcSwitchAsyncChannel.REQ_CONNECT:
                    case EVENT_CONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("DeactingState: REQ_CONNECT/EVENT_CONNECT(" +
                                msg.what + ") type=" + type + ", request is defered.");
                        }
                        deferMessage(obtainMessage(EVENT_CONNECT, type));
                        if (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                            mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT,
                                    PhoneConstants.APN_REQUEST_STARTED);
                        }
                        retVal = HANDLED;
                        break;
                    }
                    case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                        String type = (String)msg.obj;
                        if (DBG) {
                            log("DeactingState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                        }
                        mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT,
                                PhoneConstants.APN_ALREADY_INACTIVE);
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_DETACH_DONE: {
                        if (DBG) {
                            log("DeactingState: EVENT_DETACH_DONE");
                        }
                        transitionTo(mIdleState);
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CONNECTED: {
                        if (DBG) {
                            log("DeactingState: Receive invalid event EVENT_CONNECTED!");
                        }
                        retVal = HANDLED;
                        break;
                    }
                    case EVENT_CLEANUP_ALL: {
                        if (DBG) {
                            log("DeactingState: EVENT_CLEANUP_ALL, already deacting." );
                        }
                        retVal = HANDLED;
                        break;
                    }
                    default:
                        if (VDBG) {
                            log("DeactingState: nothandled msg.what=0x" +
                                    Integer.toHexString(msg.what));
                        }
                        retVal = NOT_HANDLED;
                        break;
                }
                return retVal;

#endif
}

//=============================================================================
// DcSwitchState::DefaultState
//=============================================================================
DcSwitchState::DefaultState::DefaultState(
    /* [in] */ DcSwitchState* host)
    : mHost(host)
{}

ECode DcSwitchState::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                AsyncResult ar;
                switch (msg.what) {
                    case AsyncChannel.CMD_CHANNEL_FULL_CONNECTION: {
                        if (mAc != null) {
                            if (VDBG) log("Disconnecting to previous connection mAc=" + mAc);
                            mAc.replyToMessage(msg, AsyncChannel.CMD_CHANNEL_FULLY_CONNECTED,
                                    AsyncChannel.STATUS_FULL_CONNECTION_REFUSED_ALREADY_CONNECTED);
                        } else {
                            mAc = new AsyncChannel();
                            mAc.connected(null, getHandler(), msg.replyTo);
                            if (VDBG) log("DcDefaultState: FULL_CONNECTION reply connected");
                            mAc.replyToMessage(msg, AsyncChannel.CMD_CHANNEL_FULLY_CONNECTED,
                                    AsyncChannel.STATUS_SUCCESSFUL, mId, "hi");
                        }
                        break;
                    }
                    case AsyncChannel.CMD_CHANNEL_DISCONNECT: {
                        if (VDBG) log("CMD_CHANNEL_DISCONNECT");
                        mAc.disconnect();
                        break;
                    }
                    case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                        if (VDBG) log("CMD_CHANNEL_DISCONNECTED");
                        mAc = null;
                        break;
                    }
                    case DcSwitchAsyncChannel.REQ_IS_IDLE_STATE: {
                        boolean val = getCurrentState() == mIdleState;
                        if (VDBG) log("REQ_IS_IDLE_STATE  isIdle=" + val);
                        mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_IS_IDLE_STATE, val ? 1 : 0);
                        break;
                    }
                    case DcSwitchAsyncChannel.REQ_IS_IDLE_OR_DEACTING_STATE: {
                        boolean val = (getCurrentState() == mIdleState || getCurrentState() == mDeactingState);
                        if (VDBG) log("REQ_IS_IDLE_OR_DEACTING_STATE  isIdleDeacting=" + val);
                        mAc.replyToMessage(msg, DcSwitchAsyncChannel.RSP_IS_IDLE_OR_DEACTING_STATE, val ? 1 : 0);
                        break;
                    }
                    case EVENT_TO_ACTING_DIRECTLY: {
                        log("Just transit to Acting state");
                        transitionTo(mActingState);
                        break;
                    }
                    case EVENT_TO_IDLE_DIRECTLY: {
                        log("Just transit to Idle state");
                        Iterator<String> itrType = mApnTypes.iterator();
                        while (itrType.hasNext()) {
    //                        mPhone.disableApnType(itrType.next()); TODO
                        }
                        mApnTypes.clear();
                        transitionTo(mIdleState);
                    }
                    default:
                        if (DBG) {
                            log("DefaultState: shouldn't happen but ignore msg.what=0x" +
                                    Integer.toHexString(msg.what));
                        }
                        break;
                }
                return HANDLED;

#endif
}

//=============================================================================
// DcSwitchState
//=============================================================================
CAR_INTERFACE_IMPL(DcSwitchState, StateMachine, IDcSwitchState)

const Boolean DcSwitchState::DBG = true;
const Boolean DcSwitchState::VDBG = false;
const String DcSwitchState::LOG__TAG("DcSwitchState");
const Int32 DcSwitchState::BASE = IProtocol::BASE_DATA_CONNECTION_TRACKER + 0x00001000;
const Int32 DcSwitchState::EVENT_CONNECT = BASE + 0;
const Int32 DcSwitchState::EVENT_DISCONNECT = BASE + 1;
const Int32 DcSwitchState::EVENT_CLEANUP_ALL = BASE + 2;
const Int32 DcSwitchState::EVENT_CONNECTED = BASE + 3;
const Int32 DcSwitchState::EVENT_DETACH_DONE = BASE + 4;
const Int32 DcSwitchState::EVENT_TO_IDLE_DIRECTLY = BASE + 5;
const Int32 DcSwitchState::EVENT_TO_ACTING_DIRECTLY = BASE + 6;

DcSwitchState::DcSwitchState()
{
#if 0 // TODO: Translate codes below
    CRegistrantList::New((IRegistrantList**)&mIdleRegistrants);
    CHashSet::New((IHashSet**)&mApnTypes);
    mIdleState = new IdleState(this);
    mActingState = new ActingState(this);
    mActedState = new ActedState(this);
    mDeactingState = new DeactingState(this);
    mDefaultState = new DefaultState(this);
#endif
}

ECode DcSwitchState::constructor(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& name,
    /* [in] */ Int32 id)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(name);
        if (DBG) log("DcSwitchState constructor E");
        mPhone = phone;
        mId = id;
        addState(mDefaultState);
        addState(mIdleState, mDefaultState);
        addState(mActingState, mDefaultState);
        addState(mActedState, mDefaultState);
        addState(mDeactingState, mDefaultState);
        setInitialState(mIdleState);
        if (DBG) log("DcSwitchState constructor X");

#endif
}

ECode DcSwitchState::SetupConnection(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mApnTypes.add(type);
        log("DcSwitchState:setupConnection type = " + type);
//        return mPhone.enableApnType(type); TODO
        return PhoneConstants.APN_REQUEST_STARTED;

#endif
}

ECode DcSwitchState::TeardownConnection(
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mApnTypes.remove(type);
        if (mApnTypes.isEmpty()) {
            log("No APN is using, then clean up all");
            // Since last type is removed from mApnTypes and will not be disabled in requestDataIdle()
//            mPhone.disableApnType(type); TODO
            requestDataIdle();
            transitionTo(mDeactingState);
            return PhoneConstants.APN_REQUEST_STARTED;
        } else {
//            return mPhone.disableApnType(type); TODO
            return PhoneConstants.APN_REQUEST_STARTED;
        }

#endif
}

ECode DcSwitchState::RequestDataIdle()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("requestDataIdle is triggered");
        Iterator<String> itrType = mApnTypes.iterator();
        while (itrType.hasNext()) {
//            mPhone.disableApnType(itrType.next()); TODO
        }
        mApnTypes.clear();
        PhoneBase pb = (PhoneBase)((PhoneProxy)mPhone).getActivePhone();
        pb.mCi.setDataAllowed(false, obtainMessage(EVENT_DETACH_DONE));

#endif
}

ECode DcSwitchState::NotifyDataConnection(
    /* [in] */ Int32 phoneId,
    /* [in] */ const String& state,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnName,
    /* [in] */ const String& apnType,
    /* [in] */ Boolean unavailable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (phoneId == mId &&
                TextUtils.equals(state, PhoneConstants.DataState.CONNECTED.toString())) {
            sendMessage(obtainMessage(EVENT_CONNECTED));
        }

#endif
}

ECode DcSwitchState::CleanupAllConnection()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(obtainMessage(EVENT_CLEANUP_ALL));

#endif
}

ECode DcSwitchState::RegisterForIdle(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Registrant r = new Registrant(h, what, obj);
        mIdleRegistrants.add(r);

#endif
}

ECode DcSwitchState::UnregisterForIdle(
    /* [in] */ IHandler* h)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mIdleRegistrants.remove(h);

#endif
}

ECode DcSwitchState::TransitToIdleState()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(obtainMessage(EVENT_TO_IDLE_DIRECTLY));

#endif
}

ECode DcSwitchState::TransitToActingState()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(obtainMessage(EVENT_TO_ACTING_DIRECTLY));

#endif
}

ECode DcSwitchState::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Log.d(LOG__TAG, "[" + getName() + "] " + s);

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
