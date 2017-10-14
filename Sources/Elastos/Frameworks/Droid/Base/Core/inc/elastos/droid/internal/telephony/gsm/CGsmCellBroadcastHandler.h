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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCELLBROADCASTHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCELLBROADCASTHANDLER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmCellBroadcastHandler.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/CellBroadcastHandler.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::CellBroadcastHandler;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::ISmsCbLocation;
using Elastos::Droid::Telephony::ISmsCbMessage;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * Handler for 3GPP format Cell Broadcasts. Parent class can also handle CDMA Cell Broadcasts.
 */
CarClass(CGsmCellBroadcastHandler)
    , public CellBroadcastHandler
    , public IGsmCellBroadcastHandler
{
private:
    /**
     * Holds all info about a message page needed to assemble a complete concatenated message.
     */
    class SmsCbConcatInfo
        : public Object
    {
    public:
        SmsCbConcatInfo(
            /* [in] */ ISmsCbHeader* header,
            /* [in] */ ISmsCbLocation* location);

        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        /**
         * Compare the location code for this message to the current location code. The match is
         * relative to the geographical scope of the message, which determines whether the LAC
         * and Cell ID are saved in mLocation or set to -1 to match all values.
         *
         * @param plmn the current PLMN
         * @param lac the current Location Area (GSM) or Service Area (UMTS)
         * @param cid the current Cell ID
         * @return true if this message is valid for the current location; false otherwise
         */
        CARAPI MatchesLocation(
            /* [in] */ const String& plmn,
            /* [in] */ Int32 lac,
            /* [in] */ Int32 cid,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<ISmsCbHeader> mHeader;
        AutoPtr<ISmsCbLocation> mLocation;
    };

public:
    CGsmCellBroadcastHandler();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    /**
     * Create a new CellBroadcastHandler.
     * @param context the context to use for dispatching Intents
     * @return the new handler
     */
    static CARAPI_(AutoPtr<IGsmCellBroadcastHandler>) MakeGsmCellBroadcastHandler(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

protected:
    CARAPI_(void) OnQuitting();

    /**
     * Handle 3GPP-format Cell Broadcast messages sent from radio.
     *
     * @param message the message to process
     * @return true if an ordered broadcast was sent; false on failure
     */
    CARAPI HandleSmsMessage(
        /* [in] */ IMessage* message,
        /* [out] */ Boolean* result);

private:
    /**
     * Handle 3GPP format SMS-CB message.
     * @param ar the AsyncResult containing the received PDUs
     */
    CARAPI_(AutoPtr<ISmsCbMessage>) HandleGsmBroadcastSms(
        /* [in] */ AsyncResult* ar);

private:
    static const Boolean VDBG = FALSE;  // log CB PDU data

    /** This map holds incomplete concatenated messages waiting for assembly. */
    AutoPtr<IHashMap> mSmsCbPageMap; // = new HashMap<SmsCbConcatInfo, byte[][]>(4);
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCELLBROADCASTHANDLER_H__
