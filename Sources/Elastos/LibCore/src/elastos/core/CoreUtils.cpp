#include "CoreUtils.h"
#include "CArrayOf.h"
#include "CBoolean.h"
#include "CByte.h"
#include "CChar32.h"
#include "CInteger16.h"
#include "CInteger32.h"
#include "CInteger64.h"
#include "CFloat.h"
#include "CDouble.h"
#include "CString.h"

namespace Elastos {
namespace Core {

AutoPtr<IByte> CoreUtils::ConvertByte(
    /* [in] */ Byte value)
{
    AutoPtr<CByte> obj;
    CByte::NewByFriend(value, (CByte**)&obj);
    return (IByte*)obj.Get();
}

AutoPtr<IChar32> CoreUtils::ConvertChar32(
    /* [in] */ Char32 value)
{
    AutoPtr<CChar32> obj;
    CChar32::NewByFriend(value, (CChar32**)&obj);
    return (IChar32*)obj.Get();
}

AutoPtr<IBoolean> CoreUtils::Convert(
    /* [in] */ Boolean value)
{
    AutoPtr<CBoolean> obj;
    CBoolean::NewByFriend(value, (CBoolean**)&obj);
    return (IBoolean*)obj.Get();
}

AutoPtr<IInteger16> CoreUtils::Convert(
    /* [in] */ Int16 value)
{
    AutoPtr<CInteger16> obj;
    CInteger16::NewByFriend(value, (CInteger16**)&obj);
    return (IInteger16*)obj.Get();
}

AutoPtr<IInteger32> CoreUtils::Convert(
    /* [in] */ Int32 value)
{
    AutoPtr<CInteger32> obj;
    CInteger32::NewByFriend(value, (CInteger32**)&obj);
    return (IInteger32*)obj.Get();
}

AutoPtr<IInteger64> CoreUtils::Convert(
    /* [in] */ Int64 value)
{
    AutoPtr<CInteger64> obj;
    CInteger64::NewByFriend(value, (CInteger64**)&obj);
    return (IInteger64*)obj.Get();
}

AutoPtr<IFloat> CoreUtils::Convert(
    /* [in] */ Float value)
{
    AutoPtr<CFloat> obj;
    CFloat::NewByFriend(value, (CFloat**)&obj);
    return (IFloat*)obj.Get();
}

AutoPtr<IDouble> CoreUtils::Convert(
    /* [in] */ Double value)
{
    AutoPtr<CDouble> obj;
    CDouble::NewByFriend(value, (CDouble**)&obj);
    return (IDouble*)obj.Get();
}

AutoPtr<ICharSequence> CoreUtils::Convert(
    /* [in] */ const String & value)
{
    AutoPtr<CString> obj;
    CString::NewByFriend(value, (CString**)&obj);
    return (ICharSequence*)obj.Get();
}

String CoreUtils::Unbox(
    /* [in] */ ICharSequence* value)
{
    if (value == NULL) return String(NULL);
    String str;
    value->ToString(&str);
    return str;
}

String CoreUtils::Unbox(
    /* [in] */ IString* value)
{
    return Unbox(ICharSequence::Probe(value));
}

AutoPtr<ICharSequence> CoreUtils::Convert(
    /* [in] */ const char* str)
{
    AutoPtr<CString> obj;
    CString::NewByFriend(String(str), (CString**)&obj);
    return (ICharSequence*)obj.Get();
}

AutoPtr<IArrayOf> CoreUtils::ConvertByteArray(
    /* [in] */ ArrayOf<Byte>* arr)
{
    if (arr) {
        Int32 length = arr->GetLength();
        AutoPtr<IArrayOf> arrObj;
        CArrayOf::New(EIID_IByte, length, (IArrayOf**)&arrObj);
        for (Int32 i = 0; i < length; ++i) {
            AutoPtr<IByte> item = ConvertByte((*arr)[i]);
            arrObj->Set(i, item.Get());
        }

        return arrObj;
    }
    return NULL;
}

AutoPtr<IArrayOf> CoreUtils::ConvertChar32Array(
    /* [in] */ ArrayOf<Char32>* arr)
{
    if (arr) {
        Int32 length = arr->GetLength();
        AutoPtr<IArrayOf> arrObj;
        CArrayOf::New(EIID_IChar32, length, (IArrayOf**)&arrObj);
        for (Int32 i = 0; i < length; ++i) {
            AutoPtr<IChar32> item = ConvertChar32((*arr)[i]);
            arrObj->Set(i, item.Get());
        }

        return arrObj;
    }
    return NULL;
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Boolean>* arr)
{
    return ConvertImpl<Boolean, IBoolean>(arr);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Int16>* arr)
{
    return ConvertImpl<Int16, IInteger16>(arr);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Int32>* arr)
{
    return ConvertImpl<Int32, IInteger32>(arr);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Int64>* arr)
{
    return ConvertImpl<Int64, IInteger64>(arr);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Float>* arr)
{
    return ConvertImpl<Float, IFloat>(arr);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Double>* arr)
{
    return ConvertImpl<Double, IDouble>(arr);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<String>* arr)
{
    return ConvertImpl<String, ICharSequence>(arr);
}

} // namespace Core
} // namespace Elastos
