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

#include "CFooBar.h"

namespace Reflection {

CAR_INTERFACE_IMPL(CFooBar, Object, IFooBar)

CAR_OBJECT_IMPL(CFooBar)

CFooBar::CFooBar()
    : mValue0(0)
    , mString("")
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

} // Reflection