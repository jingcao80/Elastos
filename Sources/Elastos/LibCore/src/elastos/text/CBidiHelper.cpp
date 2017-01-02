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

#include "CBidiHelper.h"
#include "Bidi.h"
#include "CBidi.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CBidiHelper, Singleton, IBidiHelper)

CAR_SINGLETON_IMPL(CBidiHelper)

ECode CBidiHelper::ReorderVisually(
    /* [in] */ ArrayOf<Byte>* levels,
    /* [in] */ Int32 levelStart,
    /* [in] */ ArrayOf<IInterface*>* objects,
    /* [in] */ Int32 objectStart,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(levels);
    VALIDATE_NOT_NULL(objects);

    if (count < 0 || levelStart < 0 || objectStart < 0
            || count > levels->GetLength() - levelStart
            || count > objects->GetLength() - objectStart) {
        //throw new IllegalArgumentException("Invalid ranges (levels=" + levels.length +
        //        ", levelStart=" + levelStart + ", objects=" + objects.length +
        //        ", objectStart=" + objectStart + ", count=" + count + ")");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<Byte> > realLevels = ArrayOf<Byte>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        (*realLevels)[i] = (*levels)[levelStart + i];
    }

    AutoPtr< ArrayOf<Int32> > indices = Bidi::Ubidi_reorderVisual(realLevels, count);
    AutoPtr< ArrayOf<IInterface *> > result = ArrayOf<IInterface *>::Alloc(count);
    Int32 i = 0;
    for (; i < count; i++) {
        result->Set(i, (*objects)[objectStart + (*indices)[i]]);
    }

    for (i = 0; i < count; i++) {
        objects->Set(objectStart + i, (*result)[i]);
    }

    return NOERROR;
}

ECode CBidiHelper::RequiresBidi(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    if (limit < 0 || start < 0 || start > limit || limit > text->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IBidi> bidi;
    CBidi::New(text, start, NULL, 0, limit - start, 0, (IBidi**)&bidi);
    Boolean isLefttoRight;
    bidi->IsLeftToRight(&isLefttoRight);
    *result = !isLefttoRight;
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
