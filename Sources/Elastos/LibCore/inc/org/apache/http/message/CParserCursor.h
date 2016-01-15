
#ifndef __ORG_APACHE_HTTP_MESSAGE_CParserCursor_H_
#define __ORG_APACHE_HTTP_MESSAGE_CParserCursor_H_

#include "_Org_Apache_Http_Message_CParserCursor.h"
#include "org/apache/http/message/BasicLineFormatter.h"

using Elastos::Utility::IList;
using Org::Apache::Http::IHeaderIterator;
using Org::Apache::Http::IHeader;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * This class represents a context of a parsing operation:
 * <ul>
 *  <li>the current position the parsing operation is expected to start at</li>
 *  <li>the bounds limiting the scope of the parsing operation</li>
 * </ul>
 *
 * @author <a href="mailto:oleg at ural.com">Oleg Kalnichevski</a>
 */
CarClass(CParserCursor)
    , public Object
    , public IParserCursor
{
public:
    CParserCursor();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetLowerBound(
        /* [out] */ Int32* bound);

    CARAPI GetUpperBound(
        /* [out] */ Int32* bound);

    CARAPI GetPos(
        /* [out] */ Int32* pos);

    CARAPI UpdatePos(
        /* [in] */ Int32 pos);

    CARAPI AtEnd(
        /* [out] */ Boolean* atEnd);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ Int32 lowerBound,
        /* [in] */ Int32 upperBound);

private:
    Int32 mLowerBound;
    Int32 mUpperBound;
    Int32 mPos;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CParserCursor_H_
