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

#ifndef __ORG_APACHE_HTTP_CLIENT_UTILS_URLENCODEDUTILS_H__
#define __ORG_APACHE_HTTP_CLIENT_UTILS_URLENCODEDUTILS_H__

#include "Elastos.CoreLibrary.h"
#include <elastos/coredef.h>

using Elastos::Net::IURI;
using Elastos::Utility::IList;
using Elastos::Utility::IScanner;
using Org::Apache::Http::IHttpEntity;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Utils {

/**
 * A collection of utilities for encoding URLs.
 */
class URLEncodedUtils
{
public:
    /**
     * Returns a list of {@link NameValuePair NameValuePairs} as built from the
     * URI's query portion. For example, a URI of
     * http://example.org/path/to/file?a=1&b=2&c=3 would return a list of three
     * NameValuePairs, one for a=1, one for b=2, and one for c=3.
     * <p>
     * This is typically useful while parsing an HTTP PUT.
     *
     * @param uri
     *            uri to parse
     * @param encoding
     *            encoding to use while parsing the query
     */
    static CARAPI_(AutoPtr<IList>) Parse(
        /* [in] */ IURI* uri,
        /* [in] */ const String& encoding);

    /**
     * Returns a list of {@link NameValuePair NameValuePairs} as parsed from an
     * {@link HttpEntity}. The encoding is taken from the entity's
     * Content-Encoding header.
     * <p>
     * This is typically used while parsing an HTTP POST.
     *
     * @param entity
     *            The entity to parse
     * @throws IOException
     *             If there was an exception getting the entity's data.
     */
    static CARAPI_(AutoPtr<IList>) Parse(
        /* [in] */ IHttpEntity* entity);

    /**
     * Returns true if the entity's Content-Type header is
     * <code>application/x-www-form-urlencoded</code>.
     */
    static CARAPI_(Boolean) IsEncoded(
        /* [in] */ IHttpEntity* entity);

    /**
     * Adds all parameters within the Scanner to the list of
     * <code>parameters</code>, as encoded by <code>encoding</code>. For
     * example, a scanner containing the string <code>a=1&b=2&c=3</code> would
     * add the {@link NameValuePair NameValuePairs} a=1, b=2, and c=3 to the
     * list of parameters.
     *
     * @param parameters
     *            List to add parameters to.
     * @param scanner
     *            Input that contains the parameters to parse.
     * @param encoding
     *            Encoding to use when decoding the parameters.
     */
    static CARAPI Parse(
        /* [in] */ IList* parameters,
        /* [in] */ IScanner* scanner,
        /* [in] */ const String& encoding);

    /**
     * Returns a String that is suitable for use as an <code>application/x-www-form-urlencoded</code>
     * list of parameters in an HTTP PUT or HTTP POST.
     *
     * @param parameters  The parameters to include.
     * @param encoding The encoding to use.
     */
    static CARAPI_(String) Format(
        /* [in] */ IList* parameters,
        /* [in] */ const String& encoding);

    static CARAPI Decode(
        /* [in] */ const String& content,
        /* [in] */ const String& encoding,
        /* [out] */ String* value);

    static CARAPI Encode(
        /* [in] */ const String& content,
        /* [in] */ const String& encoding,
        /* [out] */ String* value);

public:
    static const String CONTENT_TYPE;

private:
    static const String PARAMETER_SEPARATOR;
    static const String NAME_VALUE_SEPARATOR;
};

} // namespace Utils
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_UTILS_URLENCODEDUTILS_H__
