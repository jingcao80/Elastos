
#include "AuthProvider.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(AuthProvider, Provider, IAuthProvider)
AuthProvider::~AuthProvider()
{}

ECode AuthProvider::constructor(
    /* [in] */ const String& name,
    /* [in] */ Double version,
    /* [in] */ const String& info)
{
    return Provider::constructor(name, version, info);
}

} // namespace Security
} // namespace Elastos
