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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHEADERVALUEPARSER_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHEADERVALUEPARSER_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation for parsing header values into elements.
 * Instances of this class are stateless and thread-safe.
 * Derived classes are expected to maintain these properties.
 *
 * @author <a href="mailto:bcholmes@interlog.com">B.C. Holmes</a>
 * @author <a href="mailto:jericho@thinkfree.com">Park, Sung-Gu</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.com">Oleg Kalnichevski</a>
 * @author and others
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 595670 $
 *
 * @since 4.0
 */
class BasicHeaderValueParser
    : public Object
    , public IBasicHeaderValueParser
    , public IHeaderValueParser
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicHeaderValueParser() {}

    /**
     * Parses elements with the given parser.
     *
     * @param value     the header value to parse
     * @param parser    the parser to use, or <code>null</code> for default
     *
     * @return  array holding the header elements, never <code>null</code>
     */
    static CARAPI ParseElements(
        /* [in] */ const String& value,
        /* [in] */ IHeaderValueParser* parser,
        /* [out] */ ArrayOf<IHeaderElement*>** elements);

    /**
     * Parses a header value into elements.
     * Parse errors are indicated as <code>RuntimeException</code>.
     * <p>
     * Some HTTP headers (such as the set-cookie header) have values that
     * can be decomposed into multiple elements. In order to be processed
     * by this parser, such headers must be in the following form:
     * </p>
     * <pre>
     * header  = [ element ] *( "," [ element ] )
     * element = name [ "=" [ value ] ] *( ";" [ param ] )
     * param   = name [ "=" [ value ] ]
     *
     * name    = token
     * value   = ( token | quoted-string )
     *
     * token         = 1*&lt;any char except "=", ",", ";", &lt;"&gt; and
     *                       white space&gt;
     * quoted-string = &lt;"&gt; *( text | quoted-char ) &lt;"&gt;
     * text          = any char except &lt;"&gt;
     * quoted-char   = "\" char
     * </pre>
     * <p>
     * Any amount of white space is allowed between any part of the
     * header, element or param and is ignored. A missing value in any
     * element or param will be stored as the empty {@link String};
     * if the "=" is also missing <var>null</var> will be stored instead.
     * </p>
     *
     * @param buffer    buffer holding the header value to parse
     * @param cursor    the parser cursor containing the current position and
     *                  the bounds within the buffer for the parsing operation
     *
     * @return  an array holding all elements of the header value
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI ParseElements(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out, callee] */ ArrayOf<IHeaderElement*>** elements);

    /**
     * Parses an element with the given parser.
     *
     * @param value     the header element to parse
     * @param parser    the parser to use, or <code>null</code> for default
     *
     * @return  the parsed header element
     */
    static CARAPI ParseHeaderElement(
        /* [in] */ const String& value,
        /* [in] */ IHeaderValueParser* parser,
        /* [out] */ IHeaderElement** element);

    /**
     * Parses a single header element.
     * A header element consist of a semicolon-separate list
     * of name=value definitions.
     *
     * @param buffer    buffer holding the element to parse
     * @param cursor    the parser cursor containing the current position and
     *                  the bounds within the buffer for the parsing operation
     *
     * @return  the parsed element
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI ParseHeaderElement(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ IHeaderElement** element);

    /**
     * Parses parameters with the given parser.
     *
     * @param value     the parameter list to parse
     * @param parser    the parser to use, or <code>null</code> for default
     *
     * @return  array holding the parameters, never <code>null</code>
     */
    static CARAPI ParseParameters(
        /* [in] */ const String& value,
        /* [in] */ IHeaderValueParser* parser,
        /* [out] */ ArrayOf<INameValuePair*>** elements);

    /**
     * Parses a list of name-value pairs.
     * These lists are used to specify parameters to a header element.
     * Parse errors are indicated as <code>RuntimeException</code>.
     * <p>
     * This method comforms to the generic grammar and formatting rules
     * outlined in the
     * <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec2.html#sec2.2"
     *   >Section 2.2</a>
     * and
     * <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6"
     *   >Section 3.6</a>
     * of
     * <a href="http://www.w3.org/Protocols/rfc2616/rfc2616.txt">RFC 2616</a>.
     * </p>
     * <h>2.2 Basic Rules</h>
     * <p>
     * The following rules are used throughout this specification to
     * describe basic parsing constructs.
     * The US-ASCII coded character set is defined by ANSI X3.4-1986.
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
     * Many HTTP/1.1 header field values consist of words separated
     * by LWS or special characters. These special characters MUST be
     * in a quoted string to be used within
     * a parameter value (as defined in section 3.6).
     * <p>
     * <pre>
     * token          = 1*<any CHAR except CTLs or separators>
     * separators     = "(" | ")" | "<" | ">" | "@"
     *                | "," | ";" | ":" | "\" | <">
     *                | "/" | "[" | "]" | "?" | "="
     *                | "{" | "}" | SP | HT
     * </pre>
     * <p>
     * A string of text is parsed as a single word if it is quoted using
     * double-quote marks.
     * </p>
     * <pre>
     * quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
     * qdtext         = <any TEXT except <">>
     * </pre>
     * <p>
     * The backslash character ("\") MAY be used as a single-character
     * quoting mechanism only within quoted-string and comment constructs.
     * </p>
     * <pre>
     * quoted-pair    = "\" CHAR
     * </pre>
     * <h>3.6 Transfer Codings</h>
     * <p>
     * Parameters are in the form of attribute/value pairs.
     * </p>
     * <pre>
     * parameter               = attribute "=" value
     * attribute               = token
     * value                   = token | quoted-string
     * </pre>
     *
     * @param buffer    buffer holding the name-value list to parse
     * @param cursor    the parser cursor containing the current position and
     *                  the bounds within the buffer for the parsing operation
     *
     * @return  an array holding all items of the name-value list
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI ParseParameters(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out, callee] */ ArrayOf<INameValuePair*>** items);

    /**
     * Parses a name-value-pair with the given parser.
     *
     * @param value     the NVP to parse
     * @param parser    the parser to use, or <code>null</code> for default
     *
     * @return  the parsed name-value pair
     */
    static CARAPI ParseNameValuePair(
        /* [in] */ const String& value,
        /* [in] */ IHeaderValueParser* parser,
        /* [out] */ INameValuePair** pair);

    /**
     * Parses a name=value specification, where the = and value are optional.
     *
     * @param buffer    the buffer holding the name-value pair to parse
     * @param cursor    the parser cursor containing the current position and
     *                  the bounds within the buffer for the parsing operation
     *
     * @return  the name-value pair, where the value is <code>null</code>
     *          if no value is specified
     */
    CARAPI ParseNameValuePair(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ INameValuePair** pair);

    CARAPI ParseNameValuePair(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [in] */ ArrayOf<Char32>* delimiters,
        /* [out] */ INameValuePair** pair);

protected:
    /**
     * Creates a header element.
     * Called from {@link #parseHeaderElement}.
     *
     * @return  a header element representing the argument
     */
    CARAPI CreateHeaderElement(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [in] */ ArrayOf<INameValuePair*>* params,
        /* [out] */ IHeaderElement** element);

    /**
     * Creates a name-value pair.
     * Called from {@link #parseNameValuePair}.
     *
     * @param name      the name
     * @param value     the value, or <code>null</code>
     *
     * @return  a name-value pair representing the arguments
     */
    CARAPI CreateNameValuePair(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [out] */ INameValuePair** pair);

private:
    static CARAPI_(AutoPtr< ArrayOf<Char32> >) InitDelimiters();

    CARAPI_(Boolean) IsOneOf(
        /* [in] */ Char32 ch,
        /* [in] */ ArrayOf<Char32>* chs);

public:
    /**
     * A default instance of this class, for use as default or fallback.
     * Note that {@link BasicHeaderValueParser} is not a singleton, there
     * can be many instances of the class itself and of derived classes.
     * The instance here provides non-customized, default behavior.
     */
    static const AutoPtr<IBasicHeaderValueParser> DEFAULT;

private:
    static const Char32 PARAM_DELIMITER = ';';
    static const Char32 ELEM_DELIMITER = ',';
    static const AutoPtr< ArrayOf<Char32> > ALL_DELIMITERS;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICHEADERVALUEPARSER_H_
