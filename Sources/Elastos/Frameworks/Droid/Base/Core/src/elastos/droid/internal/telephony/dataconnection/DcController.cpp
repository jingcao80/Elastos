/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::LinkProperties::ICompareResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Dataconnection::DataConnection::IUpdateLinkPropertyResult;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

/**
 * Data Connection Controller which is a package visible class and controls
 * multiple data connections. For instance listening for unsolicited messages
 * and then demultiplexing them to the appropriate DC.
 */
class DcController extends StateMachine {
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE;

    private PhoneBase mPhone;
    private DcTrackerBase mDct;
    private DcTesterDeactivateAll mDcTesterDeactivateAll;

    // package as its used by Testing code
    ArrayList<DataConnection> mDcListAll = new ArrayList<DataConnection>();
    private HashMap<Integer, DataConnection> mDcListActiveByCid =
            new HashMap<Integer, DataConnection>();

    /**
     * Constants for the data connection activity:
     * physical link down/up
     *
     * TODO: Move to RILConstants.java
     */
    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0;
    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT = 1;
    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2;
    static const Int32 DATA_CONNECTION_ACTIVE_UNKNOWN = Integer.MAX_VALUE;

    // One of the DATA_CONNECTION_ACTIVE_XXX values
    Int32 mOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_UNKNOWN;

    private DccDefaultState mDccDefaultState = new DccDefaultState();

    /**
     * Constructor.
     *
     * @param name to be used for the Controller
     * @param phone the phone associated with Dcc and Dct
     * @param dct the DataConnectionTracker associated with Dcc
     * @param handler defines the thread/looper to be used with Dcc
     */
    private DcController(String name, PhoneBase phone, DcTrackerBase dct,
            Handler handler) {
        Super(name, handler);
        SetLogRecSize(300);
        Log("E ctor");
        mPhone = phone;
        mDct = dct;
        AddState(mDccDefaultState);
        SetInitialState(mDccDefaultState);
        Log("X ctor");
    }

    static DcController MakeDcc(PhoneBase phone, DcTrackerBase dct, Handler handler) {
        DcController dcc = new DcController("Dcc", phone, dct, handler);
        dcc->Start();
        return dcc;
    }

    void Dispose() {
        Log("dispose: call QuiteNow()");
        QuitNow();
    }

    void AddDc(DataConnection dc) {
        mDcListAll->Add(dc);
    }

    void RemoveDc(DataConnection dc) {
        mDcListActiveByCid->Remove(dc.mCid);
        mDcListAll->Remove(dc);
    }

    void AddActiveDcByCid(DataConnection dc) {
        If (DBG && dc.mCid < 0) {
            Log("addActiveDcByCid dc.mCid < 0 dc=" + dc);
        }
        mDcListActiveByCid->Put(dc.mCid, dc);
    }

    void RemoveActiveDcByCid(DataConnection dc) {
        DataConnection removedDc = mDcListActiveByCid->Remove(dc.mCid);
        If (DBG && removedDc == NULL) {
            Log("removeActiveDcByCid removedDc=NULL dc=" + dc);
        }
    }

    private class DccDefaultState extends State {
        //@Override
        CARAPI Enter() {
            mPhone.mCi->RegisterForRilConnected(GetHandler(),
                    DataConnection.EVENT_RIL_CONNECTED, NULL);
            mPhone.mCi->RegisterForDataNetworkStateChanged(GetHandler(),
                    DataConnection.EVENT_DATA_STATE_CHANGED, NULL);
            If (Build.IS_DEBUGGABLE) {
                mDcTesterDeactivateAll =
                        new DcTesterDeactivateAll(mPhone, DcController.this, GetHandler());
            }
        }

        //@Override
        CARAPI Exit() {
            If (mPhone != NULL) {
                mPhone.mCi->UnregisterForRilConnected(GetHandler());
                mPhone.mCi->UnregisterForDataNetworkStateChanged(GetHandler());
            }
            If (mDcTesterDeactivateAll != NULL) {
                mDcTesterDeactivateAll->Dispose();
            }
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            AsyncResult ar;

            Switch (msg.what) {
                case DataConnection.EVENT_RIL_CONNECTED:
                    ar = (AsyncResult)msg.obj;
                    If (ar.exception == NULL) {
                        If (DBG) {
                            Log("DccDefaultState: msg.what=EVENT_RIL_CONNECTED mRilVersion=" +
                                ar.result);
                        }
                    } else {
                        Log("DccDefaultState: Unexpected exception on EVENT_RIL_CONNECTED");
                    }
                    break;

                case DataConnection.EVENT_DATA_STATE_CHANGED:
                    ar = (AsyncResult)msg.obj;
                    If (ar.exception == NULL) {
                        OnDataStateChanged((ArrayList<DataCallResponse>)ar.result);
                    } else {
                        Log("DccDefaultState: EVENT_DATA_STATE_CHANGED:" +
                                    " exception; likely radio not available, ignore");
                    }
                    break;
            }
            return HANDLED;
        }

        /**
         * Process the new list of "known" Data Calls
         * @param dcsList as sent by RIL_UNSOL_DATA_CALL_LIST_CHANGED
         */
        private void OnDataStateChanged(ArrayList<DataCallResponse> dcsList) {
            If (DBG) {
                Lr("onDataStateChanged: dcsList=" + dcsList
                        + " mDcListActiveByCid=" + mDcListActiveByCid);
            }
            If (VDBG) {
                Log("onDataStateChanged: mDcListAll=" + mDcListAll);
            }

            // Create hashmap of cid to DataCallResponse
            HashMap<Integer, DataCallResponse> dataCallResponseListByCid =
                    new HashMap<Integer, DataCallResponse>();
            For (DataCallResponse dcs : dcsList) {
                dataCallResponseListByCid->Put(dcs.cid, dcs);
            }

            // Add a DC that is active but not in the
            // dcsList to the list of DC's to retry
            ArrayList<DataConnection> dcsToRetry = new ArrayList<DataConnection>();
            For (DataConnection dc : mDcListActiveByCid->Values()) {
                If (dataCallResponseListByCid->Get(dc.mCid) == NULL) {
                    If (DBG) Log("onDataStateChanged: add to retry dc=" + dc);
                    dcsToRetry->Add(dc);
                }
            }
            If (DBG) Log("onDataStateChanged: dcsToRetry=" + dcsToRetry);

            // Find which connections have changed state and send a notification or cleanup
            // and any that are in active need to be retried.
            ArrayList<ApnContext> apnsToCleanup = new ArrayList<ApnContext>();

            Boolean isAnyDataCallDormant = FALSE;
            Boolean isAnyDataCallActive = FALSE;

            For (DataCallResponse newState : dcsList) {

                DataConnection dc = mDcListActiveByCid->Get(newState.cid);
                If (dc == NULL) {
                    // UNSOL_DATA_CALL_LIST_CHANGED arrived before SETUP_DATA_CALL completed.
                    Loge("onDataStateChanged: no associated DC yet, ignore");
                    continue;
                }

                If (dc.mApnContexts->Size() == 0) {
                    If (DBG) Loge("onDataStateChanged: no connected apns, ignore");
                } else {
                    // Determine if the connection/apnContext should be cleaned up
                    // or just a notification should be sent out.
                    If (DBG) Log("onDataStateChanged: Found ConnId=" + newState.cid
                            + " newState=" + newState->ToString());
                    If (newState.active == DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE) {
                        If (mDct.mIsCleanupRequired) {
                            apnsToCleanup->AddAll(dc.mApnContexts);
                            mDct.mIsCleanupRequired = FALSE;
                        } else {
                            DcFailCause failCause = DcFailCause->FromInt(newState.status);
                            If (DBG) Log("onDataStateChanged: inactive failCause=" + failCause);
                            If (failCause->IsRestartRadioFail()) {
                                If (DBG) Log("onDataStateChanged: X restart radio");
                                mDct->SendRestartRadio();
                            } else If (mDct->IsPermanentFail(failCause)) {
                                If (DBG) Log("onDataStateChanged: inactive, add to cleanup list");
                                apnsToCleanup->AddAll(dc.mApnContexts);
                            } else {
                                If (DBG) Log("onDataStateChanged: inactive, add to retry list");
                                dcsToRetry->Add(dc);
                            }
                        }
                    } else {
                        // Its active so update the DataConnections link properties
                        UpdateLinkPropertyResult result = dc->UpdateLinkProperty(newState);
                        If (result.oldLp->Equals(result.newLp)) {
                            If (DBG) Log("onDataStateChanged: no change");
                        } else {
                            If (result.oldLp->IsIdenticalInterfaceName(result.newLp)) {
                                If (! result.oldLp->IsIdenticalDnses(result.newLp) ||
                                        ! result.oldLp->IsIdenticalRoutes(result.newLp) ||
                                        ! result.oldLp->IsIdenticalHttpProxy(result.newLp) ||
                                        ! result.oldLp->IsIdenticalAddresses(result.newLp)) {
                                    // If the same address type was removed and
                                    // added we need to cleanup
                                    CompareResult<LinkAddress> car =
                                        result.oldLp->CompareAddresses(result.newLp);
                                    If (DBG) {
                                        Log("onDataStateChanged: oldLp=" + result.oldLp +
                                                " newLp=" + result.newLp + " car=" + car);
                                    }
                                    Boolean needToClean = FALSE;
                                    For (LinkAddress added : car.added) {
                                        For (LinkAddress removed : car.removed) {
                                            If (NetworkUtils->AddressTypeMatches(
                                                    removed->GetAddress(),
                                                    added->GetAddress())) {
                                                needToClean = TRUE;
                                                break;
                                            }
                                        }
                                    }
                                    If (needToClean) {
                                        If (DBG) {
                                            Log("onDataStateChanged: addr change," +
                                                    " cleanup apns=" + dc.mApnContexts +
                                                    " oldLp=" + result.oldLp +
                                                    " newLp=" + result.newLp);
                                        }
                                        apnsToCleanup->AddAll(dc.mApnContexts);
                                    } else {
                                        If (DBG) Log("onDataStateChanged: simple change");

                                        For (ApnContext apnContext : dc.mApnContexts) {
                                             mPhone->NotifyDataConnection(
                                                 PhoneConstants.REASON_LINK_PROPERTIES_CHANGED,
                                                 apnContext->GetApnType());
                                        }
                                    }
                                } else {
                                    If (DBG) {
                                        Log("onDataStateChanged: no changes");
                                    }
                                }
                            } else {
                                apnsToCleanup->AddAll(dc.mApnContexts);
                                If (DBG) {
                                    Log("onDataStateChanged: interface change, cleanup apns="
                                            + dc.mApnContexts);
                                }
                            }
                        }
                    }
                }

                If (newState.active == DATA_CONNECTION_ACTIVE_PH_LINK_UP) {
                    isAnyDataCallActive = TRUE;
                }
                If (newState.active == DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT) {
                    isAnyDataCallDormant = TRUE;
                }
            }

            Int32 newOverallDataConnectionActiveState = mOverallDataConnectionActiveState;

            If (isAnyDataCallDormant && !isAnyDataCallActive) {
                // There is no way to indicate link activity per APN right now. So
                // Link Activity will be considered dormant only when all data calls
                // are dormant.
                // If a single data call is in dormant state and none of the data
                // calls are active broadcast overall link state as dormant.
                If (DBG) {
                    Log("onDataStateChanged: Data Activity updated to DORMANT. stopNetStatePoll");
                }
                mDct->SendStopNetStatPoll(DctConstants.Activity.DORMANT);
                newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT;
            } else {
                If (DBG) {
                    Log("onDataStateChanged: Data Activity updated to NONE. " +
                            "isAnyDataCallActive = " + isAnyDataCallActive +
                            " isAnyDataCallDormant = " + isAnyDataCallDormant);
                }
                If (isAnyDataCallActive) {
                    newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_UP;
                    mDct->SendStartNetStatPoll(DctConstants.Activity.NONE);
                } else {
                    newOverallDataConnectionActiveState = DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE;
                }
            }

            // Temporary notification until RIL implementation is complete.
            If (mOverallDataConnectionActiveState != newOverallDataConnectionActiveState) {
                mOverallDataConnectionActiveState = newOverallDataConnectionActiveState;
                Int64 time = SystemClock->ElapsedRealtimeNanos();
                Int32 dcPowerState;
                Switch (mOverallDataConnectionActiveState) {
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
                Log("onDataStateChanged: notify DcRtInfo changed dcRtInfo=" + dcRtInfo);
                mPhone->NotifyDataConnectionRealTimeInfo(dcRtInfo);
            }

            If (DBG) {
                Lr("onDataStateChanged: dcsToRetry=" + dcsToRetry
                        + " apnsToCleanup=" + apnsToCleanup);
            }

            // Cleanup connections that have changed
            For (ApnContext apnContext : apnsToCleanup) {
               mDct->SendCleanUpConnection(TRUE, apnContext);
            }

            // Retry connections that have disappeared
            For (DataConnection dc : dcsToRetry) {
                If (DBG) Log("onDataStateChanged: send EVENT_LOST_CONNECTION dc.mTag=" + dc.mTag);
                dc->SendMessage(DataConnection.EVENT_LOST_CONNECTION, dc.mTag);
            }

            If (DBG) Log("onDataStateChanged: X");
        }
    }

    /**
     * lr is short name for logAndAddLogRec
     * @param s
     */
    private void Lr(String s) {
        LogAndAddLogRec(s);
    }

    //@Override
    protected void Log(String s) {
        Rlog->D(GetName(), s);
    }

    //@Override
    protected void Loge(String s) {
        Rlog->E(GetName(), s);
    }

    /**
     * @return the string for msg.what as our info.
     */
    //@Override
    protected String GetWhatToString(Int32 what) {
        String info = NULL;
        info = DataConnection->CmdToString(what);
        If (info == NULL) {
            info = DcAsyncChannel->CmdToString(what);
        }
        return info;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "mDcListAll=" + mDcListAll + " mDcListActiveByCid=" + mDcListActiveByCid;
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        super->Dump(fd, pw, args);
        pw->Println(" mPhone=" + mPhone);
        pw->Println(" mDcListAll=" + mDcListAll);
        pw->Println(" mDcListActiveByCid=" + mDcListActiveByCid);
    }
}
