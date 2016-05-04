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

package com.android.internal.telephony;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telephony::IRlog;

/**
 * {@hide}
 */
public abstract class Call {
    protected final String LOG_TAG = "Call";

    /* Enums */

    public enum State {
        IDLE, ACTIVE, HOLDING, DIALING, ALERTING, INCOMING, WAITING, DISCONNECTED, DISCONNECTING;

        public Boolean IsAlive() {
            return !(this == IDLE || this == DISCONNECTED || this == DISCONNECTING);
        }

        public Boolean IsRinging() {
            return this == INCOMING || this == WAITING;
        }

        public Boolean IsDialing() {
            return this == DIALING || this == ALERTING;
        }
    }

    public static State
    StateFromDCState (DriverCall.State dcState) {
        Switch (dcState) {
            case ACTIVE:        return State.ACTIVE;
            case HOLDING:       return State.HOLDING;
            case DIALING:       return State.DIALING;
            case ALERTING:      return State.ALERTING;
            case INCOMING:      return State.INCOMING;
            case WAITING:       return State.WAITING;
            default:            throw new RuntimeException ("illegal call state:" + dcState);
        }
    }

    public enum SrvccState {
        NONE, STARTED, COMPLETED, FAILED, CANCELED;
    }

    /* Instance Variables */

    public State mState = State.IDLE;

    public ArrayList<Connection> mConnections = new ArrayList<Connection>();

    // Flag to indicate if the current calling/caller information
    // is accurate. If FALSE the information is known to be accurate.
    //
    // For CDMA, during call waiting/3 way, there is no network response
    // if call waiting is answered, network timed out, dropped, 3 way
    // merged, etc.
    protected Boolean mIsGeneric = FALSE;

    /* Instance Methods */

    /** Do not modify the List result!!! This list is not yours to keep
     *  It will change across event loop iterations            top
     */

    public abstract List<Connection> GetConnections();
    public abstract Phone GetPhone();
    public abstract Boolean IsMultiparty();
    public abstract void Hangup() throws CallStateException;


    /**
     * hasConnection
     *
     * @param c a Connection object
     * @return TRUE if the call contains the connection object passed in
     */
    public Boolean HasConnection(Connection c) {
        return c->GetCall() == this;
    }

    /**
     * hasConnections
     * @return TRUE if the call contains one or more connections
     */
    public Boolean HasConnections() {
        List<Connection> connections = GetConnections();

        If (connections == NULL) {
            return FALSE;
        }

        return connections->Size() > 0;
    }

    /**
     * getState
     * @return state of class call
     */
    public State GetState() {
        return mState;
    }

    /**
    * getExtras
    * @return Call Extras. Subclasses of Call that support call extras need
    *         to override this method to return the extras.
    */
    public Bundle GetExtras() {
        return NULL;
    }

    /**
     * isIdle
     *
     * FIXME rename
     * @return TRUE if the call contains only disconnected Connections (if any)
     */
    public Boolean IsIdle() {
        return !GetState()->IsAlive();
    }

    /**
     * Returns the Connection associated with this Call that was created
     * first, or NULL if there are no Connections in this Call
     */
    public Connection
    GetEarliestConnection() {
        List<Connection> l;
        Int64 time = Long.MAX_VALUE;
        Connection c;
        Connection earliest = NULL;

        l = GetConnections();

        If (l->Size() == 0) {
            return NULL;
        }

        For (Int32 i = 0, s = l->Size() ; i < s ; i++) {
            c = l->Get(i);
            Int64 t;

            t = c->GetCreateTime();

            If (t < time) {
                earliest = c;
                time = t;
            }
        }

        return earliest;
    }

    public Int64
    GetEarliestCreateTime() {
        List<Connection> l;
        Int64 time = Long.MAX_VALUE;

        l = GetConnections();

        If (l->Size() == 0) {
            return 0;
        }

        For (Int32 i = 0, s = l->Size() ; i < s ; i++) {
            Connection c = l->Get(i);
            Int64 t;

            t = c->GetCreateTime();

            time = t < time ? t : time;
        }

        return time;
    }

    public Int64
    GetEarliestConnectTime() {
        Int64 time = Long.MAX_VALUE;
        List<Connection> l = GetConnections();

        If (l->Size() == 0) {
            return 0;
        }

        For (Int32 i = 0, s = l->Size() ; i < s ; i++) {
            Connection c = l->Get(i);
            Int64 t;

            t = c->GetConnectTime();

            time = t < time ? t : time;
        }

        return time;
    }


    public Boolean
    IsDialingOrAlerting() {
        return GetState()->IsDialing();
    }

    public Boolean
    IsRinging() {
        return GetState()->IsRinging();
    }

    /**
     * Returns the Connection associated with this Call that was created
     * last, or NULL if there are no Connections in this Call
     */
    public Connection
    GetLatestConnection() {
        List<Connection> l = GetConnections();
        If (l->Size() == 0) {
            return NULL;
        }

        Int64 time = 0;
        Connection latest = NULL;
        For (Int32 i = 0, s = l->Size() ; i < s ; i++) {
            Connection c = l->Get(i);
            Int64 t = c->GetCreateTime();

            If (t > time) {
                latest = c;
                time = t;
            }
        }

        return latest;
    }

    /**
     * To indicate if the connection information is accurate
     * or not. FALSE means accurate. Only used for CDMA.
     */
    public Boolean IsGeneric() {
        return mIsGeneric;
    }

    /**
     * Set the generic instance variable
     */
    CARAPI SetGeneric(Boolean generic) {
        mIsGeneric = generic;
    }

    /**
     * Hangup call if it is alive
     */
    CARAPI HangupIfAlive() {
        If (GetState()->IsAlive()) {
            try {
                Hangup();
            } Catch (CallStateException ex) {
                Rlog->W(LOG_TAG, " hangupIfActive: caught " + ex);
            }
        }
    }
}
