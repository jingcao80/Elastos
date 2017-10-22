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

#ifndef __ELASTOS_DROID_TELEPHONY_GSM_GSMSMSMESSAGE_H__
#define __ELASTOS_DROID_TELEPHONY_GSM_GSMSMSMESSAGE_H__

#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package android.telephony.gsm;
// import android.telephony.TelephonyManager;
// import com.android.internal.telephony.GsmAlphabet;
// import com.android.internal.telephony.SmsHeader;
// import com.android.internal.telephony.SmsMessageBase;
//TODO using Elastos::Droid::Telephony::Gsm::MessageClass;
using Elastos::Droid::Telephony::Gsm::ISubmitPdu;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::ISmsMessageBaseSubmitPduBase;
//using Com.android.internal.telephony.SmsMessageBase;

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

/**
  * A Short Message Service message.
  * @deprecated Replaced by android.telephony.SmsMessage that supports both GSM and CDMA.
  */
// @Deprecated
class GSMSmsMessage
    : public Object
    , public ISmsMessage
{
public:
    /** @deprecated Use android.telephony.SmsMessage. */
    // @Deprecated
    class SubmitPdu
        : public Object
        , public ISubmitPdu
    {
        friend class GSMSmsMessage;

    public:
        CAR_INTERFACE_DECL();

        //Constructor
        /** @deprecated Use android.telephony.SmsMessage. */
        // @Deprecated
        SubmitPdu();

        CARAPI constructor();

        /** @deprecated Use android.telephony.SmsMessage. */
        // @Override
        // @Deprecated
        CARAPI ToString(
            /* [out] */ String* str);

    protected:
        /** @deprecated Use android.telephony.SmsMessage.
          * {@hide}
          */
        // @Deprecated
        SubmitPdu(
           /* [in] */ ISmsMessageBaseSubmitPduBase* spb);

    public:
        /** @deprecated Use android.telephony.SmsMessage. */
        // @Deprecated
        AutoPtr<ArrayOf<Byte> > encodedScAddress; // Null if not applicable.
        /** @deprecated Use android.telephony.SmsMessage. */
        // @Deprecated public
        AutoPtr<ArrayOf<Byte> > encodedMessage;
    };

public:
    CAR_INTERFACE_DECL();

    // Constructor
    /** @deprecated Use android.telephony.SmsMessage. */
    // @Deprecated
    GSMSmsMessage();

    CARAPI constructor();

    /**
      * Create an SmsMessage from a raw PDU.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    static CARAPI_(AutoPtr<GSMSmsMessage>) CreateFromPdu(
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
      * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
      * length in bytes (not hex chars) less the SMSC header
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    static CARAPI_(Int32) GetTPLayerLengthForPDU(
        /* [in] */ const String& pdu);

    /**
      * Calculates the number of SMS's required to encode the message body and
      * the number of characters remaining until the next message, given the
      * current encoding.
      *
      * @param messageBody the message to encode
      * @param use7bitOnly if true, characters that are not part of the GSM
      *         alphabet are counted as a single space char.  If false, a
      *         messageBody containing non-GSM alphabet characters is calculated
      *         for 16-bit encoding.
      * @return an int[4] with int[0] being the number of SMS's required, int[1]
      *         the number of code units used, and int[2] is the number of code
      *         units remaining until the next message. int[3] is the encoding
      *         type that should be used for the message.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    static CARAPI_(AutoPtr< ArrayOf<Int32> >) CalculateLength(
        /* [in] */ ICharSequence* messageBody,
        /* [in] */ Boolean use7bitOnly);

    /**
      * Calculates the number of SMS's required to encode the message body and
      * the number of characters remaining until the next message, given the
      * current encoding.
      *
      * @param messageBody the message to encode
      * @param use7bitOnly if true, characters that are not part of the GSM
      *         alphabet are counted as a single space char.  If false, a
      *         messageBody containing non-GSM alphabet characters is calculated
      *         for 16-bit encoding.
      * @return an int[4] with int[0] being the number of SMS's required, int[1]
      *         the number of code units used, and int[2] is the number of code
      *         units remaining until the next message. int[3] is the encoding
      *         type that should be used for the message.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    static CARAPI_(AutoPtr< ArrayOf<Int32> >) CalculateLength(
        /* [in] */ const String& messageBody,
        /* [in] */ Boolean use7bitOnly);

    /**
      * Get an SMS-SUBMIT PDU for a destination address and a message
      *
      * @param scAddress Service Centre address.  Null means use default.
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      * @deprecated Use android.telephony.SmsMessage.
      * @hide
      */
    // @Deprecated
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested,
        /* [in] */ ArrayOf<Byte>* header);

    /**
      * Get an SMS-SUBMIT PDU for a destination address and a message
      *
      * @param scAddress Service Centre address.  Null means use default.
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ const String& message,
        /* [in] */ Boolean statusReportRequested);

    /**
      * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
      *
      * @param scAddress Service Centre address. null == use default
      * @param destinationAddress the address of the destination for the message
      * @param destinationPort the port to deliver the message to at the
      *        destination
      * @param data the dat for the message
      * @return a <code>SubmitPdu</code> containing the encoded SC
      *         address, if applicable, and the encoded message.
      *         Returns null on encode error.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    static CARAPI_(AutoPtr<SubmitPdu>) GetSubmitPdu(
        /* [in] */ const String& scAddress,
        /* [in] */ const String& destinationAddress,
        /* [in] */ Int16 destinationPort,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean statusReportRequested);

    /**
      * Returns the address of the SMS service center that relayed this message
      * or null if there is none.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetServiceCenterAddress(
        /* [out] */ String* address);

    /**
      * Returns the originating address (sender) of this SMS message in String
      * form or null if unavailable
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetOriginatingAddress(
        /* [out] */ String* address);

    /**
      * Returns the originating address, or email from address if this message
      * was from an email gateway. Returns null if originating address
      * unavailable.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetDisplayOriginatingAddress(
        /* [in] */ String* address);

    /**
      * Returns the message body as a String, if it exists and is text based.
      * @return message body is there is one, otherwise null
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetMessageBody(
        /* [out] */ String* messageBody);

    /**
      * Returns the class of this message.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetMessageClass(
        /* [out] */ Int32/*TODO MessageClass*/* messageClass);

    /**
      * Returns the message body, or email message body if this message was from
      * an email gateway. Returns null if message body unavailable.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetDisplayMessageBody(
        /* [out] */ String* messageBody);

    /**
      * Unofficial convention of a subject line enclosed in parens empty string
      * if not present
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetPseudoSubject(
        /* [out] */ String* subject);

    /**
      * Returns the service centre timestamp in currentTimeMillis() format
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetTimestampMillis(
        /* [out] */ Int64* timestamp);

    /**
      * Returns true if message is an email.
      *
      * @return true if this message came through an email gateway and email
      *         sender / subject / parsed body are available
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsEmail(
        /* [out] */ Boolean* isEmail);

    /**
      * @return if isEmail() is true, body of the email sent through the gateway.
      *         null otherwise
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetEmailBody(
        /* [out] */ String* emailBody);

    /**
      * @return if isEmail() is true, email from address of email sent through
      *         the gateway. null otherwise
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetEmailFrom(
        /* [out] */ String* emailFrom);

    /**
      * Get protocol identifier.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetProtocolIdentifier(
        /* [out] */ Int32* protocolID);

    /**
      * See TS 23.040 9.2.3.9 returns true if this is a "replace short message" SMS
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsReplace(
        /* [out] */ Boolean* isReplace);

    /**
      * Returns true for CPHS MWI toggle message.
      *
      * @return true if this is a CPHS MWI toggle message See CPHS 4.2 section B.4.2
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsCphsMwiMessage(
        /* [out] */ Boolean* isCphsMwiMessage);

    /**
      * returns true if this message is a CPHS voicemail / message waiting
      * indicator (MWI) clear message
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsMWIClearMessage(
        /* [out] */ Boolean* isMWIClearMessage);

    /**
      * returns true if this message is a CPHS voicemail / message waiting
      * indicator (MWI) set message
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsMWISetMessage(
        /* [out] */ Boolean* isMWISetMessage);

    /**
      * returns true if this message is a "Message Waiting Indication Group:
      * Discard Message" notification and should not be stored.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsMwiDontStore(
        /* [out] */ Boolean* isMwiDontStore);

    /**
      * returns the user data section minus the user data header if one was present.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetUserData(
        /* [out] */ ArrayOf<Byte>** userData);

    /* Not part of the SDK interface and only needed by specific classes:
        protected SmsHeader getUserDataHeader()
     */
    /**
      * Returns the raw PDU for the message.
      *
      * @return the raw PDU for the message.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetPdu(
        /* [out */ ArrayOf<Byte>** pdu);

    /**
      * Returns the status of the message on the SIM (read, unread, sent, unsent).
      *
      * @return the status of the message on the SIM.  These are:
      *         SmsManager.STATUS_ON_SIM_FREE
      *         SmsManager.STATUS_ON_SIM_READ
      *         SmsManager.STATUS_ON_SIM_UNREAD
      *         SmsManager.STATUS_ON_SIM_SEND
      *         SmsManager.STATUS_ON_SIM_UNSENT
      * @deprecated Use android.telephony.SmsMessage and getStatusOnIcc instead.
      */
    // @Deprecated
    CARAPI GetStatusOnSim(
        /* [out] */ Int32* status);

    /**
      * Returns the status of the message on the ICC (read, unread, sent, unsent).
      *
      * @return the status of the message on the ICC.  These are:
      *         SmsManager.STATUS_ON_ICC_FREE
      *         SmsManager.STATUS_ON_ICC_READ
      *         SmsManager.STATUS_ON_ICC_UNREAD
      *         SmsManager.STATUS_ON_ICC_SEND
      *         SmsManager.STATUS_ON_ICC_UNSENT
      * @deprecated Use android.telephony.SmsMessage.
      * @hide
      */
    // @Deprecated
    CARAPI GetStatusOnIcc(
        /* [out] */ Int32* statusOnIcc);

    /**
      * Returns the record index of the message on the SIM (1-based index).
      * @return the record index of the message on the SIM, or -1 if this
      *         SmsMessage was not created from a SIM SMS EF record.
      * @deprecated Use android.telephony.SmsMessage and getIndexOnIcc instead.
      */
    // @Deprecated
    CARAPI GetIndexOnSim(
        /* [out] */ Int32* indexOnSim);

    /**
      * Returns the record index of the message on the ICC (1-based index).
      * @return the record index of the message on the ICC, or -1 if this
      *         SmsMessage was not created from a ICC SMS EF record.
      * @deprecated Use android.telephony.SmsMessage.
      * @hide
      */
    // @Deprecated
    CARAPI GetIndexOnIcc(
        /* [out] */ Int32* indexOnIcc);

    /**
      * GSM:
      * For an SMS-STATUS-REPORT message, this returns the status field from
      * the status report.  This field indicates the status of a previously
      * submitted SMS, if requested.  See TS 23.040, 9.2.3.15 TP-Status for a
      * description of values.
      * CDMA:
      * For not interfering with status codes from GSM, the value is
      * shifted to the bits 31-16.
      * The value is composed of an error class (bits 25-24) and a status code (bits 23-16).
      * Possible codes are described in C.S0015-B, v2.0, 4.5.21.
      *
      * @return 0 indicates the previously sent message was received.
      *         See TS 23.040, 9.9.2.3.15 and C.S0015-B, v2.0, 4.5.21
      *         for a description of other possible values.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI GetStatus(
        /* [out] */ Int32* status);

    /**
      * Return true iff the message is a SMS-STATUS-REPORT message.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsStatusReportMessage(
        /* [out] */ Boolean* isStatusReportMessage);

    /**
      * Returns true iff the <code>TP-Reply-Path</code> bit is set in
      * this message.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    CARAPI IsReplyPathPresent(
        /* [out] */ Boolean* isReplyPathPresent);

public:
    // @Deprecated public
    AutoPtr<ISmsMessageBase> mWrappedSmsMessage;

private:
    CARAPI constructor(
        /* [in] */ ISmsMessageBase* smb);

    /** This method returns the reference to a specific
      *  SmsMessage object, which is used for accessing its static methods.
      * @return Specific SmsMessage.
      * @deprecated Use android.telephony.SmsMessage.
      */
    // @Deprecated
    static CARAPI_(AutoPtr<ISmsMessageBase>) GetSmsFacility();
};

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_GSM_SMSMESSAGE_H__

