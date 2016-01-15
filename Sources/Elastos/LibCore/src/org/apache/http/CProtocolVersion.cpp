
#include "org/apache/http/CProtocolVersion.h"

namespace Org {
namespace Apache {
namespace Http {

CAR_OBJECT_IMPL(CProtocolVersion)

ECode CProtocolVersion::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<IProtocolVersion> ver;
    CProtocolVersion::New((IProtocolVersion**)&ver);
    CloneImpl(ver);
    *obj = ver->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CProtocolVersion::constructor(
    /* [in] */ const String& protocol,
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor)
{
    return ProtocolVersion::Init(protocol, major, minor);
}

ECode CProtocolVersion::constructor()
{
    return NOERROR;
}

} // namespace Http
} // namespace Apache
} // namespace Org
