
#include "elastos/droid/google/mms/pdu/Base64.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                                Base64
//=====================================================================
const Int32 Base64::FOURBYTE;
const Byte Base64::PAD = (byte)'=';
const Int32 Base64::BASELENGTH;
// TODO
// AutoPtr<ArrayOf<Byte> > Base64::base64Alphabet = Base64::MiddleInitBase64alphabet();

AutoPtr<ArrayOf<Byte> > Base64::DecodeBase64(
    /* [in] */ ArrayOf<Byte>* base64Data)
{
    // ==================before translated======================
    // // RFC 2045 requires that we discard ALL non-Base64 characters
    // base64Data = discardNonBase64(base64Data);
    //
    // // handle the edge case, so we don't have to worry about it later
    // if (base64Data.length == 0) {
    //     return new byte[0];
    // }
    //
    // int numberQuadruple = base64Data.length / FOURBYTE;
    // byte decodedData[] = null;
    // byte b1 = 0, b2 = 0, b3 = 0, b4 = 0, marker0 = 0, marker1 = 0;
    //
    // // Throw away anything not in base64Data
    //
    // int encodedIndex = 0;
    // int dataIndex = 0;
    // {
    //     // this sizes the output array properly - rlw
    //     int lastData = base64Data.length;
    //     // ignore the '=' padding
    //     while (base64Data[lastData - 1] == PAD) {
    //         if (--lastData == 0) {
    //             return new byte[0];
    //         }
    //     }
    //     decodedData = new byte[lastData - numberQuadruple];
    // }
    //
    // for (int i = 0; i < numberQuadruple; i++) {
    //     dataIndex = i * 4;
    //     marker0 = base64Data[dataIndex + 2];
    //     marker1 = base64Data[dataIndex + 3];
    //
    //     b1 = base64Alphabet[base64Data[dataIndex]];
    //     b2 = base64Alphabet[base64Data[dataIndex + 1]];
    //
    //     if (marker0 != PAD && marker1 != PAD) {
    //         //No PAD e.g 3cQl
    //         b3 = base64Alphabet[marker0];
    //         b4 = base64Alphabet[marker1];
    //
    //         decodedData[encodedIndex] = (byte) (b1 << 2 | b2 >> 4);
    //         decodedData[encodedIndex + 1] =
    //             (byte) (((b2 & 0xf) << 4) | ((b3 >> 2) & 0xf));
    //         decodedData[encodedIndex + 2] = (byte) (b3 << 6 | b4);
    //     } else if (marker0 == PAD) {
    //         //Two PAD e.g. 3c[Pad][Pad]
    //         decodedData[encodedIndex] = (byte) (b1 << 2 | b2 >> 4);
    //     } else if (marker1 == PAD) {
    //         //One PAD e.g. 3cQ[Pad]
    //         b3 = base64Alphabet[marker0];
    //
    //         decodedData[encodedIndex] = (byte) (b1 << 2 | b2 >> 4);
    //         decodedData[encodedIndex + 1] =
    //             (byte) (((b2 & 0xf) << 4) | ((b3 >> 2) & 0xf));
    //     }
    //     encodedIndex += 3;
    // }
    // return decodedData;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr<ArrayOf<Byte> > Base64::DiscardNonBase64(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // byte groomedData[] = new byte[data.length];
    // int bytesCopied = 0;
    //
    // for (int i = 0; i < data.length; i++) {
    //     if (isBase64(data[i])) {
    //         groomedData[bytesCopied++] = data[i];
    //     }
    // }
    //
    // byte packedData[] = new byte[bytesCopied];
    //
    // System.arraycopy(groomedData, 0, packedData, 0, bytesCopied);
    //
    // return packedData;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr<ArrayOf<Byte> > Base64::MiddleInitBase64alphabet()
{
    // ==================before translated======================
    // byte[] result = new byte[BASELENGTH];
    assert(0);
    AutoPtr<ArrayOf<Byte> > empty;
    return empty;
}

Boolean Base64::IsBase64(
    /* [in] */ Byte octect)
{
    // ==================before translated======================
    // if (octect == PAD) {
    //     return true;
    // } else if (base64Alphabet[octect] == -1) {
    //     return false;
    // } else {
    //     return true;
    // }
    assert(0);
    return FALSE;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
