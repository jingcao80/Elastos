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

package com.android.internal.telephony.imsphone;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IDisconnectCause;

using Elastos::Droid::Internal::Annotations::IVisibleForTesting;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Ims::IImsCall;
using Elastos::Droid::Ims::IImsCallProfile;
using Elastos::Droid::Ims::IImsStreamMediaProfile;

using Elastos::Utility::IList;

/**
 * {@hide}
 */
public class ImsPhoneCall extends Call {
    /*************************** Instance Variables **************************/

    private static const String LOG_TAG = "ImsPhoneCall";
    private static const Boolean DBG = FALSE;

    /*package*/ ImsPhoneCallTracker mOwner;

    private Boolean mRingbackTonePlayed = FALSE;

    /****************************** Constructors *****************************/
    /*package*/
    ImsPhoneCall() {
    }

    /*package*/
    ImsPhoneCall(ImsPhoneCallTracker owner) {
        mOwner = owner;
    }

    CARAPI Dispose() {
        try {
            mOwner->Hangup(this);
        } Catch (CallStateException ex) {
            //Rlog->E(LOG_TAG, "dispose: unexpected error on hangup", ex);
            //while disposing, ignore the exception and clean the connections
        } finally {
            For(Int32 i = 0, s = mConnections->Size(); i < s; i++) {
                ImsPhoneConnection c = (ImsPhoneConnection) mConnections->Get(i);
                c->OnDisconnect(DisconnectCause.LOST_SIGNAL);
            }
        }
    }

    /************************** Overridden from Call *************************/

    //@Override
    public List<Connection>
    GetConnections() {
        return mConnections;
    }

    //@Override
    public Phone
    GetPhone() {
        return mOwner.mPhone;
    }

    //@Override
    public Boolean
    IsMultiparty() {
        ImsCall imsCall = GetImsCall();
        If (imsCall == NULL) {
            return FALSE;
        }

        return imsCall->IsMultiparty();
    }

    /** Please note: if this is the foreground call and a
     *  background call exists, the background call will be resumed.
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

    //@Override
    public Bundle GetExtras() {
        Bundle imsCallExtras = NULL;
        ImsCall call = GetImsCall();
        ImsCallProfile callProfile;

        If (call != NULL) {
            callProfile = call->GetCallProfile();
            If (callProfile != NULL) {
                imsCallExtras = callProfile.mCallExtras;
            }
        }
        If (imsCallExtras == NULL) {
            If (DBG) Rlog->D(LOG_TAG, "ImsCall extras are NULL.");
        }
        return imsCallExtras;
    }

    //***** Called from ImsPhoneConnection

    /*package*/ void
    Attach(Connection conn) {
        ClearDisconnected();
        mConnections->Add(conn);
    }

    /*package*/ void
    Attach(Connection conn, State state) {
        this->Attach(conn);
        mState = state;
    }

    /*package*/ void
    AttachFake(Connection conn, State state) {
        Attach(conn, state);
    }

    /**
     * Called by ImsPhoneConnection when it has disconnected
     */
    Boolean
    ConnectionDisconnected(ImsPhoneConnection conn) {
        If (mState != State.DISCONNECTED) {
            /* If only disconnected connections remain, we are disconnected*/

            Boolean hasOnlyDisconnectedConnections = TRUE;

            For (Int32 i = 0, s = mConnections->Size()  ; i < s; i ++) {
                If (mConnections->Get(i).GetState() != State.DISCONNECTED) {
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
    Detach(ImsPhoneConnection conn) {
        mConnections->Remove(conn);
        ClearDisconnected();
    }

    /**
     * @return TRUE if there's no space in this call for additional
     * connections to be added via "conference"
     */
    /*package*/ Boolean
    IsFull() {
        return mConnections->Size() == ImsPhoneCallTracker.MAX_CONNECTIONS_PER_CALL;
    }

    //***** Called from ImsPhoneCallTracker
    /**
     * Called when this Call is being hung up Locally (eg, user pressed "end")
     */
    void
    OnHangupLocal() {
        For (Int32 i = 0, s = mConnections->Size(); i < s; i++) {
            ImsPhoneConnection cn = (ImsPhoneConnection)mConnections->Get(i);
            cn->OnHangupLocal();
        }
        mState = State.DISCONNECTING;
    }

    /**
     * Called when it's time to clean up disconnected Connection objects
     */
    void
    ClearDisconnected() {
        For (Int32 i = mConnections->Size() - 1 ; i >= 0 ; i--) {
            ImsPhoneConnection cn = (ImsPhoneConnection)mConnections->Get(i);

            If (cn->GetState() == State.DISCONNECTED) {
                mConnections->Remove(i);
            }
        }

        If (mConnections->Size() == 0) {
            mState = State.IDLE;
        }
    }

    /*package*/ ImsPhoneConnection
    GetFirstConnection() {
        If (mConnections->Size() == 0) return NULL;

        For (Int32 i = mConnections->Size() - 1 ; i >= 0 ; i--) {
            ImsPhoneConnection cn = (ImsPhoneConnection)mConnections->Get(i);
            If (cn->GetState()->IsAlive()) {
                Return (ImsPhoneConnection) mConnections->Get(i);
            }
        }

        return NULL;
    }

    /*package*/ void
    SetMute(Boolean mute) {
        ImsCall imsCall = GetFirstConnection() == NULL ?
                NULL : GetFirstConnection()->GetImsCall();
        If (imsCall != NULL) {
            try {
                imsCall->SetMute(mute);
            } Catch (ImsException e) {
                Rlog->E(LOG_TAG, "setMute failed : " + e->GetMessage());
            }
        }
    }

    /* package */ void
    Merge(ImsPhoneCall that, State state) {
        ImsPhoneConnection[] cc = that.mConnections->ToArray(
                new ImsPhoneConnection[that.mConnections->Size()]);
        For (ImsPhoneConnection c : cc) {
            c->Update(NULL, state);
        }
    }

    /**
     * Retrieves the {@link ImsCall} for the current {@link ImsPhoneCall}.
     * <p>
     * Marked as {@code VisibleForTesting} so that the
     * {@link com.android.internal.telephony.TelephonyTester} class can inject a test conference
     * event package into a regular ongoing IMS call.
     *
     * @return The {@link ImsCall}.
     */
    @VisibleForTesting
    public ImsCall
    GetImsCall() {
        Return (GetFirstConnection() == NULL) ? NULL : GetFirstConnection()->GetImsCall();
    }

    /*package*/ static Boolean IsLocalTone(ImsCall imsCall) {
        If ((imsCall == NULL) || (imsCall->GetCallProfile() == NULL)
                || (imsCall->GetCallProfile().mMediaProfile == NULL)) {
            return FALSE;
        }

        ImsStreamMediaProfile mediaProfile = imsCall->GetCallProfile().mMediaProfile;

        Return (mediaProfile.mAudioDirection == ImsStreamMediaProfile.DIRECTION_INACTIVE)
                ? TRUE : FALSE;
    }

    /*package*/ Boolean
    Update (ImsPhoneConnection conn, ImsCall imsCall, State state) {
        State newState = state;
        Boolean changed = FALSE;

        //ImsCall.Listener.onCallProgressing can be invoked several times
        //and ringback tone mode can be changed during the call setup procedure
        If (state == State.ALERTING) {
            If (mRingbackTonePlayed && !IsLocalTone(imsCall)) {
                mOwner.mPhone->StopRingbackTone();
                mRingbackTonePlayed = FALSE;
            } else If (!mRingbackTonePlayed && IsLocalTone(imsCall)) {
                mOwner.mPhone->StartRingbackTone();
                mRingbackTonePlayed = TRUE;
            }
        } else {
            If (mRingbackTonePlayed) {
                mOwner.mPhone->StopRingbackTone();
                mRingbackTonePlayed = FALSE;
            }
        }

        If ((newState != mState) && (state != State.DISCONNECTED)) {
            mState = newState;
            changed = TRUE;
        } else If (state == State.DISCONNECTED) {
            changed = TRUE;
        }

        return changed;
    }

    /* package */ ImsPhoneConnection
    GetHandoverConnection() {
        Return (ImsPhoneConnection) GetEarliestConnection();
    }

    void SwitchWith(ImsPhoneCall that) {
        {    AutoLock syncLock(ImsPhoneCall.class);
            ImsPhoneCall tmp = new ImsPhoneCall();
            tmp->TakeOver(this);
            this->TakeOver(that);
            that->TakeOver(tmp);
        }
    }

    private void TakeOver(ImsPhoneCall that) {
        mConnections = that.mConnections;
        mState = that.mState;
        For (Connection c : mConnections) {
            ((ImsPhoneConnection) c).ChangeParent(this);
        }
    }
}
