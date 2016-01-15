
#ifndef __ORG_APACHE_HTTP_IMPL_ENGLISHREASONPHRASECATALOG_H__
#define __ORG_APACHE_HTTP_IMPL_ENGLISHREASONPHRASECATALOG_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::IArrayOf;
using Elastos::Utility::ILocale;
using Org::Apache::Http::IReasonPhraseCatalog;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * English reason phrases for HTTP status codes.
 * All status codes defined in RFC1945 (HTTP/1.0), RFC2616 (HTTP/1.1), and
 * RFC2518 (WebDAV) are supported.
 *
 * @author Unascribed
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 *
 * @version $Revision: 505744 $
 */
class EnglishReasonPhraseCatalog
    : public Object
    , public IReasonPhraseCatalog
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetReason(
        /* [in] */ Int32 status,
        /* [in] */ ILocale* loc,
        /* [out] */ String* reason);

protected:
    EnglishReasonPhraseCatalog();

private:
    static CARAPI_(AutoPtr<IReasonPhraseCatalog>) InitInstance();

    /**
     * Stores the given reason phrase, by status code.
     * Helper method to initialize the static lookup table.
     *
     * @param status    the status code for which to define the phrase
     * @param reason    the reason phrase for this status code
     */
    static CARAPI_(void) SetReason(
        /* [in] */ Int32 status,
        /* [in] */ const String& reason);

    static CARAPI_(AutoPtr< ArrayOf<IArrayOf*> >) InitReasonPhrases();

public:
    // static array with english reason phrases defined below

    /**
     * The default instance of this catalog.
     * This catalog is thread safe, so there typically
     * is no need to create other instances.
     */
    static const AutoPtr<IReasonPhraseCatalog> INSTANCE;

private:
    /** Reason phrases lookup table. */
    static const AutoPtr< ArrayOf<IArrayOf*> > REASON_PHRASES;
    // static const String[][] REASON_PHRASES = new String[][]{
    //     null,
    //     new String[3],  // 1xx
    //     new String[8],  // 2xx
    //     new String[8],  // 3xx
    //     new String[25], // 4xx
    //     new String[8]   // 5xx
    // };

};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_ENGLISHREASONPHRASECATALOG_H__
