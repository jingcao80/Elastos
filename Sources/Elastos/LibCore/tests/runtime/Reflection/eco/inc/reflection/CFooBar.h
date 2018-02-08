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

#ifndef __CFOOBAR_H__
#define __CFOOBAR_H__

#include "_Reflection_CFooBar.h"
#include <elastos/core/Object.h>


namespace Reflection {

CarClass(CFooBar)
    , public Object
    , public IFooBar
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFooBar();

    CARAPI constructor();

    CARAPI GetValue(
        /* [out] */ String* str);

    CARAPI SetValue(
        /* [in] */ const String& str);

    CARAPI GetValue(
        /* [out] */ Int32* value);

    CARAPI SetValue(
        /* [in] */ Int32 value);

    CARAPI GetValue(
        /* [out] */ Int64* value);

    CARAPI SetValue(
        /* [in] */ Int64 value);

    CARAPI SetValue(
        /* [in] */ Int32 value0,
        /* [in] */ const String& str,
        /* [in] */ Int64 value1);

    CARAPI GetValue(
        /* [out] */ Int32* value0,
        /* [out] */ String* str,
        /* [out] */ Int64* value1);

    CARAPI SetValue(
        /* [in] */ const String& str,
        /* [in] */ Int32 value0,
        /* [in] */ Int64 value1);

    CARAPI GetValue(
        /* [out] */ String* str,
        /* [out] */ Int32* value0,
        /* [out] */ Int64* value1);

    CARAPI SetValue(
        /* [in] */ Int16 value0,
        /* [in] */ Int32 value1,
        /* [in] */ const String& str,
        /* [in] */ Boolean value2,
        /* [in] */ Int64 value3,
        /* [in] */ Int64 value4,
        /* [in] */ Int32 value5);

    CARAPI GetValue(
        /* [out] */ Int16* value0,
        /* [out] */ Int32* value1,
        /* [out] */ String* str,
        /* [out] */ Boolean* value2,
        /* [out] */ Int64* value3,
        /* [out] */ Int64* value4,
        /* [out] */ Int32* value5);

    CARAPI SetValue(
        /* [in] */ Int16 value0,
        /* [in] */ Int32 value1,
        /* [in] */ const String& str,
        /* [in] */ Boolean value2,
        /* [in] */ Int64 value3,
        /* [in] */ Int64 value4,
        /* [in] */ Int32 value5,
        /* [in] */ const String& str2);

    CARAPI GetValue(
        /* [out] */ Int16* value0,
        /* [out] */ Int32* value1,
        /* [out] */ String* str,
        /* [out] */ Boolean* value2,
        /* [out] */ Int64* value3,
        /* [out] */ Int64* value4,
        /* [out] */ Int32* value5,
        /* [out] */ String* str2);

    CARAPI SetValue(
        /* [in] */ Int16 value0,
        /* [in] */ Int32 value1,
        /* [in] */ const String& str,
        /* [in] */ Boolean value2,
        /* [in] */ Int64 value3,
        /* [in] */ Int64 value4,
        /* [in] */ Int32 value5,
        /* [in] */ const String& str2,
        /* [in] */ Int32 value6);

    CARAPI GetValue(
        /* [out] */ Int16* value0,
        /* [out] */ Int32* value1,
        /* [out] */ String* str,
        /* [out] */ Boolean* value2,
        /* [out] */ Int64* value3,
        /* [out] */ Int64* value4,
        /* [out] */ Int32* value5,
        /* [out] */ String* str2,
        /* [out] */ Int32* value6);

    CARAPI SetValue(
        /* [in] */ Int16 value0,
        /* [in] */ Int32 value1,
        /* [in] */ const String& str,
        /* [in] */ Boolean value2,
        /* [in] */ Int64 value3,
        /* [in] */ Int64 value4,
        /* [in] */ Int32 value5,
        /* [in] */ Int32 value6,
        /* [in] */ const String& str2);

    CARAPI GetValue(
        /* [out] */ Int16* value0,
        /* [out] */ Int32* value1,
        /* [out] */ String* str,
        /* [out] */ Boolean* value2,
        /* [out] */ Int64* value3,
        /* [out] */ Int64* value4,
        /* [out] */ Int32* value5,
        /* [out] */ Int32* value6,
        /* [out] */ String* str2);

    CARAPI SetValue(
        /* [in] */ Int16 value0,
        /* [in] */ Int32 value1,
        /* [in] */ const String& str,
        /* [in] */ Boolean value2,
        /* [in] */ Int64 value3,
        /* [in] */ Int64 value4,
        /* [in] */ Int32 value5,
        /* [in] */ Int32 value6,
        /* [in] */ const String& str2,
        /* [in] */ Int64 value7);

    CARAPI GetValue(
        /* [out] */ Int16* value0,
        /* [out] */ Int32* value1,
        /* [out] */ String* str,
        /* [out] */ Boolean* value2,
        /* [out] */ Int64* value3,
        /* [out] */ Int64* value4,
        /* [out] */ Int32* value5,
        /* [out] */ Int32* value6,
        /* [out] */ String* str2,
        /* [out] */ Int64* value7);

private:
    Int32 mValue0;
    String mString2;
    String mString;
    Int64 mValue1;
    Boolean mValue2;
    Int16 mValue3;
    Int32 mValue4;
    Int64 mValue5;
    Int32 mValue6;
    Int64 mValue7;
};

} // Reflection

#endif //__CFOOBAR_H__
