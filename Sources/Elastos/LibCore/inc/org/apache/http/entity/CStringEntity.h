
#ifndef __ORG_APACHE_HTTP_ENTITY_CSTRINGENTITY_H_
#define __ORG_APACHE_HTTP_ENTITY_CSTRINGENTITY_H_

#include "_Org_Apache_Http_Entity_CStringEntity.h"
#include "org/apache/http/entity/StringEntity.h"

using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 *  An entity whose content is retrieved from a string.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 618367 $
 *
 * @since 4.0
 */
CarClass(CStringEntity)
    , public StringEntity
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** o);

    CARAPI constructor(
        /* [in] */ const String& s,
        /* [in] */ const String& charset);

    CARAPI constructor(
        /* [in] */ const String& s);

    CARAPI constructor();
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_CSTRINGENTITY_H_
