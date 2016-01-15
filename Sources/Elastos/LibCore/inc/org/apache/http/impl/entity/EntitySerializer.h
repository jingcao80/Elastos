
#ifndef __ORG_APACHE_HTTP_IMPL_ENTITY_EntitySERIALIZER_H__
#define __ORG_APACHE_HTTP_IMPL_ENTITY_EntitySERIALIZER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::IO::IOutputStream;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::Entity::IBasicHttpEntity;
using Org::Apache::Http::Entity::IContentLengthStrategy;
using Org::Apache::Http::IO::ISessionOutputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Entity {

/**
 * Default implementation of an entity serializer.
 * <p>
 * This entity serializer currently supports only "chunked" and "identitiy" transfer-coding</a>
 * </p>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 560343 $
 *
 * @since 4.0
 */
class EntitySerializer : public Object
{
public:
    EntitySerializer(
        /* [in] */ IContentLengthStrategy* lenStrategy);

    CARAPI Serialize(
        /* [in] */ ISessionOutputBuffer* outbuffer,
        /* [in] */ IHttpMessage* message,
        /* [in] */ IHttpEntity* entity);

protected:
    CARAPI DoSerialize(
        /* [in] */ ISessionOutputBuffer* outbuffer,
        /* [in] */ IHttpMessage* message,
        /* [out] */ IOutputStream** output);

private:
    AutoPtr<IContentLengthStrategy> mLenStrategy;
};

} // namespace Entity
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_ENTITY_EntitySERIALIZER_H__
