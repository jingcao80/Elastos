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

#ifndef __ELASTOS_TEXT_CPARSEPOSITION_H__
#define __ELASTOS_TEXT_CPARSEPOSITION_H__

#include "_Elastos_Text_CParsePosition.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Text {

CarClass(CParsePosition)
    , public Object
    , public IParsePosition
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CParsePosition();

    CARAPI constructor(
        /* [in] */ Int32 index);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * result);

    CARAPI GetErrorIndex(
        /* [out] */ Int32* index);

    CARAPI GetIndex(
        /* [out] */ Int32* index);

    CARAPI GetHashCode(
        /* [out] */ Int32 * hash);

    CARAPI SetErrorIndex(
        /* [in] */ Int32 index);

    CARAPI SetIndex(
        /* [in] */ Int32 index);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mCurrentPosition;
    Int32 mErrorIndex;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CPARSEPOSITION_H__
