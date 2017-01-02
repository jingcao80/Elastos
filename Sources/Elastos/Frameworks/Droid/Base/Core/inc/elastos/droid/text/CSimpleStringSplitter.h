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

#ifndef __ELASTOS_DROID_TEXT_SIMPLESTRINGSPLITTER_H__
#define __ELASTOS_DROID_TEXT_SIMPLESTRINGSPLITTER_H__

#include "_Elastos_Droid_Text_CSimpleStringSplitter.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * A simple string splitter.
 *
 * <p>If the final character in the string to split is the delimiter then no empty string will
 * be returned for the empty string after that delimeter. That is, splitting <tt>"a,b,"</tt> on
 * comma will return <tt>"a", "b"</tt>, not <tt>"a", "b", ""</tt>.
 */
CarClass(CSimpleStringSplitter)
    , public Object
    , public ISimpleStringSplitter
    , public IStringSplitter
    , public IIterator
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSimpleStringSplitter();

    CARAPI constructor(
        /* [in] */ Char32 delimiter);

    CARAPI SetString(
        /*  [in] */ const String& string);

    CARAPI GetIterator(
       /* [out] */ IIterator** it);

    CARAPI HasNext(
        /* [out] */ Boolean* result);

    CARAPI GetNext(
        /* [out] */ IInterface** object);

    CARAPI GetNext(
        /* [out] */ String* str);

    CARAPI Remove();

private:
    String mString;
    Char32 mDelimiter;
    Int32 mPosition;
    Int32 mLength;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_SIMPLESTRINGSPLITTER_H__
