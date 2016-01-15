
#ifndef __ORG_APACHE_HTTP_CLIENT_ENTITY_CURLENCODEDFORMENTITY_H_
#define __ORG_APACHE_HTTP_CLIENT_ENTITY_CURLENCODEDFORMENTITY_H_

#include "_Org_Apache_Http_Client_Entity_CUrlEncodedFormEntity.h"
#include "org/apache/http/entity/StringEntity.h"

using Elastos::Utility::IList;
using Org::Apache::Http::Entity::StringEntity;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Entity {

CarClass(CUrlEncodedFormEntity)
    , public StringEntity
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor(
        /* [in] */ IList* parameters,
        /* [in] */ const String& encoding);

    CARAPI constructor(
        /* [in] */ IList* parameters);

    CARAPI constructor();
};

} // namespace Entity
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_ENTITY_CURLENCODEDFORMENTITY_H_
