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

#include "CParsePosition.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Text {

CAR_OBJECT_IMPL(CParsePosition)

CAR_INTERFACE_IMPL(CParsePosition, Object, IParsePosition)

CParsePosition::CParsePosition()
    : mCurrentPosition(0)
    , mErrorIndex(-1)
{}

ECode CParsePosition::constructor(
    /* [in] */ Int32 index)
{
    mCurrentPosition = index;
    return NOERROR;
}

ECode CParsePosition::GetErrorIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mErrorIndex;
    return NOERROR;
}

ECode CParsePosition::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    *index = mCurrentPosition;
    return NOERROR;
}

ECode CParsePosition::SetErrorIndex(
    /* [in] */ Int32 index)
{
    mErrorIndex = index;
    return NOERROR;
}

ECode CParsePosition::SetIndex(
    /* [in] */ Int32 index)
{
    mCurrentPosition = index;
    return NOERROR;
}

ECode CParsePosition::Equals(
    /* [in] */ IInterface * object,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IParsePosition* pp = IParsePosition::Probe(object);
    if (pp == NULL) {
        return NOERROR;
    }
    CParsePosition* pos = (CParsePosition*) pp;
    *result = mCurrentPosition == pos->mCurrentPosition
            && mErrorIndex == pos->mErrorIndex;
    return NOERROR;
}

ECode CParsePosition::GetHashCode(
    /* [out] */ Int32 * hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = mCurrentPosition + mErrorIndex;
    return NOERROR;
}

ECode CParsePosition::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CParsePosition[index=");
    sb += mCurrentPosition;
    sb += ", errorIndex=";
    sb += mErrorIndex;
    sb += "]";
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
