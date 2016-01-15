
#ifndef __ORG_APACHE_HTTP_IMPL_ENTITY_ENTITYDESERIALIZER_H__
#define __ORG_APACHE_HTTP_IMPL_ENTITY_ENTITYDESERIALIZER_H__

#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::Entity::IBasicHttpEntity;
using Org::Apache::Http::Entity::IContentLengthStrategy;
using Org::Apache::Http::IO::ISessionInputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Entity {

class EntityDeserializer : public Object
{
public:
    EntityDeserializer(
        /* [in] */ IContentLengthStrategy* lenStrategy);

    CARAPI Deserialize(
        /* [in] */ ISessionInputBuffer* inbuffer,
        /* [in] */ IHttpMessage* message,
        /* [out] */ IHttpEntity** entity);

protected:
    CARAPI DoDeserialize(
        /* [in] */ ISessionInputBuffer* inbuffer,
        /* [in] */ IHttpMessage* message,
        /* [out] */ IBasicHttpEntity** entity);

private:
    AutoPtr<IContentLengthStrategy> mLenStrategy;
};

} // namespace Entity
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_ENTITY_ENTITYDESERIALIZER_H__
