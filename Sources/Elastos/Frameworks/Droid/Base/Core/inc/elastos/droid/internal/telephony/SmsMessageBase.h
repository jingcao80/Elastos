
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSMESSAGEBASE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSMESSAGEBASE_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsMessageBase
    : public Object
    , public ISmsMessageBase
{
public:
    // TODO(): This class is duplicated in SmsMessage.java. Refactor accordingly.
    class SubmitPduBase: public Object
    {
    public:
        virtual ~SubmitPduBase() = 0;

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        AutoPtr<ArrayOf<Byte> > mEncodedScAddress; // Null if not applicable.
        AutoPtr<ArrayOf<Byte> > mEncodedMessage;
    };

public:
    CAR_INTERFACE_DECL()

    SmsMessageBase();

    CARAPI GetServiceCenterAddress(
        /* [out] */ String* result);

    CARAPI GetOriginatingAddress(
        /* [out] */ String* result);

    CARAPI GetDisplayOriginatingAddress(
        /* [out] */ String* result);

    CARAPI GetMessageBody(
        /* [out] */ String* result);

    /**
     * Returns the class of this message.
     */
    virtual CARAPI GetMessageClass(
        /* [out] */ MessageClass* result) = 0;

    CARAPI GetDisplayMessageBody(
        /* [out] */ String* result);

    CARAPI GetPseudoSubject(
        /* [out] */ String* result);

    CARAPI GetTimestampMillis(
        /* [out] */ Int64* result);

    CARAPI IsEmail(
        /* [out] */ Boolean* result);

    CARAPI GetEmailBody(
        /* [out] */ String* result);

    CARAPI GetEmailFrom(
        /* [out] */ String* result);

    /**
     * Get protocol identifier.
     */
    virtual CARAPI GetProtocolIdentifier(
        /* [out] */ Int32* result) = 0;

    /**
     * See TS 23.040 9.2.3.9 returns true if this is a "replace short message"
     * SMS
     */
    virtual CARAPI IsReplace(
        /* [out] */ Boolean* result) = 0;

    /**
     * Returns true for CPHS MWI toggle message.
     *
     * @return true if this is a CPHS MWI toggle message See CPHS 4.2 section
     *         B.4.2
     */
    virtual CARAPI IsCphsMwiMessage(
        /* [out] */ Boolean* result) = 0;

    /**
     * returns true if this message is a CPHS voicemail / message waiting
     * indicator (MWI) clear message
     */
    virtual CARAPI IsMWIClearMessage(
        /* [out] */ Boolean* result) = 0;

    /**
     * returns true if this message is a CPHS voicemail / message waiting
     * indicator (MWI) set message
     */
    virtual CARAPI IsMWISetMessage(
        /* [out] */ Boolean* result) = 0;

    /**
     * returns true if this message is a "Message Waiting Indication Group:
     * Discard Message" notification and should not be stored.
     */
    virtual CARAPI IsMwiDontStore(
        /* [out] */ Boolean* result) = 0;

    CARAPI GetUserData(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI GetUserDataHeader(
        /* [out] */ ISmsHeader** result);

    CARAPI GetPdu(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * For an SMS-STATUS-REPORT message, this returns the status field from
     * the status report.  This field indicates the status of a previously
     * submitted SMS, if requested.  See TS 23.040, 9.2.3.15 TP-Status for a
     * description of values.
     *
     * @return 0 indicates the previously sent message was received.
     *         See TS 23.040, 9.9.2.3.15 for a description of other possible
     *         values.
     */
    virtual CARAPI GetStatus(
        /* [out] */ Int32* result) = 0;

    /**
     * Return true iff the message is a SMS-STATUS-REPORT message.
     */
    virtual CARAPI IsStatusReportMessage(
        /* [out] */ Boolean* result) = 0;

    /**
     * Returns true iff the <code>TP-Reply-Path</code> bit is set in
     * this message.
     */
    virtual CARAPI IsReplyPathPresent(
        /* [out] */ Boolean* result) = 0;

    CARAPI GetStatusOnIcc(
        /* [out] */ Int32* result);

    CARAPI GetIndexOnIcc(
        /* [out] */ Int32* result);

    CARAPI GetRecipientAddress(
        /* [out] */ String* result);

protected:
    void ParseMessageBody();
    void ExtractEmailAddressFromMessageBody();

protected:
    /** {@hide} The address of the SMSC. May be NULL */
    String mScAddress;

    /** {@hide} The address of the sender */
    AutoPtr<ISmsAddress> mOriginatingAddress;

    /** {@hide} The address of the receiver */
    AutoPtr<ISmsAddress> mRecipientAddress;

    /** {@hide} The message body as a string. May be NULL if the message isn't text */
    String mMessageBody;

    /** {@hide} */
    String mPseudoSubject;

    /** {@hide} Non-NULL if this is an email gateway message */
    String mEmailFrom;

    /** {@hide} Non-NULL if this is an email gateway message */
    String mEmailBody;

    /** {@hide} */
    Boolean mIsEmail;

    /** {@hide} */
    Int64 mScTimeMillis;

    /** {@hide} The raw PDU of the message */
    AutoPtr<ArrayOf<Byte> > mPdu;

    /** {@hide} The raw bytes for the user data section of the message */
    AutoPtr<ArrayOf<Byte> > mUserData;

    /** {@hide} */
    AutoPtr<ISmsHeader> mUserDataHeader;

    // "Message Waiting Indication Group"
    // 23.038 Section 4
    /** {@hide} */
    Boolean mIsMwi;

    /** {@hide} */
    Boolean mMwiSense;

    /** {@hide} */
    Boolean mMwiDontStore;

    /**
     * Indicates status for messages stored on the ICC.
     */
    Int32 mStatusOnIcc;

    /**
     * Record index of message in the EF.
     */
    Int32 mIndexOnIcc;

    /** TP-Message-Reference - Message Reference of sent message. @hide */
    Int32 mMessageRef;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSMESSAGEBASE_H__
