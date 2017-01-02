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

#include "org/apache/http/message/BufferedHeader.h"
#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/message/BasicHeaderValueParser.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

BufferedHeader::BufferedHeader()
    : mValuePos(-1)
{}

CAR_INTERFACE_IMPL_3(BufferedHeader, Object, IFormattedHeader, IHeader, ICloneable)

ECode BufferedHeader::Init(
    /* [in] */ ICharArrayBuffer* buffer)
{
    if (buffer == NULL) {
        Logger::E("BufferedHeader", "Char array buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 colon;
    buffer->IndexOf(':', &colon);
    if (colon == -1) {
        String str;
        IObject::Probe(buffer)->ToString(&str);
        Logger::E("BufferedHeader", "Invalid header: %s", str.string());
        return E_PARSE_EXCEPTION;
    }
    String s;
    buffer->SubstringTrimmed(0, colon, &s);
    if (s.GetLength() == 0) {
        String str;
        IObject::Probe(buffer)->ToString(&str);
        Logger::E("BufferedHeader", "Invalid header: %s", str.string());
        return E_PARSE_EXCEPTION;
    }
    mBuffer = buffer;
    mName = s;
    mValuePos = colon + 1;
    return NOERROR;
}

ECode BufferedHeader::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode BufferedHeader::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    Int32 len;
    mBuffer->GetLength(&len);
    return mBuffer->SubstringTrimmed(mValuePos, len, value);
}

ECode BufferedHeader::GetElements(
    /* [out, callee] */ ArrayOf<IHeaderElement*>** elements)
{
    VALIDATE_NOT_NULL(elements)
    AutoPtr<IParserCursor> cursor;
    Int32 len;
    mBuffer->GetLength(&len);
    CParserCursor::New(0, len, (IParserCursor**)&cursor);
    cursor->UpdatePos(mValuePos);
    return IHeaderValueParser::Probe(BasicHeaderValueParser::DEFAULT)->ParseElements(
            mBuffer, cursor, elements);
}

ECode BufferedHeader::GetValuePos(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = mValuePos;
    return NOERROR;
}

ECode BufferedHeader::GetBuffer(
    /* [out] */ ICharArrayBuffer** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    *buffer = mBuffer;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode BufferedHeader::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return IObject::Probe(mBuffer)->ToString(str);
}

void BufferedHeader::CloneImpl(
    /* [in] */ BufferedHeader* obj)
{
    obj->mName = mName;
    obj->mBuffer = mBuffer;
    obj->mValuePos = mValuePos;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
