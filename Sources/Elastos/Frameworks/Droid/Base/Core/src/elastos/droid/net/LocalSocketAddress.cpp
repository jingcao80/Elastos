
#include "elastos/droid/net/LocalSocketAddress.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(LocalSocketAddress, Object, ILocalSocketAddress)

ECode LocalSocketAddress::constructor(
    /* [in] */ const String& name,
    /* [in] */ LocalSocketAddressNamespace ns)
{
    mName = name;
    mNamespace = ns;
    return NOERROR;
}

ECode LocalSocketAddress::constructor(
    /* [in] */ const String& name)
{
    return constructor(name, LocalSocketAddressNamespace_ABSTRACT);
}

ECode LocalSocketAddress::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode LocalSocketAddress::GetNamespace(
    /* [out] */ LocalSocketAddressNamespace* ns)
{
    VALIDATE_NOT_NULL(ns);
    *ns = mNamespace;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
