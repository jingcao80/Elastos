/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.android.internal.telephony.dataconnection;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISubscriptionManager;

using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDdsSchedulerAc;
using Elastos::Droid::Internal::Telephony::IModemStackController;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;

using Elastos::Utility::IHashMap;
using Elastos::Utility::ICollections;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/* {@hide} */
public class DdsScheduler extends StateMachine {
    static const String TAG = "DdsScheduler";

    private DefaultState mDefaultState = new DefaultState();
    private DdsIdleState mDdsIdleState = new DdsIdleState();
    private DdsReservedState mDdsReservedState = new DdsReservedState();
    private PsAttachReservedState mPsAttachReservedState = new PsAttachReservedState();
    private DdsSwitchState mDdsSwitchState = new DdsSwitchState();
    private DdsAutoRevertState mDdsAutoRevertState = new DdsAutoRevertState();

    private Int64 mCurrentDds = SubscriptionManager.INVALID_SUB_ID;
    private DctController mDctController;
    private static DdsScheduler sDdsScheduler;

    private final Int32 MODEM_DATA_CAPABILITY_UNKNOWN = -1;
    private final Int32 MODEM_SINGLE_DATA_CAPABLE = 1;
    private final Int32 MODEM_DUAL_DATA_CAPABLE = 2;

    private final String OVERRIDE_MODEM_DUAL_DATA_CAP_PROP = "persist.test.msim.config";

    private class NetworkRequestInfo {
        public final NetworkRequest mRequest;
        public Boolean mAccepted = FALSE;

        NetworkRequestInfo(NetworkRequest req) {
            mRequest = req;
        }

        CARAPI ToString(
        /* [out] */ String* str)
    {
            return mRequest + " accepted = " + mAccepted;
        }
    }
    private List<NetworkRequestInfo> mInbox = Collections->SynchronizedList(
            new ArrayList<NetworkRequestInfo>());


    private static DdsScheduler CreateDdsScheduler() {
        DdsScheduler ddsScheduler = new DdsScheduler();
        ddsScheduler->Start();

        return ddsScheduler;
    }


    public static DdsScheduler GetInstance() {
        If (sDdsScheduler == NULL) {
            sDdsScheduler = CreateDdsScheduler();
        }

        Rlog->D(TAG, "getInstance = " + sDdsScheduler);
        return sDdsScheduler;
    }

    public static void Init() {
        If (sDdsScheduler == NULL) {
            sDdsScheduler = GetInstance();
        }
        sDdsScheduler->RegisterCallbacks();
        Rlog->D(TAG, "init = " + sDdsScheduler);
    }

    private DdsScheduler() {
        Super("DdsScheduler");

        AddState(mDefaultState);
            AddState(mDdsIdleState, mDefaultState);
            AddState(mDdsReservedState, mDefaultState);
            AddState(mDdsSwitchState, mDefaultState);
            AddState(mDdsAutoRevertState, mDefaultState);
            AddState(mPsAttachReservedState, mDefaultState);
        SetInitialState(mDdsIdleState);
    }

    void AddRequest(NetworkRequest req) {
        {    AutoLock syncLock(mInbox);
            mInbox->Add(new NetworkRequestInfo(req));
        }
    }

    void RemoveRequest(NetworkRequest req) {
        {    AutoLock syncLock(mInbox);
            For(Int32 i = 0; i < mInbox->Size(); i++) {
                NetworkRequestInfo tempNrInfo = mInbox->Get(i);
                If(tempNrInfo.mRequest->Equals(req)) {
                    mInbox->Remove(i);
                }
            }
        }
    }

    void MarkAccepted(NetworkRequest req) {
        {    AutoLock syncLock(mInbox);
            For(Int32 i = 0; i < mInbox->Size(); i++) {
                NetworkRequestInfo tempNrInfo = mInbox->Get(i);
                If(tempNrInfo.mRequest->Equals(req)) {
                    tempNrInfo.mAccepted = TRUE;
                }
            }
        }
    }

    Boolean IsAlreadyAccepted(NetworkRequest nr) {
        {    AutoLock syncLock(mInbox);
            For(Int32 i = 0; i < mInbox->Size(); i++) {
                NetworkRequestInfo tempNrInfo = mInbox->Get(i);
                If(tempNrInfo.mRequest->Equals(nr)) {
                    Return (tempNrInfo.mAccepted == TRUE);
                }
            }
        }
        return FALSE;
    }

    NetworkRequest GetFirstWaitingRequest() {
        {    AutoLock syncLock(mInbox);
            If(mInbox->IsEmpty()) {
                return NULL;
            } else {
                return mInbox->Get(0).mRequest;
            }
        }
    }

    Boolean AcceptWaitingRequest() {
        Boolean anyAccepted = FALSE;
        {    AutoLock syncLock(mInbox);
            If(!mInbox->IsEmpty()) {
                For (Int32 i =0; i < mInbox->Size(); i++) {
                    NetworkRequest nr = mInbox->Get(i).mRequest;
                    If (GetSubIdFromNetworkRequest(nr) == GetCurrentDds()) {
                        NotifyRequestAccepted(nr);
                        anyAccepted = TRUE;
                    }
                }
            } else {
                Rlog->D(TAG, "No request can be accepted for current sub");
                return FALSE;
            }
        }
        return anyAccepted;
    }



    void NotifyRequestAccepted(NetworkRequest nr) {
        If (!IsAlreadyAccepted(nr)) {
            MarkAccepted(nr);
            Rlog->D(TAG, "Accepted req = " + nr);

            SubscriptionController subController = SubscriptionController->GetInstance();
            subController->NotifyOnDemandDataSubIdChanged(nr);
        } else {
            Rlog->D(TAG, "Already accepted/notified req = " + nr);
        }
    }

    Boolean IsDdsSwitchRequired(NetworkRequest n) {
        If(GetSubIdFromNetworkRequest(n) != GetCurrentDds()) {
            Rlog->D(TAG, "DDS switch required for req = " + n);
            return TRUE;
        } else {
            Rlog->D(TAG, "DDS switch not required for req = " + n);
            return FALSE;
        }
    }

    public Int64 GetCurrentDds() {
        SubscriptionController subController = SubscriptionController->GetInstance();
        If(mCurrentDds == SubscriptionManager.INVALID_SUB_ID) {
            mCurrentDds = subController->GetDefaultDataSubId();
        }

        Rlog->D(TAG, "mCurrentDds = " + mCurrentDds);
        return mCurrentDds;
    }

    CARAPI UpdateCurrentDds(NetworkRequest n) {
        mCurrentDds = GetSubIdFromNetworkRequest(n);
        Rlog->D(TAG, "mCurrentDds = " + mCurrentDds);
    }

    Int64 GetSubIdFromNetworkRequest(NetworkRequest n) {
        SubscriptionController subController = SubscriptionController->GetInstance();
        return subController->GetSubIdFromNetworkRequest(n);
    }

    private void RequestDdsSwitch(NetworkRequest n) {
        If (n != NULL) {
            mDctController->SetOnDemandDataSubId(n);
        } else {
            // set DDS to user configured defaultDds SUB.
            // requestPsAttach would make sure that OemHook api to set DDS
            // is called as well as PS ATTACH is requested.
            RequestPsAttach(NULL);
        }
    }

    private void RequestPsAttach(NetworkRequest n) {
        mDctController->DoPsAttach(n);
    }

    private void RequestPsDetach() {
        mDctController->DoPsDetach();
    }

    private Int32 GetMaxDataAllowed() {
        ModemStackController modemStackController = ModemStackController->GetInstance();
        Rlog->D(TAG, "ModemStackController = " + modemStackController);

        Int32 maxData = modemStackController->GetMaxDataAllowed();
        Rlog->D(TAG, "modem value of max_data = " + maxData);

        Int32 override = SystemProperties->GetInt(OVERRIDE_MODEM_DUAL_DATA_CAP_PROP,
                MODEM_DATA_CAPABILITY_UNKNOWN);
        If(override != MODEM_DATA_CAPABILITY_UNKNOWN) {
            Rlog->D(TAG, "Overriding modem max_data_value with " + override);
            maxData = override;
        }
        return maxData;
    }

    private void RegisterCallbacks() {
        If(mDctController == NULL) {
            Rlog->D(TAG, "registerCallbacks");
            mDctController = DctController->GetInstance();
            mDctController->RegisterForOnDemandDataSwitchInfo(GetHandler(),
                    DdsSchedulerAc.EVENT_ON_DEMAND_DDS_SWITCH_DONE, NULL);
            mDctController->RegisterForOnDemandPsAttach(GetHandler(),
                    DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE, NULL);
       }
    }

    void TriggerSwitch(NetworkRequest n) {
        Boolean multiDataSupported = FALSE;

        If (IsMultiDataSupported()) {
            multiDataSupported = TRUE;
            Rlog->D(TAG, "Simultaneous dual-data supported");
        } else {
            Rlog->D(TAG, "Simultaneous dual-data NOT supported");
        }

        If ((n != NULL) && multiDataSupported) {
            RequestPsAttach(n);
        } else {
            RequestDdsSwitch(n);
        }
    }

    Boolean IsMultiDataSupported() {
        Boolean flag = FALSE;
        If (GetMaxDataAllowed() == MODEM_DUAL_DATA_CAPABLE) {
            flag = TRUE;
        }
        return flag;
    }

    Boolean IsAnyRequestWaiting() {
        {    AutoLock syncLock(mInbox);
            return !mInbox->IsEmpty();
        }
    }

    private class DefaultState extends State {
        static const String TAG = DdsScheduler.TAG + "[DefaultState]";

        //@Override
        CARAPI Enter() {
        }

        //@Override
        CARAPI Exit() {
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch(msg.what) {
                case DdsSchedulerAc.EVENT_ADD_REQUEST: {
                    NetworkRequest nr = (NetworkRequest)msg.obj;
                    Rlog->D(TAG, "EVENT_ADD_REQUEST = " + nr);
                    AddRequest(nr);
                    SendMessage(ObtainMessage(DdsSchedulerAc.REQ_DDS_ALLOCATION, nr));
                    break;
                }

                case DdsSchedulerAc.EVENT_REMOVE_REQUEST: {
                    NetworkRequest nr = (NetworkRequest)msg.obj;
                    Rlog->D(TAG, "EVENT_REMOVE_REQUEST" + nr);
                    RemoveRequest(nr);
                    SendMessage(ObtainMessage(DdsSchedulerAc.REQ_DDS_FREE, nr));
                    break;
                }

                case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                    Rlog->D(TAG, "REQ_DDS_ALLOCATION, currentState = "
                            + GetCurrentState()->GetName());
                    return HANDLED;
                }

                case DdsSchedulerAc.REQ_DDS_FREE: {
                    Rlog->D(TAG, "REQ_DDS_FREE, currentState = " + GetCurrentState()->GetName());
                    return HANDLED;
                }

                default: {
                    Rlog->D(TAG, "unknown msg = " + msg);
                    break;
                }
            }
            return HANDLED;
        }
    }

    /*
     * DdsIdleState: System is idle, none of the subscription is reserved.
     *
     * If a new request arrives do following actions.
     *  1. If the request is for currentDds, move to DdsReservedState.
     *  2. If the request is for other sub move to DdsSwitchState.
     */
    private class DdsIdleState extends State {
        static const String TAG = DdsScheduler.TAG + "[DdsIdleState]";
        //@Override
        CARAPI Enter() {
            Rlog->D(TAG, "Enter");
            NetworkRequest nr = GetFirstWaitingRequest();

            If(nr != NULL) {
                Rlog->D(TAG, "Request pending = " + nr);

                If (!IsDdsSwitchRequired(nr)) {
                    TransitionTo(mDdsReservedState);
                } else {
                    TransitionTo(mDdsSwitchState);
                }
            } else {
                Rlog->D(TAG, "Nothing to process");
            }

        }

        //@Override
        CARAPI Exit() {
            Rlog->D(TAG, "Exit");
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch(msg.what) {
                case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                    Rlog->D(TAG, "REQ_DDS_ALLOCATION");
                    NetworkRequest n = (NetworkRequest)msg.obj;

                    If (!IsDdsSwitchRequired(n)) {
                        TransitionTo(mDdsReservedState);
                    } else {
                        TransitionTo(mDdsSwitchState);
                    }
                    return HANDLED;
                }

                default: {
                    Rlog->D(TAG, "unknown msg = " + msg);
                    return NOT_HANDLED;
                }
            }
        }
    }

    private class DdsReservedState extends State {
        static const String TAG = DdsScheduler.TAG + "[DdsReservedState]";

        private void HandleOtherSubRequests() {
            NetworkRequest nr = GetFirstWaitingRequest();
            If (nr == NULL) {
                Rlog->D(TAG, "No more requests to accept");
                TransitionTo(mDdsAutoRevertState);
            } else If (GetSubIdFromNetworkRequest(nr) != GetCurrentDds()) {
                Rlog->D(TAG, "Switch required for " + nr);
                TransitionTo(mDdsSwitchState);
            } else {
                Rlog->E(TAG, "This request could not get accepted, start over. nr = " + nr);
                //reset state machine to stable state.
                TransitionTo(mDdsAutoRevertState);
            }
        }

        //@Override
        CARAPI Enter() {
            Rlog->D(TAG, "Enter");
            If (!AcceptWaitingRequest()) {
                HandleOtherSubRequests();
            }
        }

        //@Override
        CARAPI Exit() {
            Rlog->D(TAG, "Exit");
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch(msg.what) {
                case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                    Rlog->D(TAG, "REQ_DDS_ALLOCATION");
                    NetworkRequest n = (NetworkRequest)msg.obj;

                    If (GetSubIdFromNetworkRequest(n) == GetCurrentDds()) {
                        Rlog->D(TAG, "Accepting simultaneous request for current sub");
                        NotifyRequestAccepted(n);
                    } else If (IsMultiDataSupported()) {
                        Rlog->D(TAG, "Incoming request is for on-demand subscription, n = " + n);
                        RequestPsAttach(n);
                    }
                    return HANDLED;
                }

                case DdsSchedulerAc.REQ_DDS_FREE: {
                    Rlog->D(TAG, "REQ_DDS_FREE");

                    If(!AcceptWaitingRequest()) {
                        Rlog->D(TAG, "Can't process next in this DDS");
                        HandleOtherSubRequests();
                    } else {
                        Rlog->D(TAG, "Processing next in same DDS");
                    }
                    return HANDLED;
                }

                case DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE: {
                    AsyncResult ar = (AsyncResult) msg.obj;
                    NetworkRequest n = (NetworkRequest)ar.result;
                    If (ar.exception == NULL) {
                        UpdateCurrentDds(n);
                        TransitionTo(mPsAttachReservedState);
                    } else {
                        Rlog->D(TAG, "Switch failed, ignore the req = " + n);
                        //discard the request so that we can process other pending reqeusts
                        RemoveRequest(n);
                    }
                    return HANDLED;
                }

                default: {
                    Rlog->D(TAG, "unknown msg = " + msg);
                    return NOT_HANDLED;
                }
            }
        }
    }

    private class PsAttachReservedState extends State {
        static const String TAG = DdsScheduler.TAG + "[PSAttachReservedState]";

        private void HandleOtherSubRequests() {
            NetworkRequest nr = GetFirstWaitingRequest();
            If (nr == NULL) {
                Rlog->D(TAG, "No more requests to accept");
            } else If (GetSubIdFromNetworkRequest(nr) != GetCurrentDds()) {
                Rlog->D(TAG, "Next request is not for current on-demand PS Sub(DSDA). nr = "
                        + nr);
                If (IsAlreadyAccepted(nr)) {
                    Rlog->D(TAG, "Next request is already accepted on other sub in DSDA mode. nr = "
                            + nr);
                    TransitionTo(mDdsReservedState);
                    return;
                }
            }
            TransitionTo(mDdsAutoRevertState);
        }


        //@Override
        CARAPI Enter() {
            Rlog->D(TAG, "Enter");
            If (!AcceptWaitingRequest()) {
                HandleOtherSubRequests();
            }

        }

        //@Override
        CARAPI Exit() {
            Rlog->D(TAG, "Exit");

            //Request PS Detach on currentDds.
            RequestPsDetach();
            //Update currentDds back to defaultDataSub.
            UpdateCurrentDds(NULL);
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch(msg.what) {
                case DdsSchedulerAc.REQ_DDS_ALLOCATION: {
                    Rlog->D(TAG, "REQ_DDS_ALLOCATION");

                    NetworkRequest n = (NetworkRequest)msg.obj;
                    Rlog->D(TAG, "Accepting request in dual-data mode, req = " + n);
                    NotifyRequestAccepted(n);
                    return HANDLED;
                }

                case DdsSchedulerAc.REQ_DDS_FREE: {
                    Rlog->D(TAG, "REQ_DDS_FREE");
                    If (!AcceptWaitingRequest()) {
                        //No more requests for current sub. If there are few accepted requests
                        //for defaultDds then move to DdsReservedState so that on-demand PS
                        //detach on current sub can be triggered.
                        HandleOtherSubRequests();
                    }

                    return HANDLED;
                }

                default: {
                    Rlog->D(TAG, "unknown msg = " + msg);
                    return NOT_HANDLED;
                }
            }
        }
    }

    private class DdsSwitchState extends State {
        static const String TAG = DdsScheduler.TAG + "[DdsSwitchState]";
        //@Override
        CARAPI Enter() {
            Rlog->D(TAG, "Enter");
            NetworkRequest nr = GetFirstWaitingRequest();
            If (nr != NULL) {
                TriggerSwitch(nr);
            } else {
                Rlog->D(TAG, "Error");
            }
        }

        //@Override
        CARAPI Exit() {
            Rlog->D(TAG, "Exit");
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch(msg.what) {
                case DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE:
                case DdsSchedulerAc.EVENT_ON_DEMAND_DDS_SWITCH_DONE : {
                    AsyncResult ar = (AsyncResult) msg.obj;
                    NetworkRequest n = (NetworkRequest)ar.result;
                    If (ar.exception == NULL) {
                        UpdateCurrentDds(n);

                        If (msg.what == DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE) {
                            TransitionTo(mPsAttachReservedState);
                        } else {
                            TransitionTo(mDdsReservedState);
                        }
                    } else {
                        Rlog->D(TAG, "Switch failed, move back to idle state");
                        //discard the request so that we can process other pending reqeusts
                        RemoveRequest(n);
                        TransitionTo(mDdsIdleState);
                    }
                    return HANDLED;
                }

                default: {
                    Rlog->D(TAG, "unknown msg = " + msg);
                    return NOT_HANDLED;
                }
            }

        }
    }

    private class DdsAutoRevertState extends State {
        static const String TAG = DdsScheduler.TAG + "[DdsAutoRevertState]";
        //@Override
        CARAPI Enter() {
            Rlog->D(TAG, "Enter");

            TriggerSwitch(NULL);
        }

        //@Override
        CARAPI Exit() {
            Rlog->D(TAG, "Exit");
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch(msg.what) {
                 case DdsSchedulerAc.EVENT_ON_DEMAND_PS_ATTACH_DONE: {
                    Rlog->D(TAG, "SET_DDS_DONE");
                    UpdateCurrentDds(NULL);

                    TransitionTo(mDdsIdleState);
                    return HANDLED;
                }

                default: {
                    Rlog->D(TAG, "unknown msg = " + msg);
                    return NOT_HANDLED;
                }
            }
        }
    }
}
