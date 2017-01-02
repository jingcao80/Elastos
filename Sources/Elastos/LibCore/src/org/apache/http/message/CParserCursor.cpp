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

#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::ICharArrayBuffer;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CParserCursor::CParserCursor()
    : mLowerBound(0)
    , mUpperBound(0)
    , mPos(0)
{}

CAR_INTERFACE_IMPL(CParserCursor, Object, IParserCursor)

CAR_OBJECT_IMPL(CParserCursor)

ECode CParserCursor::GetLowerBound(
    /* [out] */ Int32* bound)
{
    VALIDATE_NOT_NULL(bound)
    *bound = mLowerBound;
    return NOERROR;
}

ECode CParserCursor::GetUpperBound(
    /* [out] */ Int32* bound)
{
    VALIDATE_NOT_NULL(bound)
    *bound = mUpperBound;
    return NOERROR;
}

ECode CParserCursor::GetPos(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = mPos;
    return NOERROR;
}

ECode CParserCursor::UpdatePos(
    /* [in] */ Int32 pos)
{
    if (pos < mLowerBound) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (pos > mUpperBound) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mPos = pos;
    return NOERROR;
}

ECode CParserCursor::AtEnd(
    /* [out] */ Boolean* atEnd)
{
    VALIDATE_NOT_NULL(atEnd)
    *atEnd = mPos >= mUpperBound;
    return NOERROR;
}

ECode CParserCursor::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<ICharArrayBuffer> buffer;
    CCharArrayBuffer::New(16, (ICharArrayBuffer**)&buffer);
    buffer->Append('[');
    buffer->Append(mLowerBound);
    buffer->Append('>');
    buffer->Append(mPos);
    buffer->Append('>');
    buffer->Append(mUpperBound);
    buffer->Append(']');
    return IObject::Probe(buffer)->ToString(str);
}

ECode CParserCursor::constructor(
    /* [in] */ Int32 lowerBound,
    /* [in] */ Int32 upperBound)
{
    if (lowerBound < 0) {
        Logger::E("CParserCursor", "Lower bound cannot be negative");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (lowerBound > upperBound) {
        Logger::E("CParserCursor", "Lower bound cannot be greater then upper bound");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mLowerBound = lowerBound;
    mUpperBound = upperBound;
    mPos = lowerBound;
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
