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

#include "org/apache/http/impl/auth/RFC2617Scheme.h"
#include "org/apache/http/message/BasicHeaderValueParser.h"
#include "org/apache/http/message/CParserCursor.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::Message::IHeaderValueParser;
using Org::Apache::Http::Message::BasicHeaderValueParser;
using Org::Apache::Http::Message::CParserCursor;
using Org::Apache::Http::Message::IParserCursor;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

RFC2617Scheme::RFC2617Scheme()
    : AuthSchemeBase()
{}

ECode RFC2617Scheme::ParseChallenge(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ Int32 pos,
    /* [in] */ Int32 len)
{
    AutoPtr<IHeaderValueParser> parser = IHeaderValueParser::Probe(BasicHeaderValueParser::DEFAULT);
    Int32 length;
    buffer->GetLength(&length);
    AutoPtr<IParserCursor> cursor;
    CParserCursor::New(pos, length, (IParserCursor**)&cursor);
    AutoPtr< ArrayOf<IHeaderElement*> > elements;
    parser->ParseElements(buffer, cursor, (ArrayOf<IHeaderElement*>**)&elements);
    if (elements->GetLength() == 0) {
        Logger::E("RFC2617Scheme", "Authentication challenge is empty");
        return E_MALFORMED_CHALLENGE_EXCEPTION;
    }

    AutoPtr<IHashMap> hm;
    CHashMap::New(elements->GetLength(), (IHashMap**)&hm);
    mParams = IMap::Probe(hm);
    for (Int32 i = 0; i < elements->GetLength(); ++i) {
        AutoPtr<IHeaderElement> element = (*elements)[i];
        String name, value;
        element->GetName(&name);
        element->GetValue(&value);
        AutoPtr<ICharSequence> nameCs, valueCs;
        CString::New(name, (ICharSequence**)&nameCs);
        CString::New(value, (ICharSequence**)&valueCs);
        mParams->Put(nameCs, valueCs);
    }
    return NOERROR;
}

AutoPtr<IMap> RFC2617Scheme::GetParameters()
{
    if (mParams == NULL) {
        AutoPtr<IHashMap> hm;
        CHashMap::New((IHashMap**)&hm);
        mParams = IMap::Probe(hm);
    }
    return mParams;
}

ECode RFC2617Scheme::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ String* param)
{
    VALIDATE_NOT_NULL(param)
    *param = String(NULL);
    if (name.IsNull()) {
        Logger::E("RFC2617Scheme", "Parameter name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mParams == NULL) {
        return NOERROR;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(name.ToLowerCase(/*ILocale::ENGLISH*/), (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mParams->Get(cs, (IInterface**)&value);
    ICharSequence::Probe(value)->ToString(param);
    return NOERROR;
}

ECode RFC2617Scheme::GetRealm(
    /* [out] */ String* realm)
{
    VALIDATE_NOT_NULL(realm)
    return GetParameter(String("realm"), realm);
}

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org