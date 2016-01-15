
#ifndef __ORG_APACHE_HTTP_ENTITY_CENTITYTEMPLATE_H_
#define __ORG_APACHE_HTTP_ENTITY_CENTITYTEMPLATE_H_

#include "_Org_Apache_Http_Entity_CEntityTemplate.h"
#include "org/apache/http/entity/AbstractHttpEntity.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 * Entity that delegates the process of content generation to an abstract
 * content producer.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
CarClass(CEntityTemplate), public AbstractHttpEntity
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetContentLength(
        /* [out] */ Int64* length);

    CARAPI GetContent(
        /* [out] */ IInputStream** inputStream);

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    CARAPI WriteTo(
        /* [in] */ IOutputStream* outstream);

    CARAPI IsStreaming(
        /* [out] */ Boolean* isStreaming);

    CARAPI ConsumeContent();

    CARAPI constructor(
        /* [in] */ IContentProducer* contentproducer);

private:
    AutoPtr<IContentProducer> mContentproducer;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_CENTITYTEMPLATE_H_
