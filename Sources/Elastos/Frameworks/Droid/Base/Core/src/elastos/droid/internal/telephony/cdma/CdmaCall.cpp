/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony.cdma;

using Elastos::Utility::IList;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IDriverCall;
using Elastos::Droid::Internal::Telephony::IPhone;

/**
 * {@hide}
 */
public class CdmaCall extends Call {
    /*************************** Instance Variables **************************/

    /*package*/ CdmaCallTracker mOwner;

    /****************************** Constructors *****************************/
    /*package*/
    CdmaCall (CdmaCallTracker owner) {
        mOwner = owner;
    }

    CARAPI Dispose() {
    }

    /************************** Overridden from Call *************************/
    //@Override
    public List<Connection>
    GetConnections() {
        // FIXME should return Collections->UnmodifiableList();
        return mConnections;
    }

    //@Override
    public Phone
    GetPhone() {
        return mOwner.mPhone;
    }

    //@Override
    public Boolean IsMultiparty() {
        return mConnections->Size() > 1;
    }

    /** Please note: if this is the foreground call and a
     *  background call exists, the background call will be resumed
     *  because an AT+CHLD=1 will be sent
     */
    //@Override
    CARAPI
    Hangup() throws CallStateException {
        mOwner->Hangup(this);
    }

    //@Override
    public String
    ToString() {
        return mState->ToString();
    }

    //***** Called from CdmaConnection

    /*package*/ void
    Attach(Connection conn, DriverCall dc) {
        mConnections->Add(conn);

        mState = StateFromDCState (dc.state);
    }

    /*package*/ void
    AttachFake(Connection conn, State state) {
        mConnections->Add(conn);

        mState = state;
    }

    /**
     * Called by CdmaConnection when it has disconnected
     */
    Boolean
    ConnectionDisconnected(CdmaConnection conn) {
        If (mState != State.DISCONNECTED) {
            /* If only disconnected connections remain, we are disconnected*/

            Boolean hasOnlyDisconnectedConnections = TRUE;

            For (Int32 i = 0, s = mConnections->Size()  ; i < s; i ++) {
                If (mConnections->Get(i).GetState()
                    != State.DISCONNECTED
                ) {
                    hasOnlyDisconnectedConnections = FALSE;
                    break;
                }
            }

            If (hasOnlyDisconnectedConnections) {
                mState = State.DISCONNECTED;
                return TRUE;
            }
        }

        return FALSE;
    }


    /*package*/ void
    Detach(CdmaConnection conn) {
        mConnections->Remove(conn);

        If (mConnections->Size() == 0) {
            mState = State.IDLE;
        }
    }

    /*package*/ Boolean
    Update (CdmaConnection conn, DriverCall dc) {
        State newState;
        Boolean changed = FALSE;

        newState = StateFromDCState(dc.state);

        If (newState != mState) {
            mState = newState;
            changed = TRUE;
        }

        return changed;
    }

    /**
     * @return TRUE if there's no space in this call for additional
     * connections to be added via "conference"
     */
    /*package*/ Boolean
    IsFull() {
        return mConnections->Size() == CdmaCallTracker.MAX_CONNECTIONS_PER_CALL;
    }

    //***** Called from CdmaCallTracker


    /**
     * Called when this Call is being hung up Locally (eg, user pressed "end")
     * Note that at this point, the hangup request has been dispatched to the radio
     * but no response has yet been received so Update() has not yet been called
     */
    void
    OnHangupLocal() {
        For (Int32 i = 0, s = mConnections->Size(); i < s; i++) {
            CdmaConnection cn = (CdmaConnection)mConnections->Get(i);

            cn->OnHangupLocal();
        }
        mState = State.DISCONNECTING;
    }

    /**
     * Called when it's time to clean up disconnected Connection objects
     */
   void ClearDisconnected() {
        For (Int32 i = mConnections->Size() - 1 ; i >= 0 ; i--) {
        CdmaConnection cn = (CdmaConnection)mConnections->Get(i);

            If (cn->GetState() == State.DISCONNECTED) {
                mConnections->Remove(i);
            }
        }

        If (mConnections->Size() == 0) {
            mState = State.IDLE;
        }
    }
}
