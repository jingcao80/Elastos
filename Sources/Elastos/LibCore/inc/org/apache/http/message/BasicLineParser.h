
#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICLINEPARSER_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICLINEPARSER_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic parser for lines in the head section of an HTTP message.
 * There are individual methods for parsing a request line, a
 * status line, or a header line.
 * The lines to parse are passed in memory, the parser does not depend
 * on any specific IO mechanism.
 * Instances of this class are stateless and thread-safe.
 * Derived classes MUST maintain these properties.
 *
 * <p>
 * Note: This class was created by refactoring parsing code located in
 * various other classes. The author tags from those other classes have
 * been replicated here, although the association with the parsing code
 * taken from there has not been traced.
 * </p>
 *
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 * @author and others
 */
class BasicLineParser
    : public Object
    , public IBasicLineParser
    , public ILineParser
{
public:
    /**
     * Creates a new line parser for the given HTTP-like protocol.
     *
     * @param proto     a version of the protocol to parse, or
     *                  <code>null</code> for HTTP. The actual version
     *                  is not relevant, only the protocol name.
     */
    CARAPI constructor(
        /* [in] */ IProtocolVersion* proto);

    /**
     * Creates a new line parser for HTTP.
     */
    CARAPI constructor();

    CAR_INTERFACE_DECL()

    virtual ~BasicLineParser() {}

    static CARAPI ParseProtocolVersion(
        /* [in] */ const String& value,
        /* [in] */ ILineParser* parser,
        /* [out] */ IProtocolVersion** ver);

    /**
     * Parses the textual representation of a protocol version.
     * This is needed for parsing request lines (last element)
     * as well as status lines (first element).
     *
     * @param buffer    a buffer holding the protocol version to parse
     * @param cursor    the parser cursor containing the current position and
     *                  the bounds within the buffer for the parsing operation
     *
     * @return  the parsed protocol version
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI ParseProtocolVersion(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ IProtocolVersion** ver);

    /**
     * Checks whether there likely is a protocol version in a line.
     * This method implements a <i>heuristic</i> to check for a
     * likely protocol version specification. It does <i>not</i>
     * guarantee that {@link #parseProtocolVersion} would not
     * detect a parse error.
     * This can be used to detect garbage lines before a request
     * or status line.
     *
     * @param buffer    a buffer holding the line to inspect
     * @param cursor    the cursor at which to check for a protocol version, or
     *                  negative for "end of line". Whether the check tolerates
     *                  whitespace before or after the protocol version is
     *                  implementation dependent.
     *
     * @return  <code>true</code> if there is a protocol version at the
     *          argument index (possibly ignoring whitespace),
     *          <code>false</code> otherwise
     */
    CARAPI HasProtocolVersion(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ Boolean* result);

    static CARAPI ParseRequestLine(
        /* [in] */ const String& value,
        /* [in] */ ILineParser* parser,
        /* [out] */ IRequestLine** requestLine);

    /**
     * Parses a request line.
     *
     * @param buffer    a buffer holding the line to parse
     * @param cursor    the parser cursor containing the current position and
     *                  the bounds within the buffer for the parsing operation
     *
     * @return  the parsed request line
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI ParseRequestLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ IRequestLine** requestLine);

    static CARAPI ParseStatusLine(
        /* [in] */ const String& value,
        /* [in] */ ILineParser* parser,
        /* [out] */ IStatusLine** statusLine);

    /**
     * Parses a status line.
     *
     * @param buffer    a buffer holding the line to parse
     * @param cursor    the parser cursor containing the current position and
     *                  the bounds within the buffer for the parsing operation
     *
     * @return  the parsed status line
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI ParseStatusLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ IStatusLine** statusLine);

    static CARAPI ParseHeader(
        /* [in] */ const String& value,
        /* [in] */ ILineParser* parser,
        /* [out] */ IHeader** header);

    /**
     * Creates a header from a line.
     * The full header line is expected here. Header continuation lines
     * must be joined by the caller before invoking this method.
     *
     * @param buffer    a buffer holding the full header line.
     *                  This buffer MUST NOT be re-used afterwards, since
     *                  the returned object may reference the contents later.
     *
     * @return  the header in the argument buffer.
     *          The returned object MAY be a wrapper for the argument buffer.
     *          The argument buffer MUST NOT be re-used or changed afterwards.
     *
     * @throws ParseException        in case of a parse error
     */
    CARAPI ParseHeader(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [out] */ IHeader** header);

protected:
    /**
     * Creates a protocol version.
     * Called from {@link #parseProtocolVersion}.
     *
     * @param major     the major version number, for example 1 in HTTP/1.0
     * @param minor     the minor version number, for example 0 in HTTP/1.0
     *
     * @return  the protocol version
     */
    CARAPI CreateProtocolVersion(
        /* [in] */ Int32 major,
        /* [in] */ Int32 minor,
        /* [out] */ IProtocolVersion** ver);

    /**
     * Instantiates a new request line.
     * Called from {@link #parseRequestLine}.
     *
     * @param method    the request method
     * @param uri       the requested URI
     * @param ver       the protocol version
     *
     * @return  a new status line with the given data
     */
    CARAPI CreateRequestLine(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [in] */ IProtocolVersion* ver,
        /* [out] */ IRequestLine** requestLine);

    /**
     * Instantiates a new status line.
     * Called from {@link #parseStatusLine}.
     *
     * @param ver       the protocol version
     * @param status    the status code
     * @param reason    the reason phrase
     *
     * @return  a new status line with the given data
     */
    CARAPI CreateStatusLine(
        /* [in] */ IProtocolVersion* ver,
        /* [in] */ Int32 status,
        /* [in] */ const String& reason,
        /* [out] */ IStatusLine** statusLine);

    /**
     * Helper to skip whitespace.
     */
    CARAPI SkipWhitespace(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor);

public:
    /**
     * A default instance of this class, for use as default or fallback.
     * Note that {@link BasicLineParser} is not a singleton, there can
     * be many instances of the class itself and of derived classes.
     * The instance here provides non-customized, default behavior.
     */
    static const AutoPtr<IBasicLineParser> DEFAULT;

protected:
    /**
     * A version of the protocol to parse.
     * The version is typically not relevant, but the protocol name.
     */
    AutoPtr<IProtocolVersion> mProtocol;

};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICLINEPARSER_H_
