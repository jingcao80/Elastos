#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASERVICECATEGORYPROGRAMHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASERVICECATEGORYPROGRAMHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
//TODO #include "elastos/droid/internal/telephony/WakeLockStateMachine.h"

// package com.android.internal.telephony.cdma;
// import android.Manifest;
// import android.app.Activity;
// import android.app.AppOpsManager;
// import android.os.Bundle;
// import android.provider.Telephony.Sms.Intents;
// import android.telephony.PhoneNumberUtils;
// import android.telephony.SubscriptionManager;
// import android.telephony.cdma.CdmaSmsCbProgramData;
// import android.telephony.cdma.CdmaSmsCbProgramResults;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.cdma.sms.BearerData;
// import com.android.internal.telephony.cdma.sms.CdmaSmsAddress;
// import com.android.internal.telephony.cdma.sms.SmsEnvelope;
// import java.io.ByteArrayOutputStream;
// import java.io.DataOutputStream;
// import java.io.IOException;
// import java.util.ArrayList;

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IMessage;
//TODO using Elastos::Droid::Internal::Telephony::WakeLockStateMachine;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * Handle CDMA Service Category Program Data requests and responses.
  */
class CdmaServiceCategoryProgramHandler
    : public Object //TODO WakeLockStateMachine
    , public ICdmaServiceCategoryProgramHandler
{
private:
    class InnerBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        InnerBroadcastReceiver(
            /* [in] */ CdmaServiceCategoryProgramHandler* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CARAPI_(void) SendScpResults();

    private:
        CdmaServiceCategoryProgramHandler* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    /**
      * Create a new CDMA inbound SMS handler.
      */
    CdmaServiceCategoryProgramHandler(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* commandsInterface);

    /**
      * Create a new State machine for SCPD requests.
      * @param context the context to use
      * @param commandsInterface the radio commands interface
      * @return the new SCPD handler
      */
    static CARAPI_(AutoPtr<ICdmaServiceCategoryProgramHandler>) MakeScpHandler(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* commandsInterface);

protected:
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

private:
    /**
      * Send SCPD request to CellBroadcastReceiver as an ordered broadcast.
      * @param sms the CDMA SmsMessage containing the SCPD request
      * @return true if an ordered broadcast was sent; false on failure
      */
    CARAPI_(Boolean) HandleServiceCategoryProgramData(
        /* [in] */ ISmsMessage* sms);

public:
    AutoPtr<ICommandsInterface> mCi;

private:
    /**
      * Broadcast receiver to handle results of ordered broadcast. Sends the SCPD results
      * as a reply SMS, then sends a message to state machine to transition to idle.
      */
    AutoPtr<IBroadcastReceiver> mScpResultsReceiver;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASERVICECATEGORYPROGRAMHANDLER_H__

