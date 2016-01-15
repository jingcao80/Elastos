
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/message/CBasicHttpResponse.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicHttpResponse)

ECode CBasicHttpResponse::constructor(
    /* [in] */ IStatusLine* statusline,
    /* [in] */ IReasonPhraseCatalog* catalog,
    /* [in] */ ILocale* locale)
{
    return Init(statusline, catalog, locale);
}

ECode CBasicHttpResponse::constructor(
    /* [in] */ IStatusLine* statusline)
{
    return Init(statusline);
}

ECode CBasicHttpResponse::constructor(
    /* [in] */ IProtocolVersion* ver,
    /* [in] */ Int32 code,
    /* [in] */ const String& reason)
{
    return Init(ver, code, reason);
}

ECode CBasicHttpResponse::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
