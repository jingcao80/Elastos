//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CELLBROADCASTHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CELLBROADCASTHANDLER_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/WakeLockStateMachine.h"

using Elastos::Droid::Telephony::ISmsCbMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Dispatch new Cell Broadcasts to receivers. Acquires a private wakelock until the broadcast
 * completes and our result receiver is called.
 */
class CellBroadcastHandler
    : public WakeLockStateMachine
{
public:
    /**
     * Create a new CellBroadcastHandler.
     * @param context the context to use for dispatching Intents
     * @return the new handler
     */
    static CARAPI_(AutoPtr<CellBroadcastHandler>) MakeCellBroadcastHandler(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    /**
     * Handle Cell Broadcast messages from {@code CdmaInboundSmsHandler}.
     * 3GPP-format Cell Broadcast messages sent from radio are handled in the subclass.
     *
     * @param message the message to process
     * @return true if an ordered broadcast was sent; false on failure
     */
    // @Override
    CARAPI HandleSmsMessage(
        /* [in] */ IMessage* message,
        /* [out] */ Boolean* result);

protected:
    CellBroadcastHandler();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    CARAPI constructor(
        /* [in] */ const String& debugTag,
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    /**
     * Dispatch a Cell Broadcast message to listeners.
     * @param message the Cell Broadcast to broadcast
     */
    virtual CARAPI_(void) HandleBroadcastSms(
        /* [in] */ ISmsCbMessage* message);
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CELLBROADCASTHANDLER_H__
