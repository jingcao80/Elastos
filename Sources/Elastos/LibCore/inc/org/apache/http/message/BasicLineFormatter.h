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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICLINEFORMATTER_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICLINEFORMATTER_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Interface for formatting elements of the HEAD section of an HTTP message.
 * This is the complement to {@link LineParser}.
 * There are individual methods for formatting a request line, a
 * status line, or a header line. The formatting does <i>not</i> include the
 * trailing line break sequence CR-LF.
 * The formatted lines are returned in memory, the formatter does not depend
 * on any specific IO mechanism.
 * Instances of this interface are expected to be stateless and thread-safe.
 *
 * @author <a href="mailto:remm@apache.org">Remy Maucherat</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author and others
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 574185 $
 *
 * @since 4.0
 */
class BasicLineFormatter
    : public Object
    , public IBasicLineFormatter
    , public ILineFormatter
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicLineFormatter() {}

    /**
     * Formats a protocol version.
     *
     * @param version           the protocol version to format
     * @param formatter         the formatter to use, or
     *                          <code>null</code> for the
     *                          {@link #DEFAULT default}
     *
     * @return  the formatted protocol version
     */
    static CARAPI FormatProtocolVersion(
        /* [in] */ IProtocolVersion* version,
        /* [in] */ ILineFormatter* formatter,
        /* [out] */ String* formattedStr);

    /**
     * Formats a protocol version.
     * This method does <i>not</i> follow the general contract for
     * <code>buffer</code> arguments.
     * It does <i>not</i> clear the argument buffer, but appends instead.
     * The returned buffer can always be modified by the caller.
     * Because of these differing conventions, it is not named
     * <code>formatProtocolVersion</code>.
     *
     * @param buffer    a buffer to which to append, or <code>null</code>
     * @param version   the protocol version to format
     *
     * @return  a buffer with the formatted protocol version appended.
     *          The caller is allowed to modify the result buffer.
     *          If the <code>buffer</code> argument is not <code>null</code>,
     *          the returned buffer is the argument buffer.
     */
    CARAPI AppendProtocolVersion(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IProtocolVersion* version,
        /* [out] */ ICharArrayBuffer** buf);

    /**
     * Formats a request line.
     *
     * @param reqline           the request line to format
     * @param formatter         the formatter to use, or
     *                          <code>null</code> for the
     *                          {@link #DEFAULT default}
     *
     * @return  the formatted request line
     */
    static CARAPI FormatRequestLine(
        /* [in] */ IRequestLine* reqline,
        /* [in] */ ILineFormatter* formatter,
        /* [out] */ String* formattedStr);

    /**
     * Formats a request line.
     *
     * @param buffer    a buffer available for formatting, or
     *                  <code>null</code>.
     *                  The buffer will be cleared before use.
     * @param reqline   the request line to format
     *
     * @return  the formatted request line
     */
    CARAPI FormatRequestLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IRequestLine* reqline,
        /* [out] */ ICharArrayBuffer** buf);

    /**
     * Formats a status line.
     *
     * @param statline          the status line to format
     * @param formatter         the formatter to use, or
     *                          <code>null</code> for the
     *                          {@link #DEFAULT default}
     *
     * @return  the formatted status line
     */
    static CARAPI FormatStatusLine(
        /* [in] */ IStatusLine* statline,
        /* [in] */ ILineFormatter* formatter,
        /* [out] */ String* formattedStr);

    /**
     * Formats a status line.
     *
     * @param buffer    a buffer available for formatting, or
     *                  <code>null</code>.
     *                  The buffer will be cleared before use.
     * @param statline  the status line to format
     *
     * @return  the formatted status line
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI FormatStatusLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IStatusLine* statline,
        /* [out] */ ICharArrayBuffer** buf);

    /**
     * Formats a header.
     *
     * @param header            the header to format
     * @param formatter         the formatter to use, or
     *                          <code>null</code> for the
     *                          {@link #DEFAULT default}
     *
     * @return  the formatted header
     */
    static CARAPI FormatHeader(
        /* [in] */ IHeader* header,
        /* [in] */ ILineFormatter* formatter,
        /* [out] */ String* formattedStr);

    /**
     * Formats a header.
     * Due to header continuation, the result may be multiple lines.
     * In order to generate well-formed HTTP, the lines in the result
     * must be separated by the HTTP line break sequence CR-LF.
     * There is <i>no</i> trailing CR-LF in the result.
     * <br/>
     * See the class comment for details about the buffer argument.
     *
     * @param buffer    a buffer available for formatting, or
     *                  <code>null</code>.
     *                  The buffer will be cleared before use.
     * @param header    the header to format
     *
     * @return  a buffer holding the formatted header, never <code>null</code>.
     *          The returned buffer may be different from the argument buffer.
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI FormatHeader(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IHeader* header,
        /* [out] */ ICharArrayBuffer** buf);

protected:
    /**
     * Obtains a buffer for formatting.
     *
     * @param buffer    a buffer already available, or <code>null</code>
     *
     * @return  the cleared argument buffer if there is one, or
     *          a new empty buffer that can be used for formatting
     */
    CARAPI InitBuffer(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [out] */ ICharArrayBuffer** buf);

    /**
     * Guesses the length of a formatted protocol version.
     * Needed to guess the length of a formatted request or status line.
     *
     * @param version   the protocol version to format, or <code>null</code>
     *
     * @return  the estimated length of the formatted protocol version,
     *          in characters
     */
    CARAPI EstimateProtocolVersionLen(
        /* [in] */ IProtocolVersion* version,
        /* [out] */ Int32* len);

    /**
     * Actually formats a request line.
     * Called from {@link #formatRequestLine}.
     *
     * @param buffer    the empty buffer into which to format,
     *                  never <code>null</code>
     * @param reqline   the request line to format, never <code>null</code>
     */
    CARAPI DoFormatRequestLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IRequestLine* reqline);

    /**
     * Actually formats a status line.
     * Called from {@link #formatStatusLine}.
     *
     * @param buffer    the empty buffer into which to format,
     *                  never <code>null</code>
     * @param statline  the status line to format, never <code>null</code>
     */
    CARAPI DoFormatStatusLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IStatusLine* statline);

    /**
     * Actually formats a header.
     * Called from {@link #formatHeader}.
     *
     * @param buffer    the empty buffer into which to format,
     *                  never <code>null</code>
     * @param header    the header to format, never <code>null</code>
     */
    CARAPI DoFormatHeader(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IHeader* header);

public:
    /**
     * A default instance of this class, for use as default or fallback.
     * Note that {@link BasicLineFormatter} is not a singleton, there can
     * be many instances of the class itself and of derived classes.
     * The instance here provides non-customized, default behavior.
     */
    static const AutoPtr<IBasicLineFormatter> DEFAULT;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICLINEFORMATTER_H_
