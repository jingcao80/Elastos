
#include "org/apache/http/entity/CStringEntity.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_OBJECT_IMPL(CStringEntity)

ECode CStringEntity::Clone(
    /* [out] */ IInterface** o)
{
    VALIDATE_NOT_NULL(o)
    AutoPtr<CStringEntity> entity;
    CStringEntity::NewByFriend((CStringEntity**)&entity);
    CloneImpl((StringEntity*)entity);
    *o = entity->Probe(EIID_IInterface);
    REFCOUNT_ADD(*o)
    return NOERROR;
}

ECode CStringEntity::constructor(
    /* [in] */ const String& s,
    /* [in] */ const String& charset)
{
    return Init(s, charset);
}

ECode CStringEntity::constructor(
    /* [in] */ const String& s)
{
    return Init(s);
}

ECode CStringEntity::constructor()
{
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
