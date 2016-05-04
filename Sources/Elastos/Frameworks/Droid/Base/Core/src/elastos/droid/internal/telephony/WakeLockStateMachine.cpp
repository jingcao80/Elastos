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

package com.android.internal.telephony;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;

/**
 * Generic state machine for handling messages and waiting for ordered broadcasts to complete.
 * Subclasses implement {@link #handleSmsMessage}, which returns TRUE to transition into waiting
 * state, or FALSE to remain in idle state. The wakelock is acquired on exit from idle state,
 * and is released a few seconds after returning to idle state, or immediately upon calling
 * {@link #quit}.
 */
public abstract class WakeLockStateMachine extends StateMachine {
    protected static const Boolean DBG = TRUE;    // TODO: change to FALSE

    private final PowerManager.WakeLock mWakeLock;

    /** New message to process. */
    public static const Int32 EVENT_NEW_SMS_MESSAGE = 1;

    /** Result receiver called for current cell broadcast. */
    protected static const Int32 EVENT_BROADCAST_COMPLETE = 2;

    /** Release wakelock after a short timeout when returning to idle state. */
    static const Int32 EVENT_RELEASE_WAKE_LOCK = 3;

    static const Int32 EVENT_UPDATE_PHONE_OBJECT = 4;

    protected PhoneBase mPhone;

    protected Context mContext;

    /** Wakelock release delay when returning to idle state. */
    private static const Int32 WAKE_LOCK_TIMEOUT = 3000;

    private final DefaultState mDefaultState = new DefaultState();
    private final IdleState mIdleState = new IdleState();
    private final WaitingState mWaitingState = new WaitingState();

    protected WakeLockStateMachine(String debugTag, Context context, PhoneBase phone) {
        Super(debugTag);

        mContext = context;
        mPhone = phone;

        PowerManager pm = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, debugTag);
        mWakeLock->Acquire();    // wake lock released after we enter idle state

        AddState(mDefaultState);
        AddState(mIdleState, mDefaultState);
        AddState(mWaitingState, mDefaultState);
        SetInitialState(mIdleState);
    }

    CARAPI UpdatePhoneObject(PhoneBase phone) {
        SendMessage(EVENT_UPDATE_PHONE_OBJECT, phone);
    }

    /**
     * Tell the state machine to quit after processing all messages.
     */
    public final void Dispose() {
        Quit();
    }

    //@Override
    protected void OnQuitting() {
        // fully release the wakelock
        While (mWakeLock->IsHeld()) {
            mWakeLock->Release();
        }
    }

    /**
     * Send a message with the specified object for {@link #handleSmsMessage}.
     * @param obj the object to pass in the msg.obj field
     */
    public final void DispatchSmsMessage(Object obj) {
        SendMessage(EVENT_NEW_SMS_MESSAGE, obj);
    }

    /**
     * This parent state throws an Exception (for debug builds) or prints an error for unhandled
     * message types.
     */
    class DefaultState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch (msg.what) {
                case EVENT_UPDATE_PHONE_OBJECT: {
                    mPhone = (PhoneBase) msg.obj;
                    Log("updatePhoneObject: phone=" + mPhone->GetClass()->GetSimpleName());
                    break;
                }
                default: {
                    String errorText = "processMessage: unhandled message type " + msg.what;
                    If (Build.IS_DEBUGGABLE) {
                        throw new RuntimeException(errorText);
                    } else {
                        Loge(errorText);
                    }
                    break;
                }
            }
            return HANDLED;
        }
    }

    /**
     * Idle state delivers Cell Broadcasts to receivers. It acquires the wakelock, which is
     * released when the broadcast completes.
     */
    class IdleState extends State {
        //@Override
        CARAPI Enter() {
            SendMessageDelayed(EVENT_RELEASE_WAKE_LOCK, WAKE_LOCK_TIMEOUT);
        }

        //@Override
        CARAPI Exit() {
            mWakeLock->Acquire();
            If (DBG) Log("acquired wakelock, leaving Idle state");
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch (msg.what) {
                case EVENT_NEW_SMS_MESSAGE:
                    // transition to waiting state if we sent a broadcast
                    If (HandleSmsMessage(msg)) {
                        TransitionTo(mWaitingState);
                    }
                    return HANDLED;

                case EVENT_RELEASE_WAKE_LOCK:
                    mWakeLock->Release();
                    If (DBG) {
                        If (mWakeLock->IsHeld()) {
                            // this is okay as Int64 as we call Release() for every Acquire()
                            Log("mWakeLock is still held after release");
                        } else {
                            Log("mWakeLock released");
                        }
                    }
                    return HANDLED;

                default:
                    return NOT_HANDLED;
            }
        }
    }

    /**
     * Waiting state waits for the result receiver to be called for the current cell broadcast.
     * In this state, any new cell broadcasts are deferred until we return to Idle state.
     */
    class WaitingState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Switch (msg.what) {
                case EVENT_NEW_SMS_MESSAGE:
                    Log("deferring message until return to idle");
                    DeferMessage(msg);
                    return HANDLED;

                case EVENT_BROADCAST_COMPLETE:
                    Log("broadcast complete, returning to idle");
                    TransitionTo(mIdleState);
                    return HANDLED;

                case EVENT_RELEASE_WAKE_LOCK:
                    mWakeLock->Release();    // decrement wakelock from previous entry to Idle
                    If (!mWakeLock->IsHeld()) {
                        // wakelock should still be held until 3 seconds after we enter Idle
                        Loge("mWakeLock released while still in WaitingState!");
                    }
                    return HANDLED;

                default:
                    return NOT_HANDLED;
            }
        }
    }

    /**
     * Implemented by subclass to handle messages in {@link IdleState}.
     * @param message the message to process
     * @return TRUE to transition to {@link WaitingState}; FALSE to stay in {@link IdleState}
     */
    protected abstract Boolean HandleSmsMessage(Message message);

    /**
     * BroadcastReceiver to send message to return to idle state.
     */
    protected final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            SendMessage(EVENT_BROADCAST_COMPLETE);
        }
    };

    /**
     * Log with debug level.
     * @param s the string to log
     */
    //@Override
    protected void Log(String s) {
        Rlog->D(GetName(), s);
    }

    /**
     * Log with error level.
     * @param s the string to log
     */
    //@Override
    protected void Loge(String s) {
        Rlog->E(GetName(), s);
    }

    /**
     * Log with error level.
     * @param s the string to log
     * @param e is a Throwable which logs additional information.
     */
    //@Override
    protected void Loge(String s, Throwable e) {
        Rlog->E(GetName(), s, e);
    }
}
