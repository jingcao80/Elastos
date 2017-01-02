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

#ifndef __ELASTOS_TEXT_CCOLLATIONELEMENTITERATOR_H__
#define __ELASTOS_TEXT_CCOLLATIONELEMENTITERATOR_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "_Elastos_Text_CCollationElementIterator.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Text::ICharacterIterator;
using Libcore::ICU::ICollationElementIteratorICU;

namespace Elastos {
namespace Text {

CarClass(CCollationElementIterator)
    , public Object
    , public ICollationElementIterator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICollationElementIteratorICU* iterator);

    CARAPI GetMaxExpansion(
        /* [in] */ Int32 order,
        /* [out] */ Int32* maxExpansion);

    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    CARAPI GetNext(
        /* [out] */ Int32* nextValue);

    CARAPI GetPrevious(
        /* [out] */ Int32* previousValue);

    CARAPI Reset();

    CARAPI SetOffset(
        /* [in] */ Int32 newOffset);

    CARAPI SetText(
        /* [in] */ ICharacterIterator* source);

    CARAPI SetText(
        /* [in] */ const String& source);

private:
    AutoPtr<ICollationElementIteratorICU> mIcuIterator;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CCOLLATIONELEMENTITERATOR_H__
