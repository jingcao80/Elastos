
#ifndef __ORG_APACHE_HTTP_CHTTPVERSION_H__
#define __ORG_APACHE_HTTP_CHTTPVERSION_H__

#include "_Org_Apache_Http_CHttpVersion.h"
#include "org/apache/http/ProtocolVersion.h"

namespace Org {
namespace Apache {
namespace Http {

/**
 * Represents an HTTP version, as specified in RFC 2616.
 *
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 609106 $ $Date: 2008-01-05 01:15:42 -0800 (Sat, 05 Jan 2008) $
 */
CarClass(CHttpVersion)
    , public ProtocolVersion
    , public IHttpVersion
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Obtains a specific HTTP version.
     *
     * @param major     the major version
     * @param minor     the minor version
     *
     * @return  an instance of {@link HttpVersion} with the argument version
     */
    CARAPI ForVersion(
        /* [in] */ Int32 major,
        /* [in] */ Int32 minor,
        /* [out] */ IProtocolVersion** ver);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    /**
     * Create a protocol version designator.
     *
     * @param protocol   the name of the protocol, for example "HTTP"
     * @param major      the major version number of the protocol
     * @param minor      the minor version number of the protocol
     */
    CARAPI constructor(
        /* [in] */ Int32 major,
        /* [in] */ Int32 minor);

    CARAPI constructor();

public:
    /** The protocol name. */
    static const String HTTP;

    /** HTTP protocol version 0.9 */
    static const AutoPtr<IHttpVersion> HTTP_0_9;

    /** HTTP protocol version 1.0 */
    static const AutoPtr<IHttpVersion> HTTP_1_0;

    /** HTTP protocol version 1.1 */
    static const AutoPtr<IHttpVersion> HTTP_1_1;

private:
    static const Int64 serialVersionUID = -5856653513894415344L;
};

} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CHTTPVERSION_H__
