#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATSERVICE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/internal/telephony/cat/CommandParams.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"
#include "elastos/droid/internal/telephony/cat/ResponseData.h"
#include "elastos/droid/internal/telephony/cat/RilMessageDecoder.h"

// import android.content.Intent;
// import android.content.pm.PackageManager;
// import android.content.pm.ResolveInfo;
// import android.content.res.Resources.NotFoundException;
// import android.os.AsyncResult;
// import android.os.SystemProperties;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.SubscriptionController;
// import com.android.internal.telephony.uicc.IccUtils;
// import com.android.internal.telephony.uicc.UiccCard;
// import java.util.List;
// import java.util.Locale;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRefreshResponse;
using Elastos::Droid::Internal::Telephony::Uicc::CardState;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::IO::IByteArrayOutputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.IDLE_SCREEN_AVAILABLE_EVENT;
//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.LANGUAGE_SELECTION_EVENT;
//// import static com.android.internal.telephony.cat.CatCmdMessage.
// SetupEventListConstants.HCI_CONNECTIVITY_EVENT;
//class RilMessage
//    : public Object
//{
//public:
//    RilMessage(
//        /* [in] */ Int32 msgId,
//        /* [in] */ const String& rawData);
//
//    RilMessage(
//        /* [in] */ RilMessage* other);
//
//public:
//    Int32 mId;
//    AutoPtr<IInterface> mData;
//    ResultCode mResCode;
//};

/**
  * Class that implements SIM Toolkit Telephony Service. Interacts with the RIL
  * and application.
  *
  * {@hide}
  */
class CatService
    : public Handler
    , public IAppInterface
{
public:
    CAR_INTERFACE_DECL();

    CatService();
    /* For multisim catservice should not be singleton */
    CARAPI constructor(
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IContext* context,
        /* [in] */ IIccFileHandler* fh,
        /* [in] */ Int32 slotId);

    virtual CARAPI Dispose();

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    // @Override
    // synchronized
    CARAPI OnCmdResponse(
        /* [in] */ ICatResponseMessage* resMsg);

    virtual CARAPI UpdateIccAvailability();

protected:
    // @Override
    CARAPI Finalize();

private:
    CARAPI_(void) HandleRilMsg(
        /* [in] */ RilMessage* rilMsg);

    /**
      * This function validates the events in SETUP_EVENT_LIST which are currently
      * supported by the Android framework. In case of SETUP_EVENT_LIST has NULL events
      * or no events, all the events need to be reset.
      */
    CARAPI_(Boolean) IsSupportedSetupEventCommand(
        /* [in] */ ICatCmdMessage* cmdMsg);

    /**
      * Handles RIL_UNSOL_STK_EVENT_NOTIFY or RIL_UNSOL_STK_PROACTIVE_COMMAND command
      * from RIL.
      * Sends valid proactive command data to the application using intents.
      * RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE will be send back if the command is
      * from RIL_UNSOL_STK_PROACTIVE_COMMAND.
      */
    CARAPI_(void) HandleCommand(
        /* [in] */ CommandParams* cmdParams,
        /* [in] */ Boolean isProactiveCmd);

    CARAPI_(void) BroadcastCatCmdIntent(
        /* [in] */ ICatCmdMessage* cmdMsg);

    /**
      * Handles RIL_UNSOL_STK_SESSION_END unsolicited command from RIL.
      *
      */
    CARAPI_(void) HandleSessionEnd();

    CARAPI_(void) SendTerminalResponse(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ResultCode resultCode,
        /* [in] */ Boolean includeAdditionalInfo,
        /* [in] */ Int32 additionalInfo,
        /* [in] */ ResponseData* resp);

    CARAPI_(void) EncodeOptionalTags(
        /* [in] */ CommandDetails* cmdDet,
        /* [in] */ ResultCode resultCode,
        /* [in] */ IInput* cmdInput,
        /* [in] */ IByteArrayOutputStream* buf);

    CARAPI_(void) GetInKeyResponse(
        /* [in] */ IByteArrayOutputStream* buf,
        /* [in] */ IInput* cmdInput);

    CARAPI_(void) GetPliResponse(
        /* [in] */ IByteArrayOutputStream* buf);

    CARAPI_(void) SendMenuSelection(
        /* [in] */ Int32 menuId,
        /* [in] */ Boolean helpRequired);

    CARAPI_(void) EventDownload(
        /* [in] */ Int32 event,
        /* [in] */ Int32 sourceId,
        /* [in] */ Int32 destinationId,
        /* [in] */ ArrayOf<Byte>* additionalInfo,
        /* [in] */ Boolean oneShot);

    /**
      ** This function sends a CARD status (ABSENT, PRESENT, REFRESH) to STK_APP.
      ** This is triggered during ICC_REFRESH or CARD STATE changes. In case
      ** REFRESH, additional information is sent in 'refresh_result'
      **
      **/
    CARAPI_(void) BroadcastCardStateAndIccRefreshResp(
        /* [in] */ CardState cardState,
        /* [in] */ IIccRefreshResponse* iccRefreshState);

    CARAPI_(void) BroadcastAlphaMessage(
        /* [in] */ const String& alphaString);

    CARAPI_(Boolean) ValidateResponse(
        /* [in] */ ICatResponseMessage* resMsg);

    CARAPI_(Boolean) RemoveMenu(
        /* [in] */ IMenu* menu);

    CARAPI_(void) HandleCmdResponse(
        /* [in] */ ICatResponseMessage* resMsg);

    CARAPI_(Boolean) IsStkAppInstalled();

    /**
      * Samsung STK SEND_SMS
      * @param cmdPar
      */
    CARAPI_(void) HandleProactiveCommandSendSMS(
        /* [in] */ SendSMSParams* cmdPar);

    /**
      * Samsung STK SEND_USSD
      * @param cmdPar
      */
    CARAPI_(void) HandleProactiveCommandSendUSSD(
        /* [in] */ SendUSSDParams* cmdPar);

    CARAPI_(void) CancelTimeOut();

    CARAPI_(void) StartTimeOut(
        /* [in] */ Int32 timeout,
        /* [in] */ Int32 delay);

public:
    // Service constants.
    static const Int32 MSG_ID_SESSION_END = 1;
    static const Int32 MSG_ID_PROACTIVE_COMMAND = 2;
    static const Int32 MSG_ID_EVENT_NOTIFY = 3;
    static const Int32 MSG_ID_CALL_SETUP = 4;
    static const Int32 MSG_ID_REFRESH = 5;
    static const Int32 MSG_ID_RESPONSE = 6;
    static const Int32 MSG_ID_ICC_CHANGED = 7;
    static const Int32 MSG_ID_ALPHA_NOTIFY = 8;
    static const Int32 MSG_ID_RIL_MSG_DECODED = 9;
    static const Int32 MSG_ID_TIMEOUT = 10;
    // Samsung STK
    static const Int32 MSG_ID_SEND_SMS_RESULT = 11;
    static const String STK_DEFAULT;
    // Samsung STK SEND_SMS
    static const Int32 WAITING_SMS_RESULT = 2;
    static const Int32 WAITING_SMS_RESULT_TIME = 60000;
    static const Int32 SMS_SEND_OK = 0;
    static const Int32 SMS_SEND_FAIL = 32790;
    static const Int32 SMS_SEND_RETRY = 32810;

private:
    static const Boolean DBG;
    // Class members
    AutoPtr<ICommandsInterface> mCmdIf;
    AutoPtr<IContext> mContext;
    AutoPtr<ICatCmdMessage> mCurrntCmd;
    AutoPtr<ICatCmdMessage> mMenuCmd;
    AutoPtr<RilMessageDecoder> mMsgDecoder;
    Boolean mStkAppInstalled;
    AutoPtr<IUiccController> mUiccController;
    CardState mCardState;
    // Samsung STK
    Int32 mTimeoutDest;
    Int32 mCallControlResultCode;
    // Samsung STK
    //Events to signal SIM REFRESH notificatations
    static const Int32 MSG_ID_ICC_REFRESH = 30;
    static const Int32 DEV_ID_KEYPAD = 0x01;
    static const Int32 DEV_ID_DISPLAY = 0x02;
    static const Int32 DEV_ID_UICC = 0x81;
    static const Int32 DEV_ID_TERMINAL = 0x82;
    static const Int32 DEV_ID_NETWORK = 0x83;
    AutoPtr<IHandlerThread> mHandlerThread;
    Int32 mSlotId;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATSERVICE_H__
