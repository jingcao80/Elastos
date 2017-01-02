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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICNAMEVALUEPAIR_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICNAMEVALUEPAIR_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Org::Apache::Http::INameValuePair;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * A simple class encapsulating an attribute/value pair.
 * <p>
 *  This class comforms to the generic grammar and formatting rules outlined in the
 *  <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec2.html#sec2.2">Section 2.2</a>
 *  and
 *  <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6">Section 3.6</a>
 *  of <a href="http://www.w3.org/Protocols/rfc2616/rfc2616.txt">RFC 2616</a>
 * </p>
 * <h>2.2 Basic Rules</h>
 * <p>
 *  The following rules are used throughout this specification to describe basic parsing constructs.
 *  The US-ASCII coded character set is defined by ANSI X3.4-1986.
 * </p>
 * <pre>
 *     OCTET          = <any 8-bit sequence of data>
 *     CHAR           = <any US-ASCII character (octets 0 - 127)>
 *     UPALPHA        = <any US-ASCII uppercase letter "A".."Z">
 *     LOALPHA        = <any US-ASCII lowercase letter "a".."z">
 *     ALPHA          = UPALPHA | LOALPHA
 *     DIGIT          = <any US-ASCII digit "0".."9">
 *     CTL            = <any US-ASCII control character
 *                      (octets 0 - 31) and DEL (127)>
 *     CR             = <US-ASCII CR, carriage return (13)>
 *     LF             = <US-ASCII LF, linefeed (10)>
 *     SP             = <US-ASCII SP, space (32)>
 *     HT             = <US-ASCII HT, horizontal-tab (9)>
 *     <">            = <US-ASCII double-quote mark (34)>
 * </pre>
 * <p>
 *  Many HTTP/1.1 header field values consist of words separated by LWS or special
 *  characters. These special characters MUST be in a quoted string to be used within
 *  a parameter value (as defined in section 3.6).
 * <p>
 * <pre>
 * token          = 1*<any CHAR except CTLs or separators>
 * separators     = "(" | ")" | "<" | ">" | "@"
 *                | "," | ";" | ":" | "\" | <">
 *                | "/" | "[" | "]" | "?" | "="
 *                | "{" | "}" | SP | HT
 * </pre>
 * <p>
 *  A string of text is parsed as a single word if it is quoted using double-quote marks.
 * </p>
 * <pre>
 * quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
 * qdtext         = <any TEXT except <">>
 * </pre>
 * <p>
 *  The backslash character ("\") MAY be used as a single-character quoting mechanism only
 *  within quoted-string and comment constructs.
 * </p>
 * <pre>
 * quoted-pair    = "\" CHAR
 * </pre>
 * <h>3.6 Transfer Codings</h>
 * <p>
 *  Parameters are in the form of attribute/value pairs.
 * </p>
 * <pre>
 * parameter               = attribute "=" value
 * attribute               = token
 * value                   = token | quoted-string
 * </pre>
 *
 * @author <a href="mailto:oleg at ural.com">Oleg Kalnichevski</a>
 *
 */
class BasicNameValuePair
    : public Object
    , public IBasicNameValuePair
    , public INameValuePair
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicNameValuePair() {}

    /**
     * Returns the name.
     *
     * @return String name The name
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the value.
     *
     * @return String value The current value.
     */
    CARAPI GetValue(
        /* [out] */ String* value);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

protected:
    /**
     * Default Constructor taking a name and a value. The value may be null.
     *
     * @param name The name.
     * @param value The value.
     */
    CARAPI Init(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI_(void) CloneImpl(
        /* [in] */ BasicNameValuePair* obj);

private:
    String mName;
    String mValue;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICNAMEVALUEPAIR_H_
