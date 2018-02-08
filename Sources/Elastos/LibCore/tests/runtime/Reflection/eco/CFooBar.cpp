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

#include "reflection/CFooBar.h"

namespace Reflection {

CAR_INTERFACE_IMPL(CFooBar, Object, IFooBar)

CAR_OBJECT_IMPL(CFooBar)

CFooBar::CFooBar()
    : mValue0(0)
    , mString("CFooBar")
    , mValue1(1)
{
    PFL_EX(" >> CFooBar::CFooBar()");
}

ECode CFooBar::constructor()
{
    PFL_EX(" >> CFooBar::constructor()");
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ String* str)
{
    PFL_EX(" >> CFooBar::GetValue() String");
    assert(str != NULL);
    *str = mString;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ const String& str)
{
    PFL_EX(" >> CFooBar::SetValue() String");
    mString = str;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int32 value)
{
    PFL_EX(" >> CFooBar::SetValue() Int32");
    mValue0 = value;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int32* value)
{
    PFL_EX(" >> CFooBar::GetValue() Int32");
    assert(value != NULL);
    *value = mValue0;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int64 value)
{
    PFL_EX(" >> CFooBar::SetValue() Int64");
    mValue1 = value;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int64* value)
{
    PFL_EX(" >> CFooBar::GetValue() Int64");
    assert(value != NULL);
    *value = mValue1;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int32 value0,
    /* [in] */ const String& str,
    /* [in] */ Int64 value1)
{
    PFL_EX(" >> CFooBar::SetValue() Int32, String, Int64");
    mValue0 = value0;
    mString = str;
    mValue1 = value1;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int32* value0,
    /* [out] */ String* str,
    /* [out] */ Int64* value1)
{
    PFL_EX(" >> CFooBar::GetValue() Int32, String, Int64");
    assert(str != NULL);
    assert(value0 != NULL);
    assert(value1 != NULL);
    *str = mString;
    *value0 = mValue0;
    *value1 = mValue1;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ const String& str,
    /* [in] */ Int32 value0,
    /* [in] */ Int64 value1)
{
    PFL_EX(" >> CFooBar::SetValue() String, Int32, Int64");
    mString = str;
    mValue0 = value0;
    mValue1 = value1;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ String* str,
    /* [out] */ Int32* value0,
    /* [out] */ Int64* value1)
{
    PFL_EX(" >> CFooBar::GetValue() String, Int32, Int64");
    assert(str != NULL);
    assert(value0 != NULL);
    assert(value1 != NULL);
    *str = mString;
    *value0 = mValue0;
    *value1 = mValue1;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int16 value0,
    /* [in] */ Int32 value1,
    /* [in] */ const String& str,
    /* [in] */ Boolean value2,
    /* [in] */ Int64 value3,
    /* [in] */ Int64 value4,
    /* [in] */ Int32 value5)
{
    PFL_EX(" >> CFooBar::SetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32");
    mValue3 = value0;
    mValue0 = value1;
    mString = str;
    mValue2 = value2;
    mValue1 = value3;
    mValue5 = value4;
    mValue4 = value5;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int16* value0,
    /* [out] */ Int32* value1,
    /* [out] */ String* str,
    /* [out] */ Boolean* value2,
    /* [out] */ Int64* value3,
    /* [out] */ Int64* value4,
    /* [out] */ Int32* value5)
{
    PFL_EX(" >> CFooBar::GetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32");
    assert(value0 != NULL);
    assert(value1 != NULL);
    assert(str != NULL);
    assert(value2 != NULL);
    assert(value3 != NULL);
    assert(value4 != NULL);
    assert(value5 != NULL);
    *value0 = mValue3;
    *value1 = mValue0;
    *str = mString;
    *value2 = mValue2;
    *value3 = mValue1;
    *value4 = mValue5;
    *value5 = mValue4;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int16 value0,
    /* [in] */ Int32 value1,
    /* [in] */ const String& str,
    /* [in] */ Boolean value2,
    /* [in] */ Int64 value3,
    /* [in] */ Int64 value4,
    /* [in] */ Int32 value5,
    /* [in] */ const String& str2)
{
    PFL_EX(" >> CFooBar::SetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, String");
    mValue3 = value0;
    mValue0 = value1;
    mString = str;
    mValue2 = value2;
    mValue1 = value3;
    mValue5 = value4;
    mValue4 = value5;
    mString2 = str2;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int16* value0,
    /* [out] */ Int32* value1,
    /* [out] */ String* str,
    /* [out] */ Boolean* value2,
    /* [out] */ Int64* value3,
    /* [out] */ Int64* value4,
    /* [out] */ Int32* value5,
    /* [out] */ String* str2)
{
    PFL_EX(" >> CFooBar::GetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, String");
    assert(value0 != NULL);
    assert(value1 != NULL);
    assert(str != NULL);
    assert(value2 != NULL);
    assert(value3 != NULL);
    assert(value4 != NULL);
    assert(value5 != NULL);
    assert(str2 != NULL);
    *value0 = mValue3;
    *value1 = mValue0;
    *str = mString;
    *value2 = mValue2;
    *value3 = mValue1;
    *value4 = mValue5;
    *value5 = mValue4;
    *str2 = mString2;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int16 value0,
    /* [in] */ Int32 value1,
    /* [in] */ const String& str,
    /* [in] */ Boolean value2,
    /* [in] */ Int64 value3,
    /* [in] */ Int64 value4,
    /* [in] */ Int32 value5,
    /* [in] */ const String& str2,
    /* [in] */ Int32 value6)
{
    PFL_EX(" >> CFooBar::SetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, String, Int32");
    mValue3 = value0;
    mValue0 = value1;
    mString = str;
    mValue2 = value2;
    mValue1 = value3;
    mValue5 = value4;
    mValue4 = value5;
    mString2 = str2;
    mValue6 = value6;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int16* value0,
    /* [out] */ Int32* value1,
    /* [out] */ String* str,
    /* [out] */ Boolean* value2,
    /* [out] */ Int64* value3,
    /* [out] */ Int64* value4,
    /* [out] */ Int32* value5,
    /* [out] */ String* str2,
    /* [out] */ Int32* value6)
{
    PFL_EX(" >> CFooBar::GetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, String, Int32");
    assert(value0 != NULL);
    assert(value1 != NULL);
    assert(str != NULL);
    assert(value2 != NULL);
    assert(value3 != NULL);
    assert(value4 != NULL);
    assert(value5 != NULL);
    assert(str2 != NULL);
    assert(value6 != NULL);
    *value0 = mValue3;
    *value1 = mValue0;
    *str = mString;
    *value2 = mValue2;
    *value3 = mValue1;
    *value4 = mValue5;
    *value5 = mValue4;
    *str2 = mString2;
    *value6 = mValue6;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int16 value0,
    /* [in] */ Int32 value1,
    /* [in] */ const String& str,
    /* [in] */ Boolean value2,
    /* [in] */ Int64 value3,
    /* [in] */ Int64 value4,
    /* [in] */ Int32 value5,
    /* [in] */ Int32 value6,
    /* [in] */ const String& str2)
{
    PFL_EX(" >> CFooBar::SetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, Int32, String");
    mValue3 = value0;
    mValue0 = value1;
    mString = str;
    mValue2 = value2;
    mValue1 = value3;
    mValue5 = value4;
    mValue4 = value5;
    mValue6 = value6;
    mString2 = str2;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int16* value0,
    /* [out] */ Int32* value1,
    /* [out] */ String* str,
    /* [out] */ Boolean* value2,
    /* [out] */ Int64* value3,
    /* [out] */ Int64* value4,
    /* [out] */ Int32* value5,
    /* [out] */ Int32* value6,
    /* [out] */ String* str2)
{
    PFL_EX(" >> CFooBar::GetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, Int32, String");
    assert(value0 != NULL);
    assert(value1 != NULL);
    assert(str != NULL);
    assert(value2 != NULL);
    assert(value3 != NULL);
    assert(value4 != NULL);
    assert(value5 != NULL);
    assert(value6 != NULL);
    assert(str2 != NULL);
    *value0 = mValue3;
    *value1 = mValue0;
    *str = mString;
    *value2 = mValue2;
    *value3 = mValue1;
    *value4 = mValue5;
    *value5 = mValue4;
    *value6 = mValue6;
    *str2 = mString2;
    return NOERROR;
}

ECode CFooBar::SetValue(
    /* [in] */ Int16 value0,
    /* [in] */ Int32 value1,
    /* [in] */ const String& str,
    /* [in] */ Boolean value2,
    /* [in] */ Int64 value3,
    /* [in] */ Int64 value4,
    /* [in] */ Int32 value5,
    /* [in] */ Int32 value6,
    /* [in] */ const String& str2,
    /* [in] */ Int64 value7)
{
    PFL_EX(" >> CFooBar::SetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, Int32, String, Int64");
    mValue3 = value0;
    mValue0 = value1;
    mString = str;
    mValue2 = value2;
    mValue1 = value3;
    mValue5 = value4;
    mValue4 = value5;
    mValue6 = value6;
    mString2 = str2;
    mValue7 = value7;
    return NOERROR;
}

ECode CFooBar::GetValue(
    /* [out] */ Int16* value0,
    /* [out] */ Int32* value1,
    /* [out] */ String* str,
    /* [out] */ Boolean* value2,
    /* [out] */ Int64* value3,
    /* [out] */ Int64* value4,
    /* [out] */ Int32* value5,
    /* [out] */ Int32* value6,
    /* [out] */ String* str2,
    /* [out] */ Int64* value7)
{
    PFL_EX(" >> CFooBar::GetValue() Int16, Int32, String, Boolean, Int64, Int64, Int32, Int32, String, Int64");
    assert(value0 != NULL);
    assert(value1 != NULL);
    assert(str != NULL);
    assert(value2 != NULL);
    assert(value3 != NULL);
    assert(value4 != NULL);
    assert(value5 != NULL);
    assert(value6 != NULL);
    assert(str2 != NULL);
    assert(value7 != NULL);
    *value0 = mValue3;
    *value1 = mValue0;
    *str = mString;
    *value2 = mValue2;
    *value3 = mValue1;
    *value4 = mValue5;
    *value5 = mValue4;
    *value6 = mValue6;
    *str2 = mString2;
    *value7 = mValue7;
    return NOERROR;
}

} // Reflection