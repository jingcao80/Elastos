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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTHEADERPARSER_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTHEADERPARSER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/message/BasicHeaderValueParser.h"

using Elastos::Core::Object;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Message::IBasicHeaderValueParser;
using Org::Apache::Http::Message::BasicHeaderValueParser;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class NetscapeDraftHeaderParser : public Object
{
public:
    NetscapeDraftHeaderParser();

    CARAPI ParseHeader(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ IHeaderElement** element);

public:
    const static AutoPtr<NetscapeDraftHeaderParser> DEFAULT;

private:
    const static AutoPtr< ArrayOf<Char32> > DELIMITERS;

    AutoPtr<IBasicHeaderValueParser> mNvpParser;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_BROWSERCOMPATSPECPACTORY_H__
