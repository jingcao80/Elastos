/*
* Copyright (C) 2011-2014 MediaTek Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;

using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;

public class DcSwitchState extends StateMachine {
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE;
    private static const String LOG_TAG = "DcSwitchState";

    // ***** Event codes for driving the state machine
    private static const Int32 BASE = Protocol.BASE_DATA_CONNECTION_TRACKER + 0x00001000;
    private static const Int32 EVENT_CONNECT = BASE + 0;
    private static const Int32 EVENT_DISCONNECT = BASE + 1;
    private static const Int32 EVENT_CLEANUP_ALL = BASE + 2;
    private static const Int32 EVENT_CONNECTED = BASE + 3;
    private static const Int32 EVENT_DETACH_DONE = BASE + 4;
    private static const Int32 EVENT_TO_IDLE_DIRECTLY = BASE + 5;
    private static const Int32 EVENT_TO_ACTING_DIRECTLY = BASE + 6;

    private Int32 mId;
    private Phone mPhone;
    private AsyncChannel mAc;
    private RegistrantList mIdleRegistrants = new RegistrantList();
    private HashSet<String> mApnTypes = new HashSet<String>();

    private IdleState     mIdleState = new IdleState();
    private ActingState   mActingState = new ActingState();
    private ActedState    mActedState = new ActedState();
    private DeactingState mDeactingState = new DeactingState();
    private DefaultState  mDefaultState = new DefaultState();

    protected DcSwitchState(Phone phone, String name, Int32 id) {
        Super(name);
        If (DBG) Log("DcSwitchState constructor E");
        mPhone = phone;
        mId = id;

        AddState(mDefaultState);
        AddState(mIdleState, mDefaultState);
        AddState(mActingState, mDefaultState);
        AddState(mActedState, mDefaultState);
        AddState(mDeactingState, mDefaultState);
        SetInitialState(mIdleState);

        If (DBG) Log("DcSwitchState constructor X");
    }

    private Int32 SetupConnection(String type) {
        mApnTypes->Add(type);
        Log("DcSwitchState:setupConnection type = " + type);
//        return mPhone->EnableApnType(type); TODO
        return PhoneConstants.APN_REQUEST_STARTED;
    }

    private Int32 TeardownConnection(String type) {
        mApnTypes->Remove(type);
        If (mApnTypes->IsEmpty()) {
            Log("No APN is using, then clean up all");
            // Since last type is removed from mApnTypes and will not be disabled in RequestDataIdle()
//            mPhone->DisableApnType(type); TODO
            RequestDataIdle();
            TransitionTo(mDeactingState);
            return PhoneConstants.APN_REQUEST_STARTED;
        } else {
//            return mPhone->DisableApnType(type); TODO
            return PhoneConstants.APN_REQUEST_STARTED;
        }
    }

    private void RequestDataIdle() {
        If (DBG) Log("requestDataIdle is triggered");
        Iterator<String> itrType = mApnTypes->Iterator();
        While (itrType->HasNext()) {
//            mPhone->DisableApnType(itrType->Next()); TODO
        }
        mApnTypes->Clear();
        PhoneBase pb = (PhoneBase)((PhoneProxy)mPhone).GetActivePhone();
        pb.mCi->SetDataAllowed(FALSE, ObtainMessage(EVENT_DETACH_DONE));
    }

    CARAPI NotifyDataConnection(Int32 phoneId, String state, String reason,
            String apnName, String apnType, Boolean unavailable) {
        If (phoneId == mId &&
                TextUtils->Equals(state, PhoneConstants.DataState.CONNECTED->ToString())) {
            SendMessage(ObtainMessage(EVENT_CONNECTED));
        }
    }

    CARAPI CleanupAllConnection() {
        SendMessage(ObtainMessage(EVENT_CLEANUP_ALL));
    }

    CARAPI RegisterForIdle(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mIdleRegistrants->Add(r);
    }

    CARAPI UnregisterForIdle(Handler h) {
        mIdleRegistrants->Remove(h);
    }

    CARAPI TransitToIdleState() {
        SendMessage(ObtainMessage(EVENT_TO_IDLE_DIRECTLY));
    }

    CARAPI TransitToActingState() {
        SendMessage(ObtainMessage(EVENT_TO_ACTING_DIRECTLY));
    }

    private class IdleState extends State {
        //@Override
        CARAPI Enter() {
            mIdleRegistrants->NotifyRegistrants();
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case DcSwitchAsyncChannel.REQ_CONNECT:
                case EVENT_CONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("IdleState: REQ_CONNECT/EVENT_CONNECT(" +
                            msg.what + ") type=" + type);
                    }

                    Boolean isPrimarySubFeatureEnable =
                            SystemProperties->GetBoolean("persist.radio.primarycard", FALSE);
                    PhoneBase pb = (PhoneBase)((PhoneProxy)mPhone).GetActivePhone();
                    Int64 subId = pb->GetSubId();
                    Log("Setting default DDS on " + subId + " primary Sub feature"
                            + isPrimarySubFeatureEnable);

                    // When isPrimarySubFeatureEnable is enabled apps will take care
                    // of sending DDS request during device power-up.
                    If (!isPrimarySubFeatureEnable) {
                        SubscriptionController subscriptionController
                                = SubscriptionController->GetInstance();
                        subscriptionController->SetDefaultDataSubId(subId);
                    }

                    Int32 result = SetupConnection(type);
                    If (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                            mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT, result);
                    }
                    TransitionTo(mActingState);
                    retVal = HANDLED;
                    break;
                }
                case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("IdleState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                    }
                    mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT,
                        PhoneConstants.APN_ALREADY_INACTIVE);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CLEANUP_ALL: {
                    If (DBG) {
                        Log("IdleState: EVENT_CLEANUP_ALL" );
                    }
                    RequestDataIdle();
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CONNECTED: {
                    If (DBG) {
                        Log("IdleState: Receive invalid event EVENT_CONNECTED!");
                    }
                    retVal = HANDLED;
                    break;
                }
                default:
                    If (VDBG) {
                        Log("IdleState: nothandled msg.what=0x" +
                                Integer->ToHexString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }

    private class ActingState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case DcSwitchAsyncChannel.REQ_CONNECT:
                case EVENT_CONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("ActingState: REQ_CONNECT/EVENT_CONNECT(" + msg.what +
                            ") type=" + type);
                    }
                    Int32 result = SetupConnection(type);
                    If (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                        mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT, result);
                    }
                    retVal = HANDLED;
                    break;
                }
                case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("ActingState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                    }
                    Int32 result = TeardownConnection(type);
                    mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT, result);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CONNECTED: {
                    If (DBG) {
                        Log("ActingState: EVENT_CONNECTED");
                    }
                    TransitionTo(mActedState);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CLEANUP_ALL: {
                    If (DBG) {
                        Log("ActingState: EVENT_CLEANUP_ALL" );
                    }
                    RequestDataIdle();
                    TransitionTo(mDeactingState);
                    retVal = HANDLED;
                    break;
                }
                default:
                    If (VDBG) {
                        Log("ActingState: nothandled msg.what=0x" +
                                Integer->ToHexString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }

    private class ActedState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case DcSwitchAsyncChannel.REQ_CONNECT:
                case EVENT_CONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("ActedState: REQ_CONNECT/EVENT_CONNECT(" + msg.what + ") type=" + type);
                    }
                    Int32 result = SetupConnection(type);
                    If (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                        mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT, result);
                    }
                    retVal = HANDLED;
                    break;
                }
                case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("ActedState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                    }
                    Int32 result = TeardownConnection(type);
                    mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT, result);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CONNECTED: {
                    If (DBG) {
                        Log("ActedState: EVENT_CONNECTED");
                    }
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CLEANUP_ALL: {
                    If (DBG) {
                        Log("ActedState: EVENT_CLEANUP_ALL" );
                    }
                    RequestDataIdle();
                    TransitionTo(mDeactingState);
                    retVal = HANDLED;
                    break;
                }
                default:
                    If (VDBG) {
                        Log("ActingState: nothandled msg.what=0x" +
                                Integer->ToHexString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }

    private class DeactingState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case DcSwitchAsyncChannel.REQ_CONNECT:
                case EVENT_CONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("DeactingState: REQ_CONNECT/EVENT_CONNECT(" +
                            msg.what + ") type=" + type + ", request is defered.");
                    }
                    DeferMessage(ObtainMessage(EVENT_CONNECT, type));
                    If (msg.what == DcSwitchAsyncChannel.REQ_CONNECT) {
                        mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_CONNECT,
                                PhoneConstants.APN_REQUEST_STARTED);
                    }
                    retVal = HANDLED;
                    break;
                }
                case DcSwitchAsyncChannel.REQ_DISCONNECT: {
                    String type = (String)msg.obj;
                    If (DBG) {
                        Log("DeactingState: DcSwitchAsyncChannel.REQ_DISCONNECT type=" + type);
                    }
                    mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_DISCONNECT,
                            PhoneConstants.APN_ALREADY_INACTIVE);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_DETACH_DONE: {
                    If (DBG) {
                        Log("DeactingState: EVENT_DETACH_DONE");
                    }
                    TransitionTo(mIdleState);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CONNECTED: {
                    If (DBG) {
                        Log("DeactingState: Receive invalid event EVENT_CONNECTED!");
                    }
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CLEANUP_ALL: {
                    If (DBG) {
                        Log("DeactingState: EVENT_CLEANUP_ALL, already deacting." );
                    }
                    retVal = HANDLED;
                    break;
                }
                default:
                    If (VDBG) {
                        Log("DeactingState: nothandled msg.what=0x" +
                                Integer->ToHexString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }

    private class DefaultState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            AsyncResult ar;

            Switch (msg.what) {
                case AsyncChannel.CMD_CHANNEL_FULL_CONNECTION: {
                    If (mAc != NULL) {
                        If (VDBG) Log("Disconnecting to previous connection mAc=" + mAc);
                        mAc->ReplyToMessage(msg, AsyncChannel.CMD_CHANNEL_FULLY_CONNECTED,
                                AsyncChannel.STATUS_FULL_CONNECTION_REFUSED_ALREADY_CONNECTED);
                    } else {
                        mAc = new AsyncChannel();
                        mAc->Connected(NULL, GetHandler(), msg.replyTo);
                        If (VDBG) Log("DcDefaultState: FULL_CONNECTION reply connected");
                        mAc->ReplyToMessage(msg, AsyncChannel.CMD_CHANNEL_FULLY_CONNECTED,
                                AsyncChannel.STATUS_SUCCESSFUL, mId, "hi");
                    }
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_DISCONNECT: {
                    If (VDBG) Log("CMD_CHANNEL_DISCONNECT");
                    mAc->Disconnect();
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                    If (VDBG) Log("CMD_CHANNEL_DISCONNECTED");
                    mAc = NULL;
                    break;
                }
                case DcSwitchAsyncChannel.REQ_IS_IDLE_STATE: {
                    Boolean val = GetCurrentState() == mIdleState;
                    If (VDBG) Log("REQ_IS_IDLE_STATE  isIdle=" + val);
                    mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_IS_IDLE_STATE, val ? 1 : 0);
                    break;
                }
                case DcSwitchAsyncChannel.REQ_IS_IDLE_OR_DEACTING_STATE: {
                    Boolean val = (GetCurrentState() == mIdleState || GetCurrentState() == mDeactingState);
                    If (VDBG) Log("REQ_IS_IDLE_OR_DEACTING_STATE  isIdleDeacting=" + val);
                    mAc->ReplyToMessage(msg, DcSwitchAsyncChannel.RSP_IS_IDLE_OR_DEACTING_STATE, val ? 1 : 0);
                    break;
                }
                case EVENT_TO_ACTING_DIRECTLY: {
                    Log("Just transit to Acting state");
                    TransitionTo(mActingState);
                    break;
                }
                case EVENT_TO_IDLE_DIRECTLY: {
                    Log("Just transit to Idle state");
                    Iterator<String> itrType = mApnTypes->Iterator();
                    While (itrType->HasNext()) {
//                        mPhone->DisableApnType(itrType->Next()); TODO
                    }
                    mApnTypes->Clear();
                    TransitionTo(mIdleState);
                }
                default:
                    If (DBG) {
                        Log("DefaultState: shouldn't happen but ignore msg.what=0x" +
                                Integer->ToHexString(msg.what));
                    }
                    break;
            }
            return HANDLED;
        }
    }

    protected void Log(String s) {
        Logger::D(LOG_TAG, "[" + GetName() + "] " + s);
    }
}
