
#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHTTPRESPONSE_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHTTPRESPONSE_H_

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/message/AbstractHttpMessage.h"

using Elastos::Utility::ILocale;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IReasonPhraseCatalog;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IHttpResponse;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of an HTTP message that can be modified.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 620287 $
 *
 * @since 4.0
 */
class BasicHttpResponse
    : public AbstractHttpMessage
    , public IBasicHttpResponse
    , public IHttpResponse
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicHttpResponse() {}

    /**
     * Returns the protocol version this message is compatible with.
     */
    CARAPI GetProtocolVersion(
        /* [out] */ IProtocolVersion** ProtocolVersion);

    /**
     * Obtains the status line of this response.
     * The status line can be set using one of the
     * {@link #setStatusLine setStatusLine} methods,
     * or it can be initialized in a constructor.
     *
     * @return  the status line, or <code>null</code> if not yet set
     */
    CARAPI GetStatusLine(
        /* [out] */ IStatusLine** statusLine);

    /**
     * Obtains the message entity of this response, if any.
     * The entity is provided by calling {@link #setEntity setEntity}.
     *
     * @return  the response entity, or
     *          <code>null</code> if there is none
     */
    CARAPI GetEntity(
        /* [out] */ IHttpEntity** entity);

    /**
     * Obtains the locale of this response.
     * The locale is used to determine the reason phrase
     * for the {@link #setStatusCode status code}.
     * It can be changed using {@link #setLocale setLocale}.
     *
     * @return  the locale of this response, never <code>null</code>
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** loc);

    /**
     * Sets the status line of this response.
     *
     * @param statusline the status line of this response
     */
    CARAPI SetStatusLine(
        /* [in] */ IStatusLine* statusLine);

    /**
     * Sets the status line of this response.
     * The reason phrase will be determined based on the current
     * {@link #getLocale locale}.
     *
     * @param ver       the HTTP version
     * @param code      the status code
     */
    CARAPI SetStatusLine(
        /* [in] */ IProtocolVersion* ver,
        /* [in] */ Int32 code);

    /**
     * Sets the status line of this response with a reason phrase.
     *
     * @param ver       the HTTP version
     * @param code      the status code
     * @param reason    the reason phrase, or <code>null</code> to omit
     */
    CARAPI SetStatusLine(
        /* [in] */ IProtocolVersion* ver,
        /* [in] */ Int32 code,
        /* [in] */ const String& reason);

    /**
     * Updates the status line of this response with a new status code.
     * The status line can only be updated if it is available. It must
     * have been set either explicitly or in a constructor.
     * <br/>
     * The reason phrase will be updated according to the new status code,
     * based on the current {@link #getLocale locale}. It can be set
     * explicitly using {@link #setReasonPhrase setReasonPhrase}.
     *
     * @param code the HTTP status code.
     *
     * @throws IllegalStateException
     *          if the status line has not be set
     *
     * @see HttpStatus
     * @see #setStatusLine(StatusLine)
     * @see #setStatusLine(ProtocolVersion,int)
     */
    CARAPI SetStatusCode(
        /* [in] */ Int32 code);

    /**
     * Updates the status line of this response with a new reason phrase.
     * The status line can only be updated if it is available. It must
     * have been set either explicitly or in a constructor.
     *
     * @param reason    the new reason phrase as a single-line string, or
     *                  <code>null</code> to unset the reason phrase
     *
     * @throws IllegalStateException
     *          if the status line has not be set
     *
     * @see #setStatusLine(StatusLine)
     * @see #setStatusLine(ProtocolVersion,int)
     */
    CARAPI SetReasonPhrase(
        /* [in] */ const String& reason);

    /**
     * Associates a response entity with this response.
     *
     * @param entity    the entity to associate with this response, or
     *                  <code>null</code> to unset
     */
    CARAPI SetEntity(
        /* [in] */ IHttpEntity* entity);

    /**
     * Changes the locale of this response.
     * If there is a status line, it's reason phrase will be updated
     * according to the status code and new locale.
     *
     * @param loc       the new locale
     *
     * @see #getLocale getLocale
     * @see #setStatusCode setStatusCode
     */
    CARAPI SetLocale(
        /* [in] */ ILocale* loc);

    /**
     * Looks up a reason phrase.
     * This method evaluates the currently set catalog and locale.
     * It also handles a missing catalog.
     *
     * @param code      the status code for which to look up the reason
     *
     * @return  the reason phrase, or <code>null</code> if there is none
     */
    CARAPI GetReason(
        /* [in] */ Int32 code,
        /* [out] */ String* reason);

protected:
    /**
     * Creates a new response.
     * This is the constructor to which all others map.
     *
     * @param statusline        the status line
     * @param catalog           the reason phrase catalog, or
     *                          <code>null</code> to disable automatic
     *                          reason phrase lookup
     * @param locale            the locale for looking up reason phrases, or
     *                          <code>null</code> for the system locale
     */
    CARAPI Init(
        /* [in] */ IStatusLine* statusline,
        /* [in] */ IReasonPhraseCatalog* catalog,
        /* [in] */ ILocale* locale);

    /**
     * Creates a response from a status line.
     * The response will not have a reason phrase catalog and
     * use the system default locale.
     *
     * @param statusline        the status line
     */
    CARAPI Init(
        /* [in] */ IStatusLine* statusline);

    /**
     * Creates a response from elements of a status line.
     * The response will not have a reason phrase catalog and
     * use the system default locale.
     *
     * @param ver       the protocol version of the response
     * @param code      the status code of the response
     * @param reason    the reason phrase to the status code, or
     *                  <code>null</code>
     */
    CARAPI Init(
        /* [in] */ IProtocolVersion* ver,
        /* [in] */ Int32 code,
        /* [in] */ const String& reason);

protected:
    AutoPtr<IStatusLine>          mStatusline;
    AutoPtr<IHttpEntity>          mEntity;
    AutoPtr<IReasonPhraseCatalog> mReasonCatalog;
    AutoPtr<ILocale>              mLocale;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICHTTPRESPONSE_H_
