#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUCOMPOSER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUCOMPOSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.content.ContentResolver;
// import android.util.Log;
// import android.text.TextUtils;
// import java.io.FileNotFoundException;
// import java.io.IOException;
// import java.io.InputStream;
// import java.util.Arrays;
// import java.util.HashMap;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class PduComposer
    : public Object
    , public IPduComposer
{
private:
    /**
      *  Record current message informations.
      */
    class LengthRecordNode
        : public Object
    {
    public:
        AutoPtr<IByteArrayOutputStream> currentMessage;
        Int32 currentPosition;
        AutoPtr<LengthRecordNode> next;
    };

    /**
      * Mark current message position and stact size.
      */
    class PositionMarker
        : public Object
    {
    public:
        // Current stack size
        virtual CARAPI GetLength(
            /* [out] */ Int32* result);

    private:
        Int32 c_pos;
        // Current position
        Int32 currentStackSize;
    };

    /**
      * This implementation can be OPTIMIZED to use only
      * 2 buffers. This optimization involves changing BufferStack
      * only... Its usage (interface) will not change.
      */
    class BufferStack
        : public Object
    {
    public:
        /**
          *  Create a new message buffer and push it into the stack.
          */
        virtual CARAPI Newbuf();

        /**
          *  Pop the message before and record current message in the stack.
          */
        virtual CARAPI Pop();

        /**
          *  Append current message to the message before.
          */
        virtual CARAPI Copy();

        /**
          *  Mark current message position
          */
        virtual CARAPI Mark(
            /* [out] */ PositionMarker** result);

    public:
        Int32 stackSize;

    private:
        AutoPtr<LengthRecordNode> stack;
        AutoPtr<LengthRecordNode> toCopy;
    };

public:
    CAR_INTERFACE_DECL();

    /**
      * Constructor.
      *
      * @param context the context
      * @param pdu the pdu to be composed
      */
    PduComposer(
        /* [in] */ IContext* context,
        /* [in] */ IGenericPdu* pdu);

    /**
      * Make the message. No need to check whether mandatory fields are set,
      * because the constructors of outgoing pdus are taking care of this.
      *
      * @return OutputStream of maked message. Return null if
      *         the PDU is invalid.
      */
    virtual CARAPI Make(
        /* [out] */ ArrayOf<Byte>** result);

protected:
    /**
      *  Copy buf to mMessage.
      */
    virtual CARAPI_(void) Arraycopy(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 length);

    /**
      * Append a byte to mMessage.
      */
    virtual CARAPI_(void) Append(
        /* [in] */ Int32 value);

    /**
      * Append short integer value to mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendShortInteger(
        /* [in] */ Int32 value);

    /**
      * Append an octet number between 128 and 255 into mMessage.
      * NOTE:
      * A value between 0 and 127 should be appended by using appendShortInteger.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendOctet(
        /* [in] */ Int32 number);

    /**
      * Append a short length into mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendShortLength(
        /* [in] */ Int32 value);

    /**
      * Append long integer into mMessage. it's used for really long integers.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendLongInteger(
        /* [in] */ Int64 longInt);

    /**
      * Append text string into mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendTextString(
        /* [in] */ ArrayOf<Byte>* text);

    /**
      * Append text string into mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendTextString(
        /* [in] */ const String& str);

    /**
      * Append encoded string value to mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendEncodedString(
        /* [in] */ IEncodedStringValue* enStr);

    /**
      * Append uintvar integer into mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendUintvarInteger(
        /* [in] */ Int64 value);

    /**
      * Append date value into mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendDateValue(
        /* [in] */ Int64 date);

    /**
      * Append value length to mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendValueLength(
        /* [in] */ Int64 value);

    /**
      * Append quoted string to mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendQuotedString(
        /* [in] */ ArrayOf<Byte>* text);

    /**
      * Append quoted string to mMessage.
      * This implementation doesn't check the validity of parameter, since it
      * assumes that the values are validated in the GenericPdu setter methods.
      */
    virtual CARAPI_(void) AppendQuotedString(
        /* [in] */ const String& str);

    /**
      * Check address type.
      *
      * @param address address string without the postfix stinng type,
      *        such as "/TYPE=PLMN", "/TYPE=IPv6" and "/TYPE=IPv4"
      * @return PDU_PHONE_NUMBER_ADDRESS_TYPE if it is phone number,
      *         PDU_EMAIL_ADDRESS_TYPE if it is email address,
      *         PDU_IPV4_ADDRESS_TYPE if it is ipv4 address,
      *         PDU_IPV6_ADDRESS_TYPE if it is ipv6 address,
      *         PDU_UNKNOWN_ADDRESS_TYPE if it is unknown.
      */
    static CARAPI_(Int32) CheckAddressType(
        /* [in] */ const String& address);

private:
    CARAPI_(AutoPtr<IEncodedStringValue>) AppendAddressType(
        /* [in] */ IEncodedStringValue* address);

    /**
      * Append header to mMessage.
      */
    CARAPI_(Int32) AppendHeader(
        /* [in] */ Int32 field);

    /**
      * Make ReadRec.Ind.
      */
    CARAPI_(Int32) MakeReadRecInd();

    /**
      * Make NotifyResp.Ind.
      */
    CARAPI_(Int32) MakeNotifyResp();

    /**
      * Make Acknowledge.Ind.
      */
    CARAPI_(Int32) MakeAckInd();

    /**
      * Make Send.req.
      */
    CARAPI_(Int32) MakeSendRetrievePdu(
        /* [in] */ Int32 type);

    /**
      * Make message body.
      */
    CARAPI_(Int32) MakeMessageBody(
        /* [in] */ Int32 type);

public:
    /**
      * Address regular expression string.
      */
    static const String REGEXP_PHONE_NUMBER_ADDRESS_TYPE;
    static const String REGEXP_EMAIL_ADDRESS_TYPE;
    static const String REGEXP_IPV6_ADDRESS_TYPE;
    static const String REGEXP_IPV4_ADDRESS_TYPE;

    /**
      * The postfix strings of address.
      */
    static const String STRING_PHONE_NUMBER_ADDRESS_TYPE;
    static const String STRING_IPV4_ADDRESS_TYPE;
    static const String STRING_IPV6_ADDRESS_TYPE;

protected:
    /**
      * The output message.
      */
    AutoPtr<IByteArrayOutputStream> mMessage;
    /**
      * Current visiting position of the mMessage.
      */
    Int32 mPosition;

private:
    /**
      * Address type.
      */
    static const Int32 PDU_PHONE_NUMBER_ADDRESS_TYPE = 1;
    static const Int32 PDU_EMAIL_ADDRESS_TYPE = 2;
    static const Int32 PDU_IPV4_ADDRESS_TYPE = 3;
    static const Int32 PDU_IPV6_ADDRESS_TYPE = 4;
    static const Int32 PDU_UNKNOWN_ADDRESS_TYPE = 5;
    /**
      * Error values.
      */
    static const Int32 PDU_COMPOSE_SUCCESS = 0;
    static const Int32 PDU_COMPOSE_CONTENT_ERROR = 1;
    static const Int32 PDU_COMPOSE_FIELD_NOT_SET = 2;
    static const Int32 PDU_COMPOSE_FIELD_NOT_SUPPORTED = 3;
    /**
      * WAP values defined in WSP spec.
      */
    static const Int32 QUOTED_STRING_FLAG = 34;
    static const Int32 END_STRING_FLAG = 0;
    static const Int32 LENGTH_QUOTE = 31;
    static const Int32 TEXT_MAX = 127;
    static const Int32 SHORT_INTEGER_MAX = 127;
    static const Int32 LONG_INTEGER_LENGTH_MAX = 8;
    /**
      * Block size when read data from InputStream.
      */
    static const Int32 PDU_COMPOSER_BLOCK_SIZE = 1024;
    /**
      * The PDU.
      */
    AutoPtr<IGenericPdu> mPdu;
    /**
      * Message compose buffer stack.
      */
    AutoPtr<BufferStack> mStack;
    /**
      * Content resolver.
      */
    /*const*/ AutoPtr<IContentResolver> mResolver;
    /**
      * Header of this pdu.
      */
    AutoPtr<IPduHeaders> mPduHeader;
    /**
      * Map of all content type
      */
    static AutoPtr<IHashMap> mContentTypeMap; // String, Integer
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUCOMPOSER_H__

