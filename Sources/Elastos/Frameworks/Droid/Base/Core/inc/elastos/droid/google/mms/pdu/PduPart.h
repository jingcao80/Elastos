#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPART_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPART_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// package com.google.android.mms.pdu;
// import android.net.Uri;
// import java.util.HashMap;
// import java.util.Map;

using Elastos::Droid::Net::IUri;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

/**
  * The pdu part.
  */
class PduPart
    : public Object
    , public IPduPart
{
public:
    CAR_INTERFACE_DECL();

    /**
       * Empty Constructor.
       */
    PduPart();

    /**
       * Set part data. The data are stored as byte array.
       *
       * @param data the data
       */
    virtual CARAPI SetData(
        /* [in] */ ArrayOf<Byte>* data);

    /**
       * @return A copy of the part data or null if the data wasn't set or
       *         the data is stored as Uri.
       * @see #getDataUri
       */
    virtual CARAPI GetData(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * @return The length of the data, if this object have data, else 0.
      */
    virtual CARAPI GetDataLength(
        /* [out] */ Int32* result);

    /**
       * Set data uri. The data are stored as Uri.
       *
       * @param uri the uri
       */
    virtual CARAPI SetDataUri(
        /* [in] */ IUri* uri);

    /**
       * @return The Uri of the part data or null if the data wasn't set or
       *         the data is stored as byte array.
       * @see #getData
       */
    virtual CARAPI GetDataUri(
        /* [out] */ IUri** result);

    /**
       * Set Content-id value
       *
       * @param contentId the content-id value
       * @throws NullPointerException if the value is null.
       */
    virtual CARAPI SetContentId(
        /* [in] */ ArrayOf<Byte>* contentId);

    /**
       * Get Content-id value.
       *
       * @return the value
       */
    virtual CARAPI GetContentId(
        /* [out] */ ArrayOf<Byte>** result);

    /**
       * Set Char-set value.
       *
       * @param charset the value
       */
    virtual CARAPI SetCharset(
        /* [in] */ Int32 charset);

    /**
       * Get Char-set value
       *
       * @return the charset value. Return 0 if charset was not set.
       */
    virtual CARAPI GetCharset(
        /* [out] */ Int32* result);

    /**
       * Set Content-Location value.
       *
       * @param contentLocation the value
       * @throws NullPointerException if the value is null.
       */
    virtual CARAPI SetContentLocation(
        /* [in] */ ArrayOf<Byte>* contentLocation);

    /**
       * Get Content-Location value.
       *
       * @return the value
       *     return PduPart.disposition[0] instead of <Octet 128> (Form-data).
       *     return PduPart.disposition[1] instead of <Octet 129> (Attachment).
       *     return PduPart.disposition[2] instead of <Octet 130> (Inline).
       */
    virtual CARAPI GetContentLocation(
        /* [out] */ ArrayOf<Byte>** result);

    /**
       * Set Content-Disposition value.
       * Use PduPart.disposition[0] instead of <Octet 128> (Form-data).
       * Use PduPart.disposition[1] instead of <Octet 129> (Attachment).
       * Use PduPart.disposition[2] instead of <Octet 130> (Inline).
       *
       * @param contentDisposition the value
       * @throws NullPointerException if the value is null.
       */
    virtual CARAPI SetContentDisposition(
        /* [in] */ ArrayOf<Byte>* contentDisposition);

    /**
       * Get Content-Disposition value.
       *
       * @return the value
       */
    virtual CARAPI GetContentDisposition(
        /* [out] */ ArrayOf<Byte>** result);

    /**
       *  Set Content-Type value.
       *
       *  @param value the value
       *  @throws NullPointerException if the value is null.
       */
    virtual CARAPI SetContentType(
        /* [in] */ ArrayOf<Byte>* contentType);

    /**
       * Get Content-Type value of part.
       *
       * @return the value
       */
    virtual CARAPI GetContentType(
        /* [out] */ ArrayOf<Byte>** result);

    /**
       * Set Content-Transfer-Encoding value
       *
       * @param contentId the content-id value
       * @throws NullPointerException if the value is null.
       */
    virtual CARAPI SetContentTransferEncoding(
        /* [in] */ ArrayOf<Byte>* contentTransferEncoding);

    /**
       * Get Content-Transfer-Encoding value.
       *
       * @return the value
       */
    virtual CARAPI GetContentTransferEncoding(
        /* [out] */ ArrayOf<Byte>** result);

    /**
       * Set Content-type parameter: name.
       *
       * @param name the name value
       * @throws NullPointerException if the value is null.
       */
    virtual CARAPI SetName(
        /* [in] */ ArrayOf<Byte>* name);

    /**
       *  Get content-type parameter: name.
       *
       *  @return the name
       */
    virtual CARAPI GetName(
        /* [out] */ ArrayOf<Byte>** result);

    /**
       * Get Content-disposition parameter: filename
       *
       * @param fileName the filename value
       * @throws NullPointerException if the value is null.
       */
    virtual CARAPI SetFilename(
        /* [in] */ ArrayOf<Byte>* fileName);

    /**
       * Set Content-disposition parameter: filename
       *
       * @return the filename
       */
    virtual CARAPI GetFilename(
        /* [out] */ ArrayOf<Byte>** result);

    virtual CARAPI GenerateLocation(
        /* [out] */ String* result);

public:
    /**
      * Well-Known Parameters.
      */
    static const Int32 P_Q = 0x80;
    static const Int32 P_CHARSET = 0x81;
    static const Int32 P_LEVEL = 0x82;
    static const Int32 P_TYPE = 0x83;
    static const Int32 P_DEP_NAME = 0x85;
    static const Int32 P_DEP_FILENAME = 0x86;
    static const Int32 P_DIFFERENCES = 0x87;
    static const Int32 P_PADDING = 0x88;
    // This value of "TYPE" s used with Content-Type: multipart/related
    static const Int32 P_CT_MR_TYPE = 0x89;
    static const Int32 P_DEP_START = 0x8A;
    static const Int32 P_DEP_START_INFO = 0x8B;
    static const Int32 P_DEP_COMMENT = 0x8C;
    static const Int32 P_DEP_DOMAIN = 0x8D;
    static const Int32 P_MAX_AGE = 0x8E;
    static const Int32 P_DEP_PATH = 0x8F;
    static const Int32 P_SECURE = 0x90;
    static const Int32 P_SEC = 0x91;
    static const Int32 P_MAC = 0x92;
    static const Int32 P_CREATION_DATE = 0x93;
    static const Int32 P_MODIFICATION_DATE = 0x94;
    static const Int32 P_READ_DATE = 0x95;
    static const Int32 P_SIZE = 0x96;
    static const Int32 P_NAME = 0x97;
    static const Int32 P_FILENAME = 0x98;
    static const Int32 P_START = 0x99;
    static const Int32 P_START_INFO = 0x9A;
    static const Int32 P_COMMENT = 0x9B;
    static const Int32 P_DOMAIN = 0x9C;
    static const Int32 P_PATH = 0x9D;
    /**
      *  Header field names.
      */
    static const Int32 P_CONTENT_TYPE = 0x91;
    static const Int32 P_CONTENT_LOCATION = 0x8E;
    static const Int32 P_CONTENT_ID = 0xC0;
    static const Int32 P_DEP_CONTENT_DISPOSITION = 0xAE;
    static const Int32 P_CONTENT_DISPOSITION = 0xC5;
    // The next header is unassigned header, use reserved header(0x48) value.
    static const Int32 P_CONTENT_TRANSFER_ENCODING = 0xC8;
    /**
       * Content=Transfer-Encoding string.
       */
    static const String CONTENT_TRANSFER_ENCODING;
    /**
       * Value of Content-Transfer-Encoding.
       */
    static const String P_BINARY;
    static const String P_7BIT;
    static const String P_8BIT;
    static const String P_BASE64;
    static const String P_QUOTED_PRINTABLE;
    /**
       * Value of disposition can be set to PduPart when the value is octet in
       * the PDU.
       * "from-data" instead of Form-data<Octet 128>.
       * "attachment" instead of Attachment<Octet 129>.
       * "inline" instead of Inline<Octet 130>.
       */
    static const AutoPtr<ArrayOf<Byte> > DISPOSITION_FROM_DATA;
    static const AutoPtr<ArrayOf<Byte> > DISPOSITION_ATTACHMENT;
    static const AutoPtr<ArrayOf<Byte> > DISPOSITION_INLINE;
    /**
       * Content-Disposition value.
       */
    static const Int32 P_DISPOSITION_FROM_DATA = 0x80;
    static const Int32 P_DISPOSITION_ATTACHMENT = 0x81;
    static const Int32 P_DISPOSITION_INLINE = 0x82;

private:
    /**
       * Header of part.
       */
    AutoPtr<IMap> mPartHeader; // Integer, Object
    /**
       * Data uri.
       */
    AutoPtr<IUri> mUri;
    /**
       * Part data.
       */
    AutoPtr<ArrayOf<Byte> > mPartData;
    static const String TAG;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUPART_H__

