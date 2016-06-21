#include "Elastos.Droid.Utility.h"
#include "elastos/droid/utility/CSparseBooleanArray.h"
#include "elastos/droid/internal/telephony/cdma/sms/CdmaSmsAddress.h"

using Elastos::Droid::Utility::CSparseBooleanArray;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

//=====================================================================
//                            CdmaSmsAddress
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSmsAddress, SmsAddress, ICdmaSmsAddress);

// AutoPtr<ArrayOf<Byte> > CdmaSmsAddress::numericCharsDialable = CdmaSmsAddress::MiddleInitNumericcharsdialable();
// AutoPtr<ArrayOf<Byte> > CdmaSmsAddress::numericCharsSugar = CdmaSmsAddress::MiddleInitNumericcharssugar();
AutoPtr<ISparseBooleanArray> CdmaSmsAddress::InitSparseBooleanArray()
{
    AutoPtr<ISparseBooleanArray> array;
    Int32 length = CdmaSmsAddress::numericCharsDialable->GetLength() + CdmaSmsAddress::numericCharsSugar->GetLength();
    CSparseBooleanArray::New(length, (ISparseBooleanArray**)&array);
    return array;
}
// AutoPtr<ISparseBooleanArray> CdmaSmsAddress::numericCharDialableMap = CdmaSmsAddress::InitSparseBooleanArray();

CdmaSmsAddress::CdmaSmsAddress()
{
}

ECode CdmaSmsAddress::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;
    // ==================before translated======================
    // StringBuilder builder = new StringBuilder();
    // builder.append("CdmaSmsAddress ");
    // builder.append("{ digitMode=" + digitMode);
    // builder.append(", numberMode=" + numberMode);
    // builder.append(", numberPlan=" + numberPlan);
    // builder.append(", numberOfDigits=" + numberOfDigits);
    // builder.append(", ton=" + ton);
    // builder.append(", address=\"" + address + "\"");
    // builder.append(", origBytes=" + HexDump.toHexString(origBytes));
    // builder.append(" }");
    // return builder.toString();
    assert(0);
    return NOERROR;
}

AutoPtr<CdmaSmsAddress> CdmaSmsAddress::Parse(
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // CdmaSmsAddress addr = new CdmaSmsAddress();
    // addr.address = address;
    // addr.digitMode = DIGIT_MODE_4BIT_DTMF; // Default to 4 bit dtmf encoding
    // addr.ton = TON_UNKNOWN;
    // addr.numberMode = NUMBER_MODE_NOT_DATA_NETWORK;
    // addr.numberPlan = NUMBERING_PLAN_UNKNOWN;
    // byte[] origBytes = null;
    //
    // if (address.indexOf('+') != -1) {
    //     // This is international phone number
    //     addr.digitMode = DIGIT_MODE_8BIT_CHAR;
    //     addr.ton = TON_INTERNATIONAL_OR_IP;
    //     addr.numberMode = NUMBER_MODE_NOT_DATA_NETWORK;
    //     addr.numberPlan = NUMBERING_PLAN_ISDN_TELEPHONY;
    // }
    // if (address.indexOf('@') != -1) {
    //     // This is email address
    //     addr.digitMode = DIGIT_MODE_8BIT_CHAR;
    //     addr.ton = TON_NATIONAL_OR_EMAIL;
    //     addr.numberMode = NUMBER_MODE_DATA_NETWORK;
    // }
    //
    // // A.S0014-C 4.2.40 states: "Prefix or escape digits shall not be included"
    // String filteredAddr = filterNumericSugar(address);
    // if (addr.digitMode == DIGIT_MODE_4BIT_DTMF) {
    //     if (filteredAddr != null) {
    //         origBytes = parseToDtmf(filteredAddr);
    //     }
    //     if (origBytes == null) {
    //         // Failed to encode in 4 bit. Try in 8 bit.
    //         addr.digitMode = DIGIT_MODE_8BIT_CHAR;
    //     }
    // }
    //
    // if (addr.digitMode == DIGIT_MODE_8BIT_CHAR) {
    //     filteredAddr = filterWhitespace(filteredAddr);
    //     origBytes = UserData.stringToAscii(filteredAddr);
    //     if (origBytes == null) {
    //         return null;
    //     }
    // }
    // addr.origBytes = origBytes;
    // addr.numberOfDigits = origBytes.length;
    // return addr;
    assert(0);
    AutoPtr<CdmaSmsAddress> empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > CdmaSmsAddress::ParseToDtmf(
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // int digits = address.length();
    // byte[] result = new byte[digits];
    // for (int i = 0; i < digits; i++) {
    //     char c = address.charAt(i);
    //     int val = 0;
    //     if ((c >= '1') && (c <= '9')) val = c - '0';
    //     else if (c == '0') val = 10;
    //     else if (c == '*') val = 11;
    //     else if (c == '#') val = 12;
    //     else return null;
    //     result[i] = (byte)val;
    // }
    // return result;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > CdmaSmsAddress::MiddleInitNumericcharsdialable()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#'
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > CdmaSmsAddress::MiddleInitNumericcharssugar()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // '(', ')', ' ', '-', '+', '.', '/', '\\'
    // ->WWZ_SIGN: ARRAY_INIT_END }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

String CdmaSmsAddress::FilterNumericSugar(
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // StringBuilder builder = new StringBuilder();
    // int len = address.length();
    // for (int i = 0; i < len; i++) {
    //     char c = address.charAt(i);
    //     int mapIndex = numericCharDialableMap.indexOfKey(c);
    //     if (mapIndex < 0) return null;
    //     if (! numericCharDialableMap.valueAt(mapIndex)) continue;
    //     builder.append(c);
    // }
    // return builder.toString();
    assert(0);
    return String("");
}

String CdmaSmsAddress::FilterWhitespace(
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // StringBuilder builder = new StringBuilder();
    // int len = address.length();
    // for (int i = 0; i < len; i++) {
    //     char c = address.charAt(i);
    //     if ((c == ' ') || (c == '\r') || (c == '\n') || (c == '\t')) continue;
    //     builder.append(c);
    // }
    // return builder.toString();
    assert(0);
    return String("");
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
