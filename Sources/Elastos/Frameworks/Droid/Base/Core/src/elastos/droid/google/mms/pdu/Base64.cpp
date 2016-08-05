
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
const Byte Base64::PAD = (Byte)'=';
const Int32 Base64::BASELENGTH;

AutoPtr<ArrayOf<Byte> > Base64::mBase64Alphabet = Base64::MiddleInitBase64alphabet();

AutoPtr<ArrayOf<Byte> > Base64::DecodeBase64(
    /* [in] */ ArrayOf<Byte>* base64Data)
{
    // RFC 2045 requires that we discard ALL non-Base64 characters
    base64Data = DiscardNonBase64(base64Data);

    // handle the edge case, so we don't have to worry about it later
    if (base64Data->GetLength() == 0) {
        AutoPtr<ArrayOf<Byte> > res = ArrayOf<Byte>::Alloc(1); // new byte[0]
        return res;
    }

    Int32 numberQuadruple = base64Data->GetLength() / FOURBYTE;
    AutoPtr<ArrayOf<Byte> > decodedData;
    Byte b1 = 0, b2 = 0, b3 = 0, b4 = 0, marker0 = 0, marker1 = 0;

    // Throw away anything not in base64Data

    Int32 encodedIndex = 0;
    Int32 dataIndex = 0;
    {
        // this sizes the output array properly - rlw
        Int32 lastData = base64Data->GetLength();
        // ignore the '=' padding
        while ((*base64Data)[lastData - 1] == PAD) {
            if (--lastData == 0) {
                AutoPtr<ArrayOf<Byte> > res = ArrayOf<Byte>::Alloc(1); // new byte[0]
                return res;
            }
        }
        decodedData = ArrayOf<Byte>::Alloc(lastData - numberQuadruple);
    }

    for (Int32 i = 0; i < numberQuadruple; i++) {
        dataIndex = i * 4;
        marker0 = (*base64Data)[dataIndex + 2];
        marker1 = (*base64Data)[dataIndex + 3];

        b1 = (*mBase64Alphabet)[(*base64Data)[dataIndex]];
        b2 = (*mBase64Alphabet)[(*base64Data)[dataIndex + 1]];

        if (marker0 != PAD && marker1 != PAD) {
            //No PAD e.g 3cQl
            b3 = (*mBase64Alphabet)[marker0];
            b4 = (*mBase64Alphabet)[marker1];

            (*decodedData)[encodedIndex] = (Byte) (b1 << 2 | b2 >> 4);
            (*decodedData)[encodedIndex + 1] =
                (Byte) (((b2 & 0xf) << 4) | ((b3 >> 2) & 0xf));
            (*decodedData)[encodedIndex + 2] = (Byte) (b3 << 6 | b4);
        }
        else if (marker0 == PAD) {
            //Two PAD e.g. 3c[Pad][Pad]
            (*decodedData)[encodedIndex] = (Byte) (b1 << 2 | b2 >> 4);
        }
        else if (marker1 == PAD) {
            //One PAD e.g. 3cQ[Pad]
            b3 = (*mBase64Alphabet)[marker0];

            (*decodedData)[encodedIndex] = (Byte) (b1 << 2 | b2 >> 4);
            (*decodedData)[encodedIndex + 1] =
                (Byte) (((b2 & 0xf) << 4) | ((b3 >> 2) & 0xf));
        }
        encodedIndex += 3;
    }
    return decodedData;
}

AutoPtr<ArrayOf<Byte> > Base64::DiscardNonBase64(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<ArrayOf<Byte> > groomedData = ArrayOf<Byte>::Alloc(data->GetLength());
    Int32 bytesCopied = 0;

    for (Int32 i = 0; i < data->GetLength(); i++) {
        if (IsBase64((*data)[i])) {
            (*groomedData)[bytesCopied++] = (*data)[i];
        }
    }

    AutoPtr<ArrayOf<Byte> > packedData = ArrayOf<Byte>::Alloc(bytesCopied);

    // System.arraycopy(groomedData, 0, packedData, 0, bytesCopied);
    packedData->Copy(0, groomedData, 0, bytesCopied);

    return packedData;
}

AutoPtr<ArrayOf<Byte> > Base64::MiddleInitBase64alphabet()
{
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(BASELENGTH);
    return result;
}

Boolean Base64::IsBase64(
    /* [in] */ Byte octect)
{
    if (octect == PAD) {
        return TRUE;
    }
    else if ((*mBase64Alphabet)[octect] == -1) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
