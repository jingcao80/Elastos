
#include "elastos/droid/google/mms/pdu/EncodedStringValue.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                          EncodedStringValue
//=====================================================================
CAR_INTERFACE_IMPL_2(EncodedStringValue, Object, IEncodedStringValue, ICloneable);

const String EncodedStringValue::TAG("EncodedStringValue");
const Boolean EncodedStringValue::DEBUG = FALSE;
const Boolean EncodedStringValue::LOCAL_LOGV = FALSE;

EncodedStringValue::EncodedStringValue(
    /* [in] */ Int32 charset,
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // // TODO: CharSet needs to be validated against MIBEnum.
    // if(null == data) {
    //     throw new NullPointerException("EncodedStringValue: Text-string is null.");
    // }
    //
    // mCharacterSet = charset;
    // mData = new byte[data.length];
    // System.arraycopy(data, 0, mData, 0, data.length);
}

EncodedStringValue::EncodedStringValue(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // this(CharacterSets.DEFAULT_CHARSET, data);
}

EncodedStringValue::EncodedStringValue(
    /* [in] */ const String& data)
{
    // ==================before translated======================
    // try {
    //     mData = data.getBytes(CharacterSets.DEFAULT_CHARSET_NAME);
    //     mCharacterSet = CharacterSets.DEFAULT_CHARSET;
    // } catch (UnsupportedEncodingException e) {
    //     Log.e(TAG, "Default encoding must be supported.", e);
    // }
}

ECode EncodedStringValue::GetCharacterSet(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCharacterSet;
    assert(0);
    return NOERROR;
}

ECode EncodedStringValue::SetCharacterSet(
    /* [in] */ Int32 charset)
{
    // ==================before translated======================
    // // TODO: CharSet needs to be validated against MIBEnum.
    // mCharacterSet = charset;
    assert(0);
    return NOERROR;
}

ECode EncodedStringValue::GetTextString(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // byte[] byteArray = new byte[mData.length];
    //
    // System.arraycopy(mData, 0, byteArray, 0, mData.length);
    // return byteArray;
    assert(0);
    return NOERROR;
}

ECode EncodedStringValue::SetTextString(
    /* [in] */ ArrayOf<Byte>* textString)
{
    VALIDATE_NOT_NULL(textString);
    // ==================before translated======================
    // if(null == textString) {
    //     throw new NullPointerException("EncodedStringValue: Text-string is null.");
    // }
    //
    // mData = new byte[textString.length];
    // System.arraycopy(textString, 0, mData, 0, textString.length);
    assert(0);
    return NOERROR;
}

ECode EncodedStringValue::GetString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (CharacterSets.ANY_CHARSET == mCharacterSet) {
    //     return new String(mData); // system default encoding.
    // } else {
    //     try {
    //         String name = CharacterSets.getMimeName(mCharacterSet);
    //         return new String(mData, name);
    //     } catch (UnsupportedEncodingException e) {
    //     	if (LOCAL_LOGV) {
    //     		Log.v(TAG, e.getMessage(), e);
    //     	}
    //     	try {
    //             return new String(mData, CharacterSets.MIMENAME_ISO_8859_1);
    //         } catch (UnsupportedEncodingException _) {
    //             return new String(mData); // system default encoding.
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode EncodedStringValue::AppendTextString(
    /* [in] */ ArrayOf<Byte>* textString)
{
    VALIDATE_NOT_NULL(textString);
    // ==================before translated======================
    // if(null == textString) {
    //     throw new NullPointerException("Text-string is null.");
    // }
    //
    // if(null == mData) {
    //     mData = new byte[textString.length];
    //     System.arraycopy(textString, 0, mData, 0, textString.length);
    // } else {
    //     ByteArrayOutputStream newTextString = new ByteArrayOutputStream();
    //     try {
    //         newTextString.write(mData);
    //         newTextString.write(textString);
    //     } catch (IOException e) {
    //         e.printStackTrace();
    //         throw new NullPointerException(
    //                 "appendTextString: failed when write a new Text-string");
    //     }
    //
    //     mData = newTextString.toByteArray();
    // }
    assert(0);
    return NOERROR;
}

AutoPtr<Object> EncodedStringValue::Clone()
{
    // ==================before translated======================
    // super.clone();
    // int len = mData.length;
    // byte[] dstBytes = new byte[len];
    // System.arraycopy(mData, 0, dstBytes, 0, len);
    //
    // try {
    //     return new EncodedStringValue(mCharacterSet, dstBytes);
    // } catch (Exception e) {
    //     Log.e(TAG, "failed to clone an EncodedStringValue: " + this);
    //     e.printStackTrace();
    //     throw new CloneNotSupportedException(e.getMessage());
    // }
    assert(0);
    AutoPtr<Object> empty;
    return empty;
}

ECode EncodedStringValue::Split(
    /* [in] */ const String& pattern,
    /* [out] */ ArrayOf<EncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String[] temp = getString().split(pattern);
    // EncodedStringValue[] ret = new EncodedStringValue[temp.length];
    // for (int i = 0; i < ret.length; ++i) {
    //     try {
    //         ret[i] = new EncodedStringValue(mCharacterSet,
    //                 temp[i].getBytes());
    //     } catch (NullPointerException _) {
    //         // Can't arrive here
    //         return null;
    //     }
    // }
    // return ret;
    assert(0);
    return NOERROR;
}

AutoPtr<ArrayOf<EncodedStringValue*> > EncodedStringValue::Extract(
    /* [in] */ const String& src)
{
    // ==================before translated======================
    // String[] values = src.split(";");
    //
    // ArrayList<EncodedStringValue> list = new ArrayList<EncodedStringValue>();
    // for (int i = 0; i < values.length; i++) {
    //     if (values[i].length() > 0) {
    //         list.add(new EncodedStringValue(values[i]));
    //     }
    // }
    //
    // int len = list.size();
    // if (len > 0) {
    //     return list.toArray(new EncodedStringValue[len]);
    // } else {
    //     return null;
    // }
    assert(0);
    AutoPtr< ArrayOf< AutoPtr<EncodedStringValue> > > empty;
    return empty;
}

String EncodedStringValue::Concat(
    /* [in] */ ArrayOf<EncodedStringValue*>* addr)
{
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder();
    // int maxIndex = addr.length - 1;
    // for (int i = 0; i <= maxIndex; i++) {
    //     sb.append(addr[i].getString());
    //     if (i < maxIndex) {
    //         sb.append(";");
    //     }
    // }
    //
    // return sb.toString();
    assert(0);
    return String("");
}

AutoPtr<EncodedStringValue> EncodedStringValue::Copy(
    /* [in] */ EncodedStringValue* value)
{
    // ==================before translated======================
    // if (value == null) {
    //     return null;
    // }
    //
    // return new EncodedStringValue(value.mCharacterSet, value.mData);
    assert(0);
    AutoPtr<EncodedStringValue> empty;
    return empty;
}

AutoPtr<ArrayOf<EncodedStringValue*> > EncodedStringValue::EncodeStrings(
    /* [in] */ ArrayOf<String>* array)
{
    // ==================before translated======================
    // int count = array.length;
    // if (count > 0) {
    //     EncodedStringValue[] encodedArray = new EncodedStringValue[count];
    //     for (int i = 0; i < count; i++) {
    //         encodedArray[i] = new EncodedStringValue(array[i]);
    //     }
    //     return encodedArray;
    // }
    // return null;
    assert(0);
    AutoPtr< ArrayOf< AutoPtr<EncodedStringValue> > > empty;
    return empty;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
