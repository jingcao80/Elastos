
#ifndef __ORG_APACHE_HTTP_MESSAGE_ABSTRACTHTTPMESSAGE_H_
#define __ORG_APACHE_HTTP_MESSAGE_ABSTRACTHTTPMESSAGE_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::IArrayOf;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHeaderIterator;
using Org::Apache::Http::Params::IHttpParams;

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
class AbstractHttpMessage
    : public Object
    , public IHttpMessage
{
public:
    CAR_INTERFACE_DECL()

    virtual ~AbstractHttpMessage() {}

    /**
     * Checks if a certain header is present in this message. Header values are
     * ignored.
     *
     * @param name the header name to check for.
     * @return true if at least one header with this name is present.
     */
    CARAPI ContainsHeader(
        /* [in] */ const String& name,
        /* [out] */ Boolean* isContain);

    /**
     * Returns all the headers with a specified name of this message. Header values
     * are ignored. Headers are orderd in the sequence they will be sent over a
     * connection.
     *
     * @param name the name of the headers to return.
     * @return the headers whose name property equals <code>name</code>.
     */
    CARAPI GetHeaders(
        /* [in] */ const String& name,
        /* [out, callee] */ ArrayOf<IHeader*>** headers);

    /**
     * Returns the first header with a specified name of this message. Header
     * values are ignored. If there is more than one matching header in the
     * message the first element of {@link #getHeaders(String)} is returned.
     * If there is no matching header in the message <code>null</code> is
     * returned.
     *
     * @param name the name of the header to return.
     * @return the first header whose name property equals <code>name</code>
     *   or <code>null</code> if no such header could be found.
     */
    CARAPI GetFirstHeader(
        /* [in] */ const String& name,
        /* [out] */ IHeader** firstHeader);

    /**
     * Returns the last header with a specified name of this message. Header values
     * are ignored. If there is more than one matching header in the message the
     * last element of {@link #getHeaders(String)} is returned. If there is no
     * matching header in the message <code>null</code> is returned.
     *
     * @param name the name of the header to return.
     * @return the last header whose name property equals <code>name</code>.
     *   or <code>null</code> if no such header could be found.
     */
    CARAPI GetLastHeader(
        /* [in] */ const String& name,
        /* [out] */ IHeader** lastHeader);

    /**
     * Returns all the headers of this message. Headers are orderd in the sequence
     * they will be sent over a connection.
     *
     * @return all the headers of this message
     */
    CARAPI GetAllHeaders(
        /* [out, callee] */ ArrayOf<IHeader*>** allHeaders);

    /**
     * Adds a header to this message. The header will be appended to the end of
     * the list.
     *
     * @param header the header to append.
     */
    CARAPI AddHeader(
        /* [in] */ IHeader* header);

    /**
     * Adds a header to this message. The header will be appended to the end of
     * the list.
     *
     * @param name the name of the header.
     * @param value the value of the header.
     */
    CARAPI AddHeader(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /**
     * Overwrites the first header with the same name. The new header will be appended to
     * the end of the list, if no header with the given name can be found.
     *
     * @param header the header to set.
     */
    CARAPI SetHeader(
        /* [in] */ IHeader* header);

    /**
     * Overwrites the first header with the same name. The new header will be appended to
     * the end of the list, if no header with the given name can be found.
     *
     * @param name the name of the header.
     * @param value the value of the header.
     */
    CARAPI SetHeader(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /**
     * Overwrites all the headers in the message.
     *
     * @param headers the array of headers to set.
     */
    CARAPI SetHeaders(
        /* [in] */ ArrayOf<IHeader*>* headers);

    /**
     * Removes a header from this message.
     *
     * @param header the header to remove.
     */
    CARAPI RemoveHeader(
        /* [in] */ IHeader* header);

    /**
     * Removes all headers with a certain name from this message.
     *
     * @param name The name of the headers to remove.
     */
    CARAPI RemoveHeaders(
        /* [in] */ const String& name);

    /**
     * Returns an iterator of all the headers.
     *
     * @return Iterator that returns Header objects in the sequence they are
     *         sent over a connection.
     */
    CARAPI GetHeaderIterator(
        /* [out] */ IHeaderIterator** headerIterator);


    /**
     * Returns an iterator of the headers with a given name.
     *
     * @param name      the name of the headers over which to iterate, or
     *                  <code>null</code> for all headers
     *
     * @return Iterator that returns Header objects with the argument name
     *         in the sequence they are sent over a connection.
     */
    CARAPI GetHeaderIterator(
        /* [in] */ const String& name,
        /* [out] */ IHeaderIterator** headerIterator);

    /**
     * Returns the parameters effective for this message as set by
     * {@link #setParams(HttpParams)}.
     */
    CARAPI GetParams(
        /* [out] */ IHttpParams** httpParams);

    /**
     * Provides parameters to be used for the processing of this message.
     * @param params the parameters
     */
    CARAPI SetParams(
        /* [in] */ IHttpParams* params);

protected:
    AbstractHttpMessage();

    CARAPI_(void) Init(
        /* [in] */ IHttpParams* params);

    CARAPI_(void) Init();

protected:
    AutoPtr<IHeaderGroup> mHeadergroup;

    AutoPtr<IHttpParams> mParams;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_ABSTRACTHTTPMESSAGE_H_
