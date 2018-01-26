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

private:
    Int32 mValue0;
    String mString;
    Int64 mValue1;
};

} // Reflection

#endif //__CFOOBAR_H__
