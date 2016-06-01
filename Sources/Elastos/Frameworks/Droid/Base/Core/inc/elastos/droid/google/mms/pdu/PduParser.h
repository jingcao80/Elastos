#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPARSER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPARSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.google.android.mms.pdu;
// import com.google.android.mms.ContentType;
// import com.google.android.mms.InvalidHeaderValueException;
// import com.google.android.mms.pdu.EncodedStringValue;
// import android.util.Log;
// import java.io.ByteArrayInputStream;
// import java.io.ByteArrayOutputStream;
// import java.io.UnsupportedEncodingException;
// import java.util.Arrays;
// import java.util.HashMap;
// import android.content.res.Resources;

using Elastos::Io::IByteArrayInputStream;
using Com.google.android.mms.pdu.EncodedStringValue;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class PduParser
    : public Object
    , public IPduParser
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Constructor.
      *
      * @param pduDataStream pdu data to be parsed
      */
    PduParser(
        /* [in] */ ArrayOf<Byte>* pduDataStream);

    /**
      * Parse the pdu.
      *
      * @return the pdu structure if parsing successfully.
      *         null if parsing error happened or mandatory fields are not set.
      */
    virtual CARAPI Parse(
        /* [out] */ IGenericPdu** result);

protected:
    /**
      * Parse pdu headers.
      *
      * @param pduDataStream pdu data input stream
      * @return headers in PduHeaders structure, null when parse fail
      */
    virtual CARAPI_(AutoPtr<IPduHeaders>) ParseHeaders(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse pdu parts.
      *
      * @param pduDataStream pdu data input stream
      * @return parts in PduBody structure
      */
    static CARAPI_(AutoPtr<IPduBody>) ParseParts(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse unsigned integer.
      *
      * @param pduDataStream pdu data input stream
      * @return the integer, -1 when failed
      */
    static CARAPI_(Int32) ParseUnsignedInt(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse value length.
      *
      * @param pduDataStream pdu data input stream
      * @return the integer
      */
    static CARAPI_(Int32) ParseValueLength(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse encoded string value.
      *
      * @param pduDataStream pdu data input stream
      * @return the EncodedStringValue
      */
    static CARAPI_(AutoPtr<IEncodedStringValue>) ParseEncodedStringValue(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse Text-String or Quoted-String.
      *
      * @param pduDataStream pdu data input stream
      * @param stringType TYPE_TEXT_STRING or TYPE_QUOTED_STRING
      * @return the string without End-of-string in byte array
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ParseWapString(
        /* [in] */ IByteArrayInputStream* pduDataStream,
        /* [in] */ Int32 stringType);

    /**
      * Check TOKEN data defined in RFC2616.
      * @param ch checking data
      * @return true when ch is TOKEN, false when ch is not TOKEN
      */
    static CARAPI_(Boolean) IsTokenCharacter(
        /* [in] */ Int32 ch);

    /**
      * Check TEXT data defined in RFC2616.
      * @param ch checking data
      * @return true when ch is TEXT, false when ch is not TEXT
      */
    static CARAPI_(Boolean) IsText(
        /* [in] */ Int32 ch);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetWapString(
        /* [in] */ IByteArrayInputStream* pduDataStream,
        /* [in] */ Int32 stringType);

    /**
      * Extract a byte value from the input stream.
      *
      * @param pduDataStream pdu data input stream
      * @return the byte
      */
    static CARAPI_(Int32) ExtractByteValue(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse Short-Integer.
      *
      * @param pduDataStream pdu data input stream
      * @return the byte
      */
    static CARAPI_(Int32) ParseShortInteger(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse Long-Integer.
      *
      * @param pduDataStream pdu data input stream
      * @return long integer
      */
    static CARAPI_(Int64) ParseLongInteger(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * Parse Integer-Value.
      *
      * @param pduDataStream pdu data input stream
      * @return long integer
      */
    static CARAPI_(Int64) ParseIntegerValue(
        /* [in] */ IByteArrayInputStream* pduDataStream);

    /**
      * To skip length of the wap value.
      *
      * @param pduDataStream pdu data input stream
      * @param length area size
      * @return the values in this area
      */
    static CARAPI_(Int32) SkipWapValue(
        /* [in] */ IByteArrayInputStream* pduDataStream,
        /* [in] */ Int32 length);

    /**
      * Parse content type parameters. For now we just support
      * four parameters used in mms: "type", "start", "name", "charset".
      *
      * @param pduDataStream pdu data input stream
      * @param map to store parameters of Content-Type field
      * @param length length of all the parameters
      */
    static CARAPI_(void) ParseContentTypeParams(
        /* [in] */ IByteArrayInputStream* pduDataStream,
        /* [in] */ IHashMap* map,// Integer*, Object
        /* [in] */ IInteger32* length);

    /**
      * Parse content type.
      *
      * @param pduDataStream pdu data input stream
      * @param map to store parameters in Content-Type header field
      * @return Content-Type value
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) ParseContentType(
        /* [in] */ IByteArrayInputStream* pduDataStream,
        /* [in] */ IHashMap* map); // Integer*, Object

    /**
      * Parse part's headers.
      *
      * @param pduDataStream pdu data input stream
      * @param part to store the header informations of the part
      * @param length length of the headers
      * @return true if parse successfully, false otherwise
      */
    static CARAPI_(Boolean) ParsePartHeaders(
        /* [in] */ IByteArrayInputStream* pduDataStream,
        /* [in] */ IPduPart* part,
        /* [in] */ Int32 length);

    /**
      * Check mandatory headers of a pdu.
      *
      * @param headers pdu headers
      * @return true if the pdu has all of the mandatory headers, false otherwise.
      */
    static CARAPI_(Boolean) CheckMandatoryHeader(
        /* [in] */ IPduHeaders* headers);

private:
    /**
      * Log status.
      *
      * @param text log information
      */
    static CARAPI_(void) Log(
        /* [in] */ const String& text);

    /**
      * Check the position of a specified part.
      *
      * @param part the part to be checked
      * @return part position, THE_FIRST_PART when it's the
      * first one, THE_LAST_PART when it's the last one.
      */
    static CARAPI_(Int32) CheckPartPosition(
        /* [in] */ IPduPart* part);

private:
    /**
      *  The next are WAP values defined in WSP specification.
      */
    static const Int32 QUOTE = 127;
    static const Int32 LENGTH_QUOTE = 31;
    static const Int32 TEXT_MIN = 32;
    static const Int32 TEXT_MAX = 127;
    static const Int32 SHORT_INTEGER_MAX = 127;
    static const Int32 SHORT_LENGTH_MAX = 30;
    static const Int32 LONG_INTEGER_LENGTH_MAX = 8;
    static const Int32 QUOTED_STRING_FLAG = 34;
    static const Int32 END_STRING_FLAG = 0x00;
    //The next two are used by the interface "parseWapString" to
    //distinguish Text-String and Quoted-String.
    static const Int32 TYPE_TEXT_STRING = 0;
    static const Int32 TYPE_QUOTED_STRING = 1;
    static const Int32 TYPE_TOKEN_STRING = 2;
    /**
      * Specify the part position.
      */
    static const Int32 THE_FIRST_PART = 0;
    static const Int32 THE_LAST_PART = 1;
    /**
      * The pdu data.
      */
    AutoPtr<IByteArrayInputStream> mPduDataStream;
    /**
      * Store pdu headers
      */
    AutoPtr<IPduHeaders> mHeaders;
    /**
      * Store pdu parts.
      */
    AutoPtr<IPduBody> mBody;
    /**
      * Store the "type" parameter in "Content-Type" header field.
      */
    static AutoPtr<ArrayOf<Byte> > mTypeParam;
    /**
      * Store the "start" parameter in "Content-Type" header field.
      */
    static AutoPtr<ArrayOf<Byte> > mStartParam;
    /**
      * The log tag.
      */
    static const String LOGTAG;
    static const Boolean DEBUG;
    static const Boolean LOCAL_LOGV;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPARSER_H__
