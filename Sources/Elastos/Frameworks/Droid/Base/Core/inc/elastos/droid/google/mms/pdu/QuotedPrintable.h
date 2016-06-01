#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_QUOTEDPRINTABLE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_QUOTEDPRINTABLE_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.google.android.mms.pdu;
// import java.io.ByteArrayOutputStream;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class QuotedPrintable
{
public:
    /**
      * Decodes an array quoted-printable characters into an array of original bytes.
      * Escaped characters are converted back to their original representation.
      *
      * <p>
      * This function implements a subset of
      * quoted-printable encoding specification (rule #1 and rule #2)
      * as defined in RFC 1521.
      * </p>
      *
      * @param bytes array of quoted-printable characters
      * @return array of original bytes,
      *         null if quoted-printable decoding is unsuccessful.
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) DecodeQuotedPrintable(
        /* [in] */ ArrayOf<Byte>* bytes);

private:
    static Byte ESCAPE_CHAR;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_QUOTEDPRINTABLE_H__

