
#include "elastos/droid/internal/telephony/dataconnection/DdsScheduler.h"
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Telephony::ISubscriptionManager;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DdsScheduler::NetworkRequestInfo
//=============================================================================
DdsScheduler::NetworkRequestInfo::NetworkRequestInfo(
    /* [in] */ DdsScheduler* host)
    : mAccepted(false)
    , mHost(host)
{}


ECode DdsScheduler::NetworkRequestInfo::constructor(
    /* [in] */ INetworkRequest* req)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mRequest = req;

#endif
}

ECode DdsScheduler::NetworkRequestInfo::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return mRequest + " accepted = " + mAccepted;

#endif
}

ECode DdsScheduler::NetworkRequestInfo::GetMRequest(
    /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRequest;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DdsScheduler::NetworkRequestInfo::GetMAccepted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccepted;
    return NOERROR;
}

ECode DdsScheduler::NetworkRequestInfo::SetMAccepted(
    /* [in] */ Boolean accepted)
{
    mAccepted = accepted;
    return NOERROR;
}

//=============================================================================
// DdsScheduler::DefaultState
//=============================================================================
const String DdsScheduler::DefaultState::TAG = DdsScheduler::TAG + "[DefaultState]";

DdsScheduler::DefaultState::DefaultState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}


ECode DdsScheduler::DefaultState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode DdsScheduler::DefaultState::Exit()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode DdsScheduler::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch(msg.what) {
                    case DdsSchedulerAc.EVENT_ADD_REQUEST: {
                        NetworkRequest nr = (NetworkRequest)msg.obj;
                        Rlog.d(TAG, "EVENT_ADD_REQUEST = " + nr);
                        addRequest(nr);
                        sendMessage(obtainMessage(DdsSchedulerAc.REQ_DDS_ALLOCATION, nr));
                        break;
                    }
                    case DdsSchedulerAc.EVENT_REMOVE_REQUEST: {
                        NetworkRequest nr = (NetworkRequest)msg.obj;
                        Rlog.d(TAG, "EVENT_REMOVE_REQUEST" + nr);
                        removeRequest(nr);
                        sendMessage(obtainMessage(DdsSchedulerAc.REQ_DDS_FREE, nr));
                        break;
                    }
                    case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                        Rlog.d(TAG, "REQ_DDS_ALLOCATION, currentState = "
                                + getCurrentState().getName());
                        return HANDLED;
                    }
                    case DdsSchedulerAc.REQ_DDS_FREE: {
                        Rlog.d(TAG, "REQ_DDS_FREE, currentState = " + getCurrentState().getName());
                        return HANDLED;
                    }
                    default: {
                        Rlog.d(TAG, "unknown msg = " + msg);
                        break;
                    }
                }
                return HANDLED;

#endif
}

//=============================================================================
// DdsScheduler::DdsIdleState
//=============================================================================
const String DdsScheduler::DdsIdleState::TAG = DdsScheduler::TAG + "[DdsIdleState]";

DdsScheduler::DdsIdleState::DdsIdleState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsIdleState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Enter");
                NetworkRequest nr = getFirstWaitingRequest();
                if(nr != null) {
                    Rlog.d(TAG, "Request pending = " + nr);
                    if (!isDdsSwitchRequired(nr)) {
                        transitionTo(mDdsReservedState);
                    } else {
                        transitionTo(mDdsSwitchState);
                    }
                } else {
                    Rlog.d(TAG, "Nothing to process");
                }

#endif
}

ECode DdsScheduler::DdsIdleState::Exit()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Exit");

#endif
}

ECode DdsScheduler::DdsIdleState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch(msg.what) {
                    case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                        Rlog.d(TAG, "REQ_DDS_ALLOCATION");
                        NetworkRequest n = (NetworkRequest)msg.obj;
                        if (!isDdsSwitchRequired(n)) {
                            transitionTo(mDdsReservedState);
                        } else {
                            transitionTo(mDdsSwitchState);
                        }
                        return HANDLED;
                    }
                    default: {
                        Rlog.d(TAG, "unknown msg = " + msg);
                        return NOT_HANDLED;
                    }
                }

#endif
}

//=============================================================================
// DdsScheduler::DdsReservedState
//=============================================================================
const String DdsScheduler::DdsReservedState::TAG = DdsScheduler::TAG + "[DdsReservedState]";

DdsScheduler::DdsReservedState::DdsReservedState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsReservedState::HandleOtherSubRequests()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                NetworkRequest nr = getFirstWaitingRequest();
                if (nr == null) {
                    Rlog.d(TAG, "No more requests to accept");
                    transitionTo(mDdsAutoRevertState);
                } else if (getSubIdFromNetworkRequest(nr) != getCurrentDds()) {
                    Rlog.d(TAG, "Switch required for " + nr);
                    transitionTo(mDdsSwitchState);
                } else {
                    Rlog.e(TAG, "This request could not get accepted, start over. nr = " + nr);
                    //reset state machine to stable state.
                    transitionTo(mDdsAutoRevertState);
                }

#endif
}

ECode DdsScheduler::DdsReservedState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Enter");
                if (!acceptWaitingRequest()) {
                    handleOtherSubRequests();
                }

#endif
}

ECode DdsScheduler::DdsReservedState::Exit()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Exit");

#endif
}

ECode DdsScheduler::DdsReservedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch(msg.what) {
                    case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                        Rlog.d(TAG, "REQ_DDS_ALLOCATION");
                        NetworkRequest n = (NetworkRequest)msg.obj;
                        if (getSubIdFromNetworkRequest(n) == getCurrentDds()) {
                            Rlog.d(TAG, "Accepting simultaneous request for current sub");
                            notifyRequestAccepted(n);
                        } else if (isMultiDataSupported()) {
                            Rlog.d(TAG, "Incoming request is for on-demand subscription, n = " + n);
                            requestPsAttach(n);
                        }
                        return HANDLED;
                    }
                    case DdsSchedulerAc.REQ_DDS_FREE: {
                        Rlog.d(TAG, "REQ_DDS_FREE");
                        if(!acceptWaitingRequest()) {
                            Rlog.d(TAG, "Can't process next in this DDS");
                            handleOtherSubRequests();
                        } else {
                            Rlog.d(TAG, "Processing next in same DDS");
                        }
                        return HANDLED;
                    }
                    case DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE: {
                        AsyncResult ar = (AsyncResult) msg.obj;
                        NetworkRequest n = (NetworkRequest)ar.result;
                        if (ar.exception == null) {
                            updateCurrentDds(n);
                            transitionTo(mPsAttachReservedState);
                        } else {
                            Rlog.d(TAG, "Switch failed, ignore the req = " + n);
                            //discard the request so that we can process other pending reqeusts
                            removeRequest(n);
                        }
                        return HANDLED;
                    }
                    default: {
                        Rlog.d(TAG, "unknown msg = " + msg);
                        return NOT_HANDLED;
                    }
                }

#endif
}

//=============================================================================
// DdsScheduler::PsAttachReservedState
//=============================================================================
const String DdsScheduler::PsAttachReservedState::TAG = DdsScheduler::TAG + "[PSAttachReservedState]";

DdsScheduler::PsAttachReservedState::PsAttachReservedState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::PsAttachReservedState::HandleOtherSubRequests()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                NetworkRequest nr = getFirstWaitingRequest();
                if (nr == null) {
                    Rlog.d(TAG, "No more requests to accept");
                } else if (getSubIdFromNetworkRequest(nr) != getCurrentDds()) {
                    Rlog.d(TAG, "Next request is not for current on-demand PS sub(DSDA). nr = "
                            + nr);
                    if (isAlreadyAccepted(nr)) {
                        Rlog.d(TAG, "Next request is already accepted on other sub in DSDA mode. nr = "
                                + nr);
                        transitionTo(mDdsReservedState);
                        return;
                    }
                }
                transitionTo(mDdsAutoRevertState);

#endif
}

ECode DdsScheduler::PsAttachReservedState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Enter");
                if (!acceptWaitingRequest()) {
                    handleOtherSubRequests();
                }

#endif
}

ECode DdsScheduler::PsAttachReservedState::Exit()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Exit");
                //Request PS Detach on currentDds.
                requestPsDetach();
                //Update currentDds back to defaultDataSub.
                updateCurrentDds(null);

#endif
}

ECode DdsScheduler::PsAttachReservedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch(msg.what) {
                    case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                        Rlog.d(TAG, "REQ_DDS_ALLOCATION");
                        NetworkRequest n = (NetworkRequest)msg.obj;
                        Rlog.d(TAG, "Accepting request in dual-data mode, req = " + n);
                        notifyRequestAccepted(n);
                        return HANDLED;
                    }
                    case DdsSchedulerAc.REQ_DDS_FREE: {
                        Rlog.d(TAG, "REQ_DDS_FREE");
                        if (!acceptWaitingRequest()) {
                            //No more requests for current sub. If there are few accepted requests
                            //for defaultDds then move to DdsReservedState so that on-demand PS
                            //detach on current sub can be triggered.
                            handleOtherSubRequests();
                        }
                        return HANDLED;
                    }
                    default: {
                        Rlog.d(TAG, "unknown msg = " + msg);
                        return NOT_HANDLED;
                    }
                }

#endif
}

//=============================================================================
// DdsScheduler::DdsSwitchState
//=============================================================================
const String DdsScheduler::DdsSwitchState::TAG = DdsScheduler::TAG + "[DdsSwitchState]";

DdsScheduler::DdsSwitchState::DdsSwitchState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsSwitchState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Enter");
                NetworkRequest nr = getFirstWaitingRequest();
                if (nr != null) {
                    triggerSwitch(nr);
                } else {
                    Rlog.d(TAG, "Error");
                }

#endif
}

ECode DdsScheduler::DdsSwitchState::Exit()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Exit");

#endif
}

ECode DdsScheduler::DdsSwitchState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch(msg.what) {
                    case DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE:
                    case DdsSchedulerAc.EVENT_ON_DEMAND_DDS_SWITCH_DONE : {
                        AsyncResult ar = (AsyncResult) msg.obj;
                        NetworkRequest n = (NetworkRequest)ar.result;
                        if (ar.exception == null) {
                            updateCurrentDds(n);
                            if (msg.what == DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE) {
                                transitionTo(mPsAttachReservedState);
                            } else {
                                transitionTo(mDdsReservedState);
                            }
                        } else {
                            Rlog.d(TAG, "Switch failed, move back to idle state");
                            //discard the request so that we can process other pending reqeusts
                            removeRequest(n);
                            transitionTo(mDdsIdleState);
                        }
                        return HANDLED;
                    }
                    default: {
                        Rlog.d(TAG, "unknown msg = " + msg);
                        return NOT_HANDLED;
                    }
                }

#endif
}

//=============================================================================
// DdsScheduler::DdsAutoRevertState
//=============================================================================
const String DdsScheduler::DdsAutoRevertState::TAG = DdsScheduler::TAG + "[DdsAutoRevertState]";

DdsScheduler::DdsAutoRevertState::DdsAutoRevertState(
    /* [in] */ DdsScheduler* host)
    : mHost(host)
{}

ECode DdsScheduler::DdsAutoRevertState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Enter");
                triggerSwitch(null);

#endif
}

ECode DdsScheduler::DdsAutoRevertState::Exit()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "Exit");

#endif
}

ECode DdsScheduler::DdsAutoRevertState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch(msg.what) {
                     case DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE: {
                        Rlog.d(TAG, "SET_DDS_DONE");
                        updateCurrentDds(null);
                        transitionTo(mDdsIdleState);
                        return HANDLED;
                    }
                    default: {
                        Rlog.d(TAG, "unknown msg = " + msg);
                        return NOT_HANDLED;
                    }
                }

#endif
}

//=============================================================================
// DdsScheduler
//=============================================================================
CAR_INTERFACE_IMPL(DdsScheduler, StateMachine, IDdsScheduler)

const String DdsScheduler::TAG("DdsScheduler");

ECode DdsScheduler::CreateDdsScheduler(
    /* [out] */ IDdsScheduler** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        DdsScheduler ddsScheduler = new DdsScheduler();
        ddsScheduler.start();
        return ddsScheduler;

#endif
}

ECode DdsScheduler::GetInstance(
    /* [out] */ IDdsScheduler** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (sDdsScheduler == null) {
            sDdsScheduler = createDdsScheduler();
        }
        Rlog.d(TAG, "getInstance = " + sDdsScheduler);
        return sDdsScheduler;

#endif
}

ECode DdsScheduler::Init()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (sDdsScheduler == null) {
            sDdsScheduler = getInstance();
        }
        sDdsScheduler.registerCallbacks();
        Rlog.d(TAG, "init = " + sDdsScheduler);

#endif
}

DdsScheduler::DdsScheduler()
    : mCurrentDds(ISubscriptionManager::INVALID_SUB_ID)
    , MODEM_DATA_CAPABILITY_UNKNOWN(-1)
    , MODEM_SINGLE_DATA_CAPABLE(1)
    , MODEM_DUAL_DATA_CAPABLE(2)
    , OVERRIDE_MODEM_DUAL_DATA_CAP_PROP("persist.test.msim.config")
{
#if 0 // TODO: Translate codes below
    mDefaultState = new DefaultState(this);
    mDdsIdleState = new DdsIdleState(this);
    mDdsReservedState = new DdsReservedState(this);
    mPsAttachReservedState = new PsAttachReservedState(this);
    mDdsSwitchState = new DdsSwitchState(this);
    mDdsAutoRevertState = new DdsAutoRevertState(this);
    AutoPtr<IList> mInbox = Collections.synchronizedList(new ArrayList<NetworkRequestInfo>());

        super("DdsScheduler");
        addState(mDefaultState);
            addState(mDdsIdleState, mDefaultState);
            addState(mDdsReservedState, mDefaultState);
            addState(mDdsSwitchState, mDefaultState);
            addState(mDdsAutoRevertState, mDefaultState);
            addState(mPsAttachReservedState, mDefaultState);
        setInitialState(mDdsIdleState);

#endif
}

ECode DdsScheduler::constructor()
{
    return NOERROR;
}

ECode DdsScheduler::AddRequest(
    /* [in] */ INetworkRequest* req)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized(mInbox) {
            mInbox.add(new NetworkRequestInfo(req));
        }

#endif
}

ECode DdsScheduler::RemoveRequest(
    /* [in] */ INetworkRequest* req)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized(mInbox) {
            for(int i = 0; i < mInbox.size(); i++) {
                NetworkRequestInfo tempNrInfo = mInbox.get(i);
                if(tempNrInfo.mRequest.equals(req)) {
                    mInbox.remove(i);
                }
            }
        }

#endif
}

ECode DdsScheduler::MarkAccepted(
    /* [in] */ INetworkRequest* req)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized(mInbox) {
            for(int i = 0; i < mInbox.size(); i++) {
                NetworkRequestInfo tempNrInfo = mInbox.get(i);
                if(tempNrInfo.mRequest.equals(req)) {
                    tempNrInfo.mAccepted = true;
                }
            }
        }

#endif
}

ECode DdsScheduler::IsAlreadyAccepted(
    /* [in] */ INetworkRequest* nr,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized(mInbox) {
            for(int i = 0; i < mInbox.size(); i++) {
                NetworkRequestInfo tempNrInfo = mInbox.get(i);
                if(tempNrInfo.mRequest.equals(nr)) {
                    return (tempNrInfo.mAccepted == true);
                }
            }
        }
        return false;

#endif
}

ECode DdsScheduler::GetFirstWaitingRequest(
    /* [out] */ INetworkRequest** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized(mInbox) {
            if(mInbox.isEmpty()) {
                return null;
            } else {
                return mInbox.get(0).mRequest;
            }
        }

#endif
}

ECode DdsScheduler::AcceptWaitingRequest(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean anyAccepted = false;
        synchronized(mInbox) {
            if(!mInbox.isEmpty()) {
                for (int i =0; i < mInbox.size(); i++) {
                    NetworkRequest nr = mInbox.get(i).mRequest;
                    if (getSubIdFromNetworkRequest(nr) == getCurrentDds()) {
                        notifyRequestAccepted(nr);
                        anyAccepted = true;
                    }
                }
            } else {
                Rlog.d(TAG, "No request can be accepted for current sub");
                return false;
            }
        }
        return anyAccepted;

#endif
}

ECode DdsScheduler::NotifyRequestAccepted(
    /* [in] */ INetworkRequest* nr)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!isAlreadyAccepted(nr)) {
            markAccepted(nr);
            Rlog.d(TAG, "Accepted req = " + nr);
            SubscriptionController subController = SubscriptionController.getInstance();
            subController.notifyOnDemandDataSubIdChanged(nr);
        } else {
            Rlog.d(TAG, "Already accepted/notified req = " + nr);
        }

#endif
}

ECode DdsScheduler::IsDdsSwitchRequired(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if(getSubIdFromNetworkRequest(n) != getCurrentDds()) {
            Rlog.d(TAG, "DDS switch required for req = " + n);
            return true;
        } else {
            Rlog.d(TAG, "DDS switch not required for req = " + n);
            return false;
        }

#endif
}

ECode DdsScheduler::GetCurrentDds(
    /* [out] */ Int64* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SubscriptionController subController = SubscriptionController.getInstance();
        if(mCurrentDds == SubscriptionManager.INVALID_SUB_ID) {
            mCurrentDds = subController.getDefaultDataSubId();
        }
        Rlog.d(TAG, "mCurrentDds = " + mCurrentDds);
        return mCurrentDds;

#endif
}

ECode DdsScheduler::UpdateCurrentDds(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mCurrentDds = getSubIdFromNetworkRequest(n);
        Rlog.d(TAG, "mCurrentDds = " + mCurrentDds);

#endif
}

ECode DdsScheduler::GetSubIdFromNetworkRequest(
    /* [in] */ INetworkRequest* n,
    /* [out] */ Int64* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SubscriptionController subController = SubscriptionController.getInstance();
        return subController.getSubIdFromNetworkRequest(n);

#endif
}

ECode DdsScheduler::RequestDdsSwitch(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (n != null) {
            mDctController.setOnDemandDataSubId(n);
        } else {
            // set DDS to user configured defaultDds SUB.
            // requestPsAttach would make sure that OemHook api to set DDS
            // is called as well as PS ATTACH is requested.
            requestPsAttach(null);
        }

#endif
}

ECode DdsScheduler::RequestPsAttach(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDctController.doPsAttach(n);

#endif
}

ECode DdsScheduler::RequestPsDetach()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDctController.doPsDetach();

#endif
}

ECode DdsScheduler::GetMaxDataAllowed(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ModemStackController modemStackController = ModemStackController.getInstance();
        Rlog.d(TAG, "ModemStackController = " + modemStackController);
        int maxData = modemStackController.getMaxDataAllowed();
        Rlog.d(TAG, "modem value of max_data = " + maxData);
        int override = SystemProperties.getInt(OVERRIDE_MODEM_DUAL_DATA_CAP_PROP,
                MODEM_DATA_CAPABILITY_UNKNOWN);
        if(override != MODEM_DATA_CAPABILITY_UNKNOWN) {
            Rlog.d(TAG, "Overriding modem max_data_value with " + override);
            maxData = override;
        }
        return maxData;

#endif
}

ECode DdsScheduler::RegisterCallbacks()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if(mDctController == null) {
            Rlog.d(TAG, "registerCallbacks");
            mDctController = DctController.getInstance();
            mDctController.registerForOnDemandDataSwitchInfo(getHandler(),
                    DdsSchedulerAc.EVENT_ON_DEMAND_DDS_SWITCH_DONE, null);
            mDctController.registerForOnDemandPsAttach(getHandler(),
                    DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE, null);
       }

#endif
}

ECode DdsScheduler::TriggerSwitch(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean multiDataSupported = false;
        if (isMultiDataSupported()) {
            multiDataSupported = true;
            Rlog.d(TAG, "Simultaneous dual-data supported");
        } else {
            Rlog.d(TAG, "Simultaneous dual-data NOT supported");
        }
        if ((n != null) && multiDataSupported) {
            requestPsAttach(n);
        } else {
            requestDdsSwitch(n);
        }

#endif
}

ECode DdsScheduler::IsMultiDataSupported(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        boolean flag = false;
        if (getMaxDataAllowed() == MODEM_DUAL_DATA_CAPABLE) {
            flag = true;
        }
        return flag;

#endif
}

ECode DdsScheduler::IsAnyRequestWaiting(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized(mInbox) {
            return !mInbox.isEmpty();
        }

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
