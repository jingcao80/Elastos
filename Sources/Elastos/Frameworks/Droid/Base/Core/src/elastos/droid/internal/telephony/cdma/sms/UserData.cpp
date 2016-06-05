
#include "elastos/droid/internal/telephony/cdma/sms/UserData.h"
#include "elastos/droid/utility/CSparseInt32Array.h"

using Elastos::Droid::Utility::CSparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

//=====================================================================
//                               UserData
//=====================================================================
// AutoPtr<ArrayOf<Byte> > UserData::ASCII_MAP = UserData::MiddleInitAsciiMap();
const Byte UserData::UNENCODABLE_7_BIT_CHAR = 0x20;
static AutoPtr<ISparseInt32Array> InitSparseInt32Array()
{
    AutoPtr<ISparseInt32Array> array;
    CSparseInt32Array::New((ISparseInt32Array**)&array);
    return array;
}

const AutoPtr<ISparseInt32Array> UserData::charToAscii = InitSparseInt32Array();

AutoPtr<ArrayOf<Byte> > UserData::StringToAscii(
    /* [in] */ const String& str)
{
    // ==================before translated======================
    // int len = str.length();
    // byte[] result = new byte[len];
    // for (int i = 0; i < len; i++) {
    //     int charCode = charToAscii.get(str.charAt(i), -1);
    //     if (charCode == -1) return null;
    //     result[i] = (byte)charCode;
    // }
    // return result;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

ECode UserData::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;
    // ==================before translated======================
    // StringBuilder builder = new StringBuilder();
    // builder.append("UserData ");
    // builder.append("{ msgEncoding=" + (msgEncodingSet ? msgEncoding : "unset"));
    // builder.append(", msgType=" + msgType);
    // builder.append(", paddingBits=" + paddingBits);
    // builder.append(", numFields=" + numFields);
    // builder.append(", userDataHeader=" + userDataHeader);
    // builder.append(", payload='" + HexDump.toHexString(payload) + "'");
    // builder.append(", payloadStr='" + payloadStr + "'");
    // builder.append(" }");
    // return builder.toString();
    assert(0);
    return NOERROR;
}

AutoPtr< ArrayOf<Byte> > UserData::MiddleInitAsciiMap()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
    //          '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
    //          '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    //          'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    //          '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    //          'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~'
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
