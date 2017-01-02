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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICREQUESTLINE_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICREQUESTLINE_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IProtocolVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * The first line of an {@link org.apache.http.HttpRequest HttpRequest}.
 * It contains the method, URI, and HTTP version of the request.
 * For details, see RFC 2616.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 604625 $
 *
 * @since 4.0
 */
class BasicRequestLine
    : public Object
    , public IRequestLine
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicRequestLine() {}

    CARAPI GetMethod(
        /* [out] */ String* method);

    CARAPI GetProtocolVersion(
        /* [out] */ IProtocolVersion** ver);

    CARAPI GetUri(
        /* [out] */ String* uri);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI Init(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [in] */ IProtocolVersion* version);

    CARAPI_(void) CloneImpl(
        /* [in] */ BasicRequestLine* obj);

private:
    AutoPtr<IProtocolVersion> mProtoVersion;
    String mMethod;
    String mUri;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICREQUESTLINE_H_
