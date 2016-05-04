/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.internal.telephony.sip;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

abstract class SipCallBase extends Call {

    protected abstract void SetState(State newState);

    //@Override
    public List<Connection> GetConnections() {
        // FIXME should return Collections->UnmodifiableList();
        return mConnections;
    }

    //@Override
    public Boolean IsMultiparty() {
        return mConnections->Size() > 1;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mState->ToString() + ":" + super->ToString();
    }

    void ClearDisconnected() {
        For (Iterator<Connection> it = mConnections->Iterator(); it->HasNext(); ) {
            Connection c = it->Next();
            If (c->GetState() == State.DISCONNECTED) it->Remove();
        }

        If (mConnections->IsEmpty()) SetState(State.IDLE);
    }
}
