
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CUSIMDATADOWNLOADHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CUSIMDATADOWNLOADHANDLER_H__

#include "Elastos.Droid.App.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CUsimDataDownloadHandler.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::Uicc::IIccIoResult;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * Handler for SMS-PP data download messages.
 * See 3GPP TS 31.111 section 7.1.1
 */
CarClass(CUsimDataDownloadHandler)
    , public Handler
    , public IUsimDataDownloadHandler
{
public:
    TO_STRING_IMPL("CUsimDataDownloadHandler")

    CUsimDataDownloadHandler();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICommandsInterface* commandsInterface);

    /**
     * Handle SMS-PP data download messages. Normally these are automatically handled by the
     * radio, but we may have to deal with this type of SM arriving via the IMS stack. If the
     * data download service is not enabled, try to write to the USIM as an SMS, and send the
     * UICC response as the acknowledgment to the SMSC.
     *
     * @param ust the UsimServiceTable, to check if data download is enabled
     * @param smsMessage the SMS message to process
     * @return {@code Activity.RESULT_OK} on success; {@code RESULT_SMS_GENERIC_ERROR} on failure
     */
    CARAPI HandleUsimDataDownload(
         /* [in] */ IUsimServiceTable* ust,
         /* [in] */ ISmsMessage* smsMessage,
         /* [out] */ Int32* result);

    /**
     * Start an SMS-PP data download for the specified message. Can be called from a different
     * thread than this Handler is running on.
     *
     * @param smsMessage the message to process
     * @return {@code Activity.RESULT_OK} on success; {@code RESULT_SMS_GENERIC_ERROR} on failure
     */
    CARAPI StartDataDownload(
         /* [in] */ ISmsMessage* smsMessage,
         /* [out] */ Int32* result);

    /**
     * Handle UICC envelope response and send SMS acknowledgement.
     *
     * @param msg the message to handle
     */
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

private:
    CARAPI_(void) HandleDataDownload(
        /* [in] */ ISmsMessage* smsMessage);

    /**
     * Return the size in bytes of the envelope to send to the UICC, excluding the
     * SMS-PP download tag byte and length byte(s). If the size returned is <= 127,
     * the BER-TLV length will be encoded in 1 byte, otherwise 2 bytes are required.
     *
     * @param scAddressLength the length of the SMSC address, or zero if not present
     * @param tpduLength the length of the TPDU from the SMS-PP message
     * @return the number of bytes to allocate for the envelope command
     */
    static CARAPI_(Int32) GetEnvelopeBodyLength(
        /* [in] */ Int32 scAddressLength,
        /* [in] */ Int32 tpduLength);

    /**
     * Handle the response to the ENVELOPE command.
     * @param response UICC response encoded as hexadecimal digits. First two bytes are the
     *  UICC SW1 and SW2 status bytes.
     */
    CARAPI_(void) SendSmsAckForEnvelopeResponse(
        /* [in] */ IIccIoResult* response,
        /* [in] */ Int32 dcs,
        /* [in] */ Int32 pid);

    CARAPI_(void) AcknowledgeSmsWithError(
        /* [in] */ Int32 cause);

    /**
     * Returns whether the DCS is 7 bit. If so, set TP-UDL to the septet count of TP-UD;
     * otherwise, set TP-UDL to the octet count of TP-UD.
     * @param dcs the TP-Data-Coding-Scheme field from the original download SMS
     * @return true if the DCS specifies 7 bit encoding; false otherwise
     */
    static CARAPI_(Boolean) Is7bitDcs(
        /* [in] */ Int32 dcs);

private:
    static const String TAG; // = "UsimDataDownloadHandler";

    /** BER-TLV tag for SMS-PP download. TS 31.111 section 9.1. */
    static const Int32 BER_SMS_PP_DOWNLOAD_TAG      = 0xd1;

    /** Device identity value for UICC (destination). */
    static const Int32 DEV_ID_UICC        = 0x81;

    /** Device identity value for network (source). */
    static const Int32 DEV_ID_NETWORK     = 0x83;

    /** Message containing new SMS-PP message to process. */
    static const Int32 EVENT_START_DATA_DOWNLOAD = 1;

    /** Response to SMS-PP download envelope command. */
    static const Int32 EVENT_SEND_ENVELOPE_RESPONSE = 2;

    /** Result of writing SM to UICC (when SMS-PP service is not available). */
    static const Int32 EVENT_WRITE_SMS_COMPLETE = 3;

    AutoPtr<ICommandsInterface> mCi;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CUSIMDATADOWNLOADHANDLER_H__
