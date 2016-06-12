
#include "elastos/droid/internal/telephony/dataconnection/DcController.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcController::DccDefaultState
//=============================================================================
ECode DcController::DccDefaultState::Enter()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mPhone.mCi.registerForRilConnected(getHandler(),
                        DataConnection.EVENT_RIL_CONNECTED, null);
                mPhone.mCi.registerForDataNetworkStateChanged(getHandler(),
                        DataConnection.EVENT_DATA_STATE_CHANGED, null);
                if (Build.IS_DEBUGGABLE) {
                    mDcTesterDeactivateAll =
                            new DcTesterDeactivateAll(mPhone, DcController.this, getHandler());
                }

#endif
}

ECode DcController::DccDefaultState::Exit()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                if (mPhone != null) {
                    mPhone.mCi.unregisterForRilConnected(getHandler());
                    mPhone.mCi.unregisterForDataNetworkStateChanged(getHandler());
                }
                if (mDcTesterDeactivateAll != null) {
                    mDcTesterDeactivateAll.dispose();
                }

#endif
}

ECode DcController::DccDefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                AsyncResult ar;
                switch (msg.what) {
                    case DataConnection.EVENT_RIL_CONNECTED:
                        ar = (AsyncResult)msg.obj;
                        if (ar.exception == null) {
                            if (DBG) {
                                log("DccDefaultState: msg.what=EVENT_RIL_CONNECTED mRilVersion=" +
                                    ar.result);
                            }
                        } else {
                            log("DccDefaultState: Unexpected exception on EVENT_RIL_CONNECTED");
                        }
                        break;
                    case DataConnection.EVENT_DATA_STATE_CHANGED:
                        ar = (AsyncResult)msg.obj;
                        if (ar.exception == null) {
                            onDataStateChanged((ArrayList<DataCallResponse>)ar.result);
                        } else {
                            log("DccDefaultState: EVENT_DATA_STATE_CHANGED:" +
                                        " exception; likely radio not available, ignore");
                        }
                        break;
                }
                return HANDLED;

#endif
}

ECode DcController::DccDefaultState::OnDataStateChanged(
    /* [in] */ IArrayList* dcsList)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                if (DBG) {
                    lr("onDataStateChanged: dcsList=" + dcsList
                            + " mDcListActiveByCid=" + mDcListActiveByCid);
                }
                if (VDBG) {
                    log("onDataStateChanged: mDcListAll=" + mDcListAll);
                }
                // Create hashmap of cid to DataCallResponse
                HashMap<Integer, DataCallResponse> dataCallResponseListByCid =
                        new HashMap<Integer, DataCallResponse>();
                for (DataCallResponse dcs : dcsList) {
                    dataCallResponseListByCid.put(dcs.cid, dcs);
                }
                // Add a DC that is active but not in the
                // dcsList to the list of DC's to retry
                ArrayList<DataConnection> dcsToRetry = new ArrayList<DataConnection>();
                for (DataConnection dc : mDcListActiveByCid.values()) {
                    if (dataCallResponseListByCid.get(dc.mCid) == null) {
                        if (DBG) log("onDataStateChanged: add to retry dc=" + dc);
                        dcsToRetry.add(dc);
                    }
                }
                if (DBG) log("onDataStateChanged: dcsToRetry=" + dcsToRetry);
                // Find which connections have changed state and send a notification or cleanup
                // and any that are in active need to be retried.
                ArrayList<ApnContext> apnsToCleanup = new ArrayList<ApnContext>();
                boolean isAnyDataCallDormant = false;
                boolean isAnyDataCallActive = false;
                for (DataCallResponse newState : dcsList) {
                    DataConnection dc = mDcListActiveByCid.get(newState.cid);
                    if (dc == null) {
                        // UNSOL_DATA_CALL_LIST_CHANGED arrived before SETUP_DATA_CALL completed.
                        loge("onDataStateChanged: no associated DC yet, ignore");
                        continue;
                    }
                    if (dc.mApnContexts.size() == 0) {
                        if (DBG) loge("onDataStateChanged: no connected apns, ignore");
                    } else {
                        // Determine if the connection/apnContext should be cleaned up
                        // or just a notification should be sent out.
                        if (DBG) log("onDataStateChanged: Found ConnId=" + newState.cid
                                + " newState=" + newState.toString());
                        if (newState.active == DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE) {
                            if (mDct.mIsCleanupRequired) {
                                apnsToCleanup.addAll(dc.mApnContexts);
                                mDct.mIsCleanupRequired = false;
                            } else {
                                DcFailCause failCause = DcFailCause.fromInt(newState.status);
                                if (DBG) log("onDataStateChanged: inactive failCause=" + failCause);
                                if (failCause.isRestartRadioFail()) {
                                    if (DBG) log("onDataStateChanged: X restart radio");
                                    mDct.sendRestartRadio();
                                } else if (mDct.isPermanentFail(failCause)) {
                                    if (DBG) log("onDataStateChanged: inactive, add to cleanup list");
                                    apnsToCleanup.addAll(dc.mApnContexts);
                                } else {
                                    if (DBG) log("onDataStateChanged: inactive, add to retry list");
                                    dcsToRetry.add(dc);
                                }
                            }
                        } else {
                            // Its active so update the DataConnections link properties
                            UpdateLinkPropertyResult result = dc.updateLinkProperty(newState);
                            if (result.oldLp.equals(result.newLp)) {
                                if (DBG) log("onDataStateChanged: no change");
                            } else {
                                if (result.oldLp.isIdenticalInterfaceName(result.newLp)) {
                                    if (! result.oldLp.isIdenticalDnses(result.newLp) ||
                                            ! result.oldLp.isIdenticalRoutes(result.newLp) ||
                                            ! result.oldLp.isIdenticalHttpProxy(result.newLp) ||
                                            ! result.oldLp.isIdenticalAddresses(result.newLp)) {
                                        // If the same address type was removed and
                                        // added we need to cleanup
                                        CompareResult<LinkAddress> car =
                                            result.oldLp.compareAddresses(result.newLp);
                                        if (DBG) {
                                            log("onDataStateChanged: oldLp=" + result.oldLp +
                                                    " newLp=" + result.newLp + " car=" + car);
                                        }
                                        boolean needToClean = false;
                                        for (LinkAddress added : car.added) {
                                            for (LinkAddress removed : car.removed) {
                                                if (NetworkUtils.addressTypeMatches(
                                                        removed.getAddress(),
                                                        added.getAddress())) {
                                                    needToClean = true;
                                                    break;
                                                }
                                            }
                                        }
                                        if (needToClean) {
                                            if (DBG) {
                                                log("onDataStateChanged: addr change," +
                                                        " cleanup apns=" + dc.mApnContexts +
                                                        " oldLp=" + result.oldLp +
                                                        " newLp=" + result.newLp);
                                            }
                                            apnsToCleanup.addAll(dc.mApnContexts);
                                        } else {
                                            if (DBG) log("onDataStateChanged: simple change");
                                            for (ApnContext apnContext : dc.mApnContexts) {
                                                 mPhone.notifyDataConnection(
                                                     PhoneConstants.REASON_LINK_PROPERTIES_CHANGED,
                                                     apnContext.getApnType());
                                            }
                                        }
                                    } else {
                                        if (DBG) {
                                            log("onDataStateChanged: no changes");
                                        }
                                    }
                                } else {
                                    apnsToCleanup.addAll(dc.mApnContexts);
                                    if (DBG) {
                                        log("onDataStateChanged: interface change, cleanup apns="
                                                + dc.mApnContexts);
                                    }
                                }
                            }
                        }
                    }
                    if (newState.active == DATA_CONNECTION_ACTIVE_PH_LINK_UP) {
                        isAnyDataCallActive = true;
                    }
                    if (newState.active == DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT) {
                        isAnyDataCallDormant = true;
                    }
                }
                int newOverallDataConnectionActiveState = mOverallDataConnectionActiveState;
                if (isAnyDataCallDormant && !isAnyDataCallActive) {
                    // There is no way to indicate link activity per APN right now. So
                    // Link Activity will be considered dormant only when all data calls
                    // are dormant.
                    // If a single data call is in dormant state and none of the data
                    // calls are active broadcast overall link state as dormant.
                    if (DBG) {
                        log("onDataStateChanged: Data Activity updated to DORMANT. stopNetStatePoll");
                    }
                    mDct.sendStopNetStatPoll(DctConstants.Activity.DORMANT);
                    newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT;
                } else {
                    if (DBG) {
                        log("onDataStateChanged: Data Activity updated to NONE. " +
                                "isAnyDataCallActive = " + isAnyDataCallActive +
                                " isAnyDataCallDormant = " + isAnyDataCallDormant);
                    }
                    if (isAnyDataCallActive) {
                        newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_UP;
                        mDct.sendStartNetStatPoll(DctConstants.Activity.NONE);
                    } else {
                        newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE;
                    }
                }
                // Temporary notification until RIL implementation is complete.
                if (mOverallDataConnectionActiveState != newOverallDataConnectionActiveState) {
                    mOverallDataConnectionActiveState = newOverallDataConnectionActiveState;
                    long time = SystemClock.elapsedRealtimeNanos();
                    int dcPowerState;
                    switch (mOverallDataConnectionActiveState) {
                        case DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE:
                        case DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT:
                            dcPowerState = DataConnectionRealTimeInfo.DC_POWER_STATE_LOW;
                            break;
                        case DATA_CONNECTION_ACTIVE_PH_LINK_UP:
                            dcPowerState = DataConnectionRealTimeInfo.DC_POWER_STATE_HIGH;
                            break;
                        default:
                            dcPowerState = DataConnectionRealTimeInfo.DC_POWER_STATE_UNKNOWN;
                            break;
                    }
                    DataConnectionRealTimeInfo dcRtInfo =
                            new DataConnectionRealTimeInfo(time , dcPowerState);
                    log("onDataStateChanged: notify DcRtInfo changed dcRtInfo=" + dcRtInfo);
                    mPhone.notifyDataConnectionRealTimeInfo(dcRtInfo);
                }
                if (DBG) {
                    lr("onDataStateChanged: dcsToRetry=" + dcsToRetry
                            + " apnsToCleanup=" + apnsToCleanup);
                }
                // Cleanup connections that have changed
                for (ApnContext apnContext : apnsToCleanup) {
                   mDct.sendCleanUpConnection(true, apnContext);
                }
                // Retry connections that have disappeared
                for (DataConnection dc : dcsToRetry) {
                    if (DBG) log("onDataStateChanged: send EVENT_LOST_CONNECTION dc.mTag=" + dc.mTag);
                    dc.sendMessage(DataConnection.EVENT_LOST_CONNECTION, dc.mTag);
                }
                if (DBG) log("onDataStateChanged: X");

#endif
}

//=============================================================================
// DcController
//=============================================================================
const Boolean DcController::DBG = true;
const Boolean DcController::VDBG = false;
const Int32 DcController::DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0;
const Int32 DcController::DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT = 1;
const Int32 DcController::DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2;
const Int32 DcController::DATA_CONNECTION_ACTIVE_UNKNOWN = Elastos::Core::Math::INT32_MAX_VALUE;

DcController::DcController(
    /* [in] */ const String& name,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IDcTrackerBase* dct,
    /* [in] */ IHandler* handler)
    : mOverallDataConnectionActiveState(DATA_CONNECTION_ACTIVE_UNKNOWN)
{
#if 0 // TODO: Translate codes below
        super(name, handler);
        setLogRecSize(300);
        log("E ctor");
        mPhone = phone;
        mDct = dct;
        addState(mDccDefaultState);
        setInitialState(mDccDefaultState);
        log("X ctor");
        CArrayList::New((IArrayList**)&mDcListAll);
        CHashMap::New((IHashMap**)&mDcListActiveByCid);
        CDccDefaultState::New((IDccDefaultState**)&mDccDefaultState);
#endif
}

ECode DcController::MakeDcc(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IDcTrackerBase* dct,
    /* [in] */ IHandler* handler,
    /* [out] */ IDcController** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        DcController dcc = new DcController("Dcc", phone, dct, handler);
        dcc.start();
        return dcc;

#endif
}

ECode DcController::Dispose()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("dispose: call quiteNow()");
        quitNow();

#endif
}

ECode DcController::AddDc(
    /* [in] */ IDataConnection* dc)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDcListAll.add(dc);

#endif
}

ECode DcController::RemoveDc(
    /* [in] */ IDataConnection* dc)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mDcListActiveByCid.remove(dc.mCid);
        mDcListAll.remove(dc);

#endif
}

ECode DcController::AddActiveDcByCid(
    /* [in] */ IDataConnection* dc)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG && dc.mCid < 0) {
            log("addActiveDcByCid dc.mCid < 0 dc=" + dc);
        }
        mDcListActiveByCid.put(dc.mCid, dc);

#endif
}

ECode DcController::RemoveActiveDcByCid(
    /* [in] */ IDataConnection* dc)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        DataConnection removedDc = mDcListActiveByCid.remove(dc.mCid);
        if (DBG && removedDc == null) {
            log("removeActiveDcByCid removedDc=null dc=" + dc);
        }

#endif
}

ECode DcController::Lr(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        logAndAddLogRec(s);

#endif
}

ECode DcController::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(getName(), s);

#endif
}

ECode DcController::Loge(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.e(getName(), s);

#endif
}

ECode DcController::GetWhatToString(
    /* [in] */ Int32 what,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String info = null;
        info = DataConnection.cmdToString(what);
        if (info == null) {
            info = DcAsyncChannel.cmdToString(what);
        }
        return info;

#endif
}

ECode DcController::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return "mDcListAll=" + mDcListAll + " mDcListActiveByCid=" + mDcListActiveByCid;

#endif
}

ECode DcController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super.dump(fd, pw, args);
        pw.println(" mPhone=" + mPhone);
        pw.println(" mDcListAll=" + mDcListAll);
        pw.println(" mDcListActiveByCid=" + mDcListActiveByCid);

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
