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

#ifndef __ELASTOS_TEXT_CFIELDPOSITION_H__
#define __ELASTOS_TEXT_CFIELDPOSITION_H__

#include "_Elastos_Text_CFieldPosition.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Text {

CarClass(CFieldPosition)
    , public Object
    , public IFieldPosition
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CFieldPosition();

    CARAPI constructor(
        /* [in] */ Int32 field);

    CARAPI constructor(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute);

    CARAPI constructor(
        /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
        /* [in] */ Int32 field);

    CARAPI Clear();

    CARAPI GetBeginIndex(
        /* [out] */ Int32* index);

    CARAPI GetEndIndex(
        /* [out] */ Int32* index);

    CARAPI GetField(
        /* [out] */ Int32* field);

    CARAPI GetFieldAttribute(
        /* [out] */ IAttributedCharacterIteratorAttribute** attribute);

    CARAPI SetBeginIndex(
        /* [in] */ Int32 index);

    CARAPI SetEndIndex(
        /* [in] */ Int32 index);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

    CARAPI GetHashCode(
    /* [out] */ Int32 * value);

    CARAPI ToString(
    /* [out] */ String * str);

public:
    Int32 mMyField;
    Int32 mBeginIndex;
    Int32 mEndIndex;

    AutoPtr<IAttributedCharacterIteratorAttribute> mMyAttribute;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CFIELDPOSITION_H__
