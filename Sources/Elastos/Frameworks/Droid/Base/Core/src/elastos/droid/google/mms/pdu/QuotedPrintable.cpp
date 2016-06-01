
#include "elastos/droid/google/mms/pdu/QuotedPrintable.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                           QuotedPrintable
//=====================================================================
Byte QuotedPrintable::ESCAPE_CHAR = '=';

AutoPtr< ArrayOf<Byte> > QuotedPrintable::DecodeQuotedPrintable(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    // ==================before translated======================
    // if (bytes == null) {
    //     return null;
    // }
    // ByteArrayOutputStream buffer = new ByteArrayOutputStream();
    // for (int i = 0; i < bytes.length; i++) {
    //     int b = bytes[i];
    //     if (b == ESCAPE_CHAR) {
    //         try {
    //             if('\r' == (char)bytes[i + 1] &&
    //                     '\n' == (char)bytes[i + 2]) {
    //                 i += 2;
    //                 continue;
    //             }
    //             int u = Character.digit((char) bytes[++i], 16);
    //             int l = Character.digit((char) bytes[++i], 16);
    //             if (u == -1 || l == -1) {
    //                 return null;
    //             }
    //             buffer.write((char) ((u << 4) + l));
    //         } catch (ArrayIndexOutOfBoundsException e) {
    //             return null;
    //         }
    //     } else {
    //         buffer.write(b);
    //     }
    // }
    // return buffer.toByteArray();
    assert(0);
    AutoPtr<ArrayOf<Byte> > empty;
    return empty;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
