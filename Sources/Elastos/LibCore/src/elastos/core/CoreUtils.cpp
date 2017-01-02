//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    AutoPtr<IByte> obj;
    CByte::New(value, (IByte**)&obj);
    return obj;
}

AutoPtr<IChar32> CoreUtils::ConvertChar32(
    /* [in] */ Char32 value)
{
    AutoPtr<IChar32> obj;
    CChar32::New(value, (IChar32**)&obj);
    return obj;
}

AutoPtr<IBoolean> CoreUtils::Convert(
    /* [in] */ Boolean value)
{
    AutoPtr<IBoolean> obj;
    CBoolean::New(value, (IBoolean**)&obj);
    return obj;
}

AutoPtr<IInteger16> CoreUtils::Convert(
    /* [in] */ Int16 value)
{
    AutoPtr<IInteger16> obj;
    CInteger16::New(value, (IInteger16**)&obj);
    return obj;
}

AutoPtr<IInteger32> CoreUtils::Convert(
    /* [in] */ Int32 value)
{
    AutoPtr<IInteger32> obj;
    CInteger32::New(value, (IInteger32**)&obj);
    return obj;
}

Int32 CoreUtils::Unbox(
    /* [in] */ IInteger32* value)
{
    if (value == NULL) return 0;
    Int32 iVal;
    value->GetValue(&iVal);
    return iVal;
}

AutoPtr<IInteger64> CoreUtils::Convert(
    /* [in] */ Int64 value)
{
    AutoPtr<IInteger64> obj;
    CInteger64::New(value, (IInteger64**)&obj);
    return obj;
}

AutoPtr<IFloat> CoreUtils::Convert(
    /* [in] */ Float value)
{
    AutoPtr<IFloat> obj;
    CFloat::New(value, (IFloat**)&obj);
    return obj;
}

AutoPtr<IDouble> CoreUtils::Convert(
    /* [in] */ Double value)
{
    AutoPtr<IDouble> obj;
    CDouble::New(value, (IDouble**)&obj);
    return obj;
}

AutoPtr<ICharSequence> CoreUtils::Convert(
    /* [in] */ const String & value)
{
    AutoPtr<ICharSequence> obj;
    CString::New(value, (ICharSequence**)&obj);
    return obj;
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
    AutoPtr<ICharSequence> obj;
    CString::New(String(str), (ICharSequence**)&obj);
    return obj;
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
    return ConvertImpl<Boolean, IBoolean>(arr, EIID_IBoolean);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Int16>* arr)
{
    return ConvertImpl<Int16, IInteger16>(arr, EIID_IInteger16);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Int32>* arr)
{
    return ConvertImpl<Int32, IInteger32>(arr, EIID_IInteger32);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Int64>* arr)
{
    return ConvertImpl<Int64, IInteger64>(arr, EIID_IInteger64);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Float>* arr)
{
    return ConvertImpl<Float, IFloat>(arr, EIID_IFloat);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<Double>* arr)
{
    return ConvertImpl<Double, IDouble>(arr, EIID_IDouble);
}

AutoPtr<IArrayOf> CoreUtils::Convert(
    /* [in] */ ArrayOf<String>* arr)
{
    return ConvertImpl<String, ICharSequence>(arr, EIID_ICharSequence);
}

} // namespace Core
} // namespace Elastos
