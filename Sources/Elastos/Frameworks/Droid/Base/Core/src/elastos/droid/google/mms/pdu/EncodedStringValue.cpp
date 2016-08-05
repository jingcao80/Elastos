#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/EncodedStringValue.h"
#include "elastos/droid/google/mms/pdu/CCharacterSetsHelper.h"
#include "elastos/droid/google/mms/pdu/CEncodedStringValue.h"

#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

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

EncodedStringValue::EncodedStringValue()
{
}

ECode EncodedStringValue::constructor(
    /* [in] */ Int32 charset,
    /* [in] */ ArrayOf<Byte>* data)
{
    // TODO: CharSet needs to be validated against MIBEnum.
    if (NULL == data) {
        // throw new NullPointerException("EncodedStringValue: Text-string is null.");
        return E_NULL_POINTER_EXCEPTION;
    }

    mCharacterSet = charset;
    mData = ArrayOf<Byte>::Alloc(data->GetLength());
    // System.arraycopy(data, 0, mData, 0, data.length);
    mData->Copy(0, data, 0, data->GetLength());
    return NOERROR;
}

ECode EncodedStringValue::constructor(
    /* [in] */ ArrayOf<Byte>* data)
{
    return constructor(ICharacterSets::DEFAULT_CHARSET, data);
}

ECode EncodedStringValue::constructor(
    /* [in] */ const String& data)
{
    // try {
        mData = data.GetBytes(); // ICharacterSets::DEFAULT_CHARSET_NAME
        mCharacterSet = ICharacterSets::DEFAULT_CHARSET;
    // } catch (UnsupportedEncodingException e) {
    //     Log.e(TAG, "Default encoding must be supported.", e);
    // }
    return NOERROR;
}

ECode EncodedStringValue::GetCharacterSet(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCharacterSet;
    return NOERROR;
}

ECode EncodedStringValue::SetCharacterSet(
    /* [in] */ Int32 charset)
{
    // TODO: CharSet needs to be validated against MIBEnum.
    mCharacterSet = charset;
    return NOERROR;
}

ECode EncodedStringValue::GetTextString(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(mData->GetLength());

    // System.arraycopy(mData, 0, byteArray, 0, mData.length);
    byteArray->Copy(0, mData, 0, mData->GetLength());
    *result = byteArray;
    return NOERROR;
}

ECode EncodedStringValue::SetTextString(
    /* [in] */ ArrayOf<Byte>* textString)
{
    VALIDATE_NOT_NULL(textString);
    if (NULL == textString) {
        // throw new NullPointerException("EncodedStringValue: Text-string is null.");
        return E_NULL_POINTER_EXCEPTION;
    }

    mData = ArrayOf<Byte>::Alloc(textString->GetLength());
    // System.arraycopy(textString, 0, mData, 0, textString.length);
    mData->Copy(0, textString, 0, textString->GetLength());
    return NOERROR;
}

ECode EncodedStringValue::GetString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (ICharacterSets::ANY_CHARSET == mCharacterSet) {
        *result = String(*mData); // system default encoding.
    }
    else {
        // try {
        AutoPtr<ICharacterSetsHelper> hlp;
        CCharacterSetsHelper::AcquireSingleton((ICharacterSetsHelper**)&hlp);
        String name;
        hlp->GetMimeName(mCharacterSet, &name);
        *result = String(*mData); //name
        // } catch (UnsupportedEncodingException e) {
        // 	if (LOCAL_LOGV) {
        // 		Log.v(TAG, e.getMessage(), e);
        // 	}
        // 	try {
        //         return new String(mData, CharacterSets.MIMENAME_ISO_8859_1);
        //     } catch (UnsupportedEncodingException _) {
        //         return new String(mData); // system default encoding.
        //     }
        // }
    }
    return NOERROR;
}

ECode EncodedStringValue::AppendTextString(
    /* [in] */ ArrayOf<Byte>* textString)
{
    VALIDATE_NOT_NULL(textString);
    if (NULL == textString) {
        // throw new NullPointerException("Text-string is null.");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (NULL == mData) {
        mData = ArrayOf<Byte>::Alloc(textString->GetLength());
        // System.arraycopy(textString, 0, mData, 0, textString.length);
        mData->Copy(0, textString, 0, textString->GetLength());
    }
    else {
        AutoPtr<IByteArrayOutputStream> newTextString;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&newTextString);
        // try {
            IOutputStream::Probe(newTextString)->Write(mData);
            IOutputStream::Probe(newTextString)->Write(textString);
        // } catch (IOException e) {
        //     e.printStackTrace();
        //     throw new NullPointerException(
        //             "appendTextString: failed when write a new Text-string");
        // }

        newTextString->ToByteArray((ArrayOf<Byte>**)&mData);
    }
    return NOERROR;
}

ECode EncodedStringValue::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // super.clone();
    Int32 len = mData->GetLength();
    AutoPtr<ArrayOf<Byte> > dstBytes = ArrayOf<Byte>::Alloc(len);
    // System.arraycopy(mData, 0, dstBytes, 0, len);
    dstBytes->Copy(0, mData, 0, len);

    // try {
    AutoPtr<IEncodedStringValue> res;
    CEncodedStringValue::New(mCharacterSet, dstBytes, (IEncodedStringValue**)&res);
    *result = IEncodedStringValue::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (Exception e) {
    //     Log.e(TAG, "failed to clone an EncodedStringValue: " + this);
    //     e.printStackTrace();
    //     throw new CloneNotSupportedException(e.getMessage());
    // }
    // return NOERROR;
}

ECode EncodedStringValue::Split(
    /* [in] */ const String& pattern,
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result)
    String res;
    GetString(&res);
    AutoPtr<ArrayOf<String> > temp;
    StringUtils::Split(res, pattern, (ArrayOf<String>**)&temp);
    AutoPtr<ArrayOf<IEncodedStringValue*> > ret = ArrayOf<IEncodedStringValue*>::Alloc(temp->GetLength());
    for (Int32 i = 0; i < ret->GetLength(); ++i) {
        // try {
        CEncodedStringValue::New(mCharacterSet,
                ((*temp)[i]).GetBytes(), (IEncodedStringValue**)&((*ret)[i]));
        // } catch (NullPointerException _) {
        //     // Can't arrive here
        //     return null;
        // }
    }
    *result = ret;
    return NOERROR;
}

AutoPtr<ArrayOf<IEncodedStringValue*> > EncodedStringValue::Extract(
    /* [in] */ const String& src)
{
    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(src, String(";"), (ArrayOf<String>**)&values);

    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i].GetLength() > 0) {
            AutoPtr<IEncodedStringValue> p;
            CEncodedStringValue::New((*values)[i], (IEncodedStringValue**)&p);
            list->Add(p);
        }
    }

    Int32 len = 0;
    list->GetSize(&len);
    if (len > 0) {
        AutoPtr<ArrayOf<IEncodedStringValue*> > arr = ArrayOf<IEncodedStringValue*>::Alloc(len);
        AutoPtr<ArrayOf<IInterface*> > res;
        list->ToArray((ArrayOf<IInterface*>*)arr.Get(), (ArrayOf<IInterface*>**)&res);
        return (ArrayOf<IEncodedStringValue*>*)res.Get();
    }
    else {
        return NULL;
    }
}

String EncodedStringValue::Concat(
    /* [in] */ ArrayOf<IEncodedStringValue*>* addr)
{
    StringBuilder sb;
    Int32 maxIndex = addr->GetLength() - 1;
    for (Int32 i = 0; i <= maxIndex; i++) {
        String str;
        (*addr)[i]->GetString(&str);
        sb.Append(str);
        if (i < maxIndex) {
            sb.Append(String(";"));
        }
    }

    return sb.ToString();
}

AutoPtr<IEncodedStringValue> EncodedStringValue::Copy(
    /* [in] */ IEncodedStringValue* value)
{
    if (value == NULL) {
        return NULL;
    }

    AutoPtr<IEncodedStringValue> res;
    CEncodedStringValue::New(((EncodedStringValue*)value)->mCharacterSet,
                            ((EncodedStringValue*)value)->mData,
                            (IEncodedStringValue**)&res);
    return res;
}

AutoPtr<ArrayOf<IEncodedStringValue*> > EncodedStringValue::EncodeStrings(
    /* [in] */ ArrayOf<String>* array)
{
    Int32 count = array->GetLength();
    if (count > 0) {
        AutoPtr<ArrayOf<IEncodedStringValue*> > encodedArray = ArrayOf<IEncodedStringValue*>::Alloc(count);
        for (Int32 i = 0; i < count; i++) {
            CEncodedStringValue::New((*array)[i], (IEncodedStringValue**)&((*encodedArray)[i]));
        }
        return encodedArray;
    }
    return NULL;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
